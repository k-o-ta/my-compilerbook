#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
  TK_RESERVED,
  TK_IDENT,
  TK_NUM,
  TK_EOF,
} TokenKind;

typedef struct Token Token;
struct Token {
  TokenKind kind;
  Token *next;
  int val;
  char *str;
  int len;
};

typedef struct Type Type;
struct Type {
  enum { INT, PTR } ty;
  struct Type *ptr_to;
};

typedef struct LVar LVar;
struct LVar {
  LVar *next;
  char *name;
  int len;
  int offset;
  Type *type;
};

void error(char *fmt, ...);
void error_at(char *loc, char *fmt, ...);
Token *tokenize(char *p);

extern char *user_input;
extern Token *token;
extern LVar *locals;

typedef enum {
  ND_ADDR,
  ND_DEREF,
  ND_ADD,
  ND_SUB,
  ND_MUL,
  ND_DIV,
  ND_ASSIGN,
  ND_LVAR,
  ND_EQ,
  ND_NE,
  ND_LT,
  ND_LE,
  ND_NUM,
  ND_RETURN,
  ND_IF,
  ND_ELSE,
  ND_WHILE,
  ND_FOR,
  ND_BLOCK,
  ND_CALL,
  ND_ARG,
  ND_FUNC,
  ND_VARGS,
} NodeKind;


typedef struct Node Node;
struct Node {
  NodeKind kind;
  Node *lhs;
  Node *rhs;
  int val;
  int offset; // ND_LVARのときだけ
  Type *type; // ND_LVARのときだけ
  int block_count; // ND_BLOCKのときだけ
  char *func_name; // ND_CALL, ND_FUNCのときだけ
  int args_num; // ND_CALL, ND_FUNCのときだけ
};

extern Node *code[100];

void program();
Node *expr();

void code_gen();
void gen(Node *node);
