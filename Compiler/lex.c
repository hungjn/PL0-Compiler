// Hung Nguyen
// Nolan Schafer

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "lex.h"

// Checks if a character is a Special Symbol and returns
// the token value if yes, otherwise returns zero
int checkSpecial (char c, const char *table[])
{
  if (c == '+')
    return 4;
  if (c == '-')
    return 5;
  if (c == '*')
    return 6;
  if (c == '/')
    return 7;
  if (c == '(')
    return 15;
  if (c == ')')
    return 16;
  if (c == '=')
    return 9;
  if (c == ',')
    return 17;
  if (c == '.')
    return 19;
  if (c == '<')
    return 11;
  if (c == ':')
    return 2000;
  if (c == '>')
    return 13;
  if (c == ';')
    return 18;
  //if (c == '{')
    //return 21;
  //if (c == '}')
    //return 22;
  return 0;
}

// Checks if the buffer is a reserved word, returns the token value
// if yes, otherwise returns zero
int checkReserved (char *buffer, const char *table[])
{
  if (strcmp(buffer, "const") == 0)
    return 28;
  if (strcmp(buffer, "var") == 0)
    return 29;
  if (strcmp(buffer, "if") == 0)
    return 23;
  if (strcmp(buffer, "then") == 0)
    return 24;
  if (strcmp(buffer, "while") == 0)
    return 25;
  if (strcmp(buffer, "do") == 0)
    return 26;
  if (strcmp(buffer, "read") == 0)
    return 32;
  if (strcmp(buffer, "write") == 0)
    return 31;
  if (strcmp(buffer, "odd") == 0)
    return 8;
	if (strcmp(buffer, "begin") == 0)
		return 21;
	if (strcmp(buffer, "end") == 0)
		return 22;
  if (strcmp(buffer, "procedure") == 0)
		return 30;
  if (strcmp(buffer, "call") == 0)
    return 27;
  if (strcmp(buffer, "else") == 0)
		return 33;

  return 0;
}

list* lex_analyze(char *input, int inputCount, int *listCount, int *error)
{
  char current, next, buffer[20];
  const char *reservedWords [] = {"const", "var", "procedure", "call", "if", "then", "else", "while", "do", "read", "write", "odd"};
  const char *specialSymbols [] = {"+", "-", "*", "/", "(", ")", "=", ",", ".", "<", ">", ";", ":=", "<>", "<=", ">=", "[", "]"};
  int i = 0, n = 0, k = 0, h, bufferIndex = 0, digitFlag = 1, commentFlag = 0, white = 1;
  list *tokenList = calloc(1000, sizeof(list));

  // this function clears whta was in the buffer
  memset(buffer,0,sizeof(buffer));


  for (i = 0; i < inputCount + 1; i++)
  {
    // Enter if we have reached the beginning of a comment
    if (commentFlag == 1)
    {
      // Enter if we reached the end of a comment
      if (input[i] == '*' && input[i + 1] == '/')
      {
        commentFlag = 0;
        memset(buffer,0,sizeof(buffer));
        bufferIndex = 0;
        i++;
      }
      continue;
    }

    // Check if input[i] is an invalid symbol or alphabet of PL/0
    if (isalpha(input[i]) == 0 && isdigit(input[i]) == 0 && checkSpecial(input[i], specialSymbols) == 0 && input[i] != ' ' && input[i] != '\n' && input[i] != '\t')
    {
      printf("Error: Invalid Symbol [%c]\n", input[i]);
      *error = 1;
      return tokenList;
    }

    // If input[i] is a letter, the buffer cannot be a digit
    if (isalpha(input[i]) == 1)
      digitFlag = 0;

    // If we run into a whitespace, check the buffer for every possible symbol/word
    if (input[i] == ' ' || input[i] == '\n' || input[i] == '\t' || input[i] == '\r')
    {
      // Checks if buffer contains a reserved word
      // if it is put it into the token list and clear buffer
      if (checkReserved(buffer, reservedWords) > 0)
      {
        strcpy(tokenList[*listCount].lexeme, buffer);
        tokenList[*listCount].token = checkReserved(buffer, reservedWords);
        *listCount += 1;
        memset(buffer,0,sizeof(buffer));
        bufferIndex = 0;
        digitFlag = 1;
        continue;
      }

      // Checks if buffer contains a special symbol
      // if it is put it into the token list and clear buffer
      if (checkSpecial(buffer[0], specialSymbols) > 0)
      {
        strcpy(tokenList[*listCount].lexeme, buffer);
        tokenList[*listCount].token = checkSpecial(buffer[0], specialSymbols);
        *listCount += 1;
        memset(buffer,0,sizeof(buffer));
        bufferIndex = 0;
        digitFlag = 1;
        continue;
      }

      // Enter if buffer contains a digit and is greater than 5 digits
      if (isdigit(buffer[bufferIndex - 1]) > 0 && bufferIndex > 5)
      {
        printf("Error: Number [%s] too long\n", buffer);
        *error = 1;
        return tokenList;
      }

      // Enter if buffer is a identifier and is greater than 11 characters
      if (bufferIndex > 11)
      {
        printf("Error: Name [%s] too long\n", buffer);
        *error = 1;
        return tokenList;
      }

      // Checks if any part of the buffer is not a digit
      for (h = 0; h < bufferIndex; h++)
      {
        if (isdigit(buffer[h]) == 0)
          digitFlag = 0;
      }

      // Enter if buffer contains a identifier but doesn't start with a letter
      if (digitFlag == 0 && isdigit(buffer[0]) > 0)
      {
        printf("Error: Variable [%s] does not start with letter\n", buffer);
        *error = 1;
        return tokenList;
      }

      // Enter if buffer is a valid digit
      if (isdigit(buffer[bufferIndex - 1]) > 0 && isdigit(buffer[0]) > 0)
      {
        strcpy(tokenList[*listCount].lexeme, buffer);
        tokenList[*listCount].token = 3;
        *listCount += 1;
        memset(buffer,0,sizeof(buffer));
        bufferIndex = 0;
        digitFlag = 1;
        continue;
      }

      // Enter if the buffer is a whitespace, then activate white flag
      if (strcmp(buffer, " ") == 0)
        white = 0;

      // Enter if buffer is not empty and buffer doesnt contain whitespace
      if (bufferIndex != 0 && white == 1)
      {
        strcpy(tokenList[*listCount].lexeme, buffer);
        tokenList[*listCount].token = 2;
        *listCount += 1;
        memset(buffer,0,sizeof(buffer));
        bufferIndex = 0;
        digitFlag = 1;
        continue;
      }
    }

    // Enter if we run into a special symbol, then check the buffer
    if (checkSpecial(input[i], specialSymbols) > 0)
    {
      // Enter if the buffer contains ':' and the next character to input is not '=', therefore invalid symbol
      if (checkSpecial(buffer[0], specialSymbols) == 2000 && input[i] != '=')
      {
        printf("Error: Invalid Symbol [:]\n");
        *error = 1;
        return tokenList;
      }

      // If buffer contains one of the first symbols of a rel-op, check the next char
      if (buffer[0] == '>' || buffer[0] == '<' || buffer[0] == ':' || buffer[0] == '/')
      {
        if (buffer[0] == '>' && input[i] == '=')
        {
          strcpy(tokenList[*listCount].lexeme, ">=");
          tokenList[*listCount].token = 14;
          *listCount += 1;
        }

        if (buffer[0] == '<' && input[i] == '=')
        {
          strcpy(tokenList[*listCount].lexeme, "<=");
          tokenList[*listCount].token = 12;
          *listCount += 1;
        }

        if (buffer[0] == '<' && input[i] == '>')
        {
          strcpy(tokenList[*listCount].lexeme, "<>");
          tokenList[*listCount].token = 10;
          *listCount += 1;
        }

        if (buffer[0] == ':' && input[i] == '=')
        {
          strcpy(tokenList[*listCount].lexeme, ":=");
          tokenList[*listCount].token = 20;
          *listCount += 1;
        }

        if (buffer[0] == '/' && input[i] == '*')
        {
          commentFlag = 1;
          continue;
        }
        memset(buffer,0,sizeof(buffer));
        bufferIndex = 0;
        digitFlag = 1;
        continue;
      }

      // Checks if buffer contains a reserved word
      if (checkReserved(buffer, reservedWords) > 0)
      {
        strcpy(tokenList[*listCount].lexeme, buffer);
        tokenList[*listCount].token = checkReserved(buffer, reservedWords);
        *listCount += 1;
        memset(buffer,0,sizeof(buffer));
        bufferIndex = 0;
        digitFlag = 1;
        // Checks if input[i], is a whitespace to avoid putting whitespace in buffer
        if (input[i] != ' ')
        {
          buffer[bufferIndex] = input[i];
          bufferIndex++;
        }
        continue;
      }

      // Checks if buffer contains a reserved word
      if (checkSpecial(buffer[0], specialSymbols) > 0)
      {
        strcpy(tokenList[*listCount].lexeme, buffer);
        tokenList[*listCount].token = checkSpecial(buffer[0], specialSymbols);
        *listCount += 1;
        memset(buffer,0,sizeof(buffer));
        bufferIndex = 0;
        digitFlag = 1;
        if (input[i] != ' ')
        {
          buffer[bufferIndex] = input[i];
          bufferIndex++;
        }
        // Checks if loop is at last iteration
        if (i == (inputCount - 1) )
        {
          strcpy(tokenList[*listCount].lexeme , buffer);
          tokenList[*listCount].token = checkSpecial(input[i], specialSymbols);
          *listCount += 1;
          memset(buffer,0,sizeof(buffer));
          digitFlag = 1;
          continue;
        }
        continue;
      }

      // Enter if buffer contains a digit and is greater than 5 digits
      if (isdigit(buffer[0]) > 0 && bufferIndex > 5)
      {
        printf("Error: Number [%s] too long\n", buffer);
        *error = 1;
        return tokenList;
      }

      // Enter if buffer is a identifier and is greater than 11 characters
      if (bufferIndex > 11)
      {
        printf("Error: Name [%s] too long\n", buffer);
        *error = 1;
        return tokenList;
      }

      for (h = 0; h < bufferIndex; h++)
      {
        if (isdigit(buffer[h]) == 0)
          digitFlag = 0;
      }

      // Enter if buffer contains a identifier but doesn't start with a letter
      if (digitFlag == 0 && isdigit(buffer[0]) > 0)
      {
        printf("Error: Variable [%s] does not start with letter\n", buffer);
        *error = 1;
        return tokenList;
      }

      if (isdigit(buffer[bufferIndex - 1]) > 0 && isdigit(buffer[0]) > 0)
      {
        strcpy(tokenList[*listCount].lexeme, buffer);
        tokenList[*listCount].token = 3;
        *listCount += 1;
        memset(buffer,0,sizeof(buffer));
        bufferIndex = 0;
        digitFlag = 1;
      }
      white = 1;
      if (strcmp(buffer, " ") == 0)
        white = 0;
      if (bufferIndex != 0 && white == 1)
      {
        strcpy(tokenList[*listCount].lexeme, buffer);
        tokenList[*listCount].token = 2;
        *listCount += 1;
        memset(buffer,0,sizeof(buffer));
        bufferIndex = 0;
        digitFlag = 1;
      }

      // if its a special symbol that is by itself dont put it into buffer just input into list
      if (checkSpecial(input[i], specialSymbols) > 0 && checkSpecial(input[i + 1], specialSymbols) == 0)
      {
        buffer[bufferIndex] = input[i];
        strcpy(tokenList[*listCount].lexeme , buffer);
        tokenList[*listCount].token = checkSpecial(input[i], specialSymbols);
        *listCount += 1;
        memset(buffer,0,sizeof(buffer));
        digitFlag = 1;
        continue;
      }

    }

    // if its a special symbol that is by itself dont put it into buffer just input into list
    if (checkSpecial(input[i], specialSymbols) > 0 && checkSpecial(input[i + 1], specialSymbols) == 0)
    {
      buffer[bufferIndex] = input[i];
      strcpy(tokenList[*listCount].lexeme , buffer);
      tokenList[*listCount].token = checkSpecial(input[i], specialSymbols);
      *listCount += 1;
      memset(buffer,0,sizeof(buffer));
      digitFlag = 1;
      continue;
    }

    if (checkSpecial(input[i], specialSymbols) == 0 && checkSpecial(buffer[0], specialSymbols) > 0)
    {
      strcpy(tokenList[*listCount].lexeme, buffer);
      tokenList[*listCount].token = checkSpecial(buffer[0], specialSymbols);
      *listCount += 1;
      memset(buffer,0,sizeof(buffer));
      bufferIndex = 0;
      digitFlag = 1;
    }

    // if we get through everything else just put it into buffer
    if (input[i] != ' ' && input[i] != '\n' && input[i] != '\t')
    {
      buffer[bufferIndex] = input[i];
      bufferIndex++;
    }
  }

  return tokenList;
  }
