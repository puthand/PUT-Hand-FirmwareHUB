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

	while(1)
	{

	}
}
