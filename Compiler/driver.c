// Hung Nguyen
// Nolan Schafer

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "lex.h"
#include "parser.h"
#include "codegen.h"
#include "vm.h"

int baseFILE (int l, int base, int *stack);
void virtual_machineFILE(instruction *code, FILE *ifp2);

char *sym (int tokenValue)
{
	switch (tokenValue)
	{
		case 1: return "nulsym";
		case 2: return "identsym";
		case 3: return "numbersym";
		case 4: return "plussym";
		case 5: return "minussym";
		case 6: return "multsym";
		case 7: return "slashsym";
		case 8: return "oddsym";
		case 9: return "eqlsym";
		case 10: return "neqsym";
		case 11: return "lessym";
		case 12: return "leqsym";
		case 13: return "gtrsym";
		case 14: return "geqsym";
		case 15: return "lparentsym";
		case 16: return "rparentsym";
		case 17: return "commasym";
		case 18: return "semicolonsym";
		case 19: return "periodsym";
		case 20: return "becomessym";
		case 21: return "beginsym";
		case 22: return "endsym";
		case 23: return "ifsym";
		case 24: return "thensym";
		case 25: return "whilesym";
		case 26: return "dosym";
		case 27: return "callsym";
		case 28: return "constsym";
		case 29: return "varsym";
		case 30: return "procedure";
		case 31: return "writesym";
		case 32: return "readsym";
		case 33: return "elsesym";
	}
	return "ERROR";
}

int main(int argc, char **argv)
{
	// aflag, vflag, lflag for checking directives
	// i and k are for loop counters
	// inputCount is a counter for array of characters in input file
	// error is a variable that will be passed around to functions to catch errors
	// numLines is a counter for number of lines of code generated in codegen
	// tableCount is a counter for the elements in the symbol table
	// listCount is a counter for the lexeme/token list
	// tracePrintCount is a counter for the number of lines in the stack trace
	// input is a char array for storing input from input txt file
	// c is a char for reading the input file
	// tracePrint holds the contents of the vm's stack trace print
	int aflag = 0, vflag = 0, lflag = 0;
	int i = 0, k = 0, inputCount = 0, error = 0, numLines = 0, tableCount = 1, listCount = 0, tracePrintCount = 0;
	char *input = NULL;
	char c;
	FILE *ifp;
	trace *tracePrint = calloc(1000, sizeof(trace));


	// --------------- DIRECTIVE CHECKING -------------------------------------
	if (argc < 2)
		printf("error : please include the file name");

	if (argc == 5)
  {
    aflag = 1;
    vflag = 1;
    lflag = 1;
  }

  else if (argc == 3)
  {
    if (argv[2][1] == 'a')
      aflag = 1;
    else if (argv[2][1] == 'v')
      vflag = 1;
    else
      lflag = 1;
  }

  else if (argc == 4)
  {
    if (argv[2][1] == 'a')
      aflag = 1;
    else if (argv[2][1] == 'v')
      vflag = 1;
    else
      lflag = 1;

    if (argv[3][1] == 'a')
      aflag = 1;
    else if (argv[3][1] == 'v')
      vflag = 1;
    else
      lflag = 1;
  }


	//-----------------FILE IO: READING INPUT---------------------------

  if ((ifp = fopen(argv[1],"r")) == NULL)
  {
     printf("Error! opening file\n");
     // Program exits if the file pointer returns NULL.
     return 0;
  }

  ifp = fopen(argv[1], "r");
  c = fgetc(ifp);

  while (c != EOF)
  {
		inputCount++;
		c = fgetc(ifp);
  }

  rewind(ifp);

  // Calloc the input char array now we have the size
  input = calloc(inputCount + 1, sizeof(char));

  c = fgetc(ifp);

  // Goes back and stores all char's in input array
	for (i = 0; i < inputCount; i++)
	{
			input[i] = c;
			c = fgetc(ifp);
	}
	// secret last element set to space so the last character in input isn't ignored during lex.c
	input[inputCount] = ' ';

  fclose(ifp);

	// -------------------- PRINTING INPUT FILE-----------------------------------------

	printf("Input file:\n");
	for (i = 0; i < inputCount; i++)
			printf("%c", input[i]);

	printf("\n\n");
// -----------------------------------------------------------------------------------


	// CALLING LEX_ANALYZE AND STORING RETURN (LEX/TOKEN LIST)
	list *lexeme = lex_analyze(input, inputCount, &listCount, &error);

	printf("Lexeme Table:\n");
	printf("lexeme\ttoken type\n");
	for (i = 0; i < listCount; i++)
	{
			while (lexeme[i].lexeme[k] != '\0')
			{
				printf("%c",lexeme[i].lexeme[k]);
				k++;
			}
			printf("\t%d\n", lexeme[i].token);
			k = 0;
	}
	printf("\n\n");

	return 0;

	// Terminate if any lex analyzer errors
	if (error == 1)
		return 0;

	// CALLING PARSE AND STORING RETURN (SYMBOL TABLE)
	symbol *table = parse(lexeme, &error, &tableCount);


	// Terminate if any parsing errors
	if (error == 1)
		return 0;

	// corrects tableCount to be index of last element in symbol table
	tableCount--;


	instruction *code = generate_code(table, lexeme, &numLines, &tableCount);


	// CALLING VIRTUAL_MACHINE
	virtual_machine(code, &tracePrintCount, tracePrint);

	//-----------------FILE IO: WRITING TO FILE------------------

	FILE *ifp2 = fopen("output.txt", "w");

	// -------------------PRINTING INPUT TO FILE
	fprintf(ifp2, "Input file:\n");
	for (i = 0; i < inputCount; i++)
		fprintf(ifp2, "%c", input[i]);

	fprintf(ifp2, "\n\n");

	// --------------------PRINTING LEX TABLE TO FILE

	fprintf(ifp2,"Lexeme Table:\n");
  fprintf(ifp2,"lexeme\ttoken type\n");
  for (i = 0; i < listCount; i++)
  {
      while (lexeme[i].lexeme[k] != '\0')
      {
        fprintf(ifp2,"%c",lexeme[i].lexeme[k]);
        k++;
      }
      fprintf(ifp2,"\t\t\t\t%d\n", lexeme[i].token);
      k = 0;
  }
  fprintf(ifp2,"\n\n");

	// ----------------------PRINTING LEX LIST TO FILE
	fprintf(ifp2,"Lexeme List:\n");
	for (i = 0; i < listCount; i++)
	{
		fprintf(ifp2,"%d ", lexeme[i].token);
		if (lexeme[i].token == 2)
		{
			fprintf(ifp2,"%s", lexeme[i].lexeme);
			if (lexeme[i].lexeme != " ")
				fprintf(ifp2," ");
		}
		if (lexeme[i].token == 3)
		{
			fprintf(ifp2,"%s", lexeme[i].lexeme);
			if (lexeme[i].lexeme != " ")
				fprintf(ifp2," ");
		}
	}

	fprintf(ifp2,"\n\n");

	// ----------------PRINTING LEX LIST (symbol representation) TO FILE
	fprintf(ifp2,"Lexeme List (SYMBOL):\n");
	for (i = 0; i < listCount; i++)
	{
		fprintf(ifp2,"%s ", sym(lexeme[i].token));
		if (lexeme[i].token == 2)
		{
			fprintf(ifp2,"%s", lexeme[i].lexeme);
			if (lexeme[i].lexeme != " ")
				fprintf(ifp2," ");
		}
		if (lexeme[i].token == 3)
		{
			fprintf(ifp2,"%s", lexeme[i].lexeme);
			if (lexeme[i].lexeme != " ")
				fprintf(ifp2," ");
		}
	}

	fprintf(ifp2,"\n\n");

	// -------------------PRINTING GENERATED CODE TO FILE
	fprintf(ifp2,"Line\tOP\tL\tM\n");

	for (i = 0; i < numLines; i++)
	{
		fprintf(ifp2,"%d\t\t\t", i);

		switch (code[i].op)
		{
			case 1:
				fprintf(ifp2,"lit");
				break;

			case 2:
				fprintf(ifp2,"opr");
				break;

			case 3:
				fprintf(ifp2,"lod");
				break;

			case 4:
				fprintf(ifp2,"sto");
				break;

			case 5:
				fprintf(ifp2,"cal");
				break;

			case 6:
				fprintf(ifp2,"inc");
				break;

			case 7:
				fprintf(ifp2,"jmp");
				break;

			case 8:
				fprintf(ifp2,"jpc");
				break;

			case 9:
				fprintf(ifp2,"sio");
				break;

			case 10:
				fprintf(ifp2,"sio");
				break;

			case 11:
				fprintf(ifp2,"sio");
				break;
		}

		fprintf(ifp2,"\t%d", code[i].l);
		fprintf(ifp2,"\t%d\n", code[i].m);
	}
	fprintf(ifp2,"\n\n");

	// -----------------VIRTUAL MACHINE EXECUTION TO FILE
	fprintf(ifp2,"\t\t\t\t\t\t\t\tpc bp sp\t\tstack\n");
	fprintf(ifp2,"Initial values\t0\t999\t1000\n");
	for (i = 0; i < tracePrintCount; i++)
	{
		fprintf(ifp2,"%d \t", tracePrint[i].instructionNum);
		fprintf(ifp2,"%s ", tracePrint[i].instructionName);
		fprintf(ifp2,"%d ", tracePrint[i].l);
		fprintf(ifp2,"%d\t\t", tracePrint[i].m);
		fprintf(ifp2,"\t%d\t%d\t%d\t\t", tracePrint[i].pc, tracePrint[i].bp, tracePrint[i].sp);
		fprintf(ifp2,"%s\n", tracePrint[i].stackTrace);
	}

	fclose(ifp2);
	// -------------------- END OF FILE IO: WRITING---------------------------------


	// --------------------DIRECTIVES EXECUTION--------------------

	// -l directive: prints the lexeme table and list
	if (lflag == 1)
	{

		printf("Lexeme Table:\n");
	  printf("lexeme\ttoken type\n");
	  for (i = 0; i < listCount; i++)
	  {
	      while (lexeme[i].lexeme[k] != '\0')
	      {
	        printf("%c",lexeme[i].lexeme[k]);
	        k++;
	      }
	      printf("\t%d\n", lexeme[i].token);
	      k = 0;
	  }
	  printf("\n\n");

		printf("Lexeme List:\n");
		for (i = 0; i < listCount; i++)
	  {
	    printf("%d ", lexeme[i].token);
	    if (lexeme[i].token == 2)
	    {
	      printf("%s", lexeme[i].lexeme);
	      if (lexeme[i].lexeme != " ")
	        printf(" ");
	    }
	    if (lexeme[i].token == 3)
	    {
	      printf("%s", lexeme[i].lexeme);
	      if (lexeme[i].lexeme != " ")
	        printf(" ");
	    }
	  }
	  printf("\n\n");

		printf("Lexeme List (SYMBOL):\n");
		for (i = 0; i < listCount; i++)
	  {
	    printf("%s ", sym(lexeme[i].token));
	    if (lexeme[i].token == 2)
	    {
	      printf("%s", lexeme[i].lexeme);
	      if (lexeme[i].lexeme != " ")
	        printf(" ");
	    }
	    if (lexeme[i].token == 3)
	    {
	      printf("%s", lexeme[i].lexeme);
	      if (lexeme[i].lexeme != " ")
	        printf(" ");
	    }
	  }
		printf("\n");
	}

	// -a directive: prints the generated code
	if (aflag == 1)
	{
		printf("\n\nNo errors, program is syntactically correct\n\n");
		printf("Line\tOP\tL\tM\n");

	  for (i = 0; i < numLines; i++)
	  {
	    printf("%d\t", i);

	    switch (code[i].op)
	    {
	      case 1:
	        printf("lit");
	        break;

	      case 2:
	        printf("opr");
	        break;

	      case 3:
	        printf("lod");
	        break;

	      case 4:
	        printf("sto");
	        break;

	      case 5:
	        printf("cal");
	        break;

	      case 6:
	        printf("inc");
	        break;

	      case 7:
	        printf("jmp");
	        break;

	      case 8:
	        printf("jpc");
	        break;

	      case 9:
	        printf("sio");
	        break;

	      case 10:
	        printf("sio");
	        break;

	      case 11:
	        printf("sio");
	        break;
	    }

	    printf("\t%d", code[i].l);
	    printf("\t%d\n", code[i].m);
  	}
	}

	// -v directive: prints the virtual machine execution
	if (vflag == 1)
	{
		printf("\n\n");
		printf("\t\tpc\tbp\tsp\tstack\n");
	  printf("Initial values\t0\t999\t1000\n");
		for (i = 0; i < tracePrintCount; i++)
		{
			printf("%d ", tracePrint[i].instructionNum);
			printf("%s ", tracePrint[i].instructionName);
			printf("%d ", tracePrint[i].l);
			printf("%d", tracePrint[i].m);
			printf("\t%d\t%d\t%d\t", tracePrint[i].pc, tracePrint[i].bp, tracePrint[i].sp);
			printf("%s\n", tracePrint[i].stackTrace);
		}
	}
	return 0;
}
