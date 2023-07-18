#include "CH57x_common.h"
#include "core_riscv.h"
#include "macro.h"
#include "sched.h"
#include "util.h"
#include "cli.h"

#define MAT_CS		(GPIO_Pin_4)
#define MAT_CLK		(GPIO_Pin_13)
#define MAT_DOUT	(GPIO_Pin_14)

#define MAX7219_TEST			0x0f
#define MAX7219_BRIGHTNESS 		0x0a
#define MAX7219_SCAN_LIMIT		0x0b
#define MAX7219_DECODE_MODE		0x09
#define MAX7219_SHUTDOWN		0x0C

#define SIZE_FULL				(16)
/**
 * For SPI DMA, data should be in SRAM..??
*/
uint8 gaImage[SIZE_FULL] = {
	1, 0xAA, 2, 0x55, 3, 0xFF, 4, 0x00,
	5, 0xAA, 6, 0x55, 7, 0xFF, 8, 0x00,
};

void dot_SendPair(uint8 nCmd, uint8 nVal)
{
	GPIOA_ResetBits(MAT_CS);
	SPI0_MasterSendByte(nCmd);
	SPI0_MasterSendByte(nVal);
	GPIOA_SetBits(MAT_CS);
}

void dot_Cmd(uint8 argc, char* argv[])
{
	if(argc == 3) // dot col val
	{
		uint32 nCol = UT_GetInt(argv[1]);
		uint32 nVal = UT_GetInt(argv[2]);
		UT_Printf("dot out: %d, %08x\r\n", nCol, nVal);
		dot_SendPair((uint8)nCol, (uint8)nVal);
	}
	else if(argc == 2)
	{
		char nCmd = argv[1][0];
		
		if(nCmd == 'i')
		{
			UT_Printf("dot initialize\r\n");
			dot_SendPair(MAX7219_TEST, 0x01);  mDelaymS(10);  
			dot_SendPair(MAX7219_TEST, 0x00);        // Finish test mode.
			dot_SendPair(MAX7219_DECODE_MODE, 0x00); // Disable BCD mode. 
			dot_SendPair(MAX7219_BRIGHTNESS, 0x00);  // Use lowest intensity. 
			dot_SendPair(MAX7219_SCAN_LIMIT, 0xFF);  // Scan all digits.
			dot_SendPair(MAX7219_SHUTDOWN, 0x01);    // Turn on chip.
		}
		else if(nCmd == 'f')
		{
			UT_Printf("dot FIFO\r\n");
			GPIOA_ResetBits(MAT_CS);
			SPI0_MasterTrans(gaImage, SIZE_FULL);
			GPIOA_SetBits(MAT_CS);
		}
		else if(nCmd == 'd')
		{
			UT_Printf("dot DMA\r\n");
			GPIOA_ResetBits(MAT_CS);
			SPI0_MasterDMATrans(gaImage, SIZE_FULL);
			GPIOA_SetBits(MAT_CS);
		}
		else if(nCmd == 'c')
		{
			UT_Printf("dot clear\r\n");
			for(uint8 nCol = 1; nCol <= 8; nCol++)
			{
				dot_SendPair(nCol, 0);
			}
		}
	}
}

void DOT_Init(void)
{
	GPIOA_SetBits(MAT_CS);
	GPIOA_ModeCfg(MAT_CS | MAT_CLK | MAT_DOUT, GPIO_ModeOut_PP_5mA);
	SPI0_MasterDefInit();
	SPI0_CLKCfg(0xe8);

	CLI_Register("dot", dot_Cmd);
}

