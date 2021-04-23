
/* Taoufik Laaroussi 
Date : 4/16/2021
*/

//Global variable declaration
#define MAX_SYMBOL_TABLE_SIZE 500
//int procedurecount = 0;
// Tokens type enumeration declaration
typedef enum
{
	modsym = 1, identsym, numbersym, plussym, minussym, 
	multsym, slashsym, oddsym, eqlsym, neqsym, 
	lessym, leqsym, gtrsym, geqsym, lparentsym, 
	rparentsym, commasym, semicolonsym, periodsym, becomessym, 
	beginsym, endsym, ifsym, thensym, whilesym, 
	dosym, callsym,	constsym, varsym, procsym, 
	writesym, readsym, elsesym, returnsym
	
} token_type;

//Struct Tokens declaration
typedef struct
{
	int type;
	char value[12];
	
} tokens;

//Struct instruction declaration
typedef struct instruction
{
	int opcode;
	char opc[4];
	int l;
	int m;
	
} instruction;


//Struct symbol declaration
typedef struct symbol
{
	int kind;
	char name[12];
	int val;
	int level;
	int addr;
	int mark;
	int param;
	
} symbol;

// Functions prototypes
tokens * lex_analyze(char *inputfile, int print);
void execute(instruction *code, int print);
instruction * parser(tokens *tokenList, int print);
