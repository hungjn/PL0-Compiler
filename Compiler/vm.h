#ifndef __VM_H
#define __VM_H

#include "codegen.h"

typedef struct trace{
  int instructionNum;
  char instructionName[3];
  int l;
  int m;
  int pc;
  int bp;
  int sp;
  char stackTrace[1000];
}trace;

void virtual_machine(instruction *code, int *tracePrintCount, trace *tracePrint);
int base (int l, int base, int *stack);

#endif
