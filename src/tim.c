/*
 * tim.c
 *
 *  Created on: 11.12.2017
 *      Author: Tomasz
 */
#include "tim.h"

void TIM_MotorPolling(void);
void TIM_MotorPolling_WatchDog(void);

static LL_TIM_InitTypeDef TIM_InitStruct;

void TIM_Conf()
{
	LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM14);
	LL_APB1_GRP2_EnableClock(LL_APB1_GRP2_PERIPH_TIM15);

	TIM_MotorPolling();
	TIM_MotorPolling_WatchDog();
}

void TIM_MotorPolling(void)
{
	TIM_InitStruct.Prescaler = 191; //Inc 250 000 Hz - 191    /// 1s - 47999
	TIM_InitStruct.CounterMode = LL_TIM_COUNTERMODE_UP;
	TIM_InitStruct.Autoreload = 999; //Rel 250 Hz - 999
	TIM_InitStruct.ClockDivision = LL_TIM_CLOCKDIVISION_DIV1;
	LL_TIM_Init(TIM14, &TIM_InitStruct);

	NVIC_SetPriority(TIM14_IRQn, 3);
	NVIC_EnableIRQ(TIM14_IRQn);

	LL_TIM_ClearFlag_UPDATE(TIM14);
	LL_TIM_EnableIT_UPDATE(TIM14);

	LL_TIM_EnableCounter(TIM14);
}

void TIM_MotorPolling_WatchDog(void)
{
	TIM_InitStruct.Prescaler = 191; //Inc 250 000 Hz
	TIM_InitStruct.CounterMode = LL_TIM_COUNTERMODE_UP;
	TIM_InitStruct.Autoreload = 999; // Rel 249 = 1 ms = 1000 Hz
	TIM_InitStruct.ClockDivision = LL_TIM_CLOCKDIVISION_DIV1;
	LL_TIM_Init(TIM15, &TIM_InitStruct);

	NVIC_SetPriority(TIM15_IRQn, 2);
	NVIC_EnableIRQ(TIM15_IRQn);

	LL_TIM_ClearFlag_UPDATE(TIM15);
	LL_TIM_EnableIT_UPDATE(TIM15);

	LL_TIM_DisableCounter(TIM15);
}

static uint8_t USART_RS485_TX_ToEncode[USART_RS485_b64BufferSize];
static uint8_t USART_RS485_TX_ToEncode_Len;
static uint8_t USART_RS485_TX_Encoded_Len;

void TIM14_IRQHandler(void)
{
	if(LL_TIM_IsActiveFlag_UPDATE(TIM14))
	{
		LL_GPIO_TogglePin(LED_Port, LED4_Pin);
		//polling all connected motor drivers, and setting current data
		for(int i=0; i < MotorDriver_Count; i++)
		{
			USART_RS485_WaitResponse_Flag = 1;

			MotorDriver_Polling = &(MotorDriver_List[i]);

			USART_RS485_TX_ToEncode[0] = MotorDriver_Polling->Address;
			USART_RS485_TX_ToEncode[1] = SET_PWM_;
			USART_RS485_TX_ToEncode[2] = ((uint8_t*)&(MotorDriver_Polling->PWM))[0];
			USART_RS485_TX_ToEncode[3] = ((uint8_t*)&(MotorDriver_Polling->PWM))[1];
			USART_RS485_TX_ToEncode[4] = MotorDriver_Polling->FreeDrive | MotorDriver_Polling->Direction;

			USART_RS485_TX_ToEncode_Len = 5;

			LL_USART_DisableIT_RXNE(USART2); // disable usart receiving to be sure not to interrupt the crc calculation
			LL_CRC_ResetCRCCalculationUnit(CRC);
			for(int i=0; i< USART_RS485_TX_ToEncode_Len; i++)
			{
				LL_CRC_FeedData8(CRC, USART_RS485_TX_ToEncode[i]);
			}
			USART_RS485_TX_ToEncode[USART_RS485_TX_ToEncode_Len] = (uint8_t)LL_CRC_ReadData32(CRC);
			USART_RS485_TX_ToEncode_Len++;
			LL_USART_EnableIT_RXNE(USART2);

			USART_RS485_TX_Encoded_Len = b64_encode(USART_RS485_TX_ToEncode, USART_RS485_TX_ToEncode_Len, USART_RS485_TX_buffer);
			USART_RS485_TX_buffer[USART_RS485_TX_Encoded_Len] = '\n';
			USART_RS485_TX_Encoded_Len++;

			LL_DMA_SetDataLength(DMA1, LL_DMA_CHANNEL_4, USART_RS485_TX_Encoded_Len); //start USART_TX transfer
			LL_GPIO_SetOutputPin(RS485_DRV_EN_Port, RS485_DRV_EN_Pin);
			for(volatile int i=0; i<0xF; i++); //wait for driver enable
			LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_4);

			LL_TIM_SetCounter(TIM15, 0);
			LL_TIM_EnableCounter(TIM15);//timeout timer
			while(USART_RS485_WaitResponse_Flag); //blocking wait for response
		}

		LL_TIM_ClearFlag_UPDATE(TIM14);
	}
}

void TIM15_IRQHandler(void)
{
	if(LL_TIM_IsActiveFlag_UPDATE(TIM15))
	{
		//IRQ fired when MotorDriver module response time was exceeded
		LL_TIM_DisableCounter(TIM15);

		//reset communication, allow further communications
		//ErrorIndicatorEnable();
		LL_GPIO_TogglePin(LED_Port, LED3_Pin);
		USART_RS485_RX_Ptr = 0;
		USART_RS485_WaitResponse_Flag = 0;

		LL_TIM_ClearFlag_UPDATE(TIM15);
	}
}
