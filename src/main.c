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
#include "sched.h"
#include "led.h"
#include "cli.h"
#include "mcu_dbg.h"
#include "overlay.h"
#include "dot_mat.h"
#include "flash_spi.h"

int main()
{
	SetSysClock(CLK_SOURCE_PLL_60MHz);
//	DelayMs(1000); // Wait 1 sec.

	Cbf cbfTick = Sched_Init();
	TIMER_Init(cbfTick);

	LED_Init();
	CLI_Init();
	MCU_DbgInit();
	OVL_Init();
	FLASH_Init();
	
	CLI_Printf(gpVersion);

	while(1)
	{
		Sched_Run();
	}
}

