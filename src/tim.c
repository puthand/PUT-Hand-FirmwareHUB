/*
 * tim.c
 *
 *  Created on: 11.12.2017
 *      Author: Tomasz
 */
#include "tim.h"

#include "base64.h"
#include "crc_ccitt.h"

static void TIM_MotorPolling(void);
static void TIM_MotorPolling_WatchDog(void);
static void TIM_FT232(void);

static LL_TIM_InitTypeDef TIM_InitStruct;

void TIM_Conf()
{
	LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM14);
	LL_APB1_GRP2_EnableClock(LL_APB1_GRP2_PERIPH_TIM15);
	LL_APB1_GRP2_EnableClock(LL_APB1_GRP2_PERIPH_TIM17);


	TIM_MotorPolling_WatchDog();
	TIM_MotorPolling();
	TIM_FT232();
}

void TIM_MotorPolling(void)
{
	TIM_InitStruct.Prescaler = 191; //Inc 250 000 Hz - 191
	TIM_InitStruct.CounterMode = LL_TIM_COUNTERMODE_UP;
	TIM_InitStruct.Autoreload = 2499; //Rel 250 Hz - 999; Rel 125 Hz - 1999; Rel 100 Hz - 2499
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

	NVIC_SetPriority(TIM15_IRQn, 0);
	NVIC_EnableIRQ(TIM15_IRQn);

	LL_TIM_ClearFlag_UPDATE(TIM15);
	LL_TIM_EnableIT_UPDATE(TIM15);

	LL_TIM_DisableCounter(TIM15);
}

void TIM_FT232()
{
	TIM_InitStruct.Prescaler = 191; //Inc 250 000 Hz - 191
	TIM_InitStruct.CounterMode = LL_TIM_COUNTERMODE_UP;
	TIM_InitStruct.Autoreload = 2499; //Rel 100Hz - 2499
	TIM_InitStruct.ClockDivision = LL_TIM_CLOCKDIVISION_DIV1;
	LL_TIM_Init(TIM17, &TIM_InitStruct);

	NVIC_SetPriority(TIM17_IRQn, 2);
	NVIC_EnableIRQ(TIM17_IRQn);

	LL_TIM_ClearFlag_UPDATE(TIM17);
	LL_TIM_EnableIT_UPDATE(TIM17);

	LL_TIM_EnableCounter(TIM17);
}

static uint8_t USART_RS485_TX_ToEncode[USART_RS485_DecodedBufferSize];
static uint8_t USART_RS485_TX_ToEncode_Len;
static uint8_t USART_RS485_TX_Encoded_Len;

volatile static int32_t PID_Err;
volatile static int32_t PID_Result;

void TIM14_IRQHandler(void)//TIM_MotorPolling and Internal regulator
{
	if(LL_TIM_IsActiveFlag_UPDATE(TIM14))
	{
		LL_TIM_ClearFlag_UPDATE(TIM14);

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
			MotorDriver_Polling = &(MotorDriver_List[i]);

			if(CalibrationProcedure == CALIBRATION_Disabled)//if standard polling
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

			LL_CRC_ResetCRCCalculationUnit(CRC);
			for(int i=0; i< USART_RS485_TX_ToEncode_Len; i++)
			{
				LL_CRC_FeedData8(CRC, USART_RS485_TX_ToEncode[i]);
			}
			USART_RS485_TX_ToEncode[USART_RS485_TX_ToEncode_Len] = (uint8_t)LL_CRC_ReadData32(CRC);
			USART_RS485_TX_ToEncode_Len++;

			USART_RS485_TX_Encoded_Len = b64_encode(USART_RS485_TX_ToEncode, USART_RS485_TX_ToEncode_Len, USART_RS485_TX_buffer);
			USART_RS485_TX_buffer[USART_RS485_TX_Encoded_Len] = '\n';
			USART_RS485_TX_Encoded_Len++;

			LL_DMA_SetDataLength(DMA1, LL_DMA_CHANNEL_4, USART_RS485_TX_Encoded_Len); //start USART_TX transfer
			LL_GPIO_SetOutputPin(RS485_DRV_EN_Port, RS485_DRV_EN_Pin);
			LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_4);

			USART_RS485_State = USART_RS485_State_WAITING;
			LL_TIM_SetCounter(TIM15, 0);
			LL_TIM_EnableCounter(TIM15);//enable timeout timer

			if(CalibrationProcedure != CALIBRATION_Perform)//perform internal PID regulator if enabled
			{
				if(SystemOperationMode == MODE_IDLE)
				{
					for(int i=0; i<MotorDriver_Count; i++)
					{
						MotorDriver_Polling->PWM = 0;
					}
				}else if(SystemOperationMode == MODE_INT_REGULATOR)
				{
					PID_Err = MotorDriver_Polling->PositionSet - MotorDriver_Polling->PositionCurrent;
					MotorDriver_Polling->PID_Integral += PID_Err;
					PID_Result = 0;

					PID_Result += MotorDriver_Polling->PID_Kp * PID_Err / 1000;
					PID_Result += MotorDriver_Polling->PID_Ki * MotorDriver_List[i].PID_Integral / 1000;
					PID_Result += MotorDriver_Polling->PID_Kd * (PID_Err - MotorDriver_Polling->PID_PrevErr) / 1000;

					MotorDriver_Polling->PID_PrevErr = PID_Err;

					if(PID_Result < 0)
					{
						PID_Result *= -1;
						MotorDriver_Polling->Direction = Dir_Negative;
					}else
					{
						MotorDriver_Polling->Direction = Dir_Positive;
					}

					if(PID_Result > 65535)
					{
						PID_Result =  65535;
					}

					MotorDriver_Polling->PWM = PID_Result;
					MotorDriver_Polling->FreeDrive = FreeDrive_DIS;
				}
			}

			while(USART_RS485_State == USART_RS485_State_WAITING); //blocking wait for response

			if(USART_RS485_State == USART_RS485_State_NEWPACKET) //if new packet, not timeout
			{
				if(USART_RS485_RX_Ptr > 1)//if any b64 data could be decoded
				{
					USART_RS485_RX_Decoded_Len = b64_decode(USART_RS485_RX_buffer, USART_RS485_RX_Ptr, USART_RS485_RX_Decoded);

					if(USART_RS485_RX_Decoded_Len >= 3)//id packet caintains at least addr, cmd and crc
					{
						if(USART_RS485_RX_Decoded[0] == 0) //if packet is addressed to host
						{
							LL_CRC_ResetCRCCalculationUnit(CRC);
							for(int i = 0; i < USART_RS485_RX_Decoded_Len - 1; i++)
							{
								LL_CRC_FeedData8(CRC, USART_RS485_RX_Decoded[i]);
							}

							if(USART_RS485_RX_Decoded[USART_RS485_RX_Decoded_Len - 1] == (uint8_t)LL_CRC_ReadData32(CRC)) //if CRC is ok
							{
								switch(USART_RS485_RX_Decoded[1])//CMD
								{
									case SET_PWM_: //new status
									case GET_STATE_:
										((uint8_t*)&(MotorDriver_Polling->Current))[0] = USART_RS485_RX_Decoded[2];
										((uint8_t*)&(MotorDriver_Polling->Current))[1] = USART_RS485_RX_Decoded[3];
										((uint8_t*)&(MotorDriver_Polling->PositionCurrent))[0] = USART_RS485_RX_Decoded[4];
										((uint8_t*)&(MotorDriver_Polling->PositionCurrent))[1] = USART_RS485_RX_Decoded[5];
										MotorDriver_Polling->Operation = USART_RS485_RX_Decoded[6];
										if(MotorDriver_Polling->Operation == Operation_Fault)
										{
											ErrorIndicatorEnable(ERROR_MOTOR_FAULT);
										}
										break;
									case CALIBRATE_:
										break;
								}
							}else
							{
								ErrorIndicatorEnable(ERROR_RS485_CRC);
							}
						}
					}
				}

				USART_RS485_RX_Ptr = 0;
			}
		}

		if(CalibrationProcedure == CALIBRATION_Perform) //if calibration finished change watchdog back to 1ms
		{
			LL_TIM_SetPrescaler(TIM15, TIM_MotorPolling_WatchDog_Prescaler);
			LL_TIM_SetAutoReload(TIM15, TIM_MotorPolling_WatchDog_Period);

			LL_TIM_DisableIT_UPDATE(TIM15);
			LL_TIM_GenerateEvent_UPDATE(TIM15);
			LL_TIM_ClearFlag_UPDATE(TIM15);
			LL_TIM_EnableIT_UPDATE(TIM15);

			CalibrationProcedure = CALIBRATION_Disabled;
		}
	}
}

void TIM15_IRQHandler(void)//TIM_MotorPolling_WatchDog
{
	if(LL_TIM_IsActiveFlag_UPDATE(TIM15))
	{
		LL_TIM_ClearFlag_UPDATE(TIM15);

		//IRQ fired when MotorDriver module response time was exceeded
		LL_TIM_DisableCounter(TIM15);

		//reset communication, allow further communications
		ErrorIndicatorEnable(ERROR_RS485_TIMEOUT);
		USART_RS485_State = USART_RS485_State_TIMEOUT;
		USART_RS485_RX_Ptr = 0;

	}
}

static uint8_t USART_FT232_TX_ToEncode[USART_FT232_DecodedBufferSize];
static uint16_t USART_FT232_TX_Encoded_Len;
static uint16_t FT232_TX_Len;
static MotorDriver_Values_TypeDef* Driver;

void TIM17_IRQHandler(void)//TIM_FT232
{
	if(LL_TIM_IsActiveFlag_UPDATE(TIM17))
	{
		LL_TIM_ClearFlag_UPDATE(TIM17);

		if(SendSatusToPC == SendSatusToPC_ENABLED && SendingStatusState == SendingStatusState_SEND)
		{
			USART_FT232_TX_ToEncode[0] = SystemOperationMode;
			USART_FT232_TX_ToEncode[1] = CalibrationProcedure;
			USART_FT232_TX_ToEncode[2] = CurrentErrorType;
			USART_FT232_TX_ToEncode[3] = MotorDriver_Count;
			FT232_TX_Len = 4;

			for(int i=0; i < MotorDriver_Count; i++)
			{
				Driver = &(MotorDriver_List[i]);

				USART_FT232_TX_ToEncode[FT232_TX_Len++] = Driver->FreeDrive | Driver->Direction | Driver->Operation;
				USART_FT232_TX_ToEncode[FT232_TX_Len++] = ((uint8_t*)&(Driver->PWM))[1];
				USART_FT232_TX_ToEncode[FT232_TX_Len++] = ((uint8_t*)&(Driver->PWM))[2];
				USART_FT232_TX_ToEncode[FT232_TX_Len++] = ((uint8_t*)&(Driver->PositionSet))[1];
				USART_FT232_TX_ToEncode[FT232_TX_Len++] = ((uint8_t*)&(Driver->PositionSet))[2];
				USART_FT232_TX_ToEncode[FT232_TX_Len++] = ((uint8_t*)&(Driver->PositionCurrent))[1];
				USART_FT232_TX_ToEncode[FT232_TX_Len++] = ((uint8_t*)&(Driver->PositionCurrent))[2];
				USART_FT232_TX_ToEncode[FT232_TX_Len++] = ((uint8_t*)&(Driver->Current))[1];
				USART_FT232_TX_ToEncode[FT232_TX_Len++] = ((uint8_t*)&(Driver->Current))[2];
			}

			uint8_t CrcVal = 0x00;
			for(int i=0; i < FT232_TX_Len; i++)
			{
				CrcVal = CRC8_CCITT_Calc(CrcVal, USART_FT232_TX_ToEncode[i]);
			}

			USART_FT232_TX_ToEncode[FT232_TX_Len++] = CrcVal;

			USART_FT232_TX_Encoded_Len = b64_encode(USART_FT232_TX_ToEncode, FT232_TX_Len, USART_FT232_TX_buffer);
			USART_RS485_TX_buffer[USART_RS485_TX_Encoded_Len] = '\n';

			USART_FT232_TX_Ptr = 0;
			SendingStatusState = SendingStatusState_SENDING;
			LL_USART_EnableIT_TXE(USART1);
		}
	}
}
