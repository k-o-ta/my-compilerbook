#include "9cc.h"

char *user_input;
Token *token;

void error(char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  exit(1);
}

void error_at(char *loc, char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);

  int i;
  int **x(int);
  int **y[4][5];
  int pos = loc - user_input;
  fprintf(stderr, "%s\n", user_input);
  fprintf(stderr, "%*s", pos, " ");
  fprintf(stderr, "^ ");
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  exit(1);
}

bool consume(char *op) {
  if (token->kind != TK_RESERVED || strlen(op) != token->len ||
      memcmp(token->str, op, token->len)) {
    return false;
  }
  token = token->next;
  return true;
}

Token *consume_ident() {
  if (token->kind != TK_IDENT)
    return NULL;
  Token *t = token;
  token = token->next;
  return t;
}

bool check_func_definition() {
  if (token->kind != TK_IDENT) {
    return false;
  }

  if (memcmp(token->next->str, "(", token->next->len) != 0) {
    return false;
  }

  Token *arg = token->next->next;
  while (memcmp(arg->str, ")", arg->len) != 0) {
    arg = arg->next;
  }
  if (memcmp(arg->next->str, "{", arg->next->len) != 0) {
    return false;
  }

  return true;
}


Token *consume_number() {
  if (token->kind != TK_NUM)
    return NULL;
  Token *t = token;
  token = token->next;
  return t;
}

void expect(char *op) {
  if (token->kind != TK_RESERVED || strlen(op) != token->len ||
      memcmp(token->str, op, token->len)) {
    error_at(token->str, "'%c'ではありません", op);
  }
  token = token->next;
}

bool consume_sizeof() {
  if (token->kind != TK_SIZEOF)
    return false;
  token = token->next;
  return true;
}

int expect_number() {
  if (token->kind != TK_NUM)
    error_at(token->str, "数ではありません");
  int val = token->val;
  token = token->next;
  return val;
}

bool at_eof() { return token->kind == TK_EOF; }

LVar *locals;
LVar *find_lvar(Token *tok) {
  for (LVar *var = locals; var; var = var->next)
    if (var->len == tok->len && !memcmp(tok->str, var->name, var->len))
      return var;
  return NULL;
}

Node *new_node(NodeKind kind, Node *lhs, Node *rhs) {
  Node *node = calloc(1, sizeof(Node));
  node->kind = kind;
  node->lhs = lhs;
  node->rhs = rhs;
  return node;
}

Node *new_node_num(int val) {
  Node *node = calloc(1, sizeof(Node));
  node->kind = ND_NUM;
  node->val = val;
  return node;
}

void program();
Node *definition();
Node *stmt();
Node *vargs_list(int *count);
Node *expr();
Node *assign();
Node *equality();
Node *relational();
Node *add();
Node *mul();
Node *unary();
Node *primary();
Node *argument_expression();

int sizeof_node(Node *node);

Node *code[100];
void program() {
  int i = 0;
  while (!at_eof()) {
    code[i++] = definition();
    code[i] = NULL;
  }
}
Node *definition() {
  if( consume("int")) {
    Token *token = consume_ident();
    expect("(");
    Node *left = NULL;
    int args_num = 0;
    if (!consume(")")) {
      left = vargs_list(&args_num);
      expect(")");
    }
    Node *right = stmt();
    Node *func =  new_node(ND_FUNC, left, right);
    func->func_name = (char*)malloc(sizeof(char) * sizeof(token->len));
    strncpy(func->func_name, token->str, token->len);
    func->args_num = args_num;
    return func;
  }
}

Node *stmt() {
  if (consume("if")) {
    expect("(");
    Node *if_condition = expr();
    expect(")");
    Node *if_stmt = stmt();
    if (consume("else")) {
      Node *else_stmt = stmt();
      Node *node_stmt = new_node(ND_ELSE, if_stmt, else_stmt);
      return new_node(ND_IF, if_condition, node_stmt);
    } else {
      return new_node(ND_IF, if_condition, if_stmt);
    }
  }

  if (consume("while")) {
    expect("(");
    Node *condition = expr();
    expect(")");
    Node *while_stmt = stmt();
    return new_node(ND_WHILE, condition, while_stmt);
  }

  if (consume("for")) {
    expect("(");
    // 1
    Node *first, *second, *third;
    if (consume(";")) {
      first = NULL;
    } else {
      first = expr();
      expect(";");
    }
    // 2
    if (consume(";")) {
      second = NULL;
    } else {
      second = expr();
      expect(";");
    }
    if (consume(";")) {
      third = NULL;
    } else {
      third = expr();
      expect(";");
    }
    expect(")");
    Node *for_stmt = stmt();
    Node *final = new_node(ND_FOR, third, for_stmt);
    Node *semi_final = new_node(ND_FOR, second, final);
    return new_node(ND_FOR, first, semi_final);
  }

  Node *node;

  if (consume("{")) {
    int stmt_count = 0;
    int threshold = 100;
    Node *arr = calloc(threshold, sizeof(Node));
    while (!consume("}")) {
      arr[stmt_count++] = *stmt();

      if ((stmt_count % threshold) == (threshold - 1)) {
        arr = realloc(arr, (threshold + stmt_count) * sizeof(Node));
      }
    }
    node = calloc(1, sizeof(Node));
    node->kind = ND_BLOCK;
    node->lhs = arr;
    node->block_count = stmt_count;
    return node;
  }
  if(consume("int")) {
    node = calloc(1, sizeof(Node));
    node->kind = ND_LVAR;
    Type *type = calloc(1, sizeof(Type));
    type->ty = INT;
    while(consume("*")) {
      Type *tmp = type;
      type = calloc(1, sizeof(Type));
      type->ty = PTR;
      type->ptr_to = tmp;
    }
    Token *tok = consume_ident();
    LVar *lvar = calloc(1, sizeof(LVar));
    lvar->next = locals;
    lvar->name = tok->str;
    lvar->len = tok->len;
    lvar->type = type;
    if (locals) {
      lvar->offset = locals->offset + 8;
    } else {
      lvar->offset = 8;
    }
    node->offset = lvar->offset;
    node->type = lvar->type;
    locals = lvar;

    //    expect("=");
    //    Node *node = equality();
    //    node = new_node(ND_ASSIGN, node, assign());
    expect(";");
    return node;
  }

  if (consume("return")) {
    node = calloc(1, sizeof(Node));
    node->kind = ND_RETURN;
    node->lhs = expr();
  } else {
    node = expr();
  }
  expect(";");
  return node;
}

Node *vargs_list(int *count) {
  if (consume("int")) {
    Node *node = calloc(1, sizeof(Node));
    node->kind = ND_VARGS;
    Token *tok = consume_ident();
    LVar *lvar = calloc(1, sizeof(LVar));
    lvar->next = locals;
    lvar->name = tok->str;
    lvar->len = tok->len;
    if (locals) {
      lvar->offset = locals->offset + 8;
    } else {
      lvar->offset = 8;
    }
    node->offset = lvar->offset;
    locals = lvar;

    //  Node *node = primary();
    *count = 1;

    while(consume(",")) {
      (*count)++;
      expect("int");
      node = new_node(ND_VARGS, node, NULL);
      Token *tok = consume_ident();
      LVar *lvar = calloc(1, sizeof(LVar));
      lvar->next = locals;
      lvar->name = tok->str;
      lvar->len = tok->len;
      if (locals) {
        lvar->offset = locals->offset + 8;
      } else {
        lvar->offset = 8;
      }
      node->offset = lvar->offset;
      locals = lvar;
    }
    return node;
  }
}

Node *expr() { return assign(); }

Node *assign() {
  Node *node = equality();
  if (consume("="))
    node = new_node(ND_ASSIGN, node, assign());

  return node;
}

Node *equality() {
  Node *node = relational();

  for (;;) {
    if (consume("=="))
      node = new_node(ND_EQ, node, relational());
    else if (consume("!="))
      node = new_node(ND_NE, node, relational());
    else
      return node;
  }
}

Node *relational() {
  Node *node = add();

  for (;;) {
    if (consume("<"))
      node = new_node(ND_LT, node, add());
    else if (consume("<="))
      node = new_node(ND_LE, node, add());
    else if (consume(">"))
      node = new_node(ND_LT, add(), node);
    else if (consume(">="))
      node = new_node(ND_LE, add(), node);
    else
      return node;
  }
}

Node *add() {
  Node *node = mul();
  for (;;) {
    if (consume("+"))
      node = new_node(ND_ADD, node, mul());
    else if (consume("-"))
      node = new_node(ND_SUB, node, mul());
    else
      return node;
  }
}

Node *mul() {
  Node *node = unary();
  for (;;) {
    if (consume("*"))
      node = new_node(ND_MUL, node, unary());
    else if (consume("/"))
      node = new_node(ND_DIV, node, unary());
    else
      return node;
  }
}

Node *unary() {
  if (consume_sizeof()) {
     Node *n = unary();
    return new_node_num(sizeof_node(n));
  }
  if (consume("+"))
    return unary();
  if (consume("-"))
    return new_node(ND_SUB, new_node_num(0), unary());
  if (consume("&"))
    return new_node(ND_ADDR, unary(), NULL);
  if (consume("*"))
    return new_node(ND_DEREF, unary(), NULL);
  return primary();
}

Node *primary() {
  if (consume("(")) {
    Node *node = expr();
    expect(")");
    return node;
  }
  Token *tok = consume_ident();
  if (tok) {
    Node *node = calloc(1, sizeof(Node));
    node->kind = ND_LVAR;

    if (consume("(")) {
      node->kind = ND_CALL;
      node->func_name = (char*)malloc(sizeof(char) * sizeof(tok->len));
      strncpy(node->func_name, tok->str, tok->len);
      int args_num = 0;
      if (!consume(")")) {
        node->lhs = argument_expression(&args_num);
        expect(")");
      }
      node->args_num = args_num;
      return node;
    }

    LVar *lvar = find_lvar(tok);
    if (lvar) {
      node->offset = lvar->offset;
      node->type = lvar->type;
    } else {
      error_at(token->str, "変数が見つかりません");
    }
    return node;
  }

  return new_node_num(expect_number());
}

Node *argument_expression(int *args_num) {
  Node *node = relational();
  (*args_num)++;
  for(;;) {
    if (consume(",")) {
      (*args_num)++;
      node = new_node(ND_ARG, node, relational());
    } else {
      return node;
    }
  }
}

int sizeof_node(Node *node) {
 switch (node->kind) {
 case ND_ADDR:
   return 4;
 case ND_DEREF:
   return sizeof_node(node->lhs);
 case ND_ADD:
 case ND_SUB:
 case ND_MUL:
 case ND_DIV: {
   int left = sizeof_node(node->lhs);
   int right = sizeof_node(node->lhs);
   return ((left >= right)? left : right);
 }
 case ND_ASSIGN:
   return sizeof_node(node->rhs);
 case ND_LVAR: {
   switch (node->type->ty) {
   case INT:
     return 4;
   case PTR:
     return 8;
   default:
     error_at(token->str, "型のサイズが未実装です");
   }
 }
 case ND_EQ:
 case ND_NE:
 case ND_LT:
 case ND_LE:
     return 4;
 case ND_NUM:
   return 4;
 case ND_CALL:
   error_at(token->str, "not implemented yet");
 default:
   error_at(token->str, "sizeof演算子に入れることができません");
// case ND_RETURN:
// case ND_IF:
// case ND_ELSE:
// case ND_WHILE:
// case ND_FOR:
// case ND_BLOCK:
// case ND_ARG:
// case ND_FUNC:
// case ND_VARGS:
 }
}