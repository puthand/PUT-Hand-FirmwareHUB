/*
 * systick.c
 *
 *  Created on: 13.12.2017
 *      Author: Tomasz
 */
#include "systick.h"

void SysTick_Conf()
{
	LL_SYSTICK_SetClkSource(LL_SYSTICK_CLKSOURCE_HCLK_DIV8);

	LL_InitTick(6000000, 1U); //2 Hz

	NVIC_SetPriority(SysTick_IRQn, 0);

	LL_SYSTICK_EnableIT();
}

void SysTick_Handler(void)
{
	if(CurrentErrorType & ERROR_CRC)
	{
		LL_GPIO_TogglePin(LED_Port, LED1_Pin);
	}

	if(CurrentErrorType & ERROR_TIMEOUT)
	{
		LL_GPIO_TogglePin(LED_Port, LED2_Pin);
	}

	if(CurrentErrorType & ERROR_MOTOR_FAULT)
	{
		LL_GPIO_TogglePin(LED_Port, LED3_Pin);
	}

	if(CurrentErrorType == ERROR_OK)
	{
		LL_GPIO_TogglePin(LED_Port, LED4_Pin);
	}
}

