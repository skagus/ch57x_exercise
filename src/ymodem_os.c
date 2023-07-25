#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include "CH57x_common.h"
#include "types.h"
#include "macro.h"
#include "util.h"
#include "os.h"
#include "cli.h"
#include "ymodem.h"
#include "hal.h"

#define YMODEM_SOH 0x01
#define YMODEM_STX 0x02
#define YMODEM_ACK 0x06
#define YMODEM_NACK 0x15
#define YMODEM_EOT 0x04
#define YMODEM_C 0x43
#define YMODEM_CAN 0x18
//#define YMODEM_BS 0x08

#define MAX_FILE_NAME_LEN	(128)
#define DATA_BYTE_SMALL		(128)
#define DATA_BYTE_BIG		(1024)
#define EXTRA_SIZE			(5)
#define YMODEM_BUF_LENGTH	(DATA_BYTE_BIG + EXTRA_SIZE)

#define DBG_YM(...)			HAL_DbgLog(__VA_ARGS__)

typedef enum
{
	PKT_WAIT_HEADER,	// SOH or SOX.
	PKT_WAIT_DATA,	// from Seq ~ CRC.
	PKT_WAIT_TAIL,
	NUM_PKT_WAIT,
} PktState;

typedef enum
{
	PR_SUCC,	///< Good data.
	PR_EOT,		///< Single byte: EOT received.
	PR_CANCEL,	///< 
	PR_CRC_ERR,	///< CRC Error. or en expected.
	PR_ERROR,
} PktRet;

typedef enum _YRet
{
	YR_DONE,
	YR_CANCEL,
	YR_ERROR,
	YR_TIMEOUT,
	YR_EOT,
	NUM_YR,
} YRet;

typedef enum _YState
{
	Y_IDLE,
	Y_READY,
	Y_RUN,
	NUM_Y,
} YState;


static bool _WaitRx(uint8* pnCh)
{
	if(0 != UART_RxD((char*)pnCh))
	{
		return true;
	}

	OS_Wait(BIT(EVT_UART_YM), OS_SEC(3));
	if(0 != UART_RxD((char*)pnCh))
	{
		return true;
	}
	return false;
}

static void _EmptyRxQ()
{
	char nNewData;
	while(UART_RxD(&nNewData));
}

static void _TxHost(uint32 nCode)
{
	UART_TxD(nCode);
	DBG_YM("->(%X)\n", nCode);
}

#define SET_ERROR_JUMP(ERROR) 	{eRet = ERROR; goto END;}

/*
받은 packet에서 file정보 (file name, file size 를 뽑아온다.)
[SOH:1] [SEQ:0x00:1] [nSEQ:0xFF:1] [filename:"foo.c":] [filesize:"1064":] [pad:00:118] [CRC::2]
*/
bool ym_GetFileInfo(uint8* szFileName, uint32* pnFileLen, uint8* pData)
{
	uint32 nLen = strlen((char*)pData) + 1; // including \0
	strncpy(szFileName, (char*)pData, nLen);
	*pnFileLen = (uint32)strtoul((char*)(pData + nLen), (char **)NULL, (int)0);
	DBG_YM("File:%s(Len:%d),", szFileName, *pnFileLen);
	return true;
}

/**
 * @return true if done receiving.
 * [SOH:1] [SEQ:0x00:1] [nSEQ:0xFF:1] [DATA:128][CRC::2]
 * [STX:1] [SEQ:0x00:1] [nSEQ:0xFF:1] [DATA:1024][CRC::2]
*/
YRet ym_RcvPkt(uint8* pData, uint32* pnLen, uint8* pnSeq)
{
	uint8 nNewData;
	uint32 nDataLen;
	uint8 nSeq;

	if(false == _WaitRx(&nNewData)) return YR_TIMEOUT;
	if (nNewData == YMODEM_SOH)
	{
		nDataLen = DATA_BYTE_SMALL;
		DBG_YM("SO:");
	}
	else if (nNewData == YMODEM_STX)
	{
		nDataLen = DATA_BYTE_BIG;
		DBG_YM("ST:");
	}
	else if (nNewData == YMODEM_EOT)
	{
		DBG_YM("EO:");
		return YR_EOT;
	}
	else if (nNewData == YMODEM_CAN)
	{
		DBG_YM("CA:");
		return YR_CANCEL;
	}
	else
	{
		return YR_ERROR;
	}

	// Seq No.
	if(false == _WaitRx(&nNewData)) return YR_TIMEOUT;
	nSeq = nNewData;
	if(false == _WaitRx(&nNewData)) return YR_TIMEOUT;
	if(nSeq != ~nNewData) return YR_ERROR;

	// Data RCV.
	for(uint32 nByte = 0; nByte < nDataLen; nByte++)
	{
		if(false == _WaitRx(&nNewData)) return YR_TIMEOUT;
		pData[nByte] = nNewData;
	}

	// CRC RCV.
	uint16 nRcvCRC;
	if(false == _WaitRx(&nNewData)) return YR_TIMEOUT;
	nRcvCRC = (uint16)nNewData << 8;
	if(false == _WaitRx(&nNewData)) return YR_TIMEOUT;
	nRcvCRC |= nNewData;

	uint16 nCalcCRC = UT_Crc16(pData, nDataLen);
	if(nRcvCRC != nCalcCRC)
	{
		DBG_YM("CRC %X, %X\n", nRcvCRC, nCalcCRC);
		return YR_ERROR;
	}
	*pnLen = nDataLen;
	*pnSeq = nSeq;
	return YR_DONE;
}

/**
 * return true if All sequence is done.l
*/
YRet ym_Rx(YReq* pReq, uint8* pData)
{
	uint8 nNewData;
	uint8 nSeq;
	uint32 nFileLen;
	uint32 nPktLen;
	YRet eRet;

	_TxHost(YMODEM_C);	// Start Xfer.
	eRet = ym_RcvPkt(pData, &nPktLen, &nSeq);
	if(YR_DONE != eRet) return eRet;

	uint8 szFileName[128];
	ym_GetFileInfo(szFileName, &nFileLen, pData);
	if(NULL != pReq->pfHandle)
	{
		pReq->pfHandle((uint8*)szFileName, &nFileLen, YS_META, pReq->pParam);
	}
	_TxHost((uint8)YMODEM_ACK);

	_TxHost((uint8)YMODEM_C);
	uint32 nRestLen = nFileLen;
	while(nRestLen > 0)
	{
		eRet = ym_RcvPkt(pData, &nPktLen, &nSeq);
		if(YR_DONE != eRet) return eRet;

		if(NULL != pReq->pfHandle)
		{
			pReq->pfHandle(pData, nPktLen, YS_DATA, pReq->pParam);
		}
		nRestLen -= (nRestLen < nPktLen) ? nRestLen : nPktLen;
		_TxHost((uint8)YMODEM_ACK);
	}

	eRet = ym_RcvPkt(pData, &nPktLen, &nSeq);
	if(YR_EOT != eRet) return YR_ERROR;

	_TxHost(YMODEM_NACK);
	eRet = ym_RcvPkt(pData, &nPktLen, &nSeq);
	if(YR_EOT != eRet) return YR_ERROR;

	_TxHost(YMODEM_ACK);
	_TxHost(YMODEM_C);
	eRet = ym_RcvPkt(pData, &nPktLen, &nSeq);
	if(YR_DONE != eRet) return eRet;
	if(0 != pData[0]) return YR_ERROR;

	_TxHost(YMODEM_ACK);

	return YR_DONE;
}


uint32 _SendMeta(uint8* pBase, YmHandle pfTxHandle, void* pParam)
{
	DBG_YM("First->\n");
	uint8* pData = pBase;
	*pData = YMODEM_SOH; pData++;
	*pData = 0; pData++;
	*pData = 0xFF; pData++;

	uint32 nLen;
	pfTxHandle(pData, (uint32*)&nLen, YS_META, pParam);	// Get header.
	uint32 nSizeOff = strlen((char*)pData) + 1;
	sprintf((char*)(pData + nSizeOff), "%d", (int)nLen);
	uint16 nCRC = UT_Crc16(pData, DATA_BYTE_SMALL);
	pData += DATA_BYTE_SMALL;
	*pData =(nCRC >> 8); pData++;
	*pData =(nCRC & 0xFF);

	UART0_SendString(pBase, DATA_BYTE_SMALL + EXTRA_SIZE);
	return nLen;
}

void _SendData(uint8* pBase, YmHandle pfTxHandle, uint8 nSeqNo, void* pParam)
{
	DBG_YM("Data(%d) ->\n", nSeqNo);

	uint8* pData = pBase;
	*pData = YMODEM_STX; pData++;
	*pData = nSeqNo; pData++;
	*pData = ~nSeqNo; pData++;
	uint32 nThisLen = DATA_BYTE_BIG;
	pfTxHandle(pData, &nThisLen, YS_DATA, pParam); // Get data.
	uint16 nCRC = UT_Crc16(pData, DATA_BYTE_BIG);
	pData += DATA_BYTE_BIG;
	*pData =(nCRC >> 8); pData++;
	*pData =(nCRC & 0xFF);

	UART0_SendString(pBase, DATA_BYTE_BIG + EXTRA_SIZE);
}

void _SendNull(uint8* pBase)
{
	DBG_YM("Null ->\n");
	uint8* pData = pBase;
	*pData = YMODEM_SOH; pData++;
	*pData = 0; pData++;
	*pData = 0xFF; pData++;
	memset(pData, 0x0, DATA_BYTE_SMALL);
	uint16 nCRC = UT_Crc16(pData, DATA_BYTE_SMALL);
	pData += DATA_BYTE_SMALL;
	*pData =(nCRC >> 8); pData++;
	*pData =(nCRC & 0xFF);
	UART0_SendString(pBase, DATA_BYTE_SMALL + EXTRA_SIZE);
}

/**
 * the size of aBuf >= MAX packet size.
*/
YRet ym_Tx(YReq* pReq, uint8* aBuf)
{
	UT_Printf("Data to Device to Host\n");
	uint8 nNewData;

	DBG_YM("Empty Rcv\n");
	_EmptyRxQ();

	if(false == _WaitRx(&nNewData)) return YR_TIMEOUT;
	if(YMODEM_C != nNewData) return YR_ERROR;
	DBG_YM("Start\n");

	uint32 nLen = _SendMeta(aBuf, pReq->pfHandle, pReq->pParam);

	if(false == _WaitRx(&nNewData)) return YR_TIMEOUT;
	if(YMODEM_ACK != nNewData) return YR_ERROR;
	DBG_YM("1st ACK\n");

	if(false == _WaitRx(&nNewData)) return YR_TIMEOUT;
	if(YMODEM_C != nNewData) return YR_ERROR;
	DBG_YM("1st C\n");

	uint8 nCntPkt = nLen / 1024;
	uint8 nSeq = 0;
	while(nSeq < nCntPkt)
	{
		nSeq++;
		_SendData(aBuf, pReq->pfHandle, nSeq, pReq->pParam);
		if(false == _WaitRx(&nNewData)) return YR_TIMEOUT;
		if(YMODEM_ACK != nNewData) return YR_ERROR;
		DBG_YM("Data Ack %d\n", nSeq);
	}

	_TxHost(YMODEM_EOT);
	if(false == _WaitRx(&nNewData)) return YR_TIMEOUT;
	if(YMODEM_NACK != nNewData) return YR_ERROR;
	DBG_YM("EOT NACK\n");

	_TxHost(YMODEM_EOT);
	if(false == _WaitRx(&nNewData)) return YR_TIMEOUT;
	if(YMODEM_NACK != nNewData) return YR_ERROR;
	DBG_YM("EOT ACK\n");

	if(false == _WaitRx(&nNewData)) return YR_TIMEOUT;
	if(YMODEM_C != nNewData) return YR_ERROR;
	DBG_YM("EOT C\n");

	_SendNull(aBuf);
	if(false == _WaitRx(&nNewData)) return YR_TIMEOUT;
	if(YMODEM_ACK != nNewData) return YR_ERROR;
	DBG_YM("END ACK\n");
}


bool _RxHandle(uint8 *pBuf, uint32 *pnBytes, YMState eStep, void *pParam)
{
	UNUSED(pParam);

	switch (eStep)
	{
	case YS_META:
	{
		DBG_YM("H:%s, %d\n", pBuf, *pnBytes); // File name and File Length.
		break;
	}
	case YS_DATA:
	{
		DBG_YM("D:%X, %d\n", pBuf, *pnBytes);
		break;
	}
	default:
	{
		DBG_YM("?:%X, %X\n", pBuf, pnBytes);
		break;
	}
	}
	return true;
}

bool _TxHandle(uint8 *pBuf, uint32 *pnBytes, YMState eStep, void *pParam)
{
	UNUSED(pParam);

	switch (eStep)
	{
	case YS_META:
	{
		sprintf((char *)pBuf, "%s", "test.log");
		*pnBytes = 1024 * 2 + 100;
		break;
	}
	case YS_DATA:
	{
		uint32 nOff = *pnBytes;
		memset(pBuf, '0' + (nOff / 1024), 1024);
		break;
	}
	default:
	{
		DBG_YM("?:%X, %X\n", pBuf, pnBytes);
		break;
	}
	}
	return true;
}

void ym_Cmd(uint8 argc, char* argv[])
{
	if (argc < 2)
	{
		UT_Printf("%s r <Addr>\n", argv[0]);
		UT_Printf("%s t <Addr> <Size>\n", argv[0]);
	}
	else
	{
		uint32 nAddr = UT_GetInt(argv[1]);
		UNUSED(nAddr);
		if ('r' == argv[1][0])
		{
			YReq stReq;
			stReq.bRx = true;
			stReq.pfHandle = _RxHandle;
			stReq.pParam = NULL;
			YM_Request(&stReq);
		}
		else if ('t' == argv[1][0])
		{
			YReq stReq;
			stReq.bRx = false;
			stReq.pfHandle = _TxHandle;
			stReq.pParam = NULL;
			YM_Request(&stReq);
		}
		else 
		{
			UT_Printf("Wrong command or parameter\n");
		}
	}
}


void cbf_RxUartYm(uint32 tag, uint32 result)
{
	UNUSED(tag);
	UNUSED(result);
	OS_AsyncEvt(BIT(EVT_UART_YM));
}

YReq gstReq;
uint8 gaData[YMODEM_BUF_LENGTH];

void ym_Run(void* pParam)
{
	UNUSED(pParam);
	YReq* pReq = &(gstReq);
	while(true)
	{
		if(pReq->bReq)
		{
			UART_SetCbf(cbf_RxUartYm, NULL);
			if(pReq->bRx)
			{
				ym_Rx(pReq, gaData);
			}
			else
			{
				ym_Tx(pReq, gaData);
			}

			pReq->bReq = false;
			pReq->pfHandle(NULL, NULL, YS_END, pReq->pParam);
			CLI_RegUartEvt();
		}
		OS_SyncEvt(BIT(EVT_YMODEM));
		OS_Wait(BIT(EVT_YMODEM), 0);
	}
}

bool YM_Request(YReq* pstReq)
{
	if(true != gstReq.bReq)
	{
		gstReq = *pstReq;
		OS_SyncEvt(BIT(EVT_YMODEM)); // to start YModem.
		return true;
	}
	return false;
}

#define STK_SIZE 	(256)
static uint32 gaYmStk[STK_SIZE];
void YM_Init(void)
{
	CLI_Register("ym", ym_Cmd);
	OS_CreateTask(ym_Run, gaYmStk + STK_SIZE - 1, NULL, "ym");
}
