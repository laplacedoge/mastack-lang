#include <string.h>

#include "memory/memory.h"
#include "token.h"

const char *
TokTag_to_str(
    TokTag tag
) {
    switch (tag) {
    case TokTag_Name:           return "name";
    case TokTag_Int:            return "integer";

    case TokTag_Else:           return "else";
    case TokTag_Fn:             return "fn";
    case TokTag_If:             return "if";
    case TokTag_Let:            return "let";
    case TokTag_Return:         return "return";

    case TokTag_False:          return "false";
    case TokTag_True:           return "true";

    case TokTag_Plus:           return "+";
    case TokTag_Hyphen:         return "-";
    case TokTag_Asterisk:       return "*";
    case TokTag_ForwardSlash:   return "/";

    case TokTag_Not:            return "!";
    case TokTag_And:            return "&";
    case TokTag_Or:             return "|";

    case TokTag_Equal:          return "==";
    case TokTag_NotEqual:       return "!=";
    case TokTag_GreaterThan:    return ">";
    case TokTag_Gte:            return ">=";
    case TokTag_LessThan:       return "<";
    case TokTag_Lte:            return "<=";

    case TokTag_Dot:            return ".";
    case TokTag_RightArrow:     return "->";

    case TokTag_Assign:         return "=";
    case TokTag_Comma:          return ",";
    case TokTag_Colon:          return ":";
    case TokTag_Semicolon:      return ";";

    case TokTag_LeftParen:      return "(";
    case TokTag_RightParen:     return ")";
    case TokTag_LeftBracket:    return "[";
    case TokTag_RightBracket:   return "]";
    case TokTag_LeftBrace:      return "{";
    case TokTag_RightBrace:     return "}";

    case TokTag_SlComment:      return "sl_comment";
    }
}

void
Token_init_tagonly(
    Token * self,
    TokTag tag
) {
    memset(self, 0, sizeof(Token));
    self->tag = tag;
}

bool
Token_init_name_from_cstr(
    Token * self,
    const char * name
) {
    ImmBuf buf;
    if (!ImmBuf_init_from_cstr(&buf, name)) {
        return false;
    }

    Token_init_tagonly(self, TokTag_Name);
    self->v.name.buf = buf;

    return true;
}

void
Token_init_integer(
    Token * self,
    usize val
) {
    Token_init_tagonly(self, TokTag_Int);
    self->v.int_.val = val;
}

bool
Token_init_sl_comment_from_cstr(
    Token * self,
    const char * comment
) {
    ImmBuf buf;
    if (!ImmBuf_init_from_cstr(&buf, comment)) {
        return false;
    }

    Token_init_tagonly(self, TokTag_SlComment);
    self->v.sl_cmt.buf = buf;

    return true;
}

bool
Token_compare(
    Token * self,
    Token * other
) {
    if (other->tag != self->tag) {
        return false;
    }

    switch (other->tag) {
    case TokTag_Name:
        return ImmBuf_compare(&other->v.name.buf, &self->v.name.buf);

    case TokTag_Int:
        return other->v.int_.val == self->v.int_.val;

    case TokTag_SlComment:
        return ImmBuf_compare(&other->v.sl_cmt.buf, &self->v.sl_cmt.buf);

    default:
        return true;
    }
}

bool
Token_write(
    const Token * tok,
    BufWriter * wrt
) {
    const char * str = TokTag_to_str(tok->tag);

    if (tok->tag == TokTag_Name) {
        BufSlice name = ImmBuf_as_slice(&tok->v.name.buf);
        return BufWriter_write_fmt(wrt, "<%s: %.*s>", str, name.len, name.buf);
    }

    if (tok->tag == TokTag_Int) {
        BufSlice text = ImmBuf_as_slice(&tok->v.name.buf);
        usize val = tok->v.int_.val;
        return BufWriter_write_fmt(wrt, "<%s: %zu>", str, val);
    }

    if (tok->tag == TokTag_False ||
        tok->tag == TokTag_True) {

        return BufWriter_write_fmt(wrt, "<boolean_literal: %s>", str);
    }

    if (tok->tag == TokTag_SlComment) {
        BufSlice src_slice = ImmBuf_as_slice(&tok->v.name.buf);

        ImmBuf esc_text;
        if (!ImmBuf_init_escaped_from_slice(&esc_text, src_slice)) {
            return false;
        }

        BufSlice esc_slice = ImmBuf_as_slice(&esc_text);
        bool res = BufWriter_write_fmt(
            wrt, "<%s: \"%.*s\">", str, esc_slice.len, esc_slice.buf);
        ImmBuf_deinit(&esc_text);

        return res;
    }

    switch (tok->tag) {
    case TokTag_Fn:
    case TokTag_Let:
    case TokTag_Return:
        return BufWriter_write_fmt(wrt, "<keyword: %s>", str);

    default:
        break;
    }

    return BufWriter_write_fmt(wrt, "<punctuator: '%s'>", str);
}

void
Token_deinit(
    Token * tok
) {
    switch (tok->tag) {
    case TokTag_Name:
        ImmBuf_deinit(&tok->v.name.buf);
        break;

    case TokTag_SlComment:
        ImmBuf_deinit(&tok->v.sl_cmt.buf);
        break;

    default:
        break;
    }
}

void
TokSeq_init(
    TokSeq * self
) {
    MutBuf_init(&self->buf);
    self->cnt = 0;
}

bool
TokSeq_push(
    TokSeq * self,
    Token * tok
) {
    BufSlice buf = BufSlice_new_from_ptr(tok);
    if (!MutBuf_extend(&self->buf, buf)) {
        return false;
    }

    self->cnt += 1;

    return true;
}

bool
TokSeq_push_name(
    TokSeq * self,
    BufSlice name
) {
    ImmBuf buf;
    if (!ImmBuf_init_from_slice(&buf, name)) {
        return NULL;
    }

    Token tok;
    Token_init_tagonly(&tok, TokTag_Name);
    tok.v.name.buf = buf;

    if (!TokSeq_push(self, &tok)) {
        goto DeinitTok;
    }

    return true;

DeinitTok:
    Token_deinit(&tok);

Exit:
    return false;
}

bool
TokSeq_push_integer(
    TokSeq * self,
    usize val
) {
    Token tok;
    Token_init_tagonly(&tok, TokTag_Int);
    tok.v.int_.val = val;

    return TokSeq_push(self, &tok);
}

bool
TokSeq_push_single_line_comment(
    TokSeq * self,
    BufSlice comment
) {
    ImmBuf buf;
    if (!ImmBuf_init_from_slice(&buf, comment)) {
        return NULL;
    }

    Token tok;
    Token_init_tagonly(&tok, TokTag_SlComment);
    tok.v.sl_cmt.buf = buf;

    if (!TokSeq_push(self, &tok)) {
        goto DeinitTok;
    }

    return true;

DeinitTok:
    Token_deinit(&tok);

Exit:
    return false;
}

bool
TokSeq_push_tagonly(
    TokSeq * self,
    TokTag tag
) {
    Token tok;
    Token_init_tagonly(&tok, tag);

    return TokSeq_push(self, &tok);
}

bool
TokSeq_write(
    TokSeq * self,
    BufWriter * wrt
) {
    bool res = false;

    usize cnt = TokSeq_count(self);
    if (!BufWriter_write_fmt(wrt, "<TokSeq(%zu): [", cnt)) {
        goto Exit;
    }

    if (cnt == 1) {
        const Token * tok = TokSeq_at(self, 0);
        if (!Token_write(tok, wrt)) {
            goto Exit;
        }
    } else if (cnt > 1) {
        const Token * tok = TokSeq_at(self, 0);
        if (!Token_write(tok, wrt)) {
            goto Exit;
        }

        for (usize i = 1; i < cnt; i++) {
            const Token * tok = TokSeq_at(self, i);
            if (!BufWriter_write_str(wrt, ", ") ||
                !Token_write(tok, wrt)) {

                goto Exit;
            }
        }
    }

    if (!BufWriter_write_str(wrt, "]>")) {
        goto Exit;
    }

    res = true;

Exit:
    return res;
}

void
TokSeq_clear(
    TokSeq * self
) {
    MutBuf * buf = &self->buf;
    for (usize i = 0; i < self->cnt; i++) {
        Token_deinit(&MutBuf_at_as(buf, Token, i));
    }

    self->cnt = 0;
}

void
TokSeq_deinit(
    TokSeq * self
) {
    TokSeq_clear(self);
    MutBuf_deinit(&self->buf);
}

void
TokSeq_free(
    TokSeq * self
) {
    TokSeq_deinit(self);
    Mem_free(self);
}