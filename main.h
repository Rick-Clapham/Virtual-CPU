/******************************************
* Name: Richard Clapham
* Student #: 821-490-125
* Student #: n00663650
* Date: 4/7/2016
* Virtual CPU project Header
******************************************/

#ifndef MAIN
#define MAIN

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h> 
#include <ctype.h>

#define MEMORY_MAX 0x3E80	//Determines maximum memory size (16000)
#define MAX32 0xFFFFFFFF
#define HEX_COLUMN 16
#define REG_NUM 16			//Determines amount of registers

/* Defines all registers and flags */
#define SP R[13]			//Stack Pointer
#define LR R[14]			//Link Register
#define PC R[15]			//Program Counter
#define IR IR				//Instruction Register Combination of IR0 & IR1
#define ALU ALU				//Arithmetic Logic Unit
#define MAR MAR				//Memory Address Register
#define MBR MBR				//Memory Buffer Register
#define IR0 IR0				//Instruction Register 0
#define IR1 IR1				//Instruction Register 1
#define SIGN SIGN			//Sign Flag
#define CARRY CARRY			//Carry Flag
#define ZERO ZERO			//Zero Flag
#define STOP_FLAG STOP_FLAG	//Flag Set by Stop Instruction
#define IR_FLAG IR_FLAG		//Flag to determine the active Instruction Register

/* Registers holds all hardware except for memory 32-bit */
typedef struct{
uint32_t IR,
         ALU,
         R[REG_NUM],
         MAR,
         MBR;

/* Instruction Registers 16-bit */
uint16_t IR0,
         IR1;
  
/* Condition Code Registers 8-bit */         
uint8_t  SIGN,
         CARRY,
         ZERO;
} registers;

registers regs;

/* STOP and whichever IR is Active Flag */
bool	STOP_FLAG,
		IR_FLAG;

unsigned Current_IR;
int Current_TYPE;

/* Flag Checks */
#define ISZERO(x) (x == 0) ? 1 : 0
#define ISSIGN(x) ((0x8000 & x) == 0x8000) ? 1 : 0

/* Determining Instruction Type */ 
#define DATA_PROCESS 	0x0			//Data Processing
#define LOAD_STORE		0x1			//Load / Store
#define IMMEDIATE		0x2 | 0x3	//Immediate
#define CON_BRANCH		0x4			//Conditional Branch
#define PUSH_PULL		0x5			//Push / Pull
#define UN_BRANCH		0x6			//Unconditional Branch 
#define T_STOP			0x7			//Stop 

/* Mask Definitions */
/* Data Process */
#define DATA_OP ((Current_IR & 0xF00) >> 8)	//Operation
#define DATA_RN ((Current_IR & 0xF0) >> 4)	//(Secondary Operand)
#define DATA_RD	(Current_IR & 0x000F)		//Destination (Primary Operand)

/* Data Process Codes */
#define DATA_ADC 0x5	//Add with Carry
#define DATA_ADD 0x4	//Add / Add Immediate
#define DATA_AND 0x0	//And
#define DATA_BIC 0xE	//Bit Clear
#define DATA_CMP 0xA	//Compare / Compare Immediate
#define DATA_EOR 0x1	//Exclusive Or
#define DATA_LSL 0x7	//Logical Shift Left
#define DATA_LSR 0x6	//Logical Shift Right
#define DATA_MOV 0xD	//Move / Move Immediate
#define DATA_MVN 0xF	//Move Not
#define DATA_ORR 0xC	//Or
#define DATA_ROR 0xB	//Rotate Right
#define DATA_SUB 0x2	//Subtract / Subtract Immediate
#define DATA_SXB 0x3	//Sign Extend Byte
#define DATA_TEQ 0x9	//Test Equivalence
#define DATA_TST 0x8	//Test Bits

/* Load / Store */
#define LOAD_STORE_B 	((Current_IR & 0x800) >> 11)	//Byte / Word Bit (0 = transfer word, 1 = transfer byte)
#define LOAD_STORE_L 	((Current_IR & 0x400) >> 10)	//Load / Store Bit (0 = Store to Memory, 1 = Load from Memory)
#define LOAD_STORE_RN	((Current_IR & 0x0F0) >> 4)		//Memory Address
#define LOAD_STORE_RD	(Current_IR & 0x000F)			//Source / Destination Register

/* Immediate Operations */
#define IMM_OP 		((Current_IR & 0x3000) >> 12)	//Operation
#define IMM_VALUE 	((Current_IR & 0x0FF0) >> 4)	//Value 
#define IMM_RD 		(Current_IR & 0x000F)			//Source / Destination Register

/* Immediate Operations Code */
#define IMM_MOV 0x0		//Move
#define IMM_CMP 0x1		//Compare
#define IMM_ADD 0x2		//Add
#define IMM_SUB 0x3		//Subtract

/* Conditional Branch */
#define CBRANCH_COND ((Current_IR & 0xF00) >> 8)
#define CBRANCH_ADDR (Current_IR & 0x00FF)

/* Conditional Branch Codes */
#define CBRANCH_EQ 0x0000	//Equal	(Zero Set)
#define CBRANCH_NE 0x0001	//Not Equal	(Zero Clear)
#define CBRANCH_CS 0x0002	//Unsigned Higher or Same	(Carry Set)
#define CBRANCH_CC 0x0003	//Unsigned Lower	(Carry Clear)
#define CBRANCH_MI 0x0004	//Negative	(Sign Set)
#define CBRANCH_PL 0x0005	//Positive	(Sign Clear)
#define CBRANCH_HI 0x0008	//Unsigned Higher	(Carry Set & Zero Clear)
#define CBRANCH_LS 0x0009	//Unsigned Lower or Same	(Carry Clear || Zero Set)
#define CBRANCH_AL 0x000E	//Always	(No Flags)

/* Push / Pull */
#define PUSH_PULL_L 	((Current_IR & 0x0800) >> 11)	//Load / Store Bit (0 = Push, 1 = Pull)
#define PUSH_PULL_R		((Current_IR & 0x0100) >> 8)	//Additional Push / Pull Bit (0 = No Extra Push or Pulls, 1 = Pull PC / Push LR)
#define PUSH_PULL_H 	((Current_IR & 0x0400) >> 10)	//High / Low Bit (0 = Low Registers, 1 = High Registers)
#define PUSH_REG_LIST 	(Current_IR & 0x00FF)

/* Unconditional Branch */
#define UBRANCH_LINK_K 	((Current_IR & 0x1000) >> 12)	//Link Bit (0 = Branch (BRA), 1 = Branch with Link (BRL))
#define UBRANCH_OFFSET 	(Current_IR & 0x0FFF)

/* My Functions */
void error(char *msg);
void execute(uint16_t instruction, uint16_t type, void *memptr);
void fetch(void *memptr);
void instructionCycles(void *memptr);
int isCarry(unsigned long op1, unsigned long op2, unsigned C);
void flagsCheck(uint32_t register_1, uint32_t register_2, int flag_Check);

void memDump(void *memptr, unsigned offset, unsigned length);
void go();
int loadFile(void *buffer, unsigned int max);
void memmod(void *memptr, unsigned offset);
void quit();
void displayRegisters();
void trace(void *memptr);
void writeFile(void *memory);
void resetRegisters();
void help();
int main(int argc, char *argv[]);

#endif
