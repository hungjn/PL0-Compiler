#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "codegen.h"
#include "vm.h"

#define MAX_DATA_STACK_HEIGHT 1000
#define MAX_CODE_LENGTH 500

int base (int l, int base, int *stack)
{
  int b1;
  b1 = base;

  while (l > 0)
  {
    b1 = stack[b1-1];
    l--;
  }
  return b1;
}

void virtual_machine(instruction *code, int *tracePrintCount, trace *tracePrint)
{

	int i, j, z, numLines = 0, inputLine = 0, lineValue = 0;
  int sp = MAX_DATA_STACK_HEIGHT, bp = sp - 1, pc = 0, halt = 1, oldpc = 0, userInput, arSize, arCount = 1, dummyArCount;
  int *stack = calloc(MAX_DATA_STACK_HEIGHT, sizeof(int));
	int op, l, m;
  char *stringHold = calloc(20, sizeof(char));
  int *divider = calloc(20, sizeof(int));
  int dividerCounter = 0;

	// printf("\t\tpc\tbp\tsp\tstack\n");
  // printf("Initial values\t%d\t%d\t%d\n", pc, bp, sp);

  while (halt == 1)
  {
    // Fetch
    op = code[pc].op;
    l = code[pc].l;
    m = code[pc].m;

    oldpc = pc;
    pc++;

    // Execute
    switch (op)
    {
      case 1:
        sp -= 1;
        stack[sp] = m;
        break;

      case 2:
        switch (m)
        {
          case 0:
            sp = bp + 1;
            pc = stack[sp - 4];
            bp = stack[sp - 3];
            arCount--;
            dividerCounter--;
            break;

          case 1:
            stack[sp] = stack[sp] * -1;
            break;

          case 2:
            sp += 1;
            stack[sp] = stack[sp] + stack[sp - 1];
            break;

          case 3:
            sp += 1;
            stack[sp] = stack[sp] - stack[sp - 1];
            break;

          case 4:
            sp += 1;
            stack[sp] = stack[sp] * stack[sp - 1];
            break;

          case 5:
            sp += 1;
            stack[sp] = stack[sp] / stack[sp - 1];
            break;

          case 6:
            stack[sp] = stack[sp] % 2;
            break;

          case 7:
            sp += 1;
            stack[sp] = stack[sp] % stack[sp - 1];
            break;

          case 8:
            sp += 1;
            if (stack[sp] == stack[sp - 1])
              stack[sp] = 1;
            else
              stack[sp] = 0;
            break;

          case 9:
            sp += 1;
            if (stack[sp] != stack[sp - 1])
              stack[sp] = 1;
            else
              stack[sp] = 0;
            break;

          case 10:
            sp += 1;
            if (stack[sp] < stack[sp - 1])
              stack[sp] = 1;
            else
              stack[sp] = 0;
            break;

          case 11:
            sp += 1;
            if (stack[sp] <= stack[sp - 1])
              stack[sp] = 1;
            else
              stack[sp] = 0;
            break;

          case 12:
            sp += 1;
            if (stack[sp] > stack[sp - 1])
              stack[sp] = 1;
            else
              stack[sp] = 0;
            break;

          case 13:
            sp += 1;
            if (stack[sp] >= stack[sp - 1])
              stack[sp] = 1;
            else
              stack[sp] = 0;
            break;
        }
        break;

      case 3:
        sp -= 1;
        stack[sp] = stack[base(l, bp, stack) - m];
        break;

      case 4:
        stack[base(l, bp, stack) - m] = stack[sp];
        sp += 1;
        break;

      case 5:
        stack[sp - 1] = 0;
        stack[sp - 2] = base(l, bp, stack);
        stack[sp - 3] = bp;
        stack[sp - 4] = pc;
        bp = sp - 1;
        pc = m;
        arCount++;
        break;

      case 6:
        sp -= m;
        divider[dividerCounter] = m;
        dividerCounter++;
        arSize = m;
        break;

      case 7:
        pc = m;
        break;

      case 8:
        if (stack[sp] == 0)
          pc = m;
        sp += 1;
        break;

      case 9:
        printf("\nWRITING: %d\n\n", stack[sp]);
        sp += 1;
        break;

      case 10:
        sp -= 1;
        printf("Please enter value: ");
        scanf("%d", &userInput);
        stack[sp] = userInput;
        break;

      case 11:
        halt = 0;
        break;
    }
    // printf("%d ", oldpc);
    tracePrint[*tracePrintCount].instructionNum = oldpc;

    switch (op)
    {
      case 1:
        // printf("lit");
        strcpy(tracePrint[*tracePrintCount].instructionName, "lit");
        break;

      case 2:
        // printf("opr");
        strcpy(tracePrint[*tracePrintCount].instructionName, "opr");
        break;

      case 3:
        // printf("lod");
        strcpy(tracePrint[*tracePrintCount].instructionName, "lod");
        break;

      case 4:
        // printf("sto");
        strcpy(tracePrint[*tracePrintCount].instructionName, "sto");
        break;

      case 5:
        // printf("cal");
        strcpy(tracePrint[*tracePrintCount].instructionName, "cal");
        break;

      case 6:
        // printf("inc");
        strcpy(tracePrint[*tracePrintCount].instructionName, "inc");
        break;

      case 7:
        // printf("jmp");
        strcpy(tracePrint[*tracePrintCount].instructionName, "jmp");
        break;

      case 8:
        // printf("jpc");
        strcpy(tracePrint[*tracePrintCount].instructionName, "jpc");
        break;

      case 9:
        // printf("sio");
        strcpy(tracePrint[*tracePrintCount].instructionName, "sio");
        break;

      case 10:
        // printf("sio");
        strcpy(tracePrint[*tracePrintCount].instructionName, "sio");
        break;

      case 11:
        // printf("sio");
        strcpy(tracePrint[*tracePrintCount].instructionName, "sio");
        break;
    }

    // Print stack trace
    // printf(" %d", l);
    // printf(" %d", m);
    // printf("\t%d\t%d\t%d\t", pc, bp, sp);
    tracePrint[*tracePrintCount].l = l;
    tracePrint[*tracePrintCount].m = m;
    tracePrint[*tracePrintCount].pc = pc;
    tracePrint[*tracePrintCount].bp = bp;
    tracePrint[*tracePrintCount].sp = sp;

    dummyArCount = arCount;

    int previousearsize = 0;

    // PRINTS AR STACK REPRESENTATION
    for (i = 0; i < (MAX_DATA_STACK_HEIGHT - sp); i++)
    {
      // printf("%d ", stack[MAX_DATA_STACK_HEIGHT - i - 1]);
      sprintf(stringHold, "%d ", stack[MAX_DATA_STACK_HEIGHT - i - 1]);
      strcat(tracePrint[*tracePrintCount].stackTrace, stringHold);

      // if there is 2 or more AR's
      if (dummyArCount > 1)
      {
        // the reason i - previousearsize is so that i % the ar size will work
        // without trying to reset i
        if (((i + 1) - previousearsize) % divider[arCount - dummyArCount] == 0)
        {
          // previousearsize has to grow some amount pluse its original size
          previousearsize = divider[arCount - dummyArCount] + previousearsize;
          // puts seprating line
          strcat(tracePrint[*tracePrintCount].stackTrace, "| ");
          dummyArCount--;
        }

      }

    }
    *tracePrintCount += 1;
  }

	return;
}
