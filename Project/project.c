/*
    Erik Kantrowitz and Clarisse Vamos
	CDA 3103 - Computer Logic and Organization
	MIPS Project
*/


#include "spimcore.h"

/* ALU */
/* 10 Points */
void ALU(unsigned A,unsigned B,char ALUControl,unsigned *ALUresult,char *Zero)
{
	//Approach: go through each ALUControl option and handle appropriately
	//ALUresult <==> Z

	switch(ALUControl)
	{
		//Z = A + B
		case '0':
			*ALUresult = A + B;
			break;

		//Z = A - B
		case '1':
			*ALUresult = A - B;
			break;

		//if A < B, Z = 1; otherwise, Z = 0
		case '2':
			//A and B are unsigned, so cast A and B as integers
			*ALUresult = ((int)A < (int)B) ? 1 : 0;
			break;

		//if A < B, Z = 1; otherwise, Z = 0 (A and B are unsigned integers)
		case '3':
			*ALUresult = (A < B) ? 1 : 0;
			break;

		//Z = A AND B
		case '4':
			*ALUresult = A & B;  	//remember to use bitwise operator
			break;

		//Z = A OR B
		case '5':
			*ALUresult = A | B;  	//remember to use bitwise operator
			break;

		//Shift left B by 16 bits
		case '6':
			B<<16;
			break;

		//Z = NOT A
		case '7':
			*ALUresult = ~A;	//remember to use bitwise operator
			break;
	}

	//If the result is zero, set Zero to 1, otherwise set to 0
	*Zero = (ALUresult == 0) ? 1 : 0;
}



/* instruction fetch */
/* 10 Points */
int instruction_fetch(unsigned PC,unsigned *Mem,unsigned *instruction)
{
	if ((PC % 4) == 0)   //Check for halt condition aka check word alignment
	{
	    //Word aligned, no halt condition
	    *instruction = Mem[PC >> 2];    //Fetch the instruction addressed by PC from Mem and write it to instruction
	    return 0;
	}
    else
    {
        //Not word aligned - HALT
        return 1;
    }
}



/* instruction partition */
/* 10 Points */
void instruction_partition(unsigned instruction, unsigned *op, unsigned *r1,unsigned *r2, unsigned *r3, unsigned *funct, unsigned *offset, unsigned *jsec)
{
	//Partition instruction into several parts (op, r1, r2, r3, funct, offset, jsec).
	//Read line 41 to 47 of spimcore.c for more information.

    //Get correct values
    *op = (instruction & 0xfc000000) >> 26;  //mask last 26 bits w/ 0s and shift them away, keeping op code
    *r1 = (instruction & 0x03E00000) >> 21;	//r1 is bits 21-25
    *r2 = (instruction & 0x001F0000) >> 16;	//r2 is bits 16-20
    *r3 = (instruction & 0x0000F800) >> 11;	//r3 is bits 11-15
    *funct = (instruction & 0x0000003F);		//funct is bits 0-5
    *offset = (instruction & 0x0000FFFF);		//offset is bits 0-15 AKA constant ot address
    *jsec = (instruction & 0x03FFFFFF);         //jsec is address, bits 25-0

}



/* instruction decode */
/* 15 Points */
int instruction_decode(unsigned op,struct_controls *controls)
{
    //All "Do Not Care" control signals are set to '0' to avoid accidental instructions
    switch(op)
    {
        //000000 - r type instructions
        case (0x00):
            controls->RegDst 	= 1;
            controls->Jump 		= 0;
            controls->Branch 	= 0;
            controls->MemRead 	= 0;
            controls->MemtoReg 	= 0;
            controls->ALUOp 	= '7';
            controls->MemWrite 	= 0;
            controls->ALUSrc 	= 0;
            controls->RegWrite 	= 1;
            break;

        //000010 - jump
        case (0x02):
            controls->RegDst 	= 0;
            controls->Jump 		= 1;
			controls->Branch 	= 0;
            controls->MemRead 	= 0;
			controls->MemtoReg 	= 0;
			controls->ALUOp 	= '0';
			controls->MemWrite 	= 0;
			controls->ALUSrc 	= 0;
			controls->RegWrite	= 0;
            break;

        //000100 - beq
        case (0x04):
            controls->RegDst 	= 0;
			controls->Jump 		= 0;
			controls->Branch 	= 1;
			controls->MemRead 	= 0;
			controls->MemtoReg 	= 2;
			controls->ALUOp 	= '1';
			controls->MemWrite 	= 0;
			controls->ALUSrc 	= 0;
			controls->RegWrite	= 0;
            break;

        //001000 - addi
        case (0x08):
            controls->RegDst 	= 0;
            controls->Jump 		= 0;
			controls->Branch 	= 0;
			controls->MemRead 	= 0;
			controls->MemtoReg 	= 0;
			controls->ALUOp 	= '0';
			controls->MemWrite 	= 0;
			controls->ALUSrc 	= 1;
			controls->RegWrite	= 1;

            break;

        //001010 - slti
        case (0xA):
            controls->RegDst 	= 0;
			controls->Jump 		= 0;
			controls->Branch 	= 0;
			controls->MemRead 	= 0;
			controls->MemtoReg 	= 0;
			controls->ALUOp 	= '2';
			controls->MemWrite 	= 0;
			controls->ALUSrc 	= 1;
			controls->RegWrite	= 1;
            break;

        //001011 - sltiu
        case (0xB):
            controls->RegDst 	= 0;
			controls->Jump 		= 0;
			controls->Branch 	= 0;
			controls->MemRead 	= 0;
			controls->MemtoReg 	= 0;
			controls->ALUOp 	= '3';
			controls->MemWrite 	= 0;
			controls->ALUSrc 	= 1;
			controls->RegWrite	= 1;
            break;

        //001111 - lui
        case (0xF):
            controls->RegDst 	= 0;
			controls->Jump 		= 0;
			controls->Branch 	= 0;
			controls->MemRead 	= 0;
			controls->MemtoReg 	= 0;
			controls->ALUOp 	= '6';
			controls->MemWrite 	= 0;
			controls->ALUSrc 	= 1;
			controls->RegWrite	= 1;
            break;

        //100011 - lw
        case (0x23):
            controls->RegDst 	= 0;
			controls->Jump 		= 0;
			controls->Branch 	= 0;
			controls->MemRead 	= 1;
			controls->MemtoReg 	= 1;
			controls->ALUOp 	= '0';
			controls->MemWrite 	= 0;
			controls->ALUSrc 	= 1;
			controls->RegWrite	= 1;
            break;

        //101011 - sw
        case (0x2B):
            controls->RegDst 	= 2;
			controls->Jump 		= 0;
			controls->Branch 	= 0;
			controls->MemRead 	= 0;
			controls->MemtoReg 	= 2;
			controls->ALUOp 	= '0';
			controls->MemWrite 	= 1;
			controls->ALUSrc 	= 1;
			controls->RegWrite	= 0;
            break;

        //Not a valid instruction - HALT
        default:
            return 1;
    }
    return 0;
}

/* Read Register */
/* 5 Points */
void read_register(unsigned r1,unsigned r2,unsigned *Reg,unsigned *data1,unsigned *data2)
{
    //Read the registers that are addressed and write values appropriately
    *data1 = Reg[r1];
    *data2 = Reg[r2];
}


/* Sign Extend */
/* 10 Points */
void sign_extend(unsigned offset,unsigned *extended_value)
{
    //Assign the sign-extended value of offset to extended_value
    //If MSB of the offset is 1, then set the upper 16 bits as 1's, otherwise they are already 0's
    *extended_value = ((offset & 0x00008000) >> 15 == 1) ? 0xFFFF0000 + offset : offset;
}

/* ALU operations */
/* 10 Points */
int ALU_operations(unsigned data1,unsigned data2,unsigned extended_value,unsigned funct,char ALUOp,char ALUSrc,unsigned *ALUresult,char *Zero)
{
    //Check ALUSrc to determine data that will be used
    data2 = (ALUSrc == 1) ? extended_value : data2;

    //ALUop is char & ASCII value of 0 is 48, so use ALUop-48 ti find ALUop's value as int
    switch(ALUOp-48)
    {
        //R Type Instruction, so need to use funct to determine which ALUOp to run
        case (7):
            switch (funct)
            {
                //100000 - add
                case (0x20):
                    ALU(data1, data2, '0', ALUresult, Zero);
                    break;

                //100010 - subtract
                case (0x22):
                    ALU(data1, data2, '1', ALUresult, Zero);
                    break;

                //100100 - and
                case (0x24):
                    ALU(data1, data2, '4', ALUresult, Zero);
                    break;

                //100101 - or
                case (0x25):
                    ALU(data1, data2, '5', ALUresult, Zero);
                    break;

                //101010 - slt
                case (0x2A):
                    ALU(data1, data2, '2', ALUresult, Zero);
                    break;

                //101011 - sltu
                case (0X2B):
                    ALU(data1, data2, '3', ALUresult, Zero);
                    break;

                //Unknown funct type -  HALT
                default:
                    return 1;
            }
            break;

        //Other types
        default:
            ALU(data1, data2, ALUOp, ALUresult, Zero);
            break;
    }
    return 0;
}

/* Read / Write Memory */
/* 10 Points */
int rw_memory(unsigned ALUresult,unsigned data2,char MemWrite,char MemRead,unsigned *memdata,unsigned *Mem)
{
    //Read from memory
	if (MemRead == 1)
	{
		if ((ALUresult % 4) == 00)  //check if word-aligned
        {
            *memdata = Mem[ALUresult>>2];
        }
		else
        {
            return 1;   //HALT
        }
	}

	//Write to memory
	if (MemWrite == 1)
	{
		if ((ALUresult %4) == 0)    //check if word-aligned
		{
			Mem[ALUresult>>2] = data2;
		}
		else
        {
            return 1;   //HALT
        }
	}
	return 0;
}


/* Write Register */
/* 10 Points */
void write_register(unsigned r2,unsigned r3,unsigned memdata,unsigned ALUresult,char RegWrite,char RegDst,char MemtoReg,unsigned *Reg)
{
    //Write to the register from memory
	if (RegWrite == 1 && MemtoReg == 1)
	{
		if (RegDst == 1)
        {
            Reg[r3] = memdata;
        }
		else
        {
            Reg[r2] = memdata;
        }
	}

	//Write to the register not from memory
	else if (RegWrite == 1 && MemtoReg == 0)
	{
		if (RegDst == 1)
		{
			Reg[r3] = ALUresult;
		}
		else
        {
			Reg[r2] = ALUresult;
		}
	}
}

/* PC update */
/* 10 Points */
void PC_update(unsigned jsec,unsigned extended_value,char Branch,char Jump,char Zero,unsigned *PC)
{
    //Update the PC appropriately
    *PC = *PC + 4;

	if (Branch == 1 && Zero == 1)  //update for branches
	{
		//Update PC
		*PC = *PC + (extended_value<<2);
	}
	if (Jump == 1)  //update for jumps
	{
		*PC = (*PC & 0xF0000000) | (jsec<<2);  //update PC to address of jumped value
	}
}

