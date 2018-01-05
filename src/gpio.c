/*
 * gpio.c
 *
 *  Created on: 11.12.2017
 *      Author: Tomasz
 */
#include "gpio.h"

static void GPIO_LED();
static void GPIO_UR();
static void GPIO_RS485();
static void GPIO_FT232();

static LL_EXTI_InitTypeDef EXTI_InitStruct;
static LL_GPIO_InitTypeDef GPIO_InitStruct;

void GPIO_Conf()
{
	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);
	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOB);

	GPIO_LED();
	GPIO_UR();
	GPIO_RS485();
	GPIO_FT232();
}

void GPIO_LED()
{
	GPIO_InitStruct.Pin = LED1_Pin | LED2_Pin | LED3_Pin | LED4_Pin;
	GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
	GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
	GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
	GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
	LL_GPIO_Init(LED_Port, &GPIO_InitStruct);

	LL_GPIO_ResetOutputPin(LED_Port, LED1_Pin | LED2_Pin | LED3_Pin | LED4_Pin);
}

void GPIO_UR()
{
	GPIO_InitStruct.Pin = UR_DI0_Pin;
	GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
	GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
	GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
	GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
	LL_GPIO_Init(UR_DI0_Port, &GPIO_InitStruct);

	LL_GPIO_ResetOutputPin(UR_DI0_Port, UR_DI0_Pin);

	GPIO_InitStruct.Pin = UR_DI1_Pin;
	LL_GPIO_Init(UR_DI1_Port, &GPIO_InitStruct);

	LL_GPIO_ResetOutputPin(UR_DI1_Port, UR_DI1_Pin);

	GPIO_InitStruct.Pin = UR_DO0_Pin;
	GPIO_InitStruct.Mode = LL_GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
	LL_GPIO_Init(UR_DO0_Port, &GPIO_InitStruct);

	LL_SYSCFG_SetEXTISource(UR_DO0_EXTI_Port, UR_DO0_EXTI_Line);

	EXTI_InitStruct.Line_0_31 = UR_DO0_EXTI_Line;
	EXTI_InitStruct.LineCommand = ENABLE;
	EXTI_InitStruct.Mode = LL_EXTI_MODE_IT;
	EXTI_InitStruct.Trigger = LL_EXTI_TRIGGER_RISING_FALLING;
	LL_EXTI_Init(&EXTI_InitStruct);

	NVIC_SetPriority(UR_DO0_IRQn, 3);
	NVIC_EnableIRQ(UR_DO0_IRQn);

	LL_EXTI_EnableIT_0_31(UR_DO0_EXTI_Line);

	GPIO_InitStruct.Pin = UR_DO1_Pin;
	LL_GPIO_Init(UR_DO1_Port, &GPIO_InitStruct);

	LL_SYSCFG_SetEXTISource(UR_DO1_EXTI_Port, UR_DO1_EXTI_Line);

	EXTI_InitStruct.Line_0_31 = UR_DO1_EXTI_Line;
	LL_EXTI_Init(&EXTI_InitStruct);

	NVIC_SetPriority(UR_DO1_IRQn, 3);
	NVIC_EnableIRQ(UR_DO1_IRQn);

	LL_EXTI_EnableIT_0_31(UR_DO1_EXTI_Line);
}

void GPIO_RS485()
{
	GPIO_InitStruct.Pin = RS485_DRV_EN_Pin;
	GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
	GPIO_InitStruct.Alternate = RS485_GPIO_AF;
	LL_GPIO_Init(RS485_Port, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = RS485_TX_Pin | RS485_RX_Pin;
	GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
	GPIO_InitStruct.Alternate = RS485_GPIO_AF;
	LL_GPIO_Init(RS485_Port, &GPIO_InitStruct);
}

void GPIO_FT232()
{
	GPIO_InitStruct.Pin = FT232_TX_Pin | FT232_RX_Pin;
	GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
	GPIO_InitStruct.Alternate = FT232_GPIO_AF;
	LL_GPIO_Init(FT232_Port, &GPIO_InitStruct);
}

void UR_DO0_IRQHandler(void)
{
	if(LL_EXTI_IsActiveFlag_0_31(UR_DO0_EXTI_Line) != RESET)
	{
		LL_EXTI_ClearFlag_0_31(UR_DO0_EXTI_Line);
	}
}

void UR_DO1_IRQHandler(void)
{
	if(LL_EXTI_IsActiveFlag_0_31(UR_DO1_EXTI_Line) != RESET)
	{
		LL_EXTI_ClearFlag_0_31(UR_DO1_EXTI_Line);
	}
}
