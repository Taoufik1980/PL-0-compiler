/*Taoufik Laaroussi 
Date : 4/16/2021
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "compiler.h"

//Global variables and arrays declaration
instruction stack[MAX_SYMBOL_TABLE_SIZE];
tokens token;
symbol symbol_table[MAX_SYMBOL_TABLE_SIZE];
tokens tokenL[MAX_SYMBOL_TABLE_SIZE];

//Global variable to count the number of procedures
int procedurecount = 0;
int tokenIndex = 0;
int cx = 0;
int size = 0;
int symTableIndex = 0;
int printStatus;

//Functions prototype
int SYMBOLTABLECHECK(int lexlevel);
int SYMBOLTABLESEARCH(int lexlevel, int kind);
int FINDPROCEDURE(int procVal);
void PROGRAM();
void BLOCK(int lexlevel, int param,  int procedureIndex);
int CONSTDECLARATION(int lexlevel);
int VARDECLARATION(int lexlevel, int param);
int PROCEDUREDECLARATION(int lexlevel);
void STATEMENT(int lexlevel);
void CONDITION(int lexlevel);
void EXPRESSION(int lexlevel);
void TERM(int lexlevel);
void FACTOR(int lexlevel);
void error(int x);
void getNextToken();
void emit(int op, char opc[4], int l, int m);
void addToSymbolTable(int kind, char name[12], int val, int level, int addr, int mark, int param);

// Function parser accepts a tokens list and flag as parameters
instruction * parser(tokens * tokenList, int print)
{
	//Getting the size of the token list
	 while (tokenList[size].type != '\0')
    	size++;

	//Copy the content of token list to a global array tokenL
	for (int j = 0; j < size; j++)
		tokenL[j] = tokenList[j];

	printStatus = print;
    int i = 0;
	
// Parsing process starts here
    getNextToken();
   	PROGRAM();

// if flag parameter is 1 print the output on the screen
   if (print == 1)
   {
   		printf("\nGenerated Assembly:\nLine    OP    L    M\n");
   		for (i = 0; i < cx; i++)
    		printf("%3d %6s %4d %4d\n", i, stack[i].opc, stack[i].l, stack[i].m);
    	printf("\n");
   }
    return stack;
}

// Function to check if the symbol table contains the required token
int SYMBOLTABLECHECK(int lexlevel )
{
    int i = 0;
	for (i = MAX_SYMBOL_TABLE_SIZE - 1; i >= 0 ; i--)
	{
		if (strcmp(token.value, symbol_table[i].name) == 0 && symbol_table[i].level == lexlevel)
			if(symbol_table[i].mark == 0)
				return i;
	}

	return -1;
}

// Function to Search for a token in the symbol table
int SYMBOLTABLESEARCH(int lexlevel, int kind)
{
    int i = 0;
	for (i = MAX_SYMBOL_TABLE_SIZE - 1; i >= 0 ; i--)
	{
		if (strcmp(token.value, symbol_table[i].name) == 0 && symbol_table[i].kind == kind)// check the token name and token type
			if(symbol_table[i].level <= lexlevel && symbol_table[i].mark == 0)// check the level and mark
				return i;
	}

	return -1;
}

// Find the the procedure based on the procedure value procVal
int FINDPROCEDURE(int procVal)
{
	int i = 0;
	for (i = MAX_SYMBOL_TABLE_SIZE - 1; i >= 0 ; i--)
	{
		if (procVal == symbol_table[i].val && symbol_table[i].kind==3) // Check value and kind 3 = Procedure
			return i;
	}
	return -1;
}

// Function to mark all variables in a deleted procedure in order not to use them again
void MARK (int count)
{
	int i = 0;
	int index = symTableIndex - count;
	for (i = symTableIndex - 1; i >= index ; i--)
	{
		if ( symbol_table[i].mark == 0)
		{
			symbol_table[i].mark = 1;
			count--;
		}		
	}
}

//Function to start parsing a program
void PROGRAM()
{
	int lexlevel=0, numProc=1;
	emit(7, "JMP", 0, 0);
	for(int i = 0; i < size; i++){
		if(tokenL[i].type == procsym){
			numProc++;
			emit(7,"JMP",0,0);
		}
	}
	addToSymbolTable(3,"main",0,0,0,0,0); // add the main procedure to symbol table
	procedurecount++;
	BLOCK(0,0,0);
	if (token.type != periodsym)
		error(1);
	for(int i = 0; i<numProc; i++){
		stack[i].m =  symbol_table[FINDPROCEDURE(i)].addr;
	}
	emit(9, "SYS", 0, 3); //HALT
}

//Function to start parsing a block of code
void BLOCK(int lexlevel, int param,  int procedureIndex)
{
	int c = CONSTDECLARATION(lexlevel);
	int v = VARDECLARATION(lexlevel, param);
	int p = PROCEDUREDECLARATION(lexlevel);

	 symbol_table[procedureIndex].addr = cx;
	emit(6, "INC", 0, v + 4); 
	STATEMENT(lexlevel);

	// Mark the sum of c, v, p as marked starting from the end of symbol table
		MARK(c+v+p);
}

//Function to check if tokens are constant or list of constants
int CONSTDECLARATION(int lexlevel)
{
	char identName[12];
	int numConst = 0;
	// Check if token is a const symbol
	if (token.type == constsym)
	{
		do
		{
			numConst++;
			getNextToken();
			// Check if token is a identifier
			if (token.type != identsym){
				error(2);
			}

			if (SYMBOLTABLECHECK(lexlevel) != -1)
				error(7);

			strcpy(identName, token.value);
			getNextToken();

			if (token.type != eqlsym)
				error(4);

			getNextToken();

			if (token.type != numbersym)
				error(5);

			addToSymbolTable(1, identName, atoi(token.value), lexlevel, 0,0,0);
			getNextToken();

		} while (token.type == commasym);
		
		// Check if token is a semicolon
			if (token.type != semicolonsym)
				error(6);

			getNextToken();
	}
	return numConst;
}

//Function to check if tokens are variables
int VARDECLARATION(int lexlevel, int param)
{
	int numVars = 0;
	if(param == 1)
		numVars=1;
	// Check if token is a variable
	if (token.type == varsym)
	{
		do
		{
			numVars++;
			getNextToken();
				// Check if token is an identifier
			if (token.type != identsym){
				error(2);
			}

			if (SYMBOLTABLECHECK(lexlevel) != -1)
				error(7);

			addToSymbolTable(2, token.value, 0, lexlevel, numVars + 3,0,0);

			getNextToken();

		} while(token.type == commasym);
		
		// Check if token is a semicolon
			if (token.type != semicolonsym)
				error(6);

		getNextToken();
	}

	return numVars;
}

int PROCEDUREDECLARATION(int lexlevel)
{
	int count = 0, procIdx=0, numProc = 0;
	if (token.type == procsym)
	{
		do{
			// Increment number of procedures
			numProc++;
			getNextToken();

			if(token.type != identsym)
				error(2);	

			if(SYMBOLTABLECHECK(lexlevel)!= -1)
				error(7);
			procIdx = symTableIndex;

			// Add procedure to symbol table then increment the number of procedures
			addToSymbolTable(3,token.value, procedurecount,lexlevel,0,0,0);
			procedurecount++;
			getNextToken();

			if(token.type == lparentsym)
			{
				getNextToken();
				if(token.type != identsym)
						error(2);

				// Add an identifier to symbol table
				addToSymbolTable(2, token.value,0, lexlevel+1,3,0,0);
				symbol_table[procIdx].param = 1;
				getNextToken();
				if(token.type != rparentsym)
					error(14);

				getNextToken();
				if(token.type != semicolonsym)
						error(6);
				getNextToken();

				BLOCK(lexlevel+1, 1, procIdx);
			}
			else
			{
				if(token.type != semicolonsym)
					error(6);
				getNextToken();
				BLOCK(lexlevel+1, 0, procIdx);
			}

			if(stack[cx-1].opcode != 2 && stack[cx-1].m != 0)
			{
				emit(1,"LIT", 0, 0);
				emit(2,"OPR",0,0);
			}

			if(token.type != semicolonsym)
				error(6);
			getNextToken();

		} while(token.type == procsym);

	}
	return numProc; //return number of procedures 
}

//Function to check if a statement is valid
void STATEMENT(int lexlevel)
{
	int symIdx = 0;
	int jpcIdx = 0;
	int jmpIdx = 0;
	
	// Check if token is an identifier
	if (token.type == identsym)
	{
		symIdx = SYMBOLTABLESEARCH(lexlevel, 2);

		if (symIdx == -1)
			error(7);

		getNextToken();
			// Check if token is an :=
		if (token.type != becomessym)
			error(9);

		getNextToken();

		EXPRESSION(lexlevel);
		
		//Store the token in the symbol table
		emit(4,"STO", lexlevel-symbol_table[symIdx].level, symbol_table[symIdx].addr);
		return;
	}
	// Check if token is a call 
	if(token.type == callsym)
	{
		getNextToken();
		if(token.type!= identsym){
			error(2);
			
		}
		symIdx = SYMBOLTABLESEARCH(lexlevel,3);
		if(symIdx == -1)
				error(7);
			
		getNextToken();

			// Check if token is left parenthensis
		if(token.type == lparentsym)
		{
			getNextToken();
			if(symbol_table[symIdx].param != 1)
				error(18);

			EXPRESSION(lexlevel);

			// Check if token is right parenthensis
			if(token.type != rparentsym)
				error(14);

			getNextToken();
		}
		else
			//Store the token in the symbol table
			emit(1,"LIT", 0, 0);

		//Store the token in the symbol table
		emit(5,"CAL", lexlevel - symbol_table[symIdx].level, symbol_table[symIdx].addr);
		return;
	}
	// Check if token is return
	if(token.type == returnsym)
	{
		if(lexlevel == 0)
			error(19);

		getNextToken();

		if(token.type == lparentsym)
		{
			getNextToken();
			EXPRESSION(lexlevel);
			emit(2,"OPR",0,0);
			if(token.type != rparentsym)
				error(14);
			
			getNextToken();
		}
		else
		{
			//Store the token in the symbol table
			emit(1,"LIT", 0, 0);
			emit(2,"OPR",0,0);
		}
		return;
	}
		
	// Check if token is a begin symbol
	if (token.type == beginsym)
	{
		do
		{
			getNextToken();
			STATEMENT(lexlevel);

		} while (token.type == semicolonsym);
		
		// Check if token is a end symbol
		if (token.type != endsym)
			error(10);

		getNextToken();
		return;
	}

	// Check if token is an if symbol
	if (token.type == ifsym)
	{
		getNextToken();

		CONDITION(lexlevel);

		jpcIdx = cx;

		emit(8,"JPC", 0, jpcIdx);

		if (token.type != thensym)
			error(11);

		getNextToken();

		STATEMENT(lexlevel);

		// Check if token is an else symbol
		if(token.type == elsesym)
		{
			getNextToken();
			jmpIdx = cx;
			emit(7,"JMP", 0, jmpIdx);
			stack[jpcIdx].m = cx;
			STATEMENT(lexlevel);
			stack[jmpIdx].m = cx;

		}
		else
			stack[jpcIdx].m = cx;
		return;
	}

	// Check if token is a while symbol
	if (token.type == whilesym)
	{
		getNextToken();
		int loopIdx = cx;

		CONDITION(lexlevel);

		// Check if token is a do symbol
		if (token.type != dosym)
			error(12);

		getNextToken();

		jpcIdx = cx;

		emit(8, "JPC", 0, jpcIdx);

		STATEMENT(lexlevel);

		emit(7, "JMP", 0, loopIdx);

		stack[jpcIdx].m = cx;
		return;
	}
	
	// Check if token is a read symbol
	if (token.type == readsym)
	{
		getNextToken();

		if (token.type != identsym)
			error(2);

		symIdx = SYMBOLTABLESEARCH(lexlevel,2);

		if (symIdx == -1)
				error(7);

		if (symbol_table[symIdx].kind != 2) // check not a VAR
			error(5);

		getNextToken();

		//Store the token in the symbol table
		emit(9, "SYS",0, 2);
		emit(4,"STO", lexlevel-symbol_table[symIdx].level, symbol_table[symIdx].addr);

		return;
	}

	// Check if token is a write symbol
	if (token.type == writesym)
	{
		getNextToken();
		EXPRESSION(lexlevel);
		emit(9,"SYS",0, 1);
		return;
	}
}

//Function to check if token is an operation symbol for valid conditions
void CONDITION(int lexlevel)
{
	// Check if token is an odd symbol
	if (token.type == oddsym) 
	{
		getNextToken();
		EXPRESSION(lexlevel);
		emit(2,"OPR", 0, 6);
	}

	else
	{
		EXPRESSION(lexlevel);

		if (token.type == eqlsym) //Equal symbol
		{
			getNextToken();
			EXPRESSION(lexlevel);
			emit(2,"OPR", 0, 8);
		}

		else if (token.type == neqsym)//Not equal symbol
		{
			getNextToken();
			EXPRESSION(lexlevel);
			emit(2,"OPR", 0, 9);
		}

		else if (token.type == lessym)//Less than symbol
		{
			getNextToken();
			EXPRESSION(lexlevel);
			emit(2,"OPR", 0, 10);
		}

		else if (token.type == leqsym) //Less or equal symbol
		{
			getNextToken();
			EXPRESSION(lexlevel);
			emit(2, "OPR", 0, 11);
		}

		else if (token.type == gtrsym) //Greather than symbol
		{
			getNextToken();
			EXPRESSION(lexlevel);
			emit(2,"OPR", 0, 12);
		}

		else if (token.type == geqsym) //Greather or equal symbol
		{
			getNextToken();
			EXPRESSION(lexlevel);
			emit(2, "OPR", 0, 13);
		}

		else error(13);
	}
}

//Function to check if tokens form a valid expression
void EXPRESSION(int lexlevel)
{
	
	if(token.type == minussym) //minus symbol
	{
		getNextToken();
		TERM(lexlevel);
		emit(2, "OPR", 0, 1);

		while (token.type == plussym || token.type == minussym) //While loop to check if a token is plus or minus symbol
		{
			if (token.type == plussym)
			{
				getNextToken();
				TERM(lexlevel);
				emit(2,"OPR", 0, 2);
			}

			else
			{
				getNextToken();
				TERM(lexlevel);
				emit(2,"OPR", 0, 3);
			}
		}
	}

	else
	{
		if (token.type == plussym) //plus symbol
			getNextToken();

		TERM(lexlevel);

		while (token.type == plussym || token.type == minussym)//While loop to check if a token is plus or minus symbol
		{
			if (token.type == plussym)
			{
				getNextToken();
				TERM(lexlevel);
				emit(2, "OPR", 0, 2);
			}

			else
			{
				getNextToken();
				TERM(lexlevel);
				emit(2,"OPR", 0, 3);
			}
		}
	}
}

//Function to check if tokens form a valid term
void TERM(int lexlevel)
{
	FACTOR(lexlevel);

	while(token.type == multsym || token.type == slashsym || token.type == modsym)
	{
		if (token.type == multsym) //Multiplication symbol
		{
			getNextToken();
			FACTOR(lexlevel);
			emit(2,"OPR", 0, 4);
		}

		else if (token.type == slashsym)  //Division symbol
		{
			getNextToken();
			FACTOR(lexlevel);
			emit(2, "OPR", 0, 5);
		}

		else // Mod symbol
		{
			getNextToken();
			FACTOR(lexlevel);
			emit(2,"OPR", 0, 7);
		}
	}
}

//Function to check if tokens form a valid factor
void FACTOR(int lexlevel)
{
	int symIdxV = 0, symIdxC=0, symIdx = 0;
	 
	 //Check if token is a valid identifier
	if (token.type == identsym)
	{
		symIdxV = SYMBOLTABLESEARCH(lexlevel,2);
		symIdxC = SYMBOLTABLESEARCH(lexlevel,1);

		if (symIdxV == -1 && symIdxC == -1)
		{
			error(7);
		}
		else if(symIdxC == -1 || (symIdxV != -1 && symbol_table[symIdxV].level > symbol_table[symIdxC].level))
				emit(3,"LOD", lexlevel-symbol_table[symIdxV].level, symbol_table[symIdxV].addr);
		else 
			emit(1,"LIT", 0, symbol_table[symIdxC].val);	
		getNextToken();

	}

	//Check if token is a valid number
	else if (token.type == numbersym)
	{
		emit(1, "LIT", 0, atoi(token.value)); // Check what is m
		getNextToken();
	}

	//Check if token is a left parenthesis to start a expression
	else if (token.type == lparentsym)
	{
		getNextToken();
		EXPRESSION(lexlevel);

		if (token.type != rparentsym)
			error(14);

		getNextToken();
	}
	//Check if token is a call
	else if(token.type == callsym)
		STATEMENT(lexlevel);

	else
		error(6);
}



//Function has a list of errors sorted by numbers 
void error(int x)
{
		printf("Error: ");

		switch(x)
		{
			case 1: printf("Program must end with period\n");
				break;
			case 2: printf("const, var, procedure, call, and read keywords must be followed by identifier\n");
				break;
			case 3: printf("competing symbol declarations at the same level\n");
				break;
			case 4: printf("constants must be assigned with =\n");
			break;
			case 5: printf("constants must be assigned an integer value\n");
				break;
			case 6: printf("symbol declarations must be followed by a semicolon\n");
				break;
			case 7: printf("undeclared variable or constant in equation\n");
				break;
			case 8: printf("only variable values may be altered\n");
				break;
			case 9: printf("assignment statements must use :=\n");
				break;
			case 10: printf("begin must be followed by end\n");
				break;
			case 11: printf("if must be followed by then\n");
				break;
			case 12: printf("while must be followed by do\n");
				break;
			case 13: printf("condition must contain comparison operator\n");
				break;
			case 14: printf("right parenthesis must follow left parenthesis\n");
				break;
			case 15: printf("arithmetic equations must contain operands, parentheses, numbers, or symbols\n");
				break;
			case 16: printf("undeclared procedure for call\n");
				break;
			case 17: printf("parameters may only be specified by an identifier\n");
				break;
			case 18: printf("parameters must be declared\n");
				break;
			case 19: printf("cannot return from main\n");
				break;
		}

	exit(-1);
}

//Function to get next token from token list
void getNextToken()
{
	if (tokenIndex < size)
	{
		token = tokenL[tokenIndex];
		tokenIndex++;
	}
}

//Function to store the instruction inside the array of instructions
void emit(int op,char opc[4], int l, int m)
{
	stack[cx].opcode = op;
	strcpy(stack[cx].opc, opc);
	stack[cx].l = l;
	stack[cx].m = m;
	cx++;
}

//Function to add tokens to symbol table
void addToSymbolTable(int kind, char name[12], int val, int level, int addr, int mark, int param)
{
	symbol_table[symTableIndex].kind = kind;
	strcpy(symbol_table[symTableIndex].name, name);
	symbol_table[symTableIndex].val = val;
	symbol_table[symTableIndex].level = level;
	symbol_table[symTableIndex].addr = addr;
	symbol_table[symTableIndex].mark = mark;
	symbol_table[symTableIndex].param = param;
	symTableIndex++;
}
