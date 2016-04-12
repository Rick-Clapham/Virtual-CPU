/******************************************
* Name: Richard Clapham
* Student #: 821-490-125
* Student #: n00663650
* Date: 4/7/2016
* Virtual CPU project
******************************************/

#include "main.h"

char cmd = 'q';
char buffer[16000];

/**
Used to display error messages currently not being used
**/
void error(char *msg)
{
    perror(msg);
    exit(0);
}

/**
Function designed to execute the commands given
@param instruction - Receives the current instruction
@param type - Receives and determines the type of instruction
@param memptr - Receives the Virtual CPU's buffer
**/
void execute(uint16_t instruction, uint16_t type, void *memptr) 
{
	Current_IR = instruction;
	Current_TYPE = type;

	//Data Process
	if (type == DATA_PROCESS) {
		if(DATA_OP == DATA_ADC){	//Add with Carry
			regs.ALU = regs.R[DATA_RD] + DATA_RN + regs.CARRY;
			regs.R[DATA_RD] = regs.ALU;
			flagsCheck(regs.R[DATA_RD], regs.R[DATA_RN], 1);
		}
		else if(DATA_OP == DATA_ADD){	//Add / Add Immediate
			regs.ALU = regs.R[DATA_RD] + regs.R[DATA_RN];
			regs.R[DATA_RD] = regs.ALU;
			flagsCheck(regs.R[DATA_RD], regs.R[DATA_RN], 1);
		}
		else if(DATA_OP == DATA_AND){	//And
			regs.ALU = regs.R[DATA_RD] & regs.R[DATA_RN];
			regs.R[DATA_RD] = regs.ALU;
			flagsCheck(regs.R[DATA_RD], regs.R[DATA_RN], 0);
		}
		else if(DATA_OP == DATA_BIC){	//Bit Clear
			regs.ALU = regs.R[DATA_RD] & ~regs.R[DATA_RN];
			regs.R[DATA_RD] = regs.ALU;
			flagsCheck(regs.R[DATA_RD], regs.R[DATA_RN], 0);
		}
		else if(DATA_OP == DATA_CMP){	//Compare / Compare Immediate
			regs.ALU = regs.R[DATA_RD] & ~regs.R[DATA_RN] + 1;
			flagsCheck(regs.R[DATA_RD], regs.R[DATA_RN], 0);
		}
		else if(DATA_OP == DATA_EOR){	//Exclusive Or
			regs.ALU = regs.R[DATA_RD] ^ regs.R[DATA_RN];
			regs.R[DATA_RD] = regs.ALU;
			flagsCheck(regs.R[DATA_RD], regs.R[DATA_RN], 0);
		}
		else if(DATA_OP == DATA_LSL){	//Logical Shift Left
			regs.ALU = regs.R[DATA_RD] << regs.R[DATA_RN];
			regs.R[DATA_RD] = regs.ALU;
			flagsCheck(regs.R[DATA_RD], regs.R[DATA_RN], 1);
		}
		else if(DATA_OP == DATA_LSR){	//Logical Shift Right
			regs.ALU = regs.R[DATA_RD] >> regs.R[DATA_RN];
			regs.R[DATA_RD] = regs.ALU;
			flagsCheck(regs.R[DATA_RD], regs.R[DATA_RN], 1);
		}
		else if(DATA_OP == DATA_MOV){	//Move / Move Immediate
			regs.R[DATA_RD] = regs.R[DATA_RN];
			flagsCheck(regs.R[DATA_RD], regs.R[DATA_RN], 0);
		}
		else if(DATA_OP == DATA_MVN){	//Move Not
			regs.R[DATA_RD] = ~regs.R[DATA_RN];
			flagsCheck(regs.R[DATA_RD], regs.R[DATA_RN], 0);
		}
		else if(DATA_OP == DATA_ORR){	//Or
			regs.ALU = regs.R[DATA_RD] | regs.R[DATA_RN];
			regs.R[DATA_RD] = regs.ALU;
			flagsCheck(regs.R[DATA_RD], regs.R[DATA_RN], 0);
		}
		else if(DATA_OP == DATA_ROR){	//Rotate Right
			for (int i = 0; i < regs.R[DATA_RN]; i++)
			{
				regs.CARRY = regs.R[DATA_RD] & 0x0001;
				if (regs.CARRY)
					regs.ALU = (regs.R[DATA_RD] >> 1) | 0x80000000;
			}
			regs.R[DATA_RD] = regs.ALU;
			flagsCheck(regs.R[DATA_RD], regs.R[DATA_RN], 0);
		}
		else if(DATA_OP == DATA_SUB){	//Subtract / Subtract Immediate
			regs.ALU = regs.R[DATA_RD] + ~regs.R[DATA_RN] + 1;
			regs.R[DATA_RD] = regs.ALU;
			flagsCheck(regs.R[DATA_RD], regs.R[DATA_RN], 0);
		}
		else if(DATA_OP == DATA_SXB){	//Sign Extend Byte
			regs.R[DATA_RD] = (signed)regs.R[DATA_RN];
			flagsCheck(regs.R[DATA_RD], regs.R[DATA_RN], 0);
		}
		else if(DATA_OP == DATA_TEQ){	//Test Equivalence
			regs.ALU = regs.R[DATA_RD] ^ regs.R[DATA_RN] + 1;
			flagsCheck(regs.R[DATA_RD], regs.R[DATA_RN], 0);
		}
		else if(DATA_OP == DATA_TST){	//Test Bits
			regs.ALU = regs.R[DATA_RD] & regs.R[DATA_RN];
			flagsCheck(regs.R[DATA_RD], regs.R[DATA_RN], 0);
		}
		else{
			perror("Operation not found!");
		}
	}
	//Load / Store
	else if (type == LOAD_STORE) {
		uint32_t mask = 0xFF000000;
		if(LOAD_STORE_B == 0){	//Transfer Word
			if(LOAD_STORE_L == 0){	//Store to Memory
				regs.MAR = regs.R[LOAD_STORE_RN];
				regs.MBR = regs.R[LOAD_STORE_RD];
				for(int i = 0; i < 4; i++){
					/* Move MSB first into memory, then the rest */
					((uint8_t *)memptr)[regs.MAR++] = (regs.MBR & mask) >> (8 * (3 - i));
					mask = mask >> 8;
				}
			}
			else if(LOAD_STORE_L == 1){	//Load from Memory
				regs.MAR = regs.R[LOAD_STORE_RN];
				regs.MBR = regs.R[LOAD_STORE_RD];
				((uint8_t *) memptr)[regs.MAR] = regs.MBR;
			}
		}
		else if(LOAD_STORE_B == 1){	//Transfer Byte
			if(LOAD_STORE_L == 0){	//Store to Memory
				regs.MAR = regs.R[LOAD_STORE_RN];
				for(int i = 0; i < 4; i++){
					regs.MBR = regs.MBR << 8;
					regs.MBR += ((uint8_t *) memptr)[regs.MAR++];
				}
				regs.R[LOAD_STORE_RD] = regs.MBR;
			}
			else if(LOAD_STORE_L == 1){	//Load from Memory
				regs.MAR = regs.R[LOAD_STORE_RN] & 0x000000FF;
				regs.MBR = ((uint8_t *) memptr)[regs.MAR];
				regs.R[LOAD_STORE_RD] = regs.MBR;
			}
		}
	}
	//Immediate Operations
	else if (type == IMMEDIATE) {
		if (IMM_OP == IMM_MOV) {	//Move
			regs.ALU = IMM_VALUE;
			regs.R[IMM_RD] = regs.ALU;
			flagsCheck(regs.R[IMM_RD], IMM_VALUE, 0);
		}
		else if (IMM_OP == IMM_CMP) {	//Compare
			regs.ALU = regs.R[IMM_RD] - IMM_VALUE;
			flagsCheck(regs.ALU, IMM_VALUE, 1);
		}
		else if (IMM_OP == IMM_ADD) {	//Add
			regs.ALU = regs.R[IMM_RD] + IMM_VALUE;
			regs.R[IMM_RD] = regs.ALU;
			flagsCheck(regs.R[IMM_RD], IMM_VALUE, 1);
		}
		else if (IMM_OP == IMM_SUB) {	//Subtract
			regs.ALU = regs.R[IMM_RD] + ~IMM_VALUE + 1;
			regs.R[IMM_RD] = regs.ALU;
			flagsCheck(regs.R[IMM_RD], IMM_VALUE, 1);
		}
	}
	//Conditional Branch
	else if (type == CON_BRANCH) {
		/* Use hold value to sign CBRANCH_ADDR */
		int8_t temp = CBRANCH_ADDR;
		int32_t offset = (int32_t)temp;
		/* Decrement PC to reset the branch fetch */
		regs.PC -= 2;

		if(CBRANCH_COND == CBRANCH_EQ){	//Equal	(Zero Set)
			if (regs.ZERO == true)
				regs.PC += offset;
		}
		else if (CBRANCH_COND == CBRANCH_NE) {	//Not Equal	(Zero Clear)
			if (regs.ZERO == false)
				regs.PC += offset;
		}
		else if (CBRANCH_COND == CBRANCH_CS) {	//Unsigned Higher or Same	(Carry Set)
			if (regs.CARRY = true)
				regs.PC += offset;
		}
		else if (CBRANCH_COND == CBRANCH_CC) {	//Unsigned Lower	(Carry Clear)
			if (regs.CARRY = false)
				regs.PC += offset;
		}
		else if (CBRANCH_COND == CBRANCH_MI) {	//Negative	(Sign Set)
			if (regs.SIGN == true)
				regs.PC += offset;
		}
		else if (CBRANCH_COND == CBRANCH_PL) {	//Positive	(Sign Clear)
			if (regs.SIGN == false)
				regs.PC += offset;
		}
		else if (CBRANCH_COND == CBRANCH_HI) {	//Unsigned Higher	(Carry Set & Zero Clear)
			if (regs.CARRY == true && regs.ZERO == false)
				regs.PC += offset;
		}
		else if (CBRANCH_COND == CBRANCH_LS) {	//Unsigned Lower or Same	(Carry Clear || Zero Set)
			if (regs.CARRY == false && regs.ZERO == true)
				regs.PC += offset;
		}
		else if (CBRANCH_COND == CBRANCH_AL) {	//Always	(No Flags)
			regs.PC += offset;
		}
		regs.IR = 0;
		IR_FLAG = 0;
	}
	//Push / Pull
	else if (type == PUSH_PULL) {

		uint8_t current_Register;
		uint32_t mask = 0xFF000000;

		if (regs.SP == 0)
			regs.SP = MEMORY_MAX;

		fprintf(stdout, "PUSH_PULL_L: %X\nPUSH_PULL_R: %X\nPUSH_PULL_H: %X\n", PUSH_PULL_L, PUSH_PULL_R, PUSH_PULL_H);

		if(PUSH_PULL_L == 0){			// PUSH
			if (PUSH_PULL_R == 0) {		// No extra Push / Pull
				if (PUSH_PULL_H == 0) {	// Low Registers
					current_Register = 7;
					for (int i = 0x80; i > 0;){
						if ((PUSH_REG_LIST & i) != 0){
							regs.SP -= 0x0004;
							instruction = 0x20D0 + current_Register;
							type = LOAD_STORE;
							execute(instruction, type, buffer);
						}
						current_Register--;
						i = i >> 1;
					}
				}
				else if (PUSH_PULL_H == 1) {	// High Registers
					current_Register = 12;
					for (int i = 0x80; i > 0;){
						if ((PUSH_REG_LIST & i) != 0){
							regs.SP -= 0x0004;
							instruction = 0x20D0 + current_Register;
							type = LOAD_STORE;
							execute(instruction, type, buffer);
						}
						current_Register--;
						i = i >> 1;
					}
				}
			}
			else if (PUSH_PULL_R == 1) {	// Extra Push / Pull
				if (PUSH_PULL_H == 0) {		// Low Registers
					current_Register = 7;
					for (int i = 0x80; i > 0;) {
						if ((PUSH_REG_LIST & i) != 0) {
							regs.SP -= 0x0004;
							instruction = 0x20D0 + current_Register;
							type = LOAD_STORE;
							execute(instruction, type, buffer);
						}
						current_Register--;
						i = i >> 1;
					}
				}
				else if (PUSH_PULL_H == 1) {	//High Registers
					current_Register = 12;
					for (int i = 0x80; i > 0;) {
						if ((PUSH_REG_LIST & i) != 0) {
							regs.SP -= 0x0004;
							instruction = 0x20D0 + current_Register;
							type = LOAD_STORE;
							execute(instruction, type, buffer);
						}
						current_Register--;
						i = i >> 1;
					}
				}
				type = LOAD_STORE;
				execute(instruction, type, buffer);
			}
		}
		else if (PUSH_PULL_L == 1) {	// PULL
			if (PUSH_PULL_R == 0) {		// No Extra Push / Pull
				if (PUSH_PULL_H == 0) {	// Low Registers
					current_Register = 7;
					for (int i = 0x80; i > 0;) {
						if ((PUSH_REG_LIST & i) != 0) {
							instruction = 0x28D0 + current_Register;
							type = LOAD_STORE;
							execute(instruction, type, buffer);
							regs.SP -= 0x0004;
						}
						current_Register--;
						i = i >> 1;
					}
				}
				else if (PUSH_PULL_H == 1) {	// High Registers
					current_Register = 12;
					for (int i = 0x80; i > 0;) {
						if ((PUSH_REG_LIST & i) != 0) {
							instruction = 0x28D0 + current_Register;
							type = LOAD_STORE;
							execute(instruction, type, buffer);
							regs.SP -= 0x0004;
						}
						current_Register--;
						i = i >> 1;
					}
				}
			}
			else if (PUSH_PULL_R == 1) {	// Extra Push / Pull
				if (PUSH_PULL_H == 0) {		// Low Registers
					current_Register = 7;
					for (int i = 0x80; i > 0;){
						if ((PUSH_REG_LIST & i) != 0){
							regs.SP -= 0x0004;
							instruction = 0x20D0 + current_Register;
							type = LOAD_STORE;
							execute(instruction, type, buffer);
						}
						current_Register--;
						i = i >> 1;
					}
				}
				else if (PUSH_PULL_H == 1) {	// High Registers
					current_Register = 12;
					for (int i = 0x80; i > 0;)
					{
						if ((PUSH_REG_LIST & i) != 0)
						{
							regs.SP -= 0x0004;
							instruction = 0x20D0 + current_Register;
							type = LOAD_STORE;
							execute(instruction, type, buffer);
						}
						current_Register--;
						i = i >> 1;
					}
				}
				type = LOAD_STORE;
				execute(instruction, type, buffer);
			}
		}
	}
	//Unconditional Branch
	else if (type == UN_BRANCH) {
		if (UBRANCH_LINK_K == 0){	// Branch (BRA)
			regs.PC = UBRANCH_OFFSET;
		}
		else{						// Branch with Link (BRL)
			regs.LR = regs.PC;
			regs.PC = UBRANCH_OFFSET;
		}
	}
	//Stop
	else if (type == T_STOP) {
		STOP_FLAG = 1;
	}
	else {
		perror("Type Not Found!");
	}
}

/**
Function designed to fetch the first command from memory 
@param memptr - Receives the Virtual CPU's buffer
**/
void fetch(void *memptr)
{
	regs.MAR = regs.PC;
	
	for(int i = 0; i < 4; i++){
		regs.MBR = regs.MBR << 8;
		regs.MBR = regs.MBR + ((char*)memptr)[regs.MAR++];
	}
	
	regs.IR = regs.MBR;
	regs.PC = regs.PC + sizeof(uint32_t);
}

/**
Responsible for controlling the fetch and execute functions
@param memptr - Receives the Virtual CPU's buffer
**/
void instructionCycles(void *memptr)
{
	int type = 0;

	if (IR_FLAG == 0){
		fetch(buffer);
		regs.IR0 = (regs.IR >> 16);
		regs.IR1 = regs.IR;
		type = (regs.IR0 & 0xE000) >> 13;
		execute(regs.IR0, type, buffer);
		IR_FLAG = 1;
	}
	else{
		type = (regs.IR1 & 0xE000) >> 13;
		execute(regs.IR1, type, buffer);
		IR_FLAG = 0;
	}
}

/**
isCarry()- determine if carry is generated by addition: op1+op2+C
@param op1 - receives register 1
@param op2 - receives register 2
@param C - receives the current carry flag
@return int - returns 1 or 0 depending if the carry is set
**/
int isCarry(unsigned long op1,unsigned long op2, unsigned C)
{
	if ((op2 == MAX32) && (C == 1))
		return 1;	// special case where op2 is at MAX32
	return((op1 > (MAX32-op2-C))?1:0); 
}

/**
Checks to see if flags are set
@param register_1 - Recieves the destination register (primary operand)
@param register_2 - Recieves the (secondary operand)
@param flag_Check - Determines the type of flag check to run (0 = No carry, 1 = Carry)
**/
void flagsCheck(uint32_t register_1, uint32_t register_2, int flag_Check)
{
	if (flag_Check == 0) {
		if (ISSIGN(register_1))
			regs.SIGN = 1;
		else if (!ISSIGN(register_1))
			regs.SIGN = 0;
		if (ISZERO(register_1))
			regs.ZERO = 1;
		if (!ISZERO(register_1))
			regs.ZERO = 0;
	}
	else if (flag_Check == 1) {
		if (ISZERO(register_1))
			regs.ZERO = 1;
		else if (!ISZERO(register_1))
			regs.ZERO = 0;
		if (ISSIGN(register_1))
			regs.SIGN = 1;
		else if (!ISSIGN(register_1))
			regs.SIGN = 0;
		if (isCarry(register_1, register_2, regs.CARRY))
			regs.CARRY = 1;
		else if (!isCarry(register_1, register_2, regs.CARRY))
			regs.CARRY = 0;
	}
}

/**
Allows the user to view a portion of the memory 
@param memptr - Receives the Virtual CPU's buffer
@param offset - Receives the point in memory to begin viewing
@param length - Receives the length of memory to view
**/
void memDump(void *memptr, unsigned offset, unsigned length)
{
	/* Ensure offset is less than 16000 if not it defaults to 0 */
	if(offset >= MEMORY_MAX){
		offset = 0x0000;
	}
	/* Ensures that the maximum length is 16000 */
	if(length > MEMORY_MAX){
		length = MEMORY_MAX;
	}

	unsigned int end = offset + length;

	/* Ensures that if it is a valid offset and valid length it won't display data 
	that is not inside the buffer */
	while (end > MEMORY_MAX) {
		end = MEMORY_MAX;
	}

	for (int i = offset; i < end; i += HEX_COLUMN)
	{
		/* Prints the current location in memory */
		if (i != offset) putchar('\n'); {
			fprintf(stdout, "%04X\t", i);
		}

		for (int j = i; j < (i + HEX_COLUMN); j++)
		{
			/* If the length is reached before the end of the line exit loop */
			if (j == (length + offset))
			{
				fprintf(stdout, "\n\t");
				for (int k = i; k <= (length + offset - 1); k++) 
				{
					//This if statement cause the error with visual studio (runtime error only)
					if (isprint(((char*)memptr)[k])) {        
						fprintf(stdout, " %c ", ((char*)memptr)[k]);
					}
					else {                                    
						fprintf(stdout, " . ");				  
					}
				}
				break;
			}

			/* Printf the Hex values */
			fprintf(stdout, "%02X ", ((unsigned char*)memptr)[j]);

			/* Printf the ASCII character of hex values if possible, else print '.' */
			if (j == (i + HEX_COLUMN) - 1)
			{
				fprintf(stdout, "\n\t");
				for (int k = i; k < (i + HEX_COLUMN); k++)
				{
					//This if statement cause the error with visual studio (runtime error only)
					if (isprint(((char*)memptr)[k])) {
						fprintf(stdout, " %c ", ((char*)memptr)[k]);
					}
					else {
						fprintf(stdout, " . ");
					}
				}
			}
		}
	}
	fprintf(stdout, "\n");
}

void go()
{
	fprintf(stdout, "\nGo has not been implemented\n");
}

/**
Requests a file name from the user and then attempts to load the file into memory
@param buffer - Receives the Virtual CPU's buffer
@param max - Receives the Virtual CPU's maximum buffer size
@return int - Returns The file size (Returns -1 if file does not exist)
**/
int loadFile(void *memptr,  unsigned int max) 
{ 
	FILE *fp;
	char filename[50];
	int readCount;
	int check = 0;
	
	/* Prompts user for File Name */
	fprintf(stdout, "\nPlease Enter a File name:\t");
	fscanf(stdin, "%s", filename);

	/* Attempts to open file with given name will state if file is truncated or if it does not exist*/
	fp = fopen(filename, "r");
	if(fp != NULL)
	{
		fseek(fp, 0L, SEEK_END);
		check = ftell(fp);
		fseek(fp, 0L, SEEK_SET);
		readCount = fread(memptr,1,max,fp);
		fclose(fp);
		if(check > 16000){
			fprintf(stdout, "The File was Truncated");
		}else{
			fprintf(stdout, "The File was not Truncated");
		}
	}
	else
	{
		fprintf(stdout, "FILE DOES NOT EXIST\n");
		return(-1);
	}
	return readCount;
}

/**
Allows the user to modify individual bytes in memory based off of a given location
uses "." to exit
@param memptr - Receives the Virtual CPU's buffer
@param offset - Receives the point in memory to begin modifying
**/
void memmod(void *memptr, unsigned offset)
{
	/* Ensure offset is less than 16000 if not it defaults to 0 */
	if (offset >= MEMORY_MAX) {
		offset = 0x0000;
	}

	char user_input[sizeof(int) + 1];
	unsigned int value;

	while (1)
	{
		/* Displays information about current offset */
		fprintf(stdout, "Address\t0x%04X\n", offset);
		fprintf(stdout, "Value:\t(%02X)\n", ((unsigned char*)memptr)[offset]);
		fprintf(stdout, "Please Enter a Valid 2 Digit Hex Number, '.' to exit:\t");
		fgets(user_input, sizeof(int) + 1, stdin);

		/* Checks if user_input is a '.' or '.\n' then exits function if user entered a '.' */
		if (strcmp(user_input, ".\n") == 0 || strcmp(user_input, ".") == 0) {
			fprintf(stdout, "Exiting Memory Modify...\n");
			break;
		}

		/* Checks if user_input is a valid 2 digit hexidecimal number */
		else if (!isxdigit(user_input[0]) || !isxdigit(user_input[1])){
			fprintf(stdout, "Enter a valid 2 digit hex number.\n\n");
			continue;
		}

		/* If user_input is valid hexidecimal value then modify memory and increment the offset */
		else{
			sscanf(user_input, "%X", &value);
			fprintf(stdout, "New value is: %02X\n\n", value);
			((char*)memptr)[offset] = value;
			offset++;
		}
	}
}

/**
Simply Displays messages that the program is exiting
**/
void quit()
{
	fprintf(stdout, "\nExiting Program...\n\n");
}

/**
Simply Displays the current value of all the registers
**/
void displayRegisters()
{
	fprintf(stdout, "R0\t\t R1\t\t R2\t\t R3\n");
	fprintf(stdout, "%08X\t %08X\t %08X\t %08X\n", regs.R[0], regs.R[1], regs.R[2], regs.R[3]);
	fprintf(stdout, "R4\t\t R5\t\t R6\t\t R7\n");
	fprintf(stdout, "%08X\t %08X\t %08X\t %08X\n", regs.R[4], regs.R[5], regs.R[6], regs.R[7]);
	fprintf(stdout, "R8\t\t R9\t\t R10\t\t R11\n");
	fprintf(stdout, "%08X\t %08X\t %08X\t %08X\n", regs.R[8], regs.R[9], regs.R[10], regs.R[11]);
	fprintf(stdout, "R12\t\t R13(SP)\t R14(LR)\t R15(PC)\n");
	fprintf(stdout, "%08X\t %08X\t %08X\t %08X\n", regs.R[12], regs.SP, regs.LR, regs.PC);
	fprintf(stdout, "MAR\t\t MBR\t\t SIGN\t\t CARRY\t\t ZERO\n");
	fprintf(stdout, "%08X\t %08X\t %01X\t\t %01X\t\t %01X\n", regs.MAR, regs.MBR, regs.SIGN, regs.CARRY, regs.ZERO);
	fprintf(stdout, "IR\t\t IR Flag\t IR0\t\t IR1\n");
	fprintf(stdout, "%08X\t %01X\t\t %04X\t\t %04X\n", regs.IR, IR_FLAG, regs.IR0, regs.IR1);
}

/**
Responsible for running the instruction cycle and then displaying the registers
@param memory - Receives the Virtual CPU's buffer
**/
void trace(void *memptr)
{
	instructionCycles(&memptr);
	displayRegisters();
}

/**
Function that creates a file of specified size
@param memory - Receives the Virtual CPU's buffer
**/
void writeFile(void *memptr)
{
	char filename[50];
	int fileSize = 0;

	/* Prompts user for File Name */
	fprintf(stdout, "\nPlease Enter a File name:\t");
	fscanf(stdin, "%s", filename);
	getchar();

	/* Prompts user for File Size*/
	fprintf(stdout, "Please Enter A File Size:\t");
	fscanf(stdin, "%d", &fileSize);

	/* Creates a file of the designated name and size */
	FILE *fp;
	fp = fopen(filename, "w+");
	for(int i = 0; i < fileSize; i++){
		fprintf(fp, "\0");
	}
	fwrite(memptr, 1, fileSize, fp);
	fclose(fp);
	fprintf(stdout, "File Created Successfully\n");
}

/**
Simply resets the value of all the registers to 0
**/
void resetRegisters()
{
	regs.IR = 0x00000000;
	regs.ALU = 0x00000000;
	for(int i = 0; i < REG_NUM; i++){
		regs.R[i] = 0x00000000;
	}
	regs.MAR = 0x00000000;
	regs.MBR = 0x00000000;
	regs.IR0 = 0x0000;
	regs.IR1 = 0x0000;
	regs.SIGN = 0x00;
	regs.CARRY = 0x00;
	regs.ZERO = 0x00;
	STOP_FLAG = false;
	IR_FLAG = false;

	fprintf(stdout, "\nRegisters Have Been Reset\n");
}

/**
Simply Displays the main menu when called
**/
void help()
{
	fprintf(stdout, "\nMain Menu\n");
	fprintf(stdout, "-----------------------------------\n");
	fprintf(stdout, "d)\tDump Memory\n");
	fprintf(stdout, "g)\tGo - Run the Entire Program\n");
	fprintf(stdout, "l)\tLoad a File into Memory\n");
	fprintf(stdout, "m)\tMemory Modify\n");
	fprintf(stdout, "q)\tQuit\n");
	fprintf(stdout, "r)\tDisplay Registers\n");
	fprintf(stdout, "t)\tTrace Execute One Instruction\n");
	fprintf(stdout, "w)\tWrite File\n");
	fprintf(stdout, "z)\tReset All Registers to Zero\n");
	fprintf(stdout, "h)\tDisplay List of Commands\n");
}

/**
Main Function that is responsible for receiving the users choice
@param argc - Currently not being used
@param argv - Currently not being used
**/
int main(int argc, char *argv[])
{
	resetRegisters(); //ARM x86 resets registers when started
	/* While loop for main prompt checks user input */
	while(1)
	{
		fprintf(stdout, "\nVirtual CPU ('h' help) >\t");
		fscanf(stdin, "%s", &cmd);
      
		if(cmd == 'd' || cmd == 'D'){
			unsigned int myOffset = 0;
			unsigned int myLength = 0;

			fprintf(stdout, "\nPlease Enter an Offset:\t");
			fscanf(stdin, "%d", &myOffset);
			getchar();

			fprintf(stdout, "\nPlease Enter a Length:\t");
			fscanf(stdin, "%d", &myLength);
			fprintf(stdout, "\n");

			memDump(buffer, myOffset, myLength);
		}
		else if(cmd == 'g' || cmd == 'G'){
			go();
		}
		else if(cmd == 'l' || cmd == 'L'){
			int bytesRead;
			bytesRead = loadFile(buffer, MEMORY_MAX);
			if(bytesRead != -1){
				fprintf(stdout, "\nRead %u bytes (0x%04x in hex)\n", bytesRead, bytesRead);
			} 
		}
		else if(cmd == 'm' || cmd == 'M'){
			unsigned int myOffset = 0;

			fprintf(stdout, "\nPlease Enter an Offset:\t");
			fscanf(stdin, "%d", &myOffset);
			fprintf(stdout, "\n");
			getchar();

			memmod(buffer, myOffset);
		}
		else if(cmd == 'q' || cmd == 'Q'){
			quit();
			break;
		}
		else if (cmd == 'r' || cmd == 'R'){
			displayRegisters();
		}
		else if(cmd == 't' || cmd == 'T'){
			trace(buffer);
		}
		else if(cmd == 'w' || cmd == 'W'){
			writeFile(buffer);
		}
		else if(cmd == 'z' || cmd == 'Z'){
			resetRegisters();
		}
		else if(cmd == 'h' || cmd == 'H'){
			help();
		}
		else { 
			fprintf(stdout, "\nCommand Invalid ('h' help)\n"); 
		}

		while(cmd != '\n'){
			fscanf(stdin, "%c", &cmd);
		}
	}
	return 0;
}
