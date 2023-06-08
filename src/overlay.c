#include "cli.h"
#include "overlay.h"

#define OVL_DST_IDX 	(0)
#define OVL_SRC_IDX 	(1)
#define OVL_SIZE_IDX 	(2)

extern uint32 __ovly_table[2][3];


/// @brief Overlay 0 ///////////////////////////////
uint32 gnOvl0 = 10000;

__attribute__((section(".ovl0")))
uint32 OVL0_DoAction()		// Slot 1
{
	gnOvl0++;
	CLI_Printf("In OVL0 : %d\n", gnOvl0);
	return gnOvl0;
}

/// @brief Overlay 1 ///////////////////////////////
uint32 gnOvl1 = 10100;

__attribute__((section(".ovl1")))
uint32 OVL1_Func()
{
	return gnOvl1;
}

__attribute__((section(".ovl1")))
uint32 OVL1_DoAction()		// Slot 1
{
	gnOvl1++;
	CLI_Printf("In OVL1 : %d\n", OVL1_Func());
	return gnOvl1;
}

/// @brief Overlay engine ///////////////////////////////
void OVL_LoadSlot(uint32 nSlot)
{
	memcpy((uint8*)__ovly_table[nSlot][OVL_DST_IDX],
			(uint8*)__ovly_table[nSlot][OVL_SRC_IDX],
			__ovly_table[nSlot][OVL_SIZE_IDX]);
}

void ovl_Load(uint8 argc, char* argv[])
{
	if(argc < 2)
	{
		CLI_Printf("%s <address>\r\n", argv[0]);
	}
	else
	{
		uint32 nSlot;
		nSlot = CLI_GetInt(argv[1]);
		OVL_LoadSlot(nSlot);
		if(0 == nSlot)
		{
			CLI_Printf("Call %p\r\n", OVL0_DoAction);
			OVL0_DoAction();
		}
		else if(1 == nSlot)
		{
			CLI_Printf("Call %p\r\n", OVL1_DoAction);
			OVL1_DoAction();
		}
	}
}

void OVL_Init()
{
	CLI_Register("ovl", ovl_Load);
}

