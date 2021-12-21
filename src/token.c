//
// Created by koji on 9/19/21.
//
#include "9cc.h"

Token *new_token(TokenKind kind, Token *cur, char *str, int len) {
    Token *tok = calloc(1, sizeof(Token));
    tok->kind = kind;
    tok->str = str;
    tok->len = len;
    cur->next = tok;
    return tok;
}

bool startswith(char *p, char *q) { return memcmp(p, q, strlen(q)) == 0; }
int is_alnum_or_u_bar(char c) { return (isalnum(c) || c == '-'); }

Token *tokenize(char *p) {
    Token head;
    head.next = NULL;
    Token *cur = &head;

    while (*p) {
        if (isspace(*p)) {
            p++;
            continue;
        }
        if (startswith(p, "return") && !is_alnum_or_u_bar(p[6])) {
            cur = new_token(TK_RESERVED, cur, p, 6);
            p += 6;
            continue;
        }

        if (startswith(p, "if") && !is_alnum_or_u_bar(p[2])) {
            cur = new_token(TK_RESERVED, cur, p, 2);
            p += 2;
            continue;
        }

        if (startswith(p, "else") && !is_alnum_or_u_bar(p[4])) {
            cur = new_token(TK_RESERVED, cur, p, 4);
            p += 4;
            continue;
        }

        if (startswith(p, "while") && !is_alnum_or_u_bar(p[5])) {
            cur = new_token(TK_RESERVED, cur, p, 5);
            p += 5;
            continue;
        }

        if (startswith(p, "for") && !is_alnum_or_u_bar(p[3])) {
            cur = new_token(TK_RESERVED, cur, p, 3);
            p += 3;
            continue;
        }

        if (startswith(p, "==") || startswith(p, "!=") || startswith(p, "<=") ||
            startswith(p, ">=")) {
            cur = new_token(TK_RESERVED, cur, p, 2);
            p += 2;
            continue;
        }

       if (startswith(p, "int") && !is_alnum_or_u_bar(p[3])) {
         cur = new_token(TK_RESERVED, cur, p, 3);
         p += 3;
         continue;
       }

       if (startswith(p, "sizeof") && !is_alnum_or_u_bar(p[6])) {
         cur = new_token(TK_SIZEOF, cur, p, 6);
         p += 6;
         continue;
       }

        if (strchr("=+-*/()<>{};,&", *p)) {
            cur = new_token(TK_RESERVED, cur, p++, 1);
            continue;
        }

        if (isdigit(*p)) {
            cur = new_token(TK_NUM, cur, p, 0);
            char *q = p;
            cur->val = strtol(p, &p, 10);
            cur->len = p - q;
            continue;
        }

        if (isalpha(*p)) {
            char *q = p++;
            while (isalnum(*p)) p++;
            cur = new_token(TK_IDENT, cur, q, p - q);
            continue;
        }

        error_at(p, "トークナイズできません");
    }
    new_token(TK_EOF, cur, p, 0);
    return head.next;
}
