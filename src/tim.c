/*
 * tim.c
 *
 *  Created on: 11.12.2017
 *      Author: Tomasz
 */
#include "tim.h"

static void TIM_MotorPolling(void);
static void TIM_MotorPolling_WatchDog(void);
static void TIM_InternalRegulator(void);

static LL_TIM_InitTypeDef TIM_InitStruct;

void TIM_Conf()
{
	LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM14);
	LL_APB1_GRP2_EnableClock(LL_APB1_GRP2_PERIPH_TIM15);
	LL_APB1_GRP2_EnableClock(LL_APB1_GRP2_PERIPH_TIM16);


	TIM_MotorPolling_WatchDog();
	TIM_MotorPolling();
	TIM_InternalRegulator();
}

void TIM_MotorPolling(void)
{
	TIM_InitStruct.Prescaler = 191; //Inc 250 000 Hz - 191
	TIM_InitStruct.CounterMode = LL_TIM_COUNTERMODE_UP;
	TIM_InitStruct.Autoreload = 1999; //Rel 250 Hz - 999; Rel 125 Hz - 1999
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
	TIM_InitStruct.Prescaler = TIM_MotorPolling_WatchDog_Prescaler;
	TIM_InitStruct.CounterMode = LL_TIM_COUNTERMODE_UP;
	TIM_InitStruct.Autoreload = TIM_MotorPolling_WatchDog_Period;
	TIM_InitStruct.ClockDivision = LL_TIM_CLOCKDIVISION_DIV1;
	LL_TIM_Init(TIM15, &TIM_InitStruct);

	NVIC_SetPriority(TIM15_IRQn, 2);
	NVIC_EnableIRQ(TIM15_IRQn);

	LL_TIM_ClearFlag_UPDATE(TIM15);
	LL_TIM_EnableIT_UPDATE(TIM15);

	LL_TIM_DisableCounter(TIM15);
}

void TIM_InternalRegulator(void)
{
	TIM_InitStruct.Prescaler = 191; //Inc 250 000 Hz - 191
	TIM_InitStruct.CounterMode = LL_TIM_COUNTERMODE_UP;
	TIM_InitStruct.Autoreload = 2499; //Rel 100Hz - 2049
	TIM_InitStruct.ClockDivision = LL_TIM_CLOCKDIVISION_DIV1;
	LL_TIM_Init(TIM16, &TIM_InitStruct);

	NVIC_SetPriority(TIM16_IRQn, 3);
	NVIC_EnableIRQ(TIM16_IRQn);

	LL_TIM_ClearFlag_UPDATE(TIM16);
	LL_TIM_EnableIT_UPDATE(TIM16);

	LL_TIM_EnableCounter(TIM16);
}

static uint8_t USART_RS485_TX_ToEncode[USART_RS485_b64BufferSize];
static uint8_t USART_RS485_TX_ToEncode_Len;
static uint8_t USART_RS485_TX_Encoded_Len;

void TIM14_IRQHandler(void)
{
	if(LL_TIM_IsActiveFlag_UPDATE(TIM14))
	{

		if(CalibrationProcedure == CALIBRATION_Perform) //if calibration procedure change watchdog to 5s
		{
			LL_TIM_SetPrescaler(TIM15, TIM_MotorCalibration_WatchDog_Prescaler);
			LL_TIM_SetAutoReload(TIM15, TIM_MotorCalibration_WatchDog_Period);

			LL_TIM_DisableIT_UPDATE(TIM15);
			LL_TIM_GenerateEvent_UPDATE(TIM15);
			LL_TIM_ClearFlag_UPDATE(TIM15);
			LL_TIM_EnableIT_UPDATE(TIM15);
		}

		//polling all connected motor drivers, and setting current data
		for(int i=0; i < MotorDriver_Count; i++)
		{
			USART_RS485_WaitResponse_Flag = 1;

			MotorDriver_Polling = &(MotorDriver_List[i]);

			if(CalibrationProcedure == CALIBRATION_None)//if standard polling
			{
				USART_RS485_TX_ToEncode[0] = MotorDriver_Polling->Address;
				USART_RS485_TX_ToEncode[1] = SET_PWM_;
				USART_RS485_TX_ToEncode[2] = ((uint8_t*)&(MotorDriver_Polling->PWM))[0];
				USART_RS485_TX_ToEncode[3] = ((uint8_t*)&(MotorDriver_Polling->PWM))[1];
				USART_RS485_TX_ToEncode[4] = MotorDriver_Polling->FreeDrive | MotorDriver_Polling->Direction;
				if(MotorDriver_Polling->ResetFaultFlag == FaultFlag_Reset)
				{
					USART_RS485_TX_ToEncode[4] |= FaultFlag_Reset;
					MotorDriver_Polling->ResetFaultFlag = FaultFlag_Keep;
				}

				USART_RS485_TX_ToEncode_Len = 5;
			}else //if calibration procedure
			{
				USART_RS485_TX_ToEncode[0] = MotorDriver_Polling->Address;
				USART_RS485_TX_ToEncode[1] = CALIBRATE_;

				USART_RS485_TX_ToEncode_Len = 2;
			}

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
			LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_4);

			LL_TIM_SetCounter(TIM15, 0);
			LL_TIM_EnableCounter(TIM15);//timeout timer
			while(USART_RS485_WaitResponse_Flag); //blocking wait for response
		}

		if(CalibrationProcedure == CALIBRATION_Perform) //if calibration finished change watchdog back to 1ms
		{
			LL_TIM_SetPrescaler(TIM15, TIM_MotorPolling_WatchDog_Prescaler);
			LL_TIM_SetAutoReload(TIM15, TIM_MotorPolling_WatchDog_Period);

			LL_TIM_DisableIT_UPDATE(TIM15);
			LL_TIM_GenerateEvent_UPDATE(TIM15);
			LL_TIM_ClearFlag_UPDATE(TIM15);
			LL_TIM_EnableIT_UPDATE(TIM15);

			CalibrationProcedure = CALIBRATION_None;
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
		ErrorIndicatorEnable(ERROR_TIMEOUT);
		//LL_GPIO_TogglePin(LED_Port, LED2_Pin);
		USART_RS485_RX_Ptr = 0;
		USART_RS485_WaitResponse_Flag = 0;

		LL_TIM_ClearFlag_UPDATE(TIM15);
	}
}

void TIM16_IRQHandler(void)
{
	if(LL_TIM_IsActiveFlag_UPDATE(TIM16))
	{
		if(SystemOperationMode == MODE_IDLE)
		{
			for(int i=0; i<MotorDriver_Count; i++)
			{
				MotorDriver_List[i].PWM = 0;
				MotorDriver_List[i].FreeDrive = FreeDrive_EN;
			}
		}else if(SystemOperationMode == MODE_INT_REGULATOR)
		{

		}
		LL_TIM_ClearFlag_UPDATE(TIM16);
	}
}
