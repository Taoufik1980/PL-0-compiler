/* Taoufik Laaroussi 
4/16/2021
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "compiler.h"

// static variables
#define MAX_STACK_HEIGHT 500
// #define MAX_CODE_LENGTH 500 

// function prototype
int base(int stack[], int level, int BP);

void execute(instruction *code, int print){

int stack[MAX_STACK_HEIGHT] = { 0 };
int stackARtrack[MAX_STACK_HEIGHT] = { 0 };
int SP = -1; // stack pointer reg
int BP = 0; // Base pointer reg
int PC = 0; // Program Counter reg
int Halt = 1;

    instruction IR;

    if (print == 1)
    {
        printf("\n%15s %4s %4s %4s %8s\n", "", "PC", "BP", "SP", "stack"); // Printing header
        printf("Initial values: %4d %4d %4d\n", PC, BP, SP); // Printing intial values
    }

 while(Halt == 1){

        // fetch cycle
        int prevPC = PC; // saving previous PC
        IR = code[PC];
        PC++;

        // Execution Cycle
    switch(IR.opcode){
        case 1: // LIT
            SP++;
            stack[SP] = IR.m;
            break;
        case 2: // OPR
            switch(IR.m){
                case 0: //RET or RTN
                    stackARtrack[BP] = 0;
                    stack[BP-1] = stack[SP];
                    SP = BP - 1;
                    BP = stack[SP + 2];
                    PC = stack[SP + 3];
                    
                    break;
                case 1: //NEG
                    stack[SP] = -1 * stack [SP];
                    break;
                case 2: // SDD
                    SP--;
                    stack[SP] += stack[SP + 1];
                    break;
                case 3: //SUB
                    SP--;
                    stack[SP] -=  stack[SP + 1];
                    break;
                case 4: //MUL
                    SP--;
                    stack[SP] *=  stack[SP + 1];
                    break;
                case 5: // DIV
                    SP--;
                    stack[SP] /= stack[SP + 1];
                    break;
                case 6: //ODD
                    stack[SP] %= 2;
                    break;
                case 7: //MOD
                    SP--;
                    stack[SP] %= stack[SP + 1];
                    break;
                case 8: // EQL
                    SP--;
                    stack[SP] = stack[SP] == stack[SP + 1];
                    break;
                case 9: // NEQ
                    SP--;
                    stack[SP] = stack[SP] != stack[SP + 1];
                    break;
                case 10: // LSS
                    SP--;
                    stack[SP] = stack[SP] < stack[SP + 1];
                    break;
                case 11: //LEQ
                    SP--;
                    stack[SP] = stack[SP] <= stack[SP + 1];
                    break;
                case 12: // GTR
                    SP--;
                    stack[SP] = stack[SP] > stack[SP + 1];
                    break;
                case 13: // GRQ
                    SP--;
                    stack[SP] = stack[SP] >= stack[SP + 1];
                    break;
                default:
                        break;
            }
            break;
            case 3: // LOD
                SP++;
                stack[SP] = stack[base(stack, IR.l, BP) + IR.m];
                break;
            case 4: // STO
                stack[base(stack, IR.l, BP) + IR.m] = stack[SP];
                SP--;
                break;
            case 5: // CAL
                stack[SP+1] = base(stack,IR.l,BP);
                stack[SP+2] = BP;
                stack[SP+3] = PC;
                stack[SP+4] = stack[SP];
                BP = SP + 1;
                PC = IR.m;
                stackARtrack[BP] = 1;
                break;
            case 6: // INC
                SP = SP + IR.m;
                break;
            case 7: // JMP
                PC = IR.m;
                break;
            case 8: // JPC
                if(stack[SP] == 0)
                    PC = IR.m;
                SP--;
                break;
            case 9: //SYS
                switch(IR.m){
                    case 1: // TOP element in the screen
                        if (print == 1)
                            printf("Top of Stack Value: %d\n", stack[SP]);
                        SP--;
                        break;
                    case 2: // Store input in top of stack
                        SP++;
                        if (print == 1)
                            printf("Please Enter an Integer: ");
                        scanf("%d", &stack[SP]);
                        if (print == 1)
                            printf("\n");
                        break;
                    case 3: // End of Program
                        Halt = 0;
                        break;
                    default:
                        break;
                }
                break;
            default:
                  break;
    }

    // Outputs the values of previous PC, three letter opcode, L, M, PC, BP, SP
     if (print == 1)
     {
        printf("%2d %2s %2d %4d %5d %4d %4d   ", prevPC, IR.opc, IR.l, IR.m, PC, BP, SP);

                // Outputs the contents of the stack with AR handling using "|"
                for(int k =0 ; k <= SP; k++){
                    if(stackARtrack[k] == 1 && k<BP+1){
                        printf(" | ");
                        printf("%2d ", stack[k]);
                    }else{
                        printf("%2d ", stack[k]);
                    }
                    }
     }
    if (print == 1)
        printf("\n");
 }
    //return 0;
}

// Given function to find a variable in a different Activation Record some L Levels down 
int base(int stack[], int level, int BP)
{
    int base = BP;
    int counter = level;
    while (counter > 0)
    {
        base = stack[base];
        counter--;
    }
    return base;
}
