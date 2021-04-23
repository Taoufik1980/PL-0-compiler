/*Taoufik Laaroussi  
Date : 4/16/2021
*/

#include <stdio.h>
#include <stdlib.h>
#include "compiler.h"

//Main function for whole project
int main(int argc, char **argv)
{
	//Printing choice and input file declaration
	int lflag = 0, aflag = 0, vflag = 0;
	FILE *ifp;
	char *inputfile;
	char c;
	int i;
	
	if (argc < 2)
	{
		printf("Error : please include the file name");
		return 0;
	}
	else if (argc > 4) //flag parameter values based on provided arguments
	{
		lflag = 1;
		aflag = 1;
		vflag = 1;
	}
	else if (argc == 3 || argc == 4)
	{
		if (argv[2][1] == 'l')
			lflag = 1;
		else if (argv[2][1] == 'a')
			aflag = 1;
		else
			vflag = 1;
		
		if (argc == 4)
		{
			if (argv[3][1] == 'l')
				lflag = 1;
			else if (argv[3][1] == 'a')
				aflag = 1;
			else
				vflag = 1;
		}
	}
	
	ifp = fopen(argv[1], "r");
	inputfile = malloc(500 * sizeof(char));
	c = fgetc(ifp);
	i = 0;
	while (1)
	{
		inputfile[i++] = c;
		c = fgetc(ifp);
		if (c == EOF)
			break;
	}
	inputfile[i] = '\0';
	
	//Function calls
	tokens * list = lex_analyze(inputfile, lflag);
	instruction *code = parser(list, aflag);
	execute(code, vflag);
	
	return 0;
}
