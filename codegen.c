#include "9cc.h"

Node *code[100];
unsigned long int label_num = 0;

/* 変数のアドレスを計算してスタックにプッシュする */
void gen_lval(Node *node) {
  if (node->kind != ND_LVAR) error("代入の左辺値が変数ではありません");
  printf("#---gen_lvar\n");
  printf("  mov rax, rbp\n");
  printf("  sub rax, %d\n", node->offset);
  printf("  push rax\n");
}

void gen(Node *node) {
    if (node == NULL) {
       return;
    }
    unsigned long int label;
    switch (node->kind) {
    case ND_NUM:
      printf("  push %d\n", node->val);
      return;
    case ND_LVAR:
      printf("#---ND_LVAR_START\n");
      printf("#---ND_LVAR_GEN_LVAR_START\n");
      gen_lval(node);
      printf("#---ND_LVAR_GEN_LVAR_END\n");
      printf("  pop rax\n");
      printf("  mov rax, [rax]\n");
      printf("  push rax\n");
      printf("#---ND_LVAR_END\n");
      return;
    case ND_ASSIGN:
      printf("#---assign start\n");
      gen_lval(node->lhs);
      gen(node->rhs);

      printf("  pop rdi\n");
      printf("  pop rax\n");
      printf("  mov [rax], rdi\n");
      printf("  push rdi\n");
      printf("#---assign end\n");
      return;
    case ND_RETURN:
      printf("#--return\n");
      gen(node->lhs);
      printf("  pop rax\n");
      printf("  mov rsp, rbp\n");
      printf("  pop rbp\n");
      printf("  ret\n");
      return;
    case ND_IF:
      gen(node->lhs);
      label = label_num++;
      switch (node->rhs->kind) {
          case ND_ELSE:
              printf("  pop rax\n");
              printf("  push rax\n"); // while loopを1回も実行しないとstackframeがずれる
              printf("  cmp rax, 0\n");
              printf("  je .Lelse%ld\n", label);
              gen(node->rhs->lhs);
              printf("  jmp .Lend%ld\n", label);
              printf(".Lelse%ld:\n", label);
              gen(node->rhs->rhs);
              printf(".Lend%ld:\n", label);
              return;
          default:
              printf("  pop rax\n");
              printf("  push rax\n"); // while loopを1回も実行しないとstackframeがずれる
              printf("  cmp rax, 0\n");
              printf("  je .Lend%ld\n", label);
              gen(node->rhs);
              printf(".Lend%ld:\n", label);
              return;
      }
    case ND_WHILE:
      label = label_num++;
      printf(".Lbegin%ld:\n", label);
      printf("#begin while cond\n");
      gen(node->lhs);
      printf("#end while cond\n" );
      printf("  pop rax\n");
      printf("  push rax\n"); // while loopを1回も実行しないとstackframeがずれる
      printf("  cmp rax, 0\n");
      printf("  je .Lend%ld\n", label);
      printf("#begin while do\n" );
      gen(node->rhs);
      printf("#end while do\n");
      printf("  jmp .Lbegin%ld\n", label);
      printf(".Lend%ld:\n", label);
//      printf("  push 1\n");
      return;
    case ND_FOR:
        label = label_num++;
        printf("#begin for1\n");
        gen(node->lhs);
        printf("#end for1\n");
        printf(".Lbegin%ld:\n", label);
        if (node->rhs->lhs != NULL) {
          printf("#begin for2\n");
          gen(node->rhs->lhs);
          printf("#end for2\n");
          printf("  pop rax\n");
          printf("  cmp rax, 0\n");
          printf("  je .Lend%ld\n", label);
        }
        printf("#begin for stmt\n");
        gen(node->rhs->rhs->lhs);
        printf("#end for stmt\n");
        printf("#begin for3\n");
        gen(node->rhs->rhs->rhs);
        printf("#end for3\n");
        printf("  jmp .Lbegin%ld\n", label);
        printf(".Lend%ld:\n", label);
      return;
    }

  gen(node->lhs);
  gen(node->rhs);

  printf(" pop rdi\n");
  printf(" pop rax\n");

  switch (node->kind) {
    case ND_ADD:
      printf("  add rax, rdi\n");
      break;
    case ND_SUB:
      printf("  sub rax, rdi\n");
      break;
    case ND_MUL:
      printf(" imul rax, rdi\n");
      break;
    case ND_DIV:
      printf("  cqo\n");
      printf("  idiv rdi\n");
      break;
    case ND_EQ:
      printf("  cmp rax, rdi\n");
      printf("  sete al\n");
      printf("  movzb rax, al\n");
      break;
    case ND_NE:
      printf("  cmp rax, rdi\n");
      printf("  setne al\n");
      printf("  movzb rax, al\n");
      break;
    case ND_LT:
      printf("  cmp rax, rdi\n");
      printf("  setl al\n");
      printf("  movzb rax, al\n");
      break;
    case ND_LE:
      printf("  cmp rax, rdi\n");
      printf("  setle al\n");
      printf("  movzb rax, al\n");
      break;
  }
  printf("  push rax\n");
}

void code_gen() {
  printf(".intel_syntax noprefix\n");
  printf(".global main\n");
  printf("main:\n");

  printf("  push rbp\n");
  printf("  mov rbp, rsp\n");
  if (locals) printf("  sub rsp, %d\n", locals->offset);

  for (int i = 0; code[i]; i++) {
    gen(code[i]);

    printf("# before pop rax%d\n",i);
    printf("  pop rax\n");
  }

  printf("  mov rsp, rbp\n");
  printf("  pop rbp\n");
  printf("  ret\n");
}
