// Matthew Husted
// Pipeline Simulator

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define NUMMEMORY 65536 /* maximum number of data words in memory */
#define NUMREGS 8 /* number of machine registers */
#define MAXLINELENGTH 1000

#define ADD 0
#define NAND 1
#define LW 2
#define SW 3
#define BEQ 4
#define JALR 5  /* didn't implement */
#define HALT 6
#define NOOP 7

#define NOOPINSTRUCTION 0x1c00000
#define TRUE 1
#define FALSE 0

typedef struct IFIDStruct {
    int instr;
    int pcPlus1;
} IFIDType;

typedef struct IDEXStruct {
    int instr;
    int pcPlus1;
    int readRegA;
    int readRegB;
    int offset;
} IDEXType;

typedef struct EXMEMStruct {
    int instr;
    int branchTarget;
    int aluResult;
    int readRegB;
} EXMEMType;

typedef struct MEMWBStruct {
    int instr;
    int writeData;
} MEMWBType;

typedef struct WBENDStruct {
    int instr;
    int writeData;
} WBENDType;

typedef struct stateStruct {
    int pc;
    int instrMem[NUMMEMORY];
    int dataMem[NUMMEMORY];
    int reg[NUMREGS];
    int numMemory;
    IFIDType IFID;
    IDEXType IDEX;
    EXMEMType EXMEM;
    MEMWBType MEMWB;
    WBENDType WBEND;
    int cycles; /* number of cycles run so far */
} stateType;

void printState(stateType *);
int field0(int);
int field1(int);
int field2(int);
int opcode(int);
void printInstruction(int);

int convertNum(int);

int
main(int argc, char *argv[])
{
	
	int tempRegA; // used for forwarding
	int tempRegB;
	
	stateType state;
	stateType newState;
	
	// sets all registers to zero '0'
	int i;
	for (i = 0; i < NUMREGS; i++)
		state.reg[i] = 0;
		
	char line[MAXLINELENGTH];
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
		if (sscanf(line, "%d", state.dataMem+state.numMemory) != 1) {
	    	printf("error in reading address %d\n", state.numMemory);
	    	exit(1);
		}
	}
	
	int j;
	for (j = 0; j < state.numMemory; j++) {
		printf("memory[%d] = %d\n", j, state.dataMem[j]);
	}
	printf("%d memory words\n",j);
	printf("\tinstruction memory:\n");
	for (j = 0; j < state.numMemory; j++) {
		printf("\t\tinstrMem[ %d ] ", j);
		printInstruction(state.dataMem[j]);
		//printf("\n");
	}
	
	// initialize all instructions to NOOP
	state.IFID.instr = NOOPINSTRUCTION;
	state.IDEX.instr = NOOPINSTRUCTION;
	state.EXMEM.instr = NOOPINSTRUCTION;
	state.MEMWB.instr = NOOPINSTRUCTION;
	state.WBEND.instr = NOOPINSTRUCTION;
	
	state.pc = 0;
	state.cycles = 0;
	
	
	while (1) {
	
		if (state.cycles == 5000)
			exit(0); // infinite loop
	
		printState(&state);

		/* check for halt */
		if (opcode(state.MEMWB.instr) == HALT) {
	    	printf("machine halted\n");
	    	printf("total of %d cycles executed\n", state.cycles);
	    	exit(0);
		}

		newState = state;
		newState.cycles++;

		/* --------------------- IF stage --------------------- */
		
		newState.IFID.instr = state.dataMem[state.pc];
		newState.IFID.pcPlus1 = state.pc + 1;
		++newState.pc;
		
		/* --------------------- ID stage --------------------- */
				
		newState.IDEX.instr = state.IFID.instr;
		newState.IDEX.pcPlus1 = state.IFID.pcPlus1;
		newState.IDEX.readRegA = state.reg[field0(state.IFID.instr)];
		newState.IDEX.readRegB = state.reg[field1(state.IFID.instr)];
		newState.IDEX.offset = convertNum(field2(state.IFID.instr));
		
		// stalls for lw	
		if ((opcode(state.IDEX.instr) == LW) && 
				(((field0(newState.IDEX.instr) == field1(state.IDEX.instr)) || 
				(field1(newState.IDEX.instr) == field1(state.IDEX.instr)))))
		{
			newState.IDEX.instr = NOOPINSTRUCTION;
			newState.pc--;
			newState.IFID.pcPlus1--;
			newState.IFID.instr = state.IFID.instr;
		}		
						
		/* --------------------- EX stage --------------------- */
		
		newState.EXMEM.instr = state.IDEX.instr;
	

		// forwarding
		if ((field0(state.IDEX.instr) == field2(state.EXMEM.instr) 
				&& opcode(state.EXMEM.instr) == ADD)
		 		|| (field0(state.IDEX.instr) == field2(state.EXMEM.instr) 
				&& opcode(state.EXMEM.instr) == NAND))
		{
			tempRegA = state.EXMEM.aluResult;
		}
		else if ((field0(state.IDEX.instr) == field1(state.MEMWB.instr) 
				&& opcode(state.MEMWB.instr) == LW)
			  	|| (field0(state.IDEX.instr) == field2(state.MEMWB.instr) 
				&& opcode(state.MEMWB.instr) == ADD)
			  	|| (field0(state.IDEX.instr) == field2(state.MEMWB.instr) 
				&& opcode(state.MEMWB.instr) == NAND))
		{
			tempRegA = state.MEMWB.writeData;
		}
		else if ((field0(state.IDEX.instr) == field1(state.WBEND.instr) 
				&& opcode(state.WBEND.instr) == LW)
			  	|| (field0(state.IDEX.instr) == field2(state.WBEND.instr) 
				&& opcode(state.WBEND.instr) == ADD)
			  	|| (field0(state.IDEX.instr) == field2(state.WBEND.instr) 
				&& opcode(state.WBEND.instr) == NAND))
		{
			tempRegA = state.WBEND.writeData;
		}		
		else 
			tempRegA = state.IDEX.readRegA;
	
		if ((field1(state.IDEX.instr) == field2(state.EXMEM.instr) 
				&& opcode(state.EXMEM.instr) == ADD)
		 		|| (field1(state.IDEX.instr) == field2(state.EXMEM.instr) 
				&& opcode(state.EXMEM.instr) == NAND))
		{
			tempRegB = state.EXMEM.aluResult;
		}
		else if ((field1(state.IDEX.instr) == field1(state.MEMWB.instr) 
				&& opcode(state.MEMWB.instr) == LW)
			  	|| (field1(state.IDEX.instr) == field2(state.MEMWB.instr) 
				&& opcode(state.MEMWB.instr) == ADD)
			  	|| (field1(state.IDEX.instr) == field2(state.MEMWB.instr) 
				&& opcode(state.MEMWB.instr) == NAND))
		{
			tempRegB = state.MEMWB.writeData;
		}
		else if ((field1(state.IDEX.instr) == field1(state.WBEND.instr) 
				&& opcode(state.WBEND.instr) == LW)
			  	|| (field1(state.IDEX.instr) == field2(state.WBEND.instr)
			 	&& opcode(state.WBEND.instr) == ADD)
			  	|| (field1(state.IDEX.instr) == field2(state.WBEND.instr) 
				&& opcode(state.WBEND.instr) == NAND))
		{
			tempRegB = state.WBEND.writeData;
		}		
		else 
			tempRegB = state.IDEX.readRegB;	
		// end forwarding
		
		newState.EXMEM.branchTarget = state.IDEX.offset + state.IDEX.pcPlus1;	
		newState.EXMEM.readRegB = tempRegB;
	
		if (opcode(state.IDEX.instr) == ADD)
		{
			newState.EXMEM.aluResult  = tempRegA + tempRegB;
		}
		else if (opcode(state.IDEX.instr) == NAND)
		{
			newState.EXMEM.aluResult  = ~(tempRegA & tempRegB);
		}		
		else if (opcode(state.IDEX.instr) == BEQ)
		{
			if (tempRegA == tempRegB)
			{
				newState.EXMEM.aluResult = 1;
			}
			else
			{
				newState.EXMEM.aluResult = 0;
			}	
		}
		else if (opcode(state.IDEX.instr) == LW)
		{
			newState.EXMEM.aluResult  = tempRegA + state.IDEX.offset;
		}
		else if (opcode(state.IDEX.instr) == SW)
		{
			newState.EXMEM.aluResult  = tempRegA + state.IDEX.offset;
		}
		
		/* --------------------- MEM stage --------------------- */

		newState.MEMWB.instr = state.EXMEM.instr;
		
		if (opcode(newState.MEMWB.instr) == BEQ) // branch prediction
		{
			if (state.EXMEM.aluResult == 1)
			{
				newState.pc = state.EXMEM.branchTarget;
				newState.IDEX.instr = NOOPINSTRUCTION;
				newState.IFID.instr = NOOPINSTRUCTION;
				newState.EXMEM.instr = NOOPINSTRUCTION;
			}
		}
		else if (opcode(state.EXMEM.instr) == LW)
		{	
			newState.MEMWB.writeData = state.dataMem[state.EXMEM.aluResult];
		}
		else if (opcode(state.EXMEM.instr) == SW)
		{
			newState.MEMWB.writeData = state.EXMEM.aluResult;
		}
		else if (opcode(newState.MEMWB.instr) == ADD || opcode(newState.MEMWB.instr) == NAND)
		{
			newState.MEMWB.writeData = state.EXMEM.aluResult;
		}
	
		/* --------------------- WB stage --------------------- */

		newState.WBEND.instr = state.MEMWB.instr;
		newState.WBEND.writeData = state.MEMWB.writeData;
		
		if (opcode(state.MEMWB.instr) == ADD || opcode(state.MEMWB.instr) == NAND)
		{
			newState.reg[field2(newState.WBEND.instr)] = newState.WBEND.writeData;
		}
		else if (opcode(state.MEMWB.instr) == SW)
		{
			// fix this
			newState.dataMem[newState.WBEND.writeData] = 
					newState.reg[field1(newState.WBEND.instr)];
		}
		else if (opcode(state.MEMWB.instr) == LW)
		{
			newState.reg[field1(newState.WBEND.instr)] = newState.WBEND.writeData;
		}
		
		/* --------------------- End stages --------------------- */
		state = newState; /* this is the last statement before end of the loop.
			    	It marks the end of the cycle and updates the
			    	current state with the values calculated in this
			    	cycle */
	}

    return(0);
}


void
printState(stateType *statePtr)
{
    int i;
    printf("\n@@@\nstate before cycle %d starts\n", statePtr->cycles);
    printf("\tpc %d\n", statePtr->pc);

    printf("\tdata memory:\n");
	for (i=0; i<statePtr->numMemory; i++) {
	    printf("\t\tdataMem[ %d ] %d\n", i, statePtr->dataMem[i]);
	}
    printf("\tregisters:\n");
	for (i=0; i<NUMREGS; i++) {
	    printf("\t\treg[ %d ] %d\n", i, statePtr->reg[i]);
	}
    printf("\tIFID:\n");
	printf("\t\tinstruction ");
	printInstruction(statePtr->IFID.instr);
	printf("\t\tpcPlus1 %d\n", statePtr->IFID.pcPlus1);
    printf("\tIDEX:\n");
	printf("\t\tinstruction ");
	printInstruction(statePtr->IDEX.instr);
	printf("\t\tpcPlus1 %d\n", statePtr->IDEX.pcPlus1);
	printf("\t\treadRegA %d\n", statePtr->IDEX.readRegA);
	printf("\t\treadRegB %d\n", statePtr->IDEX.readRegB);
	printf("\t\toffset %d\n", statePtr->IDEX.offset);
    printf("\tEXMEM:\n");
	printf("\t\tinstruction ");
	printInstruction(statePtr->EXMEM.instr);
	printf("\t\tbranchTarget %d\n", statePtr->EXMEM.branchTarget);
	printf("\t\taluResult %d\n", statePtr->EXMEM.aluResult);
	printf("\t\treadRegB %d\n", statePtr->EXMEM.readRegB);
    printf("\tMEMWB:\n");
	printf("\t\tinstruction ");
	printInstruction(statePtr->MEMWB.instr);
	printf("\t\twriteData %d\n", statePtr->MEMWB.writeData);
    printf("\tWBEND:\n");
	printf("\t\tinstruction ");
	printInstruction(statePtr->WBEND.instr);
	printf("\t\twriteData %d\n", statePtr->WBEND.writeData);
}

int
field0(int instruction)
{
    return( (instruction>>19) & 0x7);
}

int
field1(int instruction)
{
    return( (instruction>>16) & 0x7);
}

int
field2(int instruction)
{
    return(instruction & 0xFFFF);
}

int opcode(int instruction)
{
    return(instruction>>22);
}

void
printInstruction(int instr)
{
    char opcodeString[10];
    if (opcode(instr) == ADD) {
	strcpy(opcodeString, "add");
    } else if (opcode(instr) == NAND) {
	strcpy(opcodeString, "nand");
    } else if (opcode(instr) == LW) {
	strcpy(opcodeString, "lw");
    } else if (opcode(instr) == SW) {
	strcpy(opcodeString, "sw");
    } else if (opcode(instr) == BEQ) {
	strcpy(opcodeString, "beq");
    } else if (opcode(instr) == JALR) {
	strcpy(opcodeString, "jalr");
    } else if (opcode(instr) == HALT) {
	strcpy(opcodeString, "halt");
    } else if (opcode(instr) == NOOP) {
	strcpy(opcodeString, "noop");
    } else {
	strcpy(opcodeString, "data");
    }

    printf("%s %d %d %d\n", opcodeString, field0(instr), field1(instr), field2(instr));
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