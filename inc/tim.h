/*
 * tim.h
 *
 *  Created on: 11.12.2017
 *      Author: Tomasz
 */

#ifndef TIM_H_
#define TIM_H_

#include "system_conf.h"

void TIM_Conf();

void TIM14_IRQHandler(void);
void TIM15_IRQHandler(void);

#endif /* TIM_H_ */
