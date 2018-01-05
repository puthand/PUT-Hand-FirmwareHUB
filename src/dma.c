/*
 * dma.c
 *
 *  Created on: 11.12.2017
 *      Author: Tomasz
 */
#include "dma.h"

static void DMA_USART_RS485();
static void DMA_USART_FT232();

static LL_DMA_InitTypeDef DMA_InitStruct;

void DMA_Conf()
{
	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA1);

	DMA_USART_RS485();
	DMA_USART_FT232();
}

void DMA_USART_RS485()
{
	DMA_InitStruct.Direction = LL_DMA_DIRECTION_MEMORY_TO_PERIPH;
	DMA_InitStruct.MemoryOrM2MDstAddress = (uint32_t)&USART_RS485_TX_buffer;
	DMA_InitStruct.MemoryOrM2MDstDataSize = LL_DMA_MDATAALIGN_BYTE;
	DMA_InitStruct.MemoryOrM2MDstIncMode = LL_DMA_MEMORY_INCREMENT;
	DMA_InitStruct.Mode = LL_DMA_MODE_NORMAL;
	DMA_InitStruct.NbData = 0;
	DMA_InitStruct.PeriphOrM2MSrcAddress = LL_USART_DMA_GetRegAddr(USART2, LL_USART_DMA_REG_DATA_TRANSMIT);
	DMA_InitStruct.PeriphOrM2MSrcDataSize = LL_DMA_PDATAALIGN_BYTE;
	DMA_InitStruct.PeriphOrM2MSrcIncMode = LL_DMA_PERIPH_NOINCREMENT;
	DMA_InitStruct.Priority = LL_DMA_PRIORITY_HIGH;
	LL_DMA_Init(DMA1, LL_DMA_CHANNEL_4, &DMA_InitStruct);

	NVIC_SetPriority(DMA1_Channel4_5_IRQn, 0);
	NVIC_EnableIRQ(DMA1_Channel4_5_IRQn);

	LL_DMA_EnableIT_TC(DMA1, LL_DMA_CHANNEL_4);
}

void DMA_USART_FT232()
{
	DMA_InitStruct.Direction = LL_DMA_DIRECTION_MEMORY_TO_PERIPH;
	DMA_InitStruct.MemoryOrM2MDstAddress = (uint32_t)&USART_FT232_TX_buffer;
	DMA_InitStruct.MemoryOrM2MDstDataSize = LL_DMA_MDATAALIGN_BYTE;
	DMA_InitStruct.MemoryOrM2MDstIncMode = LL_DMA_MEMORY_INCREMENT;
	DMA_InitStruct.Mode = LL_DMA_MODE_NORMAL;
	DMA_InitStruct.NbData = 0;
	DMA_InitStruct.PeriphOrM2MSrcAddress = LL_USART_DMA_GetRegAddr(USART1, LL_USART_DMA_REG_DATA_TRANSMIT);
	DMA_InitStruct.PeriphOrM2MSrcDataSize = LL_DMA_PDATAALIGN_BYTE;
	DMA_InitStruct.PeriphOrM2MSrcIncMode = LL_DMA_PERIPH_NOINCREMENT;
	DMA_InitStruct.Priority = LL_DMA_PRIORITY_MEDIUM;
	LL_DMA_Init(DMA1, LL_DMA_CHANNEL_2, &DMA_InitStruct);

	NVIC_SetPriority(DMA1_Channel2_3_IRQn , 1);
	NVIC_EnableIRQ(DMA1_Channel2_3_IRQn);

	LL_SYSCFG_SetRemapDMA_USART(LL_SYSCFG_USART1TX_RMP_DMA1CH2);

	LL_DMA_EnableIT_TC(DMA1, LL_DMA_CHANNEL_2);
}

void DMA1_Channel4_5_IRQHandler(void)
{
	if(LL_DMA_IsActiveFlag_TC4(DMA1))
	{
		LL_DMA_DisableChannel(DMA1, LL_DMA_CHANNEL_4);

		LL_DMA_ClearFlag_TC4(DMA1);
	}
}

void DMA1_Channel2_3_IRQHandler(void)
{
	if(LL_DMA_IsActiveFlag_TC2(DMA1))
	{
		LL_DMA_DisableChannel(DMA1, LL_DMA_CHANNEL_2);

		SendingStatusState = SendingStatusState_SEND;

		LL_DMA_ClearFlag_TC2(DMA1);
	}
}
