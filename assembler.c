// Matthew Husted
// Assembler

/* Assembler code fragment for LC3101 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MAXLINELENGTH 1000
#define TRUE 1
#define	FALSE 0

int readAndParse(FILE *, char *, char *, char *, char *, char *);
int getNumber(char *);
int getOpCode(char *);

int main(int argc, char *argv[])
{
    char *inFileString, *outFileString;
    FILE *inFilePtr, *outFilePtr;
    char label[MAXLINELENGTH], opcode[MAXLINELENGTH], arg0[MAXLINELENGTH],
            arg1[MAXLINELENGTH], arg2[MAXLINELENGTH];

	int counter = 0;
	char binary[32];

	int decimal = 0;
	int temp = 0;

	char * labels[100];

    if (argc != 3) {
        printf("error: usage: %s <assembly-code-file> <machine-code-file>\n",
            argv[0]);
        exit(1);
    }

    inFileString = argv[1];
    outFileString = argv[2];

    inFilePtr = fopen(inFileString, "r");
    if (inFilePtr == NULL) {
        printf("error in opening %s\n", inFileString);
        exit(1);
    }
    outFilePtr = fopen(outFileString, "w");
    if (outFilePtr == NULL) {
        printf("error in opening %s\n", outFileString);
        exit(1);
    }

		while(readAndParse(inFilePtr, label, opcode, arg0, arg1, arg2))
		{			
			if (label == '\0')
			{
				strcpy(labels[counter], '\0');
			}
			else 
			{
				labels[counter] = (char *) malloc(100);
				strcpy(labels[counter], label);
			}

			counter++;
		}
/*
		int k;
		for(k = 0; k < counter; k++)
		{
			if (strcmp(labels[k], ""))
				printf("\n%s - %i\n", labels[k], k);
		}
		printf("\n");
*/
	    rewind(inFilePtr);
		
		counter = 0;
		while(readAndParse(inFilePtr, label, opcode, arg0, arg1, arg2))
		{
			if (!strcmp(opcode, "add") || !strcmp(opcode, "nand")) 
			{
        
				decimal = getOpCode(opcode) << 22;
				if (isNumber(arg0))
					if (getNumber(arg0) != -1)
						decimal+= getNumber(arg0) << 19;
					else 
					{
						printf("Register not available. - %s\n", arg0);
						exit(1);
					}
				else
				{
					printf("Labels not available.\n");
					exit(1);
				}
				if (isNumber(arg1))
					if (getNumber(arg1) != -1)
						decimal+= getNumber(arg1) << 16;
					else 
					{
						printf("Register not available. - %s\n", arg1);
						exit(1);
					}
				else
				{
					printf("Labels not available.\n");
					exit(1);
				}
				if (isNumber(arg2))
					if (getNumber(arg2) != -1)
						decimal+= getNumber(arg2);
					else 
					{
						printf("Register not available. - %s\n", arg2);
						exit(1);
					}
				else
				{
					printf("Labels not available.\n");
					exit(1);
				}
				
				fprintf(outFilePtr, "%i\n", decimal);
				
    		}

			if (!strcmp(opcode, "noop") || !strcmp(opcode, "halt"))
			{
				decimal = getOpCode(opcode) << 22;
				fprintf(outFilePtr, "%i\n", decimal);
			}
		
			if (!strcmp(opcode, ".fill"))
			{
				int k;
				int lab = FALSE;
				if (!isNumber(arg0))
				{
					for(k = 0; k < 100; k++)
					{
						if (!strcmp(labels[k], arg0))
						{	
							decimal = k; 
							break;
						}
						//else printf("Labels does not exist.\n");
					}
				}
				else 
				{
					decimal = atoi(arg0);
				}
				fprintf(outFilePtr, "%i\n", decimal);
			}

			if (!strcmp(opcode, "lw") || !strcmp(opcode, "sw"))
			{
				decimal = getOpCode(opcode) << 22;
				int k;
				int lab = FALSE;
				if (!isNumber(arg2))
				{
					for(k = 0; k < 100; k++)
					{
						if (!strcmp(labels[k], arg2))
						{	
							decimal += k << 0; 
							break;
						}
						//else printf("Labels does not exist.\n");
					}
				}
				if (!lab)
					decimal += atoi(arg2);

				if (isNumber(arg0))
					if (getNumber(arg0) != -1)
						decimal+= getNumber(arg0) << 19;
					else 
					{
						printf("Register not available. - %s\n", arg0);
						exit(1);
					}
				else
				{
					printf("Labels not available.\n");
					exit(1);
				}
				if (isNumber(arg1))
					if (getNumber(arg1) != -1)
						decimal+= getNumber(arg1) << 16;
					else 
					{
						printf("Register not available. - %s\n", arg1);
						exit(1);
					}
				else
				{
					printf("Labels not available.\n");
					exit(1);
				}
				
				fprintf(outFilePtr, "%i\n", decimal);
			}

			if (!strcmp(opcode, "beq"))
			{
				decimal = getOpCode(opcode) << 22;
				int k;
				int lab = FALSE;
				if (!isNumber(arg2))
				{
					for(k = 0; k < 100; k++)
					{
						if (!strcmp(labels[k], arg2))
						{	
							int tmpDec = k - counter - 1;
							if (tmpDec < 0)
							{
								tmpDec = 65535 ^ -tmpDec;
								tmpDec = tmpDec + 1;
								decimal += tmpDec; 
							}	
							else decimal += tmpDec; 
							
							lab = TRUE;
							break;
						}
						//else printf("Labels does not exist.\n");
					}
				}
				if (!lab)
					decimal += atoi(arg2);

				if (isNumber(arg0))
					if (getNumber(arg0) != -1)
						decimal+= getNumber(arg0) << 19;
					else 
					{
						printf("Register not available. - %s\n", arg0);
						exit(1);
					}
				else
				{
					printf("Labels not available.\n");
					exit(1);
				}
				if (isNumber(arg1))
					if (getNumber(arg1) != -1)
						decimal+= getNumber(arg1) << 16;
					else 
					{
						printf("Register not available. - %s\n", arg1);
						exit(1);
					}
				else
				{
					printf("Labels not available.\n");
					exit(1);
				}
				
				fprintf(outFilePtr, "%i\n", decimal);
			}
			counter++;	
		}

    return(0);
}

/*
 * Read and parse a line of the assembly-language file.  Fields are returned
 * in label, opcode, arg0, arg1, arg2 (these strings must have memory already
 * allocated to them).
 *
 * Return values:
 *     0 if reached end of file
 *     1 if all went well
 *
 * exit(1) if line is too long.
 */
int
readAndParse(FILE *inFilePtr, char *label, char *opcode, char *arg0,
    char *arg1, char *arg2)
{
    char line[MAXLINELENGTH];
    char *ptr = line;

    /* delete prior values */
    label[0] = opcode[0] = arg0[0] = arg1[0] = arg2[0] = '\0';

    /* read the line from the assembly-language file */
    if (fgets(line, MAXLINELENGTH, inFilePtr) == NULL) {
	/* reached end of file */
        return(0);
    }

    /* check for line too long (by looking for a \n) */
    if (strchr(line, '\n') == NULL) {
        /* line too long */
	printf("error: line too long\n");
	exit(1);
    }

    /* is there a label? */
    ptr = line;
    if (sscanf(ptr, "%[^\t\n ]", label)) {
	/* successfully read label; advance pointer over the label */
        ptr += strlen(label);
    }
    /*
     * Parse the rest of the line.  Would be nice to have real regular
     * expressions, but scanf will suffice.
     */
	
	

    sscanf(ptr, "%*[\t\n ]%[^\t\n ]%*[\t\n ]%[^\t\n ]%*[\t\n ]%[^\t\n ]%*[\t\n ]%[^\t\n ]",
        opcode, arg0, arg1, arg2);

    return(1);
}

int
isNumber(char *string)
{
    /* return 1 if string is a number */
    int i;
    return( (sscanf(string, "%d", &i)) == 1);
}

int getOpCode(char * op)
{
	if (!strcmp(op, "add"))
		return 0;
	else if (!strcmp(op, "nand"))
		return 1;
	else if (!strcmp(op, "lw"))
		return 2;
	else if (!strcmp(op, "sw"))
		return 3;
	else if (!strcmp(op, "beq"))
		return 4;
	else if (!strcmp(op, "jalr"))
		return 5;
	else if (!strcmp(op, "halt"))
		return 6;
	else if (!strcmp(op, "noop"))
		return 7;
	else return -1; // returns -1 if error
}

int getNumber(char *num)
{
	if (!strcmp(num, "0"))
		return 0; 
	else if (!strcmp(num, "1"))
		return 1;
	else if (!strcmp(num, "2"))
		return 2;
	else if (!strcmp(num, "3"))
		return 3;
	else if (!strcmp(num, "4"))
		return 4;
	else if (!strcmp(num, "5"))
		return 5;
	else if (!strcmp(num, "6"))
		return 6;
	else if (!strcmp(num, "7"))
		return 7;
	else return -1; // returns -1 if error
}
