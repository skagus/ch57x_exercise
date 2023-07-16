

#include "CH57x_common.h"
#include "macro.h"
#include "os.h"
#include "util.h"
#include "cli.h"
#include "led.h"

#define LED_PIN_CMD			GPIO_Pin_8
#define LED_PIN_BUT			BIT(3)
#define LED_PIN_BLINK		GPIO_Pin_8
#define LED_ALL_PIN 		(LED_PIN_BLINK | LED_PIN_BUT | LED_PIN_CMD)

static uint16 gnLedPeriod = 20;

void led_Run(void* pParam)
{
	UNUSED(pParam);
	uint8 nCnt = 0;
	while(1)
	{
		if(gnLedPeriod > 0)
		{
			nCnt++;
			if(0 == (nCnt % 2))
			{
				GPIOA_SetBits(LED_PIN_BLINK);
			}
			else
			{
				GPIOA_ResetBits(LED_PIN_BLINK);			
			}
		}

		OS_Wait(BIT(EVT_LED_CMD), gnLedPeriod);
	}
}

void led_Cmd(uint8 argc, char* argv[])
{
	if(2 == argc)
	{
		uint32 nIn = UT_GetInt(argv[1]);
		if(0 == nIn)
		{
			GPIOA_ResetBits(LED_PIN_CMD);
		}
		else if(1 == nIn)
		{
			GPIOA_SetBits(LED_PIN_CMD);
		}
		else // 0,1이 아닐 땐, blink period 조절함.
		{
			gnLedPeriod = nIn;
		}
	}
	else
	{
		UT_Printf("Number of parameter\r\n");
	}

	OS_SyncEvt(BIT(EVT_LED_CMD));
}


void LED_Toggle(void)
{
	static uint8 snState = 0;
	snState++;
	if(snState & 0x1)
	{
		GPIOA_SetBits(LED_PIN_CMD);
	}
	else
	{
		GPIOA_ResetBits(LED_PIN_CMD);
	}
}

#define SIZE_STK	(1024)	// DW.
static uint32 gaLedStk[SIZE_STK];

void LED_Init(void)
{
	GPIOA_SetBits(LED_PIN_CMD);
	GPIOA_ModeCfg(LED_PIN_CMD, GPIO_ModeOut_PP_5mA);

	CLI_Register("led", led_Cmd);
	OS_CreateTask(led_Run, gaLedStk + SIZE_STK - 1, NULL, "led");

//	BUT_AddAction(0, EDGE_RISING | EDGE_FALLING, led_But, 0xFF);
}

