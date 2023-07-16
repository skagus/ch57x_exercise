#pragma once

#include "types.h"


typedef void (CmdHandler)(uint8 argc, char* argv[]);

void CLI_Init(void);
void CLI_Register(char* szCmd, CmdHandler *pHandle);
void CLI_RegUartEvt(void);



