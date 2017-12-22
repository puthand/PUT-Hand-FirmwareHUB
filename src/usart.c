/*
 * usart.c
 *
 *  Created on: 11.12.2017
 *      Author: Tomasz
 */
#include "usart.h"

#include "base64.h"

static void USART_RS485();

static LL_USART_InitTypeDef USART_InitStruct;

void USART_Conf()
{
	LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_USART2);

	USART_RS485();
}

static volatile uint8_t USART_RS485_RX_buffer[USART_RS485_MaxBufferSize];

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

	NVIC_SetPriority(USART2_IRQn, 2);
	NVIC_EnableIRQ(USART2_IRQn);

	LL_USART_EnableDMAReq_TX(USART2);

	LL_USART_EnableIT_RXNE(USART2);

	LL_USART_Enable(USART2);
}

static volatile uint8_t USART_RS485_RX_Decoded[USART_RS485_b64BufferSize];
static volatile uint8_t USART_RS485_RX_Decoded_Len;

void USART2_IRQHandler(void)
{
	while(LL_USART_IsActiveFlag_RXNE(USART2))
	{
		USART_RS485_RX_buffer[USART_RS485_RX_Ptr] = LL_USART_ReceiveData8(USART2);

		if(USART_RS485_RX_buffer[USART_RS485_RX_Ptr] == '\n')
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
							LL_TIM_DisableCounter(TIM15);
							USART_RS485_WaitResponse_Flag = 0;//enable further polling operation

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
						}
					}
				}
			}
			USART_RS485_RX_Ptr = 0;
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
