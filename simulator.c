// Matthew Husted
// Simulator

/* instruction-level simulator for LC3101 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define NUMMEMORY 65536 /* maximum number of words in memory */
#define NUMREGS 8 /* number of machine registers */
#define MAXLINELENGTH 1000
#define TRUE 1
#define FALSE 0

typedef struct stateStruct {
    int pc;
    int mem[NUMMEMORY];
    int reg[NUMREGS];
    int numMemory;
} stateType;

void printState(stateType *);
int convertNum(int num);

int
main(int argc, char *argv[])
{
    char line[MAXLINELENGTH];
    stateType state;
    FILE *filePtr;

    if (argc != 2) {
	printf("error: usage: %s <machine-code file>\n", argv[0]);
	exit(1);
    }

    filePtr = fopen(argv[1], "r");
    if (filePtr == NULL) {
	printf("error: can't open file %s", argv[1]);
	perror("fopen");
	exit(1);
    }

    /* read in the entire machine-code file into memory */
    for (state.numMemory = 0; fgets(line, MAXLINELENGTH, filePtr) != NULL;
	state.numMemory++) {
	if (sscanf(line, "%d", state.mem+state.numMemory) != 1) {
	    printf("error in reading address %d\n", state.numMemory);
	    exit(1);
	}
	printf("memory[%d]=%d\n", state.numMemory, state.mem[state.numMemory]);
	//int val = (state.mem[state.numMemory] >> 22) & 7;
	//printf("\n%i\n", val);
    }
    
    // sets all registers to zero '0'
	int i;
    for (i = 0; i < NUMREGS; i++)
    	state.reg[i] = 0;
    
    int halt = FALSE; // flag to get out
    int runs = 0; // counter for total runs
    while (!halt)
    {
    printState(&state);
	if (runs == 1000)
		break; // infinite loop	

	//if (state.pc > state.numMemory)
	//	break; // exceeded memory
	
	//printf("%i\n", ((state.mem[runs]) & 7));
    	if (((state.mem[state.pc] >> 22) & 7) == 0)
    	{
		// add
    		//printf("%i\n", state.reg[((state.mem[runs]) & 7)]);
			int regA = ((state.mem[state.pc] >> 19) & 7);
			int regB = ((state.mem[state.pc] >> 16) & 7);
			int dest = ((state.mem[state.pc]) & 7);

    		state.reg[dest] = state.reg[regA] + state.reg[regB];
    				
    	}
    	else if (((state.mem[state.pc] >> 22) & 7) == 1)
    	{
    		//printf("nand\n");
			int regA = ((state.mem[state.pc] >> 19) & 7);
			int regB = ((state.mem[state.pc] >> 16) & 7);
			int dest = ((state.mem[state.pc]) & 7);

			state.reg[dest] = ~(state.reg[regA] & state.reg[regB]);
    	}  
    	else if (((state.mem[state.pc] >> 22) & 7) == 2)
    	{
    		//printf("lw\n");
			int regA = ((state.mem[state.pc] >> 19) & 7);
			int regB = ((state.mem[state.pc] >> 16) & 7);
			int offSet = ((state.mem[state.pc]) & 65535);
			offSet = convertNum(offSet);
			state.reg[regB] = state.mem[state.reg[regA] + offSet];     		

    	} 
    	else if (((state.mem[state.pc] >> 22) & 7) == 3)
    	{
    		//printf("sw\n");
			int regA = ((state.mem[state.pc] >> 19) & 7);
			int regB = ((state.mem[state.pc] >> 16) & 7);
			int offSet = ((state.mem[state.pc]) & 65535);
			offSet = convertNum(offSet);
			state.mem[state.reg[regA] + offSet] = state.reg[regB];
    	} 
    	else if (((state.mem[state.pc] >> 22) & 7) == 4)
    	{
    		//printf("beq\n");
			int regA = ((state.mem[state.pc] >> 19) & 7);
			int regB = ((state.mem[state.pc] >> 16) & 7);
			int offSet = ((state.mem[state.pc]) & 65535);
			offSet = convertNum(offSet);

			//printf("RegA is %i\n", regA);
    		//printf("RegB is %i\n", regB);
    		
    		if (state.reg[regA] == state.reg[regB])
    		{
    			//printf("RegA = %i\tRegB = %i\n", state.reg[regA], state.reg[regB]);
    			state.pc =  state.pc + offSet;
    		}
    	} 
    	else if (((state.mem[state.pc] >> 22) & 7) == 5)
    	{
    		//printf("jalr\n");
			int regA = ((state.mem[state.pc] >> 19) & 7);
			int regB = ((state.mem[state.pc] >> 16) & 7);
			int dest = ((state.mem[state.pc]) & 7);


			state.reg[dest] = state.reg[regA];
			state.pc = state.reg[regB];
	
    		state.reg[((state.mem[runs] >> 16) & 7)] = state.pc + 1;
    		state.pc = state.reg[((state.mem[runs] >> 19) & 7)];
    	
    	} 
    	else if (((state.mem[state.pc] >> 22) & 7) == 6)
    	{
    		//printf("halt\n");
    		//state.pc++;
    		halt = TRUE;
    	} 
    	else if (((state.mem[state.pc] >> 22) & 7) == 7)
    	{
    		//printf("noop\n");
    		//state.pc++;
    	}
    	state.pc++;
		runs++;
    }
    
    // final state of machine output
    printf("end state\n");
    printf("machine halted\n");
    printf("total of %i instructions executed\n", runs);
    printf("final state of machine:\n");
    printState(&state);

    return(0);
}

void
printState(stateType *statePtr)
{
    int i;
    printf("\n@@@\nstate:\n");
    printf("\tpc %d\n", statePtr->pc);
    printf("\tmemory:\n");
	for (i=0; i<statePtr->numMemory; i++) {
	    printf("\t\tmem[ %d ] %d\n", i, statePtr->mem[i]);
	}
    printf("\tregisters:\n");
	for (i=0; i<NUMREGS; i++) {
	    printf("\t\treg[ %d ] %d\n", i, statePtr->reg[i]);
	}
    printf("end state\n");
}

int
convertNum(int num)
{
	/* convert a 16-bit number into a 32-bit Sun integer */
	if (num & (1<<15) ) {
	    num -= (1<<16);
	}
	return(num);
}