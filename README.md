processor-simulator
===================

Assembler, Simple Instruction Level Processor Simulator, and Pipeline Simulator. Written for computer orginization 2. 

Assembler 
----
Assembles Mips code and outputs a file that the simulators can read. 


`usage: ./a.out <assembly-code-file> <machine-code-file>` <br>

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


