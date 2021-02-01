// Hung Nguyen
// Nolan Schafer

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lex.h"
#include "parser.h"
#include "codegen.h"

// Things to note:
// - tableCount is now the index of the last item in the symbol table, we will use it
// as a tail pointer to navigate the sentinel technique
//
// - numLines is an int declared and initialized in driver.c. We are passing the address
// of numLines to generate_code in driver.c because this will tell us how many lines of
// instruction code is generated for future vm printing loop

instruction* generate_code(symbol *table, list *lexeme, int *numLines, int *tableCount)
{
	// ifjump will be reused for the false conditionCGing of while loops
	// cx is the generated code index counter
	int ifJump, whileJump, i = 0, cx = 0;
	instruction *code = malloc(500 * sizeof(instruction));
	programCG(lexeme, table, tableCount, &i, code, &cx);
	// return cx (which is now the total number of lines of assembly code) to driver for printing loops
	*numLines = cx;

	return code;
}

void programCG (list *lexeme, symbol *table, int *tableCount, int *i, instruction *code, int *cx)
{
	// j, k, and z are all counters for loops (Z IS NOT USED)
	// procCount counts up number of procedures
	// lexlevel acts as the current lex level during execution
	// mainIndex captures the code index of main's INC
	// pooz is a the procedureIndex we will need to store addr (RENAMED TO procedureIndex IN BLOCK DECLARATION)
	// tableCount2 is the symbol table counter that will move in parallel to parser's tableCounter to correctly locate symbols to unmark
	int j, k, z = 0, procCount = 0, lexlevel = 0, mainIndex, pooz = 0, tableCount2 = 1;

	// Count number of procedures in the symbol table
	for (j = 1; j < *tableCount + 1; j++)
	{
		if (table[j].kind == 3)
			procCount++;
	}

	// emit main's jmp with m = 0 for now
	emit(7, 0, 0, cx, code);

	// emit jmps for every procedure with m = 0 for now
	for (j = 1; j < *tableCount + 1; j++)
	{
		if (table[j].kind == 3)
			emit(7, 0, 0, cx, code);
	}

	blockCG(lexeme, table, tableCount, i, code, cx, &lexlevel, &mainIndex, &z, pooz, &tableCount2);

	// Correct main's jmp to it's first instruction (INC)
	code[0].m = mainIndex;

	// Correct all procedure's jmps for their first instructions (INC)
	k = 1;
	for (j = 1; j < *tableCount + 1; j++)
	{
		if (table[j].kind == 3)
		{
			code[k].m = table[j].addr;
			k++;
		}
	}

	emit(11, 0 , 3, cx, code); // emit SIO END
}

void blockCG (list *lexeme, symbol *table, int *tableCount, int *i, instruction *code, int *cx, int *lexlevel, int *mainIndex, int *z, int procedureIndex, int *tableCount2)
{
	// c is a looping flag for multiple constant declarations
	// v is a looping flag for multiple variable declarations
	// varCount is the total number of variables in its respective block, used for calculating m of INC
	// symCount is the total number of symbols in its respective block, used for marking the symbols of its block in the sym table (done using them at end of block)
	// check is for storing index of symbols searched
	int c = 1, v = 1, varCount = 0, symCount = 0, k, check;

	if (lexeme[*i].token == 28) // token is const
	{
		while (c == 1)
		{
			*i += 1;
			// token is now identifier
			check = positionSymbol(table, tableCount, lexeme[*i].lexeme, lexlevel, 1);

			table[*tableCount2].mark = 0;
			*i += 1;
			// token is now equalsym
			*i += 1;
			// token is now number
			*i += 1;
			// token is now comma or semicolon
			if (lexeme[*i].token != 17) // token for comma
				c = 0;
			*tableCount2 += 1;
			symCount++;
		}
		// token is now semicolon
		*i += 1;
	}

	if (lexeme[*i].token == 29) // token for var
	{
		varCount = 0;
		while (v == 1)
		{
			*i += 1;
			// token is now identifier
			check = positionSymbol(table, tableCount, lexeme[*i].lexeme, lexlevel, 2);
			table[*tableCount2].mark = 0;
			varCount++;
			symCount++;
			*tableCount2 += 1;
			*i += 1;
			// token is now comma or semicolon
			if (lexeme[*i].token != 17)	// token for comma
				v = 0;
		}
		// token is now semicolon
		*i += 1;
	}

	if (lexeme[*i].token == 30) // token for procedure
	{
		do
		{
			*i += 1;
			// token is now identifier
			check = positionSymbol(table, tableCount, lexeme[*i].lexeme, lexlevel, 3);
			table[*tableCount2].mark = 0;
			symCount++;
			*tableCount2 += 1;
			*i += 1;
			// token is now semicolon
			*i += 1;
			// token is now first token of block
			*lexlevel += 1;
			blockCG(lexeme, table, tableCount, i, code, cx, lexlevel, mainIndex, z, check, tableCount2);
			emit(2, 0, 0, cx, code);
			*i += 1;
			// token is now semicolon
			*lexlevel -= 1;
		} while (lexeme[*i].token == 30);
	}

	// Gets the code index of main's first instruction (INC)
	if (*lexlevel == 0)
		*mainIndex = *cx;

	// Stores the code index of each procedures first instruction in their addr of their symbol table position
	if (*lexlevel != 0)
		table[procedureIndex].addr = *cx;

	emit(6, 0, varCount + 4, cx, code); // INC
	statementCG(lexeme, table, tableCount, i, code, cx, lexlevel);
	markerCG(table, tableCount, symCount);
}

void statementCG (list *lexeme, symbol *table, int *tableCount, int *i, instruction *code, int *cx, int *lexlevel)
{
	int ifJump, whileJump, elseJump, check;

	if (lexeme[*i].token == 2) // token is identifier
	{
		check = positionClosest(table, tableCount, lexeme[*i].lexeme, lexlevel);
		*i += 2;
		expressionCG(lexeme, table, tableCount, i, code, cx, lexlevel);
		emit(4, *lexlevel - table[check].level, table[check].addr, cx, code);
	}

	else if (lexeme[*i].token == 27) // token is call
	{
		*i += 1;
		// token is now procedure identifier
		check = positionClosest(table, tableCount, lexeme[*i].lexeme, lexlevel);

		*i += 1;

		emit(5, *lexlevel - table[check].level, table[check].addr, cx, code);
	}

	else if (lexeme[*i].token == 21) // token is begin
	{
		*i += 1;
		statementCG(lexeme, table, tableCount, i, code, cx, lexlevel);

		while (lexeme[*i].token == 18) // token is semicolon
		{
			*i += 1;
			statementCG(lexeme, table, tableCount, i, code, cx, lexlevel);
		}

		*i += 1;
	}

	else if (lexeme[*i].token == 23) // token is if
	{
		*i += 1;
		conditionCG(lexeme, table, tableCount, i, code, cx, lexlevel);
		ifJump = *cx; // saving jpc location
		emit(8, 0, 0, cx, code); // EMIT JPC M = 0 for now
		*i += 1;
		statementCG(lexeme, table, tableCount, i, code, cx, lexlevel);
		if (lexeme[*i].token == 33) // token is else
		{
			*i += 1;
			elseJump = *cx; // saving jmp location
			emit(7, 0, 0, cx, code);
			code[ifJump].m = *cx; // FIX JPC
			statementCG(lexeme, table, tableCount, i, code, cx, lexlevel);
			code[elseJump].m = *cx; // FIX JMP
		}
		else
			code[ifJump].m = *cx; // FIX JPC
	}

	else if (lexeme[*i].token == 25) // token is while
	{
		*i += 1;
		whileJump = *cx;
		conditionCG(lexeme, table, tableCount, i, code, cx, lexlevel);
		*i += 1;
		ifJump = *cx;
		emit(8, 0, 0, cx, code);
		statementCG(lexeme, table, tableCount, i, code, cx, lexlevel);
		emit(7, 0, whileJump, cx, code);
		code[ifJump].m = *cx; // fixing jump

	}

	else if (lexeme[*i].token == 32) // token is read
	{
		*i += 1;
		// token is now variable identifier
		check = positionClosest(table, tableCount, lexeme[*i].lexeme, lexlevel);

		*i += 1;
		emit(10, 0, 2, cx, code); // emit SIO READ
		emit(4, *lexlevel - table[check].level, table[check].addr, cx, code); // emit STO
	}

	else if (lexeme[*i].token == 31) // token is write
	{
		*i += 1;
		expressionCG(lexeme, table, tableCount, i, code, cx, lexlevel);
		emit(9, 0, 1, cx, code); // emit SIO WRITE

	}
}

void conditionCG (list *lexeme, symbol *table, int *tableCount, int *i, instruction *code, int *cx, int *lexlevel)
{
	if (lexeme[*i].token == 8) // token is odd
	{
		*i += 1;
		expressionCG(lexeme, table, tableCount, i, code, cx, lexlevel);
		emit(2, 0, 6, cx, code); // emit OPR ODD
	}

	else
	{
		expressionCG(lexeme, table, tableCount, i, code, cx, lexlevel);
		if (lexeme[*i].token == 9) // token is =
		{
			*i += 1;
			expressionCG(lexeme, table, tableCount, i, code, cx, lexlevel);
			emit(2, 0, 8, cx, code); // emit OPR EQL
		}
		else if (lexeme[*i].token == 10) // token is <>
		{
			*i += 1;
			expressionCG(lexeme, table, tableCount, i, code, cx, lexlevel);
			emit(2, 0, 9, cx, code); // emit OPR NEQ
		}
		else if (lexeme[*i].token == 11) // token is <
		{
			*i += 1;
			expressionCG(lexeme, table, tableCount, i, code, cx, lexlevel);
			emit(2, 0, 10, cx, code); // emit OPR LSS
		}
		else if (lexeme[*i].token == 12) // token is <=
		{
			*i += 1;
			expressionCG(lexeme, table, tableCount, i, code, cx, lexlevel);
			emit(2, 0, 11, cx, code); // emit OPR LEQ
		}
		else if (lexeme[*i].token == 13) // token is >
		{
			*i += 1;
			expressionCG(lexeme, table, tableCount, i, code, cx, lexlevel);
			emit(2, 0, 12, cx, code); // emit OPR GTR
		}
		else if (lexeme[*i].token == 14) // token is >=
		{
			*i += 1;
			expressionCG(lexeme, table, tableCount, i, code, cx, lexlevel);
			emit(2, 0, 13, cx, code); // emit OPR GEQ
		}
	}
}

void expressionCG (list *lexeme, symbol *table, int *tableCount, int *i, instruction *code, int *cx, int *lexlevel)
{

	if (lexeme[*i].token == 4) // token is +
		*i += 1;
	if (lexeme[*i].token == 5) // token is -
	{
		*i += 1;
		termCG(lexeme, table, tableCount, i, code, cx, lexlevel);
		emit(2, 0, 1, cx, code); // emit OPR NEG

		while (lexeme[*i].token == 4 || lexeme[*i].token ==  5)
		{
			if (lexeme[*i].token == 4) // token is +
			{
				*i += 1;
				termCG(lexeme, table, tableCount, i, code, cx, lexlevel);
				emit(2, 0, 2, cx, code); // emit OPR ADD
			}
			if (lexeme[*i].token == 5) // token is -
			{
				*i += 1;
				termCG(lexeme, table, tableCount, i, code, cx, lexlevel);
				emit(2, 0, 3, cx, code); // emit OPR SUB
			}
		}
		return;
	}
	termCG(lexeme, table, tableCount, i, code, cx, lexlevel);

	while (lexeme[*i].token == 4 || lexeme[*i].token ==  5)
	{
		if (lexeme[*i].token == 4) // token is +
		{
			*i += 1;
			termCG(lexeme, table, tableCount, i, code, cx, lexlevel);
			emit(2, 0, 2, cx, code); // emit OPR ADD
		}
		if (lexeme[*i].token == 5) // token is -
		{
			*i += 1;
			termCG(lexeme, table, tableCount, i, code, cx, lexlevel);
			emit(2, 0, 3, cx, code); // emit OPR SUB
		}
	}
}

void termCG (list *lexeme, symbol *table, int *tableCount, int *i, instruction *code, int *cx, int *lexlevel)
{

	factorCG(lexeme, table, tableCount, i, code, cx, lexlevel);
	while (lexeme[*i].token == 6 || lexeme[*i].token == 7) // mult or slash
	{

		if (lexeme[*i].token == 6) // mult
		{
			*i += 1;
			factorCG(lexeme, table, tableCount, i, code, cx, lexlevel);
			emit(2, 0, 4, cx, code); // emit OPR MULT
		}
		if (lexeme[*i].token == 7) // slash
		{
			*i += 1;
			factorCG(lexeme, table, tableCount, i, code, cx, lexlevel);
			emit(2, 0, 5, cx, code); // emit OPR DIV
		}
	}
}

void factorCG (list *lexeme, symbol *table, int *tableCount, int *i, instruction *code, int *cx, int *lexlevel)
{
	int check2;
	if (lexeme[*i].token == 2) // token is identifier
	{
		check2 = positionClosestFactor(table, tableCount, lexeme[*i].lexeme, lexlevel);

		if (table[check2].kind == 1)// kind is constant
			emit(1, 0, table[check2].val, cx, code); // EMIT LIT

		if (table[check2].kind == 2) // kind is variable
		{
			emit(3, *lexlevel - table[check2].level, table[check2].addr, cx, code); // emit LOD
		}

		*i += 1;

	}

	else if (lexeme[*i].token == 3) // token is number
	{
		emit(1, 0, atoi(lexeme[*i].lexeme), cx, code); // emit LIT
		*i += 1;
	}
	else
	{
		*i += 1;
		expressionCG(lexeme, table, tableCount, i, code, cx, lexlevel);
		*i += 1;
	}
}

void emit (int op, int l, int m, int *cx, instruction *code)
{

	if (*cx > 500)
		printf("Error #, too much code generated\n");
	else
	{
		code[*cx].op = op;
		code[*cx].l = l;
		code[*cx].m = m;
		*cx += 1;
	}
}
