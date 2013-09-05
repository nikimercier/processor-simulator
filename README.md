processor-simulator
===================

Assembler, Simple Instruction Level Processor Simulator, and Pipeline Simulator.   
Processor has 8 registers.    
Written for computer orginization 2. 

Assembler 
----
Assembles Mips code and outputs a file that the simulators can read. 


`usage: ./a.out <assembly-code-file> <machine-code-file>`     

Example Input:
```
lw	0	1	five	
lw	1	2	3		
start	add	1	2	1		
beq	0	1	2		
beq	0	0	
noop
done	halt				
five	.fill	5
neg1	.fill	-1
stAddr	.fill	start	    
```
Example Output:
```
8454151
9043971
655361
16842754
16842749
29360128
25165824
5
-1
2
```
Instructions:
*   add   
*   nand   
*   lw   
*   sw   
*   beq   
*   jalr   
*   halt   
*   noop   

Instruction Level Processor Simulator
---

Reads files assembles by the assembbler. Shows every step of the processor including    
whats in each register on each state.

`usage: ./a.out <machine-code file>` 

Snippet of execution:

```
@@@
state:
	pc 4
	memory:
		mem[ 0 ] 8454151
		mem[ 1 ] 9043971
		mem[ 2 ] 655361
		mem[ 3 ] 16842754
		mem[ 4 ] 16842749
		mem[ 5 ] 29360128
		mem[ 6 ] 25165824
		mem[ 7 ] 5
		mem[ 8 ] -1
		mem[ 9 ] 2
	registers:
		reg[ 0 ] 0
		reg[ 1 ] 2
		reg[ 2 ] -1
		reg[ 3 ] 0
		reg[ 4 ] 0
		reg[ 5 ] 0
		reg[ 6 ] 0
		reg[ 7 ] 0
end state

```

Pipeline Simulator
----

5 Stage Pileline simulator.     
Reads in the same files as the instruction level processor simulator except uses
a 5 stage pipeline to complete. 

`usage: ./a.out <machine-code file>`     

Snippet of execution:

```
@@@
state before cycle 35 starts
	pc 8
	data memory:
		dataMem[ 0 ] 8454151
		dataMem[ 1 ] 9043971
		dataMem[ 2 ] 655361
		dataMem[ 3 ] 16842754
		dataMem[ 4 ] 16842749
		dataMem[ 5 ] 29360128
		dataMem[ 6 ] 25165824
		dataMem[ 7 ] 5
		dataMem[ 8 ] -1
		dataMem[ 9 ] 2
	registers:
		reg[ 0 ] 0
		reg[ 1 ] 0
		reg[ 2 ] -1
		reg[ 3 ] 0
		reg[ 4 ] 0
		reg[ 5 ] 0
		reg[ 6 ] 0
		reg[ 7 ] 0
	IFID:
		instruction add 0 0 5
		pcPlus1 8
	IDEX:
		instruction halt 0 0 0
		pcPlus1 7
		readRegA 0
		readRegB 0
		offset 0
	EXMEM:
		instruction noop 0 0 0
		branchTarget 7
		aluResult 1
		readRegB 0
	MEMWB:
		instruction noop 0 0 0
		writeData 0
	WBEND:
		instruction noop 0 0 0
		writeData 0
```

