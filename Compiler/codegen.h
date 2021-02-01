#ifndef __CODEGEN_H
#define __CODEGEN_H

#include "lex.h"
#include "parser.h"

typedef struct
{
  int op;
  int l;
  int m;

} instruction;

instruction* generate_code(symbol *table, list *lexeme, int *numLines, int *tableCount);
void programCG (list *lexeme, symbol *table, int *tableCount, int *i, instruction *code, int *cx);
void blockCG (list *lexeme, symbol *table, int *tableCount, int *i, instruction *code, int *cx, int *lexlevel, int *mainIndex, int *z, int procedureIndex, int *tableCount2);
void statementCG (list *lexeme, symbol *table, int *tableCount, int *i, instruction *code, int *cx, int *lexlevel);
void conditionCG (list *lexeme, symbol *table, int *tableCount, int *i, instruction *code, int *cx, int *lexlevel);
void expressionCG (list *lexeme, symbol *table, int *tableCount, int *i, instruction *code, int *cx, int *lexlevel);
void termCG (list *lexeme, symbol *table, int *tableCount, int *i, instruction *code, int *cx, int *lexlevel);
void factorCG (list *lexeme, symbol *table, int *tableCount, int *i, instruction *code, int *cx, int *lexlevel);
void emit (int op, int l, int m, int *cx, instruction *code);

#endif
