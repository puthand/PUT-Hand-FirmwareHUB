/*
 * usart.c
 *
 *  Created on: 11.12.2017
 *      Author: Tomasz
 */
#include "usart.h"

#include "base64.h"
#include "crc_ccitt.h"

static void USART_RS485();
static void USART_FT232();

static LL_USART_InitTypeDef USART_InitStruct;

void USART_Conf()
{
	LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_USART2);
	LL_APB1_GRP2_EnableClock(LL_APB1_GRP2_PERIPH_USART1);

	USART_RS485();
	USART_FT232();
}

static void USART_RS485()
{
	USART_RS485_RX_Ptr = 0;

	USART_InitStruct.BaudRate = 500000;
	USART_InitStruct.DataWidth = LL_USART_DATAWIDTH_8B;
	USART_InitStruct.StopBits = LL_USART_STOPBITS_2;
	USART_InitStruct.Parity = LL_USART_PARITY_NONE;
	USART_InitStruct.TransferDirection = LL_USART_DIRECTION_TX_RX;
	USART_InitStruct.HardwareFlowControl = LL_USART_HWCONTROL_NONE;
	USART_InitStruct.OverSampling = LL_USART_OVERSAMPLING_16;
	LL_USART_Init(USART2, &USART_InitStruct);

	LL_USART_DisableOverrunDetect(USART2);
	LL_USART_ConfigAsyncMode(USART2);

	NVIC_SetPriority(USART2_IRQn, 0);
	NVIC_EnableIRQ(USART2_IRQn);

	LL_USART_EnableDMAReq_TX(USART2);

	LL_USART_EnableIT_RXNE(USART2);

	LL_USART_Enable(USART2);
}

static volatile uint8_t USART_FT232_RX_buffer[USART_FT232_MaxBufferSize];
static volatile uint16_t USART_FT232_RX_Ptr;

void USART_FT232()
{
	USART_FT232_RX_Ptr = 0;
	USART_FT232_TX_Ptr = 0;

	USART_InitStruct.BaudRate = 1000000;
	USART_InitStruct.DataWidth = LL_USART_DATAWIDTH_8B;
	USART_InitStruct.StopBits = LL_USART_STOPBITS_1;
	USART_InitStruct.Parity = LL_USART_PARITY_NONE;
	USART_InitStruct.TransferDirection = LL_USART_DIRECTION_TX_RX;
	USART_InitStruct.HardwareFlowControl = LL_USART_HWCONTROL_NONE;
	USART_InitStruct.OverSampling = LL_USART_OVERSAMPLING_16;
	LL_USART_Init(USART1, &USART_InitStruct);

	LL_USART_DisableOverrunDetect(USART1);
	LL_USART_ConfigAsyncMode(USART1);

	NVIC_SetPriority(USART1_IRQn, 1);
	NVIC_EnableIRQ(USART1_IRQn);

	LL_USART_EnableIT_RXNE(USART1);

	LL_USART_Enable(USART1);
}

void USART2_IRQHandler(void)
{
	while(LL_USART_IsActiveFlag_RXNE(USART2))
	{
		USART_RS485_RX_buffer[USART_RS485_RX_Ptr] = LL_USART_ReceiveData8(USART2);

		if(USART_RS485_RX_buffer[USART_RS485_RX_Ptr] == '\n') //if data is outgoing (to MotorDriver modules)
		{
			USART_RS485_RX_Ptr = 0;

		}else if(USART_RS485_RX_buffer[USART_RS485_RX_Ptr] == '\r') //if data is incoming (to Hub)
		{
			LL_TIM_DisableCounter(TIM15);
			USART_RS485_State = USART_RS485_State_NEWPACKET;
		}else
		{
			USART_RS485_RX_Ptr++;
			if(USART_RS485_RX_Ptr >= USART_RS485_MaxBufferSize)
			{
				USART_RS485_RX_Ptr = 0;
			}
		}
	}

	if(LL_USART_IsActiveFlag_TXE(USART2) && LL_USART_IsEnabledIT_TXE(USART2))
	{
		while(!LL_USART_IsActiveFlag_TC(USART2));

		LL_GPIO_ResetOutputPin(RS485_DRV_EN_Port, RS485_DRV_EN_Pin);//disable RS trasmitter

		LL_USART_DisableIT_TXE(USART2);
	}
}

static volatile uint8_t USART_FT232_RX_Decoded[USART_FT232_DecodedBufferSize];
static volatile uint8_t USART_FT232_RX_Decoded_Len;

void USART1_IRQHandler(void)
{
	while(LL_USART_IsActiveFlag_RXNE(USART1))
	{
		USART_FT232_RX_buffer[USART_FT232_RX_Ptr] = LL_USART_ReceiveData8(USART1);

		if(USART_FT232_RX_buffer[USART_FT232_RX_Ptr] == '\n')
		{
			if(USART_FT232_RX_Ptr > 1)//if any b64 data could be decoded
			{
				USART_FT232_RX_Decoded_Len = b64_decode(USART_FT232_RX_buffer, USART_FT232_RX_Ptr, USART_FT232_RX_Decoded);

				//DO SHIT
			}

			USART_FT232_RX_Ptr = 0;
		}else
		{
			USART_FT232_RX_Ptr++;
			if(USART_FT232_RX_Ptr >= USART_FT232_MaxBufferSize)
			{
				USART_FT232_RX_Ptr = 0;
			}
		}
	}

	while(LL_USART_IsActiveFlag_TXE(USART1))
	{
		LL_USART_TransmitData8(USART1, USART_FT232_TX_buffer[USART_FT232_TX_Ptr]);

		if(USART_FT232_TX_buffer[USART_FT232_TX_Ptr] == '\n' || USART_FT232_TX_Ptr >= (USART_FT232_MaxBufferSize-1))
		{
			LL_USART_DisableIT_TXE(USART1);
			SendingStatusState = SendingStatusState_SEND;
			break;
		}

		USART_FT232_TX_Ptr++;
	}
}
