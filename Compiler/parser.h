#ifndef __PARSER_H
#define __PARSER_H

#include "lex.h"

typedef struct symbol
{
	int kind;
	char name[12];
	int val;
	int level;
	int addr;
	int mark;

} symbol;

int positionSymbol (symbol *table, int *tableCount, char *name, int *lexlevel, int kind);
int positionSameScope (symbol *table, int *tableCount, char *name, int *lexlevel);
int positionClosest (symbol *table, int *tableCount2, char *name, int *lexlevel);
int positionClosestFactor (symbol *table, int *tableCount, char *name, int *lexlevel);
int position2 (symbol *table, int *tableCount, char *name);
void markerP (symbol *table, int *tableCount, int symCount);
void markerCG (symbol *table, int *tableCount, int symCount);
symbol* parse(list *lexeme, int *error, int *tableCount);
void programP (list *lexeme, symbol *table, int *index, int *i, int *error);
void statementP (list *lexeme, symbol *table, int *index, int *i, int *error, int *lexlevel);
void blockP (list *lexeme, symbol *table, int *index, int *i, int *error, int *lexlevel);
void conditionP (list *lexeme, symbol *table, int *index, int *i, int *error);
void expressionP (list *lexeme, symbol *table, int *index, int *i, int *error);
void termP (list *lexeme, symbol *table, int *index, int *i, int *error);
void factorP (list *lexeme, symbol *table, int *index, int *i, int *error);

#endif
