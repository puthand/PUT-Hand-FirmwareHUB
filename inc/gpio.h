/*
 * gpio.h
 *
 *  Created on: 11.12.2017
 *      Author: Tomasz
 */

#ifndef GPIO_H_
#define GPIO_H_

#include "system_conf.h"

void GPIO_Conf();

void UR_DO0_IRQHandler(void);
void UR_DO1_IRQHandler(void);

#endif /* GPIO_H_ */
