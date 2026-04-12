#include <string.h>

#include "lexer.h"

typedef enum _State: u8 {
    State_Start,
    State_Cr,
    State_Name,
    State_Integer,
    State_MinusOrRightArrow,
    State_AssignOrEqual,
    State_ForwardSlashOrSingleLineComment,
    State_SingleLineComment,
} State;

typedef enum _Action {
    Action_Continue,
    Action_Again,
    Action_Panic,
} Action;

void
Lexer_init(
    Lexer * self
) {
    LineCache_init(&self->lc);
    MutBuf_init(&self->str);
    TokSeq_init(&self->stm);
    LexRpt_init(&self->rpt);

    self->stat = State_Start;
    self->should_mark_newline = false;
    self->cur_idx = 0;
    self->nl_idx = 0;
    self->row = 0;
    self->col = 0;
}

static
bool
Lexer_report_error_invalid_byte(
    Lexer * self,
    u8 byte
) {
    return LexRpt_add_error_invalid_byte(&self->rpt, self->row, self->col, byte);
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
Lexer_post_byte_consumption(
    Lexer * self
) {
    self->cur_idx += 1;
    self->col += 1;
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
        self->row += 1;
        self->col = 0;
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
    LineInfo_init(&info, self->nl_idx, end_idx - self->nl_idx, eol);
    if (!LineCache_push(&self->lc, &info)) {
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
        return false;
    }

    return true;
}

static
void
Lexer_reset_integer_cache(
    Lexer * self
) {
    self->int_.val = 0;
    self->int_.cnt = 0;
}

static
void
Lexer_collect_integer_byte(
    Lexer * self,
    u8 byte
) {
    self->int_.val *= 10;
    self->int_.val += byte - '0';
    self->int_.cnt += 1;
}

static
bool
Lexer_add_tagonly_token(
    Lexer * self,
    TokTag tag
) {
    if (!TokSeq_push_tagonly(&self->stm, tag)) {
        return false;
    }

    return true;
}

/**
 * @brief String-token pair.
 */
typedef struct _StPair {
    const char * cstr;
    TokTag tag;
} StPair;

/* Keyword/literal entries. */
static
StPair *
KL_ENTRIES[] = {
    (StPair[]) { { NULL, 0 } },
    (StPair[]) { { NULL, 0 } },
    (StPair[]) {
        { "fn", TokTag_Fn },
        { "if", TokTag_If },
        { NULL, 0 },
    },
    (StPair[]) {
        { "let", TokTag_Let },
        { NULL, 0 },
    },
    (StPair[]) {
        { "else", TokTag_Else },
        { "true", TokTag_True },
        { NULL, 0 },
    },
    (StPair[]) {
        { "false", TokTag_False },
        { NULL, 0 },
    },
    (StPair[]) {
        { "return", TokTag_Return },
        { NULL, 0 },
    },
};

// Number of the keyword/literal entries.
static const usize NUM_KL_ENTRIES =
    sizeof(KL_ENTRIES) / sizeof(KL_ENTRIES[0]);

// Maximum length of the keyword/literal string.
static const usize MAX_KL_LENGTH = NUM_KL_ENTRIES - 1;

static
bool
name_to_keyword(
    BufSlice name,
    TokTag * tag
) {
    if (name.len > MAX_KL_LENGTH) {
        return false;
    }

    StPair * tok = KL_ENTRIES[name.len];
    while (tok->cstr != NULL) {
        if (strncmp((char *)name.buf, tok->cstr, name.len) == 0) {
            *tag = tok->tag;
            return true;
        }

        tok += 1;
    }

    return false;
}

static
bool
Lexer_add_name_token(
    Lexer * self
) {
    TokTag tag;
    bool res;
    BufSlice name = MutBuf_as_slice(&self->str);
    if (name_to_keyword(name, &tag)) {
        res = TokSeq_push_tagonly(&self->stm, tag);
    } else {
        res = TokSeq_push_name(&self->stm, name);
    }

    MutBuf_clear(&self->str);

    return res;
}

static
bool
Lexer_add_integer_token(
    Lexer * self
) {
    bool res = TokSeq_push_integer(&self->stm, (usize)self->int_.val);

    MutBuf_clear(&self->str);

    return res;
}

static
bool
Lexer_add_single_line_comment_token(
    Lexer * self
) {
    BufSlice buf = MutBuf_as_slice(&self->str);
    bool res = TokSeq_push_single_line_comment(&self->stm, buf);

    MutBuf_clear(&self->str);

    return res;
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
bool
is_integer_byte(
    u8 byte
) {
    return '0' <= byte && byte <= '9';
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
            return Action_Panic;
        }

        return Action_Continue;
    }

    // Integer characters
    if (is_integer_byte(byte)) {
        Lexer_reset_integer_cache(self);
        Lexer_collect_integer_byte(self, byte);

        Lexer_set_state(self, State_Integer);
        return Action_Continue;
    }

    // Name characters
    if (is_name_start_byte(byte)) {
        if (!Lexer_collect_str_byte(self, byte)) {
            return Action_Panic;
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
    case '/': {
        Lexer_set_state(self, State_ForwardSlashOrSingleLineComment);
        return Action_Continue;
    }

    case '.': tag = TokTag_Dot; break;

    case '=': {
        Lexer_set_state(self, State_AssignOrEqual);
        return Action_Continue;
    }
    case ',': tag = TokTag_Comma; break;
    case ':': tag = TokTag_Colon; break;
    case ';': tag = TokTag_Semicolon; break;

    case '(': tag = TokTag_LeftParen; break;
    case ')': tag = TokTag_RightParen; break;
    case '[': tag = TokTag_LeftBracket; break;
    case ']': tag = TokTag_RightBracket; break;
    case '{': tag = TokTag_LeftBrace; break;
    case '}': tag = TokTag_RightBrace; break;

    default:
        if (!Lexer_report_error_invalid_byte(self, byte)) {
            return Action_Panic;
        }

        return Action_Continue;
    }
    if (!Lexer_add_tagonly_token(self, tag)) {
        return Action_Panic;
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
        return Action_Panic;
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
    if (is_name_other_byte(byte)) {
        if (!Lexer_collect_str_byte(self, byte)) {
            return Action_Panic;
        }

        return Action_Continue;
    } else {
        if (!Lexer_add_name_token(self)) {
            return Action_Panic;
        }

        Lexer_set_state(self, State_Start);

        return Action_Again;
    }
}

static
Action
Lexer_run_fsm_integer(
    Lexer * self,
    u8 byte
) {
    if (is_integer_byte(byte)) {
        Lexer_collect_integer_byte(self, byte);

        return Action_Continue;
    } else {
        if (!Lexer_add_integer_token(self)) {
            return Action_Panic;
        }

        Lexer_set_state(self, State_Start);

        return Action_Again;
    }
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
            return Action_Panic;
        }

        act = Action_Continue;
    } else {
        if (!Lexer_add_tagonly_token(self, TokTag_Hyphen)) {
            return Action_Panic;
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
            return Action_Panic;
        }

        act = Action_Continue;
    } else {
        if (!Lexer_add_tagonly_token(self, TokTag_Assign)) {
            return Action_Panic;
        }

        act = Action_Again;
    }

    Lexer_set_state(self, State_Start);

    return act;
}

static
Action
Lexer_run_fsm_forward_slash_or_single_line_comment(
    Lexer * self,
    u8 byte
) {
    if (byte == '/') {
        Lexer_set_state(self, State_SingleLineComment);
        return Action_Continue;
    } else {
        if (!Lexer_add_tagonly_token(self, TokTag_ForwardSlash)) {
            return Action_Panic;
        }

        Lexer_set_state(self, State_Start);
        return Action_Again;
    }
}

static
Action
Lexer_run_fsm_single_line_comment(
    Lexer * self,
    u8 byte
) {
    if (byte == '\r' ||
        byte == '\n') {

        if (!Lexer_add_single_line_comment_token(self)) {
            return Action_Panic;
        }

        Lexer_set_state(self, State_Start);

        return Action_Again;
    } {
        if (!Lexer_collect_str_byte(self, byte)) {
            return Action_Panic;
        }

        return Action_Continue;
    }
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
    case State_Integer:             act = Lexer_run_fsm_integer(self, byte); break;
    case State_MinusOrRightArrow:   act = Lexer_run_fsm_minus_or_right_arrow(self, byte); break;
    case State_AssignOrEqual:       act = Lexer_run_fsm_assign_or_equal(self, byte); break;
    case State_ForwardSlashOrSingleLineComment:
        act = Lexer_run_fsm_forward_slash_or_single_line_comment(self, byte);
        break;
    case State_SingleLineComment:   act = Lexer_run_fsm_single_line_comment(self, byte); break;
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
            Lexer_post_byte_consumption(self);
            Lexer_check_newline_marking(self);
            return true;

        case Action_Again:
            Lexer_check_newline_marking(self);
            continue;

        case Action_Panic:
            return false;
        }
    }
}

static
bool
Lexer_feed_eol(
    Lexer * self
) {
    bool res = false;
    Eol eol = Eol_None;

    switch ((State)self->stat) {
    case State_Start:
        break;

    case State_Cr:
        eol = Eol_Cr;
        break;

    case State_Name:
        if (!Lexer_add_name_token(self)) {
            goto Exit;
        }

        break;

    case State_Integer:
        if (!Lexer_add_integer_token(self)) {
            goto Exit;
        }

        break;

    case State_MinusOrRightArrow:
        if (!Lexer_add_tagonly_token(self, TokTag_Hyphen)) {
            goto Exit;
        }

        break;

    case State_AssignOrEqual:
        if (!Lexer_add_tagonly_token(self, TokTag_Assign)) {
            goto Exit;
        }

        break;

    case State_ForwardSlashOrSingleLineComment:
        if (!Lexer_add_tagonly_token(self, TokTag_ForwardSlash)) {
            goto Exit;
        }

        break;

    case State_SingleLineComment:
        if (!Lexer_add_single_line_comment_token(self)) {
            goto Exit;
        }

        break;
    }

    if (!Lexer_mark_newline(self, eol)) {
        goto Exit;
    }

    res = true;

Exit:
    return res;
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

    if (!Lexer_feed_eol(self)) {
        return false;
    }

    return true;
}

void
Lexer_extract(
    Lexer * self,
    LineCache * lc,
    TokSeq * stm,
    LexRpt * rpt
) {
    if (lc != NULL) {
        memcpy(lc, &self->lc, sizeof(LineCache));
        LineCache_init(&self->lc);
    }

    if (stm != NULL) {
        memcpy(stm, &self->stm, sizeof(TokSeq));
        TokSeq_init(&self->stm);
    }

    if (rpt != NULL) {
        memcpy(rpt, &self->rpt, sizeof(LexRpt));
        LexRpt_init(&self->rpt);
    }
}

void
Lexer_deinit(
    Lexer * self
) {
    LexRpt_deinit(&self->rpt);
    TokSeq_deinit(&self->stm);
    MutBuf_deinit(&self->str);
    LineCache_deinit(&self->lc);
}