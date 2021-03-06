#include "asm.h"
#include <string.h>

int64_t asm_add(int64_t a, int64_t b) {//返回有符号64位加法
  asm("add %1, %0;"
      : "+r"(a)//%0
      : "r"(b)//%1
  );
  return a;
}

int asm_popcnt(uint64_t x) {//无符号64位整数x二进制表示中1的数量
  int s = 0;
 //先使用 gcc -S 将c代码转换至汇编代码再写内联汇编
  asm("push %%rbp;"
      "mov %%rsp, %%rbp;"
      "mov %1, -24(%%rbp);"//mov %rdi -24(%rbp) rdi寄存器中存储x 
      "movl $0, -4(%%rbp);"
      "jmp L2;"
      "L4: mov -4(%%rbp), %%eax;"//eax 存储 i
      "mov -24(%%rbp), %%rdx;" //rdx 存储 x
      "mov %%eax, %%ecx;"//ecx存储i
      "shr %%cl, %%rdx;" //逻辑右移
      "mov %%rdx, %%rax;" 
      "and $1, %%eax;"
      "test %%rax, %%rax;"
      "je L3;"
      "addl $1, %0;"
      "L3: addl $1, -4(%%rbp);"
      "L2: cmpl $63, -4(%%rbp);"
      "jle L4;"
      "pop %%rbp;"
      : "+r"(s)
      : "r"(x)
      : "%rax", "%eax", "%ecx","%rdx","%cl"

  );
  return s;
}

void *asm_memcpy(void *dest, const void *src, size_t n) {
  //return memcpy(dest, src, n);         
  asm("mov %0, %%rdi;"//dest
      "mov %1, %%rsi;"//src
      "jmp mem1;"
      "mem2: sub $1, %2;"
      "movzbl (%%rsi), %%edx;"
      "mov %%dl, (%%rdi);"
      "addq $1, %%rdi;"
      "addq $1, %%rsi;"
      "mem1: mov %2, %%rax;"
      "test %%rax, %%rax;"
      "jne mem2;"
      :
      : "r"(dest), "r"(src), "r"(n)
      : "%rax","%rdx","%rdi","%rsi","%dl"
  );
  return dest;
}

int asm_setjmp(asm_jmp_buf env) {
  //return setjmp(env);
  asm(
      "mov %0, %%rdi;"
      "mov %%rbx, (%%rdi);"
      "mov %%rbp, %%rax;"
      "mov %%rax, 8(%%rdi);"
      "mov %%r12, 16(%%rdi);"
      "mov %%r13, 24(%%rdi);"
      "mov %%r14, 32(%%rdi);"
      "mov %%r15, 40(%%rdi);"
      "lea 8(%%rsp), %%rdx;"
      "mov %%rdx, 48(%%rdi);"
      "mov (%%rsp), %%rax;"
      "mov %%rax, 56(%%rdi);" 
      :
      : "m"(env)
      : "%rax", "%rcx", "%rdi", "%rsi", "memory","%rdx"
  );
  return 0;
}

void asm_longjmp(asm_jmp_buf env, int val) {
  //longjmp(env, val);
  asm("mov %0, %%rdi;"
      "mov 48(%%rdi), %%r8;"
      "mov 8(%%rdi), %%r9;"
      "mov 56(%%rdi), %%rdx;"
      "mov (%%rdi), %%rbx;"
      "mov 16(%%rdi), %%r12;"
      "mov 24(%%rdi), %%r13;"
      "mov 32(%%rdi), %%r14;"
      "mov 40(%%rdi), %%r15;"
      //"mov %%esi, %%eax;"
      "mov %1, %%rax;"
      "mov %%r8, %%rsp;"
      "mov %%r9, %%rbp;"
      "jmpq *%%rdx;" 
      :
      : "m"(env), "m"(val)
      :  "%rdi", "memory","%rdx"
  );
}