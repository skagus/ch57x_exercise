/********************************** (C) COPYRIGHT *******************************
 * File Name          : CH57x_usbdev.c
 * Author             : WCH
 * Version            : V1.2
 * Date               : 2021/11/17
 * Description
 *********************************************************************************
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for 
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/

#include "CH57x_common.h"

uint8_t *pEP0_RAM_Addr;
uint8_t *pEP1_RAM_Addr;
uint8_t *pEP2_RAM_Addr;
uint8_t *pEP3_RAM_Addr;

/*********************************************************************
 * @fn      USB_DeviceInit
 *
 * @brief   USB设备功能初始化，4个端点，8个通道。
 *
 * @param   none
 *
 * @return  none
 */
void USB_DeviceInit(void)
{
	R8_USB_CTRL = 0x00; // 先设定模式,取消 RB_UC_CLR_ALL

	// End point configuration.
	R8_UEP4_1_MOD = RB_UEP4_RX_EN | RB_UEP4_TX_EN | RB_UEP1_RX_EN | RB_UEP1_TX_EN; // 端点4 OUT+IN,端点1 OUT+IN
	R8_UEP2_3_MOD = RB_UEP2_RX_EN | RB_UEP2_TX_EN | RB_UEP3_RX_EN | RB_UEP3_TX_EN; // 端点2 OUT+IN,端点3 OUT+IN

	R16_UEP0_DMA = (uint16_t)(uint32_t)pEP0_RAM_Addr;
	R16_UEP1_DMA = (uint16_t)(uint32_t)pEP1_RAM_Addr;
	R16_UEP2_DMA = (uint16_t)(uint32_t)pEP2_RAM_Addr;
	R16_UEP3_DMA = (uint16_t)(uint32_t)pEP3_RAM_Addr;

	R8_UEP0_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;
	R8_UEP1_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK | RB_UEP_AUTO_TOG;
	R8_UEP2_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK | RB_UEP_AUTO_TOG;
	R8_UEP3_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK | RB_UEP_AUTO_TOG;
	R8_UEP4_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;

	// Device initialize.
	R8_USB_DEV_AD = 0x00;
	// Start the USB device and DMA, 
	// and automatically return the NAK before the interrupt flag is cleared during the interrupt
	R8_USB_CTRL = RB_UC_DEV_PU_EN | RB_UC_INT_BUSY | RB_UC_DMA_EN;
	R16_PIN_ANALOG_IE |= RB_PIN_USB_IE | RB_PIN_USB_DP_PU;         // Prevents USB port floats and pull-up resistors
	R8_USB_INT_FG = 0xFF;                                          // Clear the interrupt flag
	R8_UDEV_CTRL = RB_UD_PD_DIS | RB_UD_PORT_EN;                   // Enable USB port.
	R8_USB_INT_EN = RB_UIE_SUSPEND | RB_UIE_BUS_RST | RB_UIE_TRANSFER;
}

/*********************************************************************
 * @fn      DevEP1_IN_Deal
 * @brief   EP1 Data Upload.
 * @param   l   - Upload data len (<64B)
 * @return  none
 */
void DevEP1_IN_Deal(uint8_t l)
{
	R8_UEP1_T_LEN = l;
	R8_UEP1_CTRL = (R8_UEP1_CTRL & ~MASK_UEP_T_RES) | UEP_T_RES_ACK;
}

void DevEP2_IN_Deal(uint8_t l)
{
	R8_UEP2_T_LEN = l;
	R8_UEP2_CTRL = (R8_UEP2_CTRL & ~MASK_UEP_T_RES) | UEP_T_RES_ACK;
}

void DevEP3_IN_Deal(uint8_t l)
{
	R8_UEP3_T_LEN = l;
	R8_UEP3_CTRL = (R8_UEP3_CTRL & ~MASK_UEP_T_RES) | UEP_T_RES_ACK;
}

void DevEP4_IN_Deal(uint8_t l)
{
	R8_UEP4_T_LEN = l;
	R8_UEP4_CTRL = (R8_UEP4_CTRL & ~MASK_UEP_T_RES) | UEP_T_RES_ACK;
}

