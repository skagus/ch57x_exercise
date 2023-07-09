#pragma once

#define OS_MSEC(x)		(x)
#define MS_PER_TICK     (10)

typedef enum _TaskId
{
	TID_CONSOLE,
	TID_LED,
	TID_ADC,
	TID_BUT,
	TID_DOT,
	TID_YMODEM,
	NUM_TASK,
} TaskId;

typedef enum _Evt
{
	EVT_TICK,
	EVT_UART,
	EVT_LED_CMD,
	EVT_BUT_CHANGE,
	EVT_ECALL_DONE,
	EVT_UART_YM,	///< UART Event for Y modem.
	EVT_YMODEM,			///< Ymodem request or done.
	NUM_EVT,
} EvtId;
 