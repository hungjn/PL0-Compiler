// Hung Nguyen
// Nolan Schafer

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"
#include "lex.h"

// Things to note:
// - tableCount was an int declared and initialized to 1 (leave 0th index for sentinel technique)
//   in driver.c. We gave parse the address of tableCount because it will act as the index counter
//   for the symbol table and eventually end as the index of the last item in the sym table.
//
// - error was an int declared and initialized to 0 in driver.c. We gave parse the address of error
//   because if error is ever set to 1, we must termPinate all execution and return to driver.c.
//
// - 	if (*error == 1)  is placed after the return of every function call for immediate termination from error detection
//				return;

// TYPES OF ERROR:

// Error 1, Period expected
// error 2, const, var or procedure must be followed by identifier
// Error 3, double declaration within the same scope and level is not allowed
// Error 4, constant identifier must be followed by =
// Error 5, = must be followed by a number
// Error 6, Semicolon or comma missing
// Error 7, undeclared identifier
// Error 8, assignment to constant or procedure is not allowed
// Error 9, assignment operator expected
// Error 10, end expected
// Error 11, then expected
// Error 12, do expected
// Error 13, read must be followed by variable type identifier
// Error 14, call sym must be followed by a proc identifier
// Error 15, Relational operator expected
// Error 16, Right parenthesis missing
// Error 17, identifier, number or ( expected in factor
// Error 18, used = instead of :=
// Error 19, There should be nothing after period
// Error 20, identifier expected after call
// Error 21, use of procedure identifier in factor


int positionSymbol (symbol *table, int *tableCount, char *name, int *lexlevel, int kind)
// Searches the symbol table for matching name/kind/level, if not found will return 0
{
	int count = *tableCount;
	strcpy(table[0].name, name);
	table[0].mark == 0;
	table[0].level = *lexlevel;
	table[0].kind = kind;

	while (1)
	{
		if (strcmp(table[count].name, name) == 0 && table[count].kind == kind && table[count].level == *lexlevel)
		{
			return count;
		}
		count -= 1;
	}
}

//           symbol table ,index of last item , name searching for
//                 v                v                v
int position2 (symbol *table, int *tableCount, char *name)
// position returns the index of where name is found, if 0 then it's not in the sym table
{
	// input the name and mark into the first element of the table
	// set count to last index of table array to work backwards
	int count = *tableCount;
	strcpy(table[0].name, name);
	table[0].mark == 0;

	// will continue to loop throug until aomthing is matched this wont create a infinite loop
	// since we put the ident in the first element so it will always find it just weather
	// its the one we just inputed or it was already in the table
	while (1)
	{
		// check to see if the names match and its unmarked
		if (strcmp(table[count].name, name) == 0 && table[count].mark == 0)
			return count; // this will couse it to leave the function

		count -= 1;
	}
}

int positionSameScope (symbol *table, int *tableCount, char *name, int *lexlevel)
// position returns the index of where name is found, if 0 then it's not in the sym table
{
	// input the name, level and mark into the first element of the table
	// set count to last index of table array to work backwards
	int count = *tableCount;
	strcpy(table[0].name, name);
	table[0].mark = 0;
	table[0].level = *lexlevel;

	// will continue to loop throug until aomthing is matched this wont create a infinite loop
	// since we put the ident in the first element so it will always find it just weather
	// its the one we just inputed or it was already in the table
	while (1)
	{
		// check the name match if its unmarked and if its at the right lex level

		if (strcmp(table[count].name, name) == 0 && table[count].mark == 0 && table[count].level == (*lexlevel))
			return count; // this will couse it to leave the function

		count -= 1;
	}
}

// can't be index 0 in codegen
int positionClosest (symbol *table, int *tableCount, char *name, int *lexlevel)
{
	// input the name, level and mark into the first element of the table
	// set count to last index of table array to work backwards
	// set closest level to big number so that the first comparison it would be the biggest
	int count = *tableCount, i, closestIndex, closestLevel = 900, levelHold;
	strcpy(table[0].name, name);
	table[0].mark = 0;
	table[0].level = *lexlevel;

	// i is increasing but because it is table[count - i] we are still working backwards
	// but we make sure not to get to the first element which is where we put the copy of what we are looking for
	for (i = 0; i < count; i++)
	{
		// if the idnt matches with somthing unmarked in the table
		if (strcmp(table[count - i].name, name) == 0 && table[count - i].mark == 0)
		{
			// we check it level to see the diffrence
			// if the level in the table is less than when we find the diffrence it will be negative so we want to
			// * by -1 to make positive and set them both into and holder int levelHold

			if (table[count - i].level - *lexlevel < 0)
				levelHold = (table[count - i].level - *lexlevel) * (-1);

			else
				levelHold = table[count - i].level - *lexlevel;

			// here we check to see if the level hold is the less than closestLevel if it is we make closestLevel what levelHold is
			// the first interation levelHold will be less than closestLevel
			// if the name is unmared in the table multiple times it will find which one has the lest diffrence betweeen lex levels
			if (levelHold < closestLevel)
			{
				closestLevel = levelHold;
				closestIndex = count - i;
			}

		}
	}

	return closestIndex;
}

// Calculates the index of the symbol that is matches name/mark and closest in level to the current level
// (same as positionClosest, except the symbol cannot be a procedure identifier)
int positionClosestFactor (symbol *table, int *tableCount, char *name, int *lexlevel)
{
	// input the name, level and mark into the first element of the table
	// set count to last index of table array to work backwards
	// set closest level to big number so that the first comparison it would be the biggest
	int count = *tableCount, i, closestIndex, closestLevel = 900, levelHold;
	strcpy(table[0].name, name);
	table[0].mark = 0;
	table[0].level = *lexlevel;

	// i is increasing but because it is table[count - i] we are still working backwards
	// but we make sure not to get to the first element which is where we put the copy of what we are looking for
	for (i = 0; i < count; i++)
	{
		// if the idnt matches with somthing unmarked and isnt a proc and in the table
		if (strcmp(table[count - i].name, name) == 0 && table[count - i].mark == 0 && table[count - i].kind != 3)
		{
			// we check it level to see the diffrence
			// if the level in the table is less than when we find the diffrence it will be negative so we want to
			// * by -1 to make positive and set them both into and holder int levelHold

			if (table[count - i].level - *lexlevel < 0)
				levelHold = (table[count - i].level - *lexlevel) * (-1);

			else
				levelHold = table[count - i].level - *lexlevel;

			// here we check to see if the level hold is the less than closestLevel if it is we make closestLevel what levelHold is
			// the first interation levelHold will be less than closestLevel
			// if the name is unmared in the table multiple times it will find which one has the lest diffrence betweeen lex levels
			if (levelHold < closestLevel)
			{
				closestLevel = levelHold;
				closestIndex = count - i;
			}

		}
	}

	return closestIndex;
}



// Marks the last n (symCount) number of unmarked symbols in parser
void markerP (symbol *table, int *tableCount, int symCount)
{
	// Notice that count must start as tableCount - 1 because in parser tableCount will be at the index after the last element
	int i = 0, marked = 0, count = *tableCount - 1;
	while (marked != symCount)
	{
		// starts back to front to get the most recent imputs
		if (table[count - i].mark == 1) // if the symbol is already marked, skip it
		{
			i++;
			continue;
		}
		else
		{
			table[count - i].mark = 1;
			i++;
			marked++;
		}
	}
}

// Marks the last n (symCount) number of unmarked symbols in codegen
void markerCG (symbol *table, int *tableCount, int symCount)
{
	// In codegen tableCount should be the index of the last symbol in the symbol table
	int i = 0, marked = 0, count = *tableCount;
	while (marked != symCount)
	{
		if (table[count - i].mark == 1) // if the symbol is already marked, skip it
		{
			i++;
			continue;
		}
		else
		{
			table[count - i].mark = 1;
			i++;
			marked++;
		}
	}
}

// this is like the starting function
// also callocs the symbole tables memory
symbol* parse(list *lexeme, int *error, int *tableCount)
{
	// i is the lex list counter
	int i = 0;
	symbol *table = calloc(500, sizeof(symbol));

	programP(lexeme, table, tableCount, &i, error);

	return table;
}

// starts off the recursive stack by making first call to block
// also checks to make sure there is a .  at the end of the program and nothing else
void programP (list *lexeme, symbol *table, int *tableCount, int *i, int *error)
{
	int lexlevel = 0;
	blockP(lexeme, table, tableCount, i, error, &lexlevel);
	if (*error == 1)
		return;
	if (lexeme[*i].token != 19) // period token
	{
		printf("Error 1, Period expected\n");
		*error = 1;
		return;
	}
	*i += 1;
	if (lexeme[*i].token != 0) // if there is anything after the .
	{
		printf("Error 19, There should be nothing after period\n");
		*error = 1;
		return;
	}
}


void blockP (list *lexeme, symbol *table, int *tableCount, int *i, int *error, int *lexlevel)
{
	// c is a looping flag for multiple constant declarations
	// v is a looping flag for multiple variable declarations
	// varCount is a counter for the number of variables in its respective block, used for storing a variable at the correct addr
	// symCount is the total number of symbols in its respective block, used for marking the symbols of its block in the sym table (done using them at end of block)
	int c = 1, v = 1, indexCheck, symCount = 0, varCount = 0;
	// this char array is used to temp store the name of a ident before we input to sym table
	char *tempIdentName = calloc(20, sizeof(char)); // holds idents for confirmation before storing in sym table

	if (lexeme[*i].token == 28) // token is const
	{
		while (c == 1)
		{
			*i += 1;
			if (lexeme[*i].token != 2) // token for identifier
			{
				printf("Error 2, const, var or procedure must be followed by identifier\n");
				*error = 1;
				return;
			}
			// store ident name because we cant input it into the sym table
			// until we know it has an associated value
			strcpy(tempIdentName, lexeme[*i].lexeme);
			// check to see if this ident is already in the table
			indexCheck = positionSameScope(table, tableCount, lexeme[*i].lexeme, lexlevel);
			if (indexCheck != 0)
			{
				printf("Error 3, double declaration within same scope and level not allowed\n");
				*error = 1;
				return;
			}
			*i += 1;
			if (lexeme[*i].token != 9)	// token for equalsym
			{
				printf("Error 4, constant identifier must be followed by =\n");
				*error = 1;
				return;
			}
			*i += 1;
			if (lexeme[*i].token != 3) // token for numbers
			{
				printf("Error 5, = must be followed by a number\n");
				*error = 1;
				return;
			}
			// now we can add this constant to the sym table because it has a value
			strcpy(table[*tableCount].name, tempIdentName);
			table[*tableCount].kind = 1;
			table[*tableCount].val = atoi(lexeme[*i].lexeme);
			table[*tableCount].level = *lexlevel;
			table[*tableCount].mark = 0;
			*i += 1;
			if (lexeme[*i].token != 17) // token for comma
				c = 0;
			*tableCount += 1;
			symCount++;
		}
		if (lexeme[*i].token != 18) // token for semicolon
		{
			printf("Error 6, Semicolon or comma missing\n");
			*error = 1;
			return;
		}
		*i += 1;
	}

	if (lexeme[*i].token == 29) // token for var
	{
		varCount = 0;
		while (v == 1)
		{
			*i += 1;
			if (lexeme[*i].token != 2) // token for ident
			{
				printf("Error 2, const, var or procedure must be followed by identifier\n");
				*error = 1;
				return;
			}
			// this checks to make sure the ident isnt all ready in the table
			indexCheck = positionSameScope(table, tableCount, lexeme[*i].lexeme, lexlevel);
			if (indexCheck != 0)
			{
				printf("Error 3, double declaration within same scope and level not allowed\n");
				*error = 1;
				return;
			}
			strcpy(table[*tableCount].name, lexeme[*i].lexeme);
			table[*tableCount].kind = 2;
			table[*tableCount].addr = varCount + 4;
			table[*tableCount].level = *lexlevel;
			table[*tableCount].mark = 0;
			*i += 1;
			if (lexeme[*i].token != 17)	// token for comma
				v = 0;
			*tableCount += 1;
			symCount++;
			varCount++;
		}
		if (lexeme[*i].token != 18) // token for semicolon
		{
			printf("Error 6, Semicolon or comma missing\n");
			*error = 1;
			return;
		}
		*i += 1;
	}

	if (lexeme[*i].token == 30) // token for procedure
	{
		do
		{
			*i += 1;
			if (lexeme[*i].token != 2) // token is identifier
			{
				printf("Error 2, const, var or procedure must be followed by identifier\n");
				*error = 1;
				return;
			}
			indexCheck = positionSameScope(table, tableCount, lexeme[*i].lexeme, lexlevel);
			if (indexCheck != 0)
			{
				printf("Error 3, double declaration within same scope and level not allowed\n");
				*error = 1;
				return;
			}
			strcpy(table[*tableCount].name, lexeme[*i].lexeme);
			table[*tableCount].kind = 3;
			table[*tableCount].level = *lexlevel;
			table[*tableCount].mark = 0;
			table[*tableCount].addr = 0;
			table[*tableCount].val = 0;
			*tableCount += 1;
			symCount++;
			*i += 1;
			if (lexeme[*i].token != 18) // token is semicolon
			{
				printf("Error 6, Semicolon or comma missing\n"); //fix, add new error
				*error = 1;
				return;
			}
			*i += 1;
			*lexlevel += 1;
			// recursive call to block causes a increas in lexlevel
			blockP(lexeme, table, tableCount, i, error, lexlevel);
			if (*error == 1)
				return;
			if (lexeme[*i].token != 18) // token is semicolon
			{
				printf("Error 6, Semicolon or comma missing\n"); //fix, add new error
				*error = 1;
				return;
			}
			// once you get back from recursive call you need to decrease lexlevel
			*i += 1;
			*lexlevel -= 1;
		} while (lexeme[*i].token == 30);
	}
	statementP(lexeme, table, tableCount, i, error, lexlevel);
	if (*error == 1)
		return;

	markerP(table, tableCount, symCount);
}

// statement kinda tells what the first token of a line is and determins what should be after it
void statementP (list *lexeme, symbol *table, int *tableCount, int *i, int *error, int *lexlevel)
{
	// used to check what index of the symbol table a ident is at
	int indexCheck2;

	if (lexeme[*i].token == 2) // token is identifier, should be a variable type
	{
		indexCheck2 = position2(table, tableCount, lexeme[*i].lexeme);
		if (indexCheck2 == 0)
		{
			printf("Error 7, undeclared identifier\n");
			*error = 1;
			return;
		}
		if (table[indexCheck2].kind != 2)
		{
			printf("Error 8, assignment to constant or procedure is not allowed\n");
			*error = 1;
			return;
		}
		if (table[indexCheck2].mark == 1)
		{
			printf("Error 7, undeclared identifier\n");// fix
			*error = 1;
			return;
		}
		*i += 1;
		if (lexeme[*i].token == 20) // token is becomes
		{
			*i += 1;
			expressionP(lexeme, table, tableCount, i, error);
			if (*error == 1)
				return;
		}
		else if (lexeme[*i].token == 9) // token is =
		{
			printf("Error 18, used = instead of :=\n");
			*error = 1;
			return;
		}
		else
		{
			printf("Error 9, assignment operator expected\n");
			*error = 1;
			return;
		}
	}

	else if (lexeme[*i].token == 27) // token is call
	{
		*i += 1;
		if (lexeme[*i].token != 2) // token is identifier
		{
			printf("Error 20, identifier expected after call\n");
			*error = 1;
			return;
		}
		indexCheck2 = position2(table, tableCount, lexeme[*i].lexeme);
		if (indexCheck2 == 0)
		{
			printf("Error 7, undeclared identifier\n");
			*error = 1;
			return;
		}
		if (table[indexCheck2].mark == 1)
		{
			printf("Error 7, undeclared identifier\n");// fix
			*error = 1;
			return;
		}
		if (table[indexCheck2].kind != 3)
		{
			printf("Error 14, call sym must be followed by a proc identifier\n");
			*error = 1;
			return;
		}
		*i += 1;
	}

	else if (lexeme[*i].token == 21) // token is begin
	{
		*i += 1;
		statementP(lexeme, table, tableCount, i, error, lexlevel);
		if (*error == 1)
			return;

		while (lexeme[*i].token == 18 || lexeme[*i].token == 33) // token is semicolon or else
		{
			*i += 1;
			statementP(lexeme, table, tableCount, i, error, lexlevel);
			if (*error == 1)
				return;
		}

		if (lexeme[*i].token == 22) // token is end
			*i += 1;
		else
		{
			printf("Error 10, end expected\n");
			*error = 1;
			return;
		}
	}

	else if (lexeme[*i].token == 23) // token is if
	{
		*i += 1;
		conditionP(lexeme, table, tableCount, i, error);
		if (*error == 1)
			return;
		if (lexeme[*i].token == 24) // token is then
		{
			*i += 1;
			statementP(lexeme, table, tableCount, i, error, lexlevel);
			if (*error == 1)
				return;
			if (lexeme[*i].token == 33) // token is else
				statementP(lexeme, table, tableCount, i, error, lexlevel);

			if (*error == 1)
				return;
		}
		else
		{
			printf("Error 11, then expected\n");
			*error = 1;
			return;
		}
	}

	else if (lexeme[*i].token == 25) // token is while
	{
		*i += 1;
		conditionP(lexeme, table, tableCount, i, error);
		if (*error == 1)
			return;
		if (lexeme[*i].token == 26) // token is do
		{
			*i += 1;
			statementP(lexeme, table, tableCount, i, error, lexlevel);
			if (*error == 1)
				return;
		}
		else
		{
			printf("Error 12, do expected\n");
			*error = 1;
			return;
		}
	}

	else if (lexeme[*i].token == 32) // token is read
	{
		*i += 1;
		if (lexeme[*i].token == 2) // token is identifier
		{
			indexCheck2 = position2(table, tableCount, lexeme[*i].lexeme);
			if (indexCheck2 == 0)
			{
				printf("Error 7, undeclared identifier\n");
				*error = 1;
				return;
			}
			if (table[indexCheck2].kind != 2)
			{
				printf("Error 13, read must be followed by variable type identifier\n");
				*error = 1;
				return;
			}
			if (table[indexCheck2].mark == 1)
			{
				printf("Error 7, undeclared identifier\n");
				*error = 1;
				return;
			}
			*i += 1;
		}
		else
		{
			printf("Error 13, read must be followed by variable type identifier\n");
			*error = 1;
			return;
		}
	}

	else if (lexeme[*i].token == 31) // token is write
	{
		*i += 1;
		expressionP(lexeme, table, tableCount, i, error);
		if (*error == 1)
			return;
	}
}


void conditionP (list *lexeme, symbol *table, int *tableCount, int *i, int *error)
{
	if (lexeme[*i].token == 8) // token is odd
	{
		*i += 1;
		expressionP(lexeme, table, tableCount, i, error);
		if (*error == 1)
			return;
	}
	else
	{
		expressionP(lexeme, table, tableCount, i, error);
		if (*error == 1)
			return;
		if (lexeme[*i].token == 10 || lexeme[*i].token == 12 || lexeme[*i].token == 14 || lexeme[*i].token == 9 || lexeme[*i].token == 11 || lexeme[*i].token == 13)
		{
			*i += 1;
			expressionP(lexeme, table, tableCount, i, error);
			if (*error == 1)
				return;
		}
		else
		{
			printf("Error 15, Relational operator expected\n");
			*error = 1;
			return;
		}
	}
}

void expressionP (list *lexeme, symbol *table, int *tableCount, int *i, int *error)
{
	if (lexeme[*i].token == 4 || lexeme[*i].token == 5) // token is plussym or minussym
		*i += 1;
	termP(lexeme, table, tableCount, i, error);
	if (*error == 1)
		return;
	while (lexeme[*i].token == 4 || lexeme[*i].token ==  5) // token is plussym or minussym
	{
		*i += 1;
		termP(lexeme, table, tableCount, i, error);
		if (*error == 1)
			return;
	}
}

void termP (list *lexeme, symbol *table, int *tableCount, int *i, int *error)
{
	factorP(lexeme, table, tableCount, i, error);
	if (*error == 1)
		return;
	while (lexeme[*i].token == 6 || lexeme[*i].token == 7) // token is multsym or slashsym
	{
		*i += 1;
		factorP(lexeme, table, tableCount, i, error);
		if (*error == 1)
			return;
	}
}

void factorP (list *lexeme, symbol *table, int *tableCount, int *i, int *error)
{
	int indexCheck3;

		if (lexeme[*i].token == 2) // token is identifier
		{
			indexCheck3 = position2(table, tableCount, lexeme[*i].lexeme);
			if (indexCheck3 == 0)
			{
				printf("Error 7, undeclared identifier\n");
				*error = 1;
				return;
			}
			if (table[indexCheck3].mark == 1)
			{
				printf("Error 7, undeclared identifier\n");
				*error = 1;
				return;
			}
			if (table[indexCheck3].kind == 3)
			{
				printf("Error 21, use of procedure identifier in factor\n");
				*error = 1;
				return;
			}
			*i += 1;
		}
		else if (lexeme[*i].token == 3) // token is number
			*i += 1;
		else if (lexeme[*i].token == 15) // token is (
			{
				*i += 1;
				expressionP(lexeme, table, tableCount, i, error);
				if (*error == 1)
					return;
				if (lexeme[*i].token == 16) // token is )
				*i += 1;
				else
				{
					printf("Error 16, Right parenthesis missing\n");
					*error = 1;
					return;
				}
			}

		else
		{
			printf("Error 17, identifier, number or ( expected in factor\n");
			*error = 1;
			return;
		}
}
