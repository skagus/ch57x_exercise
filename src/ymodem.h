#pragma once
#include "types.h"


typedef enum
{
	YS_META,		///< 1st packet. (File name and size, or EOT or end pkt)
	YS_DATA,		///< Some data received.
	YS_END,
	YS_FAIL,
} YMState;


#define YM_TIMEOUT			(2000)	// 20 secs.
#define YM_RETRY_PERIOD		(100)	// 1 sec.
/**
YModem�� ������ ��û�� ������ Handler�� �Ѱ���� �Ѵ�. 
RX�϶� handler��. 
	YS_HEADER --> pBuf: file name, pnBytes: file size.
	YS_DATA --> pBuf: data, pnBytes: ���� data packet size.
	YS_FAIL, YS_DONE --> parameter �ǹ̾���
*/

typedef bool (*YmHandle)(uint8* pBuf, uint32* pnBytes, YMState eStep, void* pParam);

typedef struct _YReq
{
	bool bReq; // Requested.
	bool bRx; // TX or RX.
	YmHandle pfHandle;
	void* pParam;
} YReq;


//void YM_DoRx(YmHandle pfRxHandle, void* pParam);
//void YM_DoTx(YmHandle pfTxHandle, void* pParam);

void YM_Init(void);
bool YM_Request(YReq* pstReq);
