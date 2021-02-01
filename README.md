# PL0-Compiler
Compiler for PL0 language written in C for Systems Software Project
Equipped with Virtual Machine, Lexical Analyzer, Parser and Code Generator

                                      HOW TO COMPILE:
Enter the following into a Linux Shell Environment (requires GCC):
"gcc driver.c lex.c parser.c codegen.c vm.c"

------------------------------------------------------------------------------------------------------

NOTE: Running any file will only print to the terminal the input/output and any directives you enter.
      All information (input, lex table/list, generated code, vm execution) will be printed into
      a text file called "output.txt" regardless of directives


                                         HOW TO RUN:

If you want to run all directives: (note for running any files, you don't have to give all directives like the examples)

"./a.out input.txt -l -a -v"

(Whatever input text file you want to test goes into the second argument)


RUNNING THE GIVEN FILES: (NOTE, ALL WRITE PRINTS WILL BE RIGHT AFTER THE INPUT FILE PRINT IN TERMINAL, NOT DURING STACK TRACE)

- "noelleexampleinput.txt" is the name of the sample run input text file I submitted that parses and generates code perfectly with no errors
- To run just type "./a.out noelleexampleinput.txt -l -a -v"
- The output of that run is in "noelleexampleoutput.txt"


ERROR FILES:

- I created multiple error text files to test every different invalid input
- To run them just type "./a.out error1.txt -l -a -v", this example runs error test case 1 (there are 21 total error txt files)

ERROR VALUE MEANINGS:

Error 1, Period expected
Error 2, const, var or procedure must be followed by identifier
Error 3, double declaration within the same scope and level is not allowed
Error 4, constant identifier must be followed by =
Error 5, = must be followed by a number
Error 6, Semicolon or comma missing
Error 7, undeclared identifier
Error 8, assignment to constant or procedure is not allowed
Error 9, assignment operator expected
Error 10, end expected
Error 11, then expected
Error 12, do expected
Error 13, read must be followed by variable type identifier
Error 14, call sym must be followed by a proc identifier
Error 15, Relational operator expected
Error 16, Right parenthesis missing
Error 17, identifier, number or ( expected in factor
Error 18, used = instead of :=
Error 19, There should be nothing after period
Error 20, identifier expected after call
Error 21, use of procedure identifier in factor
