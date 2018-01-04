/*
 * system_conf.h
 *
 *  Created on: 06.12.2017
 *      Author: Tomasz
 */

#ifndef SYSTEM_CONF_H_
#define SYSTEM_CONF_H_

#define TestSetup_9

#include "stm32f0xx.h"

#include "stm32f0xx_ll_adc.h"
#include "stm32f0xx_ll_bus.h"
#include "stm32f0xx_ll_comp.h"
#include "stm32f0xx_ll_cortex.h"
#include "stm32f0xx_ll_crc.h"
#include "stm32f0xx_ll_crs.h"
#include "stm32f0xx_ll_dac.h"
#include "stm32f0xx_ll_dma.h"
#include "stm32f0xx_ll_exti.h"
#include "stm32f0xx_ll_gpio.h"
#include "stm32f0xx_ll_i2c.h"
#include "stm32f0xx_ll_iwdg.h"
#include "stm32f0xx_ll_pwr.h"
#include "stm32f0xx_ll_rcc.h"
#include "stm32f0xx_ll_rtc.h"
#include "stm32f0xx_ll_spi.h"
#include "stm32f0xx_ll_system.h"
#include "stm32f0xx_ll_tim.h"
#include "stm32f0xx_ll_usart.h"
#include "stm32f0xx_ll_utils.h"
#include "stm32f0xx_ll_wwdg.h"

#include "system_vars.h"

#define LED1_Pin	 				LL_GPIO_PIN_4
#define LED2_Pin					LL_GPIO_PIN_5
#define LED3_Pin					LL_GPIO_PIN_6
#define LED4_Pin					LL_GPIO_PIN_7
#define LED_Port			 		GPIOB

#define UR_DI0_Pin					LL_GPIO_PIN_7
#define UR_DI0_Port					GPIOA
#define UR_DI1_Pin					LL_GPIO_PIN_0
#define UR_DI1_Port					GPIOB
#define UR_DO0_Pin					LL_GPIO_PIN_6
#define UR_DO0_Port					GPIOA
#define UR_DO0_EXTI_Port			LL_SYSCFG_EXTI_PORTA
#define UR_DO0_EXTI_Line			LL_SYSCFG_EXTI_LINE6
#define UR_DO0_IRQn					EXTI4_15_IRQn
#define UR_DO0_IRQHandler			EXTI4_15_IRQHandler
#define UR_DO1_Pin					LL_GPIO_PIN_1
#define UR_DO1_Port					GPIOB
#define UR_DO1_EXTI_Port			LL_SYSCFG_EXTI_PORTB
#define UR_DO1_EXTI_Line			LL_SYSCFG_EXTI_LINE1
#define UR_DO1_IRQn					EXTI0_1_IRQn
#define UR_DO1_IRQHandler			EXTI0_1_IRQHandler

#define RS485_DRV_EN_Pin			LL_GPIO_PIN_1
#define RS485_DRV_EN_Port			GPIOA
#define RS485_TX_Pin				LL_GPIO_PIN_2
#define RS485_RX_Pin				LL_GPIO_PIN_3
#define RS485_Port					GPIOA
#define RS485_GPIO_AF				LL_GPIO_AF_1

#define FT232_TX_Pin				LL_GPIO_PIN_9
#define FT232_RX_Pin				LL_GPIO_PIN_10
#define FT232_Port					GPIOA
#define FT232_GPIO_AF				LL_GPIO_AF_1

__STATIC_INLINE void ErrorIndicatorEnable(ErrorIndicator_Type Error)
{
	CurrentErrorType |= Error;
}

__STATIC_INLINE void ErrorIndicatorDisable(ErrorIndicator_Type Error)
{
	CurrentErrorType &= ~Error;
}

#endif /* SYSTEM_CONF_H_ */
