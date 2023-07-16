#pragma once

#include "macro.h"

#define MSEC_PER_TICK			(10)
#define OS_MSEC(msec)			(DIV_UP(msec, MSEC_PER_TICK))	///< tick per msec.
#define OS_SEC(sec)				((sec) * OS_MSEC(1000UL))	///< tick per sec.

#define LONG_TIME				(OS_SEC(3))	// 3 sec.

typedef enum
{
	EVT_TICK,
	EVT_UART,
	EVT_LED_CMD,
#if 1
	EVT_BUT_CHANGE,
	EVT_ECALL_DONE,
	EVT_UART_YM,	///< UART Event for Y modem.
	EVT_YMODEM,			///< Ymodem request or done.
#endif
	NUM_EVT
} evt_id;

typedef enum
{
	TID_CONSOLE,
	TID_LED,
	TID_ADC,
	TID_BUT,
	TID_DOT,
	TID_YMODEM,
	NUM_TASK,
} TaskId;
