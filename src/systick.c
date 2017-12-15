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

	LL_InitTick(6000000, 2U); //2 Hz

	NVIC_SetPriority(SysTick_IRQn, 0);
}

void SysTick_Handler(void)
{
	LL_GPIO_TogglePin(LED_Port, LED3_Pin);
}

