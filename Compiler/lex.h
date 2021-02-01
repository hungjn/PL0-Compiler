#ifndef __LEX_H
#define __LEX_H

typedef struct
{
  char lexeme[11]; // lexeme
  int token; // token type value
} list;

int checkSpecial (char c, const char *table[]);
int checkReserved (char *buffer, const char *table[]);
int tokenValue (char *buffer);
list* lex_analyze(char *input, int inputCount, int *listCount, int *error);


#endif
