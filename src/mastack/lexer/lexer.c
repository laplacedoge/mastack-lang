
#include "lexer.h"

typedef enum _State: u8 {
    State_Start,
    State_Cr,
    State_Name,
    State_MinusOrRightArrow,
    State_AssignOrEqual,
} State;

typedef enum _Action {
    Action_Continue,
    Action_Again,
    Action_Error,
} Action;

void
Lexer_init(
    Lexer * self
) {
    LineCache_init(&self->lc);
    MutBuf_init(&self->str);
    TokSeq_init(&self->stm);

    self->cur_idx = 0;
    self->nl_idx = 0;
}

static
inline
usize
Lexer_current_index(
    Lexer * self
) {
    return self->cur_idx;
}

static
inline
void
Lexer_increment_current_index(
    Lexer * self
) {
    self->cur_idx += 1;
}

static
inline
void
Lexer_check_newline_marking(
    Lexer * self
) {
    if (self->should_mark_newline) {
        self->should_mark_newline = false;
        self->nl_idx = self->cur_idx;
    }
}

static
inline
void
Lexer_set_state(
    Lexer * self,
    State stat
) {
    self->stat = (u8)stat;
}

static
void
Lexer_set_error_kind(
    Lexer * self,
    LexErrKind kind
) {
    self->err.kind = kind;
}

static
inline
bool
Lexer_mark_newline(
    Lexer * self,
    Eol eol
) {
    usize end_idx;
    switch (eol) {
    case Eol_None:
    case Eol_Lf:
        end_idx = self->cur_idx;
        break;

    case Eol_Cr:
    case Eol_CrLf:
        end_idx = self->cur_idx - 1;
        break;
    }

    LineInfo info;
    info.off = self->nl_idx;
    info.len = end_idx - self->nl_idx;
    if (!LineCache_push(&self->lc, &info)) {
        Lexer_set_error_kind(self, LexErrKind_NoMem);
        return false;
    }

    self->should_mark_newline = true;

    return true;
}

static
bool
Lexer_collect_str_byte(
    Lexer * self,
    u8 byte
) {
    if (!MutBuf_push(&self->str, byte)) {
        Lexer_set_error_kind(self, LexErrKind_NoMem);
        return false;
    }

    return true;
}

static
bool
Lexer_add_tagonly_token(
    Lexer * self,
    TokTag tag
) {
    if (!TokSeq_push_tagonly(&self->stm, tag)) {
        Lexer_set_error_kind(self, LexErrKind_NoMem);
        return false;
    }

    return true;
}

static
bool
Lexer_add_name_token(
    Lexer * self,
    BufSlice name
) {
    if (!TokSeq_push_name(&self->stm, name)) {
        Lexer_set_error_kind(self, LexErrKind_NoMem);
        return false;
    }

    return true;
}

static
bool
is_name_start_byte(
    u8 byte
) {
    return 'A' <= byte && byte <= 'Z' ||
        'a' <= byte && byte <= 'z' ||
        byte == '_';
}

static
bool
is_name_other_byte(
    u8 byte
) {
    return 'A' <= byte && byte <= 'Z' ||
        'a' <= byte && byte <= 'z' ||
        '0' <= byte && byte <= '9' ||
        byte == '_';
}

static
Action
Lexer_run_fsm_start(
    Lexer * self,
    u8 byte
) {
    // whitespace characters
    if (byte == ' ' ||
        byte == '\t') {

        return Action_Continue;
    }

    // CR character
    if (byte == '\r') {
        Lexer_set_state(self, State_Cr);
        return Action_Continue;
    }

    // LF character
    if (byte == '\n') {
        if (!Lexer_mark_newline(self, Eol_Lf)) {
            return Action_Error;
        }

        return Action_Continue;
    }

    // Name characters
    if (is_name_start_byte(byte)) {
        if (!Lexer_collect_str_byte(self, byte)) {
            return Action_Error;
        }

        Lexer_set_state(self, State_Name);
        return Action_Continue;
    }

    TokTag tag;
    switch (byte) {
    case '+': tag = TokTag_Plus; break;
    case '-': {
        Lexer_set_state(self, State_MinusOrRightArrow);
        return Action_Continue;
    }
    case '*': tag = TokTag_Asterisk; break;
    case '/': tag = TokTag_ForwardSlash; break;

    case '.': tag = TokTag_Dot; break;

    case '=': {
        Lexer_set_state(self, State_AssignOrEqual);
        return Action_Continue;
    }
    case ':': tag = TokTag_Colon; break;
    case ';': tag = TokTag_Semicolon; break;

    case '(': tag = TokTag_LeftParen; break;
    case ')': tag = TokTag_RightParen; break;
    case '[': tag = TokTag_LeftBracket; break;
    case ']': tag = TokTag_RightBracket; break;
    case '{': tag = TokTag_LeftBrace; break;
    case '}': tag = TokTag_RightBrace; break;

    default:
        // TODO: Invalid byte error
        return Action_Error;
    }
    if (!Lexer_add_tagonly_token(self, tag)) {
        return Action_Error;
    }

    return Action_Continue;
}

static
Action
Lexer_run_fsm_cr(
    Lexer * self,
    u8 byte
) {
    Action act;
    Eol eol;

    if (byte == '\n') {
        eol = Eol_CrLf;
        act = Action_Continue;
    } else {
        eol = Eol_Cr;
        act = Action_Again;
    }

    if (!Lexer_mark_newline(self, eol)) {
        return Action_Error;
    }

    Lexer_set_state(self, State_Start);

    return act;
}

static
Action
Lexer_run_fsm_name(
    Lexer * self,
    u8 byte
) {
    Action act;
    if (is_name_other_byte(byte)) {
        if (!Lexer_collect_str_byte(self, byte)) {
            return Action_Error;
        }

        act = Action_Continue;
    } else {
        BufSlice name = MutBuf_as_slice(&self->str);
        if (!Lexer_add_name_token(self, name)) {
            return Action_Error;
        }

        MutBuf_clear(&self->str);

        act = Action_Again;
    }

    return act;
}

static
Action
Lexer_run_fsm_minus_or_right_arrow(
    Lexer * self,
    u8 byte
) {
    Action act;
    if (byte == '>') {
        if (!Lexer_add_tagonly_token(self, TokTag_RightArrow)) {
            return Action_Error;
        }

        act = Action_Continue;
    } else {
        if (!Lexer_add_tagonly_token(self, TokTag_Hyphen)) {
            return Action_Error;
        }

        act = Action_Again;
    }

    Lexer_set_state(self, State_Start);

    return act;
}

static
Action
Lexer_run_fsm_assign_or_equal(
    Lexer * self,
    u8 byte
) {
    Action act;
    if (byte == '=') {
        if (!Lexer_add_tagonly_token(self, TokTag_Equal)) {
            return Action_Error;
        }

        act = Action_Continue;
    } else {
        if (!Lexer_add_tagonly_token(self, TokTag_Assign)) {
            return Action_Error;
        }

        act = Action_Again;
    }

    Lexer_set_state(self, State_Start);

    return act;
}

static
Action
Lexer_run_fsm(
    Lexer * self,
    u8 byte
) {
    Action act;
    switch ((State)self->stat) {
    case State_Start:               act = Lexer_run_fsm_start(self, byte); break;
    case State_Cr:                  act = Lexer_run_fsm_cr(self, byte); break;
    case State_Name:                act = Lexer_run_fsm_name(self, byte); break;
    case State_MinusOrRightArrow:   act = Lexer_run_fsm_minus_or_right_arrow(self, byte); break;
    case State_AssignOrEqual:       act = Lexer_run_fsm_assign_or_equal(self, byte); break;
    }

    return act;
}

static
bool
Lexer_feed_byte(
    Lexer * self,
    u8 byte
) {
    while (true) {
        switch (Lexer_run_fsm(self, byte)) {
        case Action_Continue:
            Lexer_increment_current_index(self);
            Lexer_check_newline_marking(self);
            return true;

        case Action_Again:
            Lexer_check_newline_marking(self);
            continue;

        case Action_Error:
            return false;
        }
    }
}

bool
Lexer_tokenize(
    Lexer * self,
    BufSlice text
) {
    for (usize i = 0; i < text.len; i++) {
        if (!Lexer_feed_byte(self, text.buf[i])) {
            return false;
        }
    }

    return true;
}

void
Lexer_extract(
    Lexer * self,
    TokSeq * seq
);

void
Lexer_deinit(
    Lexer * self
) {
    TokSeq_deinit(&self->stm);
    MutBuf_deinit(&self->str);
    LineCache_deinit(&self->lc);
}