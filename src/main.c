/**
  ******************************************************************************
  * @file    main.c
  * @author  Ac6
  * @version V1.0
  * @date    01-December-2013
  * @brief   Default main function.
  ******************************************************************************
*/

#include "main.h"

int main(void)
{
	Init();

	GPIO_Conf();
	CRC_Conf();
	DMA_Conf();
	TIM_Conf();
	USART_Conf();

	LL_GPIO_SetOutputPin(LED_Port, LED1_Pin);

	static uint8_t USART_TX_ToEncode[24];
	static uint8_t USART_TX_Encoded_Len;

	int i = 10000;
	USART_TX_ToEncode[0] = 0x01;
	USART_TX_ToEncode[1] = SET_PWM_; // CMD
	USART_TX_ToEncode[2] = ((uint8_t*)&i)[0];
	USART_TX_ToEncode[3] = ((uint8_t*)&i)[1];
	USART_TX_ToEncode[4] = 0x02; //turn into higher value

	LL_CRC_ResetCRCCalculationUnit(CRC);
	LL_CRC_FeedData8(CRC, USART_TX_ToEncode[0]);
	LL_CRC_FeedData8(CRC, USART_TX_ToEncode[1]);
	LL_CRC_FeedData8(CRC, USART_TX_ToEncode[2]);
	LL_CRC_FeedData8(CRC, USART_TX_ToEncode[3]);
	LL_CRC_FeedData8(CRC, USART_TX_ToEncode[4]);
	USART_TX_ToEncode[5] = (uint8_t)LL_CRC_ReadData32(CRC);

	USART_TX_Encoded_Len = b64_encode(USART_TX_ToEncode, 6, USART_RS485_TX_buffer);
	USART_RS485_TX_buffer[USART_TX_Encoded_Len] = '\n';
	USART_TX_Encoded_Len++;

	LL_DMA_SetDataLength(DMA1, LL_DMA_CHANNEL_4, USART_TX_Encoded_Len); //start USART_TX transfer
	LL_GPIO_SetOutputPin(RS485_DRV_EN_Port, RS485_DRV_EN_Pin);
	LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_4);

	LL_GPIO_SetOutputPin(LED_Port, LED2_Pin);

	while(1)
	{

	}
}
