/*
Taoufik Laaroussi  
Date : 4/16/2021
*/

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "compiler.h"

#define MAX_LENGTH 500

// Reserved words array declaration
char * reservedWords[] = {"const", "var", "procedure", "call", "if", "then", 
							"else", "while", "do", "read", "write", "odd", 
						 "return", "begin", "end"};
// Special characters array declaration
char * specialSymbols[] = {"+", "-", "*", "/", "(", ")", "=", ",", 
							 ".",  "<", ">",  ";" , ":" , "%"};
// global variable
int lexIndex = 0;
tokens tokenList[MAX_SYMBOL_TABLE_SIZE];
// function prototypes
int checkString(char * str);
int reservedWord(char * str);
int checkSymbol(char * c, char * str, char *in);

tokens * lex_analyze(char *inputfile, int print)
{
  // Variable pointers used to handle input
	char * lookAhead = (char*)malloc(sizeof(char));
	char * checkStar = (char*)malloc(sizeof(char));
	char * checkSlash = (char*)malloc(sizeof(char));
	//char * identArray[MAX_SYMBOL_TABLE_SIZE];
	int tokensArray[MAX_SYMBOL_TABLE_SIZE];
	int index = 0, identIndex = 0, tokensIndex = 0;

 // Dynamic memory reservation
	char * c = (char*)malloc(sizeof(char));
	char * str = (char*)malloc(sizeof(char) * MAX_LENGTH);
	// FILE * textFile;

	*c = inputfile[0];
	strcat(str, c);

// Start process of getting tokens charahter by charachter
	int retVal = 0;
	if (print == 1)
		printf("Lexeme Table:\nlexeme          token type");

	while (inputfile[lexIndex] != '\0')
	{
		 index = 0;
		 
  // Comments check handling process
		if (strcmp(str, "/") == 0)
		{
			if ((*checkStar = inputfile[++lexIndex]) == '*')
			{
				*checkStar = inputfile[++lexIndex];
				while (*checkStar != '*')
				{
					*checkStar = inputfile[++lexIndex];
					if (*checkStar == '*')
					{
						*checkSlash = inputfile[++lexIndex];
						if (*checkSlash == '/')
							break;
						else 
							*checkStar = *checkSlash;
					}
				}
			}

			else 
			{
				retVal = checkSymbol(c, str, inputfile);
				//printf("\ncheckSymbol 1 : %d\n", retVal);
				if (retVal == 0)
				{
					printf("\nError : Invalid Symbol");
					exit(0);
				}
				else 
				{
					if (print == 1)
						printf("\n%10s%5d", str, retVal);
					tokenList[tokensIndex].type = retVal;
					strcpy(tokenList[tokensIndex].value, "");
					tokensIndex++;  
				}
			}

			*c = inputfile[++lexIndex];
			strcpy(str, c);
			continue;
		}
   
  // Identifiers and reserved words handling process
		else if ((isalpha(*c) || isdigit(*c)))
		{
			*lookAhead = inputfile[++lexIndex];
			while (isdigit(*lookAhead) || isalpha(*lookAhead))
			{
				strcat(str, lookAhead);
				index++;
				*lookAhead = inputfile[++lexIndex];
				//printf("=====%c=====", *lookAhead);
			}
			
      //Check the validity of the string as indentifier or reserverd word or invalid identifier
			int strCheckVal = checkString(str);
			if (strCheckVal == 1)
			{
				retVal = reservedWord(str);
				if (retVal != 0)
				{
        
        //Reserved word handling process
					if (print == 1)
						printf("\n%10s %5d", str, retVal);
					tokenList[tokensIndex].type = retVal;
					strcpy(tokenList[tokensIndex].value, "");
					tokensIndex++;
					*c = *lookAhead;
					strcpy(str, c);
					continue;
				}
				else 
				{
        
        // check the length of an indentifier and output it
					if (strlen(str) <= 11)
					{
						if (print == 1)
							printf("\n%10s%5d", str, identsym);
						tokenList[tokensIndex].type = identsym;
						strcpy(tokenList[tokensIndex].value, str);
						tokensIndex++;
						//printf("%d++++", index);

						if(strcmp(lookAhead, ";") == 0 || strcmp(lookAhead, ")") == 0 || index ==0){
						strcpy(str, lookAhead);
							strcpy(c, str);
							continue;
						}
					}
           
           // Output error message when an identifier is exceeding 11 charachters
					else
					{
						
						printf("\nError : Identifier names cannot exceed 11 characters");
						strcpy(c, lookAhead);
						strcpy(str, c);
						exit(0);
					}
				}
			}

			else if (strCheckVal == 0)
			{
      
      // Number handling process
				retVal = atoi(str);
				if (strlen(str) <= 5)
				{
					if (print == 1)
						printf("\n%10d%5d", retVal, numbersym);
					tokenList[tokensIndex].type = numbersym;
					sprintf(tokenList[tokensIndex].value, "%d", retVal);
					tokensIndex++;
					strcpy(c, lookAhead);
					strcpy(str, c);
					continue;
				}
        
        // output error message when the number exceed 5 digits
				else 
				{
					
					printf("\nError : Numbers cannot exceed 5 digits");
					exit(0);
				}
			}

			else //strcheckval == -1
			{
      
      //ouput an error message when the identifier start with a digit process
				
				printf("\nError : Identifiers cannot begin with a digit");
				strcpy(c, lookAhead);
				strcpy(str, c);
				exit(0);;
			}
		}

		else
		{ 
   
   // Tab, white space and new line handling process
			if (iscntrl(*c) || *c == '\n' || *c == ' ')
			{
				*c = inputfile[++lexIndex];
				strcpy(str, c);
				continue;
			}
      
			 // Symbols handling process
			else
			{ 
				retVal = checkSymbol(c, str, inputfile);
				//printf("\ncheckSymbol 2 : %d\n", retVal);
				if (retVal == 0)
				{
					
					printf("\nError : Invalid Symbol");
					exit(0);
				}
				else 
				{
					if (print == 1)
						printf("\n%10s%5d", str, retVal);
					tokenList[tokensIndex].type = retVal;
					strcpy(tokenList[tokensIndex].value, "");
					tokensIndex++;  
				}
			}
		}
   
   // Fetch next character and store it in str
		*c = inputfile[++lexIndex];
		strcpy(str,c);
	}
 
  //output the token list process
	if (print == 1)
	{
		printf("\n\nToken List:\n");
		for (int j = 0; j < tokensIndex; j++)
		{
			printf("%d ", tokenList[j].type);
			if (strcmp(tokenList[j].value, "") != 0)
				printf("%s ", tokenList[j].value);
		}
	}
	if (print == 1)		
		printf("\n\n");
   free(lookAhead);
   free(checkStar);
   free(checkSlash);
   free(c);
   free(str);
 
  return tokenList;
}

// Function to check for reserved words and return their token number
int reservedWord(char * str)
{
	int i;
	for (i = 0; i < 15; i++)
	{
		if (strcmp(str, reservedWords[i]) == 0)
			break;
	}

	switch (i)
	{
		case 0:
			return constsym;
		case 1:
			return varsym;
		case 2:
			return procsym;
		case 3:
			return callsym;
		case 4:
			return ifsym;
		case 5:
			return thensym;
		case 6:
			return elsesym;
		case 7:
			return whilesym;
		case 8:
			return dosym;
		case 9:
			return readsym;
		case 10:
			return writesym;
		case 11:
			return oddsym;
		case 12:
			return returnsym;
		case 13:
			return beginsym;
		case 14:
			return endsym;
		default:
		return 0;
	}
}

// function to check the identifier's validity
int checkString(char * str)
{
	int i = 0, error = 0;
	if (isalpha(str[i]))
		return 1; // valid identifier

	else
	{
		i++;
		while (str[i] != '\0') 
		{
			if (isalpha(str[i]) && i < 5)
			{
				error =  -1; // invalid identifier
				break;
			}
			i++;
		}

		if (error == -1)
			return error;
	}

	return 0; // valid number
}

// Function check for symbols and return their token number
int checkSymbol(char * c, char * str, char *in)
{
	int i = 0;
	char * lookAhead = (char*)malloc(sizeof(char));

	for (i; i < 14; i++)
	{
		if (strcmp(c, specialSymbols[i]) == 0)
			break;
	}

	if (i == 9) // '<' 
	{
		*lookAhead = in[++lexIndex];
		if (*lookAhead != '>' && *lookAhead != '=')
			return lessym;
		else if (*lookAhead == '>')
		{
			strcat(str, lookAhead);
			return neqsym; 
		}
		else
		{
			strcat(str, lookAhead);
			return leqsym;
		}
	} 

	else if (i == 10) // '>'
	{
		*lookAhead = in[++lexIndex];
		if (*lookAhead == '=')
		{
			strcat(str, lookAhead);
			return geqsym;
		}
		else return gtrsym;
	}

	else if (i == 12)
	{
		*lookAhead = in[++lexIndex];
		if (*lookAhead == '=')
		{
			strcat(str, lookAhead);
			return becomessym;
		}
    else
       return 0;
	}

	else
	{
		switch (i)
		{
			case 0:
				return plussym;
			case 1:
				return minussym;
			case 2:
				return multsym;
			case 3:
				return slashsym;
			case 4:
				return lparentsym;
			case 5:
				return rparentsym;
			case 6:
				return eqlsym;
			case 7:
				return commasym;
			case 8:
				return periodsym;
			case 11:
				return semicolonsym;
			case 13:
				return modsym;
			default:
			return 0;
		}
	}
}
