
#include "CH57x_common.h"
#include "core_riscv.h"
#include "cli.h"
#include "mcu_dbg.h"

/**
 * memw <addr> <value> [bytes]
*/
void mcu_MemWrite(uint8 argc, char* argv[])
{
	if(argc < 3)
	{
		CLI_Printf("%s <address> <data> [size]\r\n", argv[0]);
	}
	else
	{
		uint32 nByte = 4;
		uint32 nAddr = CLI_GetInt(argv[1]);
		uint32 nVal = CLI_GetInt(argv[2]);
		if(4 == argc) nByte = CLI_GetInt(argv[3]);

		if(1 == nByte)  *((uint8*)nAddr) = (uint8)nVal;
		else if(2 == nByte) *((uint16*)nAddr) = (uint16)nVal;
		else if(4 == nByte) *((uint32*)nAddr) = (uint32)nVal;
		else CLI_Printf("%s <address> <data> [size]\r\n", argv[0]);
	}
}

void mcu_MemRead(uint8 argc, char* argv[])
{
	if(argc < 2)
	{
		CLI_Printf("%s <address> [size]\r\n", argv[0]);
	}
	else
	{
		uint32 nByte = 4;
		uint32 nAddr = CLI_GetInt(argv[1]);
		if(3 == argc) nByte = CLI_GetInt(argv[2]);
		
		if(1 == nByte)      CLI_Printf("rd_1B 0x%08lx 0x%02x\r\n", nAddr, *((uint8*)nAddr));
		else if(2 == nByte) CLI_Printf("rd_2B 0x%08lx 0x%04x\r\n", nAddr, *((uint16*)nAddr));
		else if(4 == nByte) CLI_Printf("rd_4B 0x%08lx 0x%08lx\r\n", nAddr, *((uint32*)nAddr));
		else CLI_Printf("%s <address> <data> [size]\r\n", argv[0]);
	}
}

__attribute__((naked))
__attribute__((section(".highcode")))
uint32 crs_Read()
{
	uint32 nRet;
	asm volatile("csrr %0, mtvec":"=r"(nRet));
	asm volatile("ret");
	return nRet;
}


void mcu_CsrRead(uint8 argc, char* argv[])
{
	if(argc < 2)
	{
		CLI_Printf("%s <address>\r\n", argv[0]);
	}
	else
	{
		uint32 nAddr = CLI_GetInt(argv[1]);
		*((uint32*)crs_Read) = (nAddr << 20) | 0x02573;
		asm volatile("fence");
		uint32 nVal = crs_Read();
		CLI_Printf("CSR 0x%04lx 0x%08lx\r\n", nAddr, nVal);
	}
}

void mcu_Ecall(uint8 argc, char* argv[])
{
	if(argc < 2)
	{
		CLI_Printf("%s <address>\r\n", argv[0]);
	}
	else
	{
		uint32 nParam0, nParam1, nParam2, nParam3;
		nParam0 = CLI_GetInt(argv[1]);
		if(argc > 2) nParam1 = CLI_GetInt(argv[2]);
		if(argc > 3) nParam2 = CLI_GetInt(argv[3]);
		if(argc > 4) nParam3 = CLI_GetInt(argv[4]);
//		asm volatile("mv a0, %0"::"r"(nParam));
		uint32 nResult = RV_ecall(nParam0, nParam1, nParam2, nParam3);
		CLI_Printf("Result: %X\r\n", nResult);
	}
}

void MCU_DbgInit(void)
{
	CLI_Register("ecall", mcu_Ecall);
	CLI_Register("csrr", mcu_CsrRead);
	CLI_Register("memw", mcu_MemWrite);
	CLI_Register("memr", mcu_MemRead);
}
