/********************************** (C) COPYRIGHT *******************************
 * File Name          : Main.c
 * Author             : WCH
 * Version            : V1.1
 * Date               : 2022/01/25
 * Description        : 친콰쇗휭HID구
 *********************************************************************************
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for 
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/

#include "CH57x_common.h"
#include "version.h"
#include "os.h"
#include "hal.h"
#include "util.h"
#include "led.h"
#include "cli.h"
#include "mcu_dbg.h"
#include "overlay.h"
#include "dot_mat.h"
#include "flash_spi.h"
#include "ymodem.h"

#define OS_TEST_	(0)
#if (OS_TEST == 1)
#define SIZE_STK	(1024)	// DW.
static uint32 gaT1Stk[SIZE_STK];
static uint32 gaT2Stk[SIZE_STK];

void t1_Run(void* pParam)
{
	UNUSED(pParam);
	int nIdx = 0;
	while(1)
	{
		UT_Printf("\tT1:%4d\n", nIdx);
		nIdx++;
		OS_Idle(OS_SEC(2));
	}
}

void t2_Run(void* pParam)
{
	UNUSED(pParam);
	int nIdx = 0;
	while(1)
	{
		UT_Printf("T2:%4d\n", nIdx);
		nIdx ++;
		OS_Idle(OS_SEC(3));
	}
}
#endif

int main(void)
{
	SetSysClock(CLK_SOURCE_PLL_60MHz);
//	DelayMs(1000); // Wait 1 sec.

	HAL_DbgInit();
	UART_Init(0);
	HAL_DbgLog("Hello\n");
	UT_Printf(gpVersion);
	
#if (OS_TEST == 1)
	Cbf cbfTick = OS_Init();
	TIMER_Init(cbfTick);
	OS_CreateTask(t1_Run, gaT1Stk + SIZE_STK - 1, (void*)1, "t1");
	OS_CreateTask(t2_Run, gaT2Stk + SIZE_STK - 1, (void*)2, "t2");
	OS_Start();
#else
	Cbf cbfTick = OS_Init();
	TIMER_Init(cbfTick);

	MCU_DbgInit();
	LED_Init();
	CLI_Init();
	OVL_Init();
	DOT_Init();

#if 0
	FLASH_Init();
	YM_Init();
#endif
	OS_Start();
#endif
}

