/*
 * crc_ccitt.h
 *
 *  Created on: 04.01.2018
 *      Author: Tomasz
 */

#ifndef CRC_CCITT_H_
#define CRC_CCITT_H_

#include "stm32f0xx.h"

uint8_t CRC8_CCITT_Calc(uint8_t inCrc, uint8_t inData);

#endif /* CRC_CCITT_H_ */
