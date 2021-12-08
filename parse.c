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

Node *code[100];
void program() {
  int i = 0;
  while (!at_eof()) {
    code[i++] = definition();
    code[i] = NULL;
  }
}
Node *definition() {
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

//  if(check_func_definition()) {
//    Token *token = consume_ident();
//    expect("(");
//    Node *left = NULL;
//    int args_num = 0;
//    if (!consume(")")) {
//      left = vargs_list(&args_num);
//      expect(")");
//    }
//    Node *right = stmt();
//    Node *func =  new_node(ND_FUNC, left, right);
//    func->func_name = (char*)malloc(sizeof(char) * sizeof(token->len));
//    strncpy(func->func_name, token->str, token->len);
//    func->args_num = args_num;
//    return func;
//  }

//  Token *tok = consume_ident();
//  if(tok) {
//    Node *left = vargs_list();
//    Node *right = stmt();
//    Node *func =  new_node(ND_FUNC, left, right);
//    return func;
//  }
//
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
  Node *node = primary();
  *count = 1;
  if(node) {
    while(consume(",")) {
      (*count)++;
      node = new_node(ND_VARGS, node, primary());
    }
  }
  return node;
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
    } else {
      lvar = calloc(1, sizeof(LVar));
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