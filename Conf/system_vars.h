/*
 * system_vars.h
 *
 *  Created on: 06.12.2017
 *      Author: Tomasz
 */

#ifndef SYSTEM_VARS_H_
#define SYSTEM_VARS_H_

typedef enum
{
	SET_PWM_ = 0x01,
	GET_STATE_ = 0x02,
	CALIBRATE_ = 0x03
}USART_RS485_CMD;

#define USART_RS485_MaxBufferSize			32
#define USART_RS485_b64BufferSize			USART_RS485_MaxBufferSize * 6 / 8

uint8_t USART_RS485_TX_buffer[USART_RS485_MaxBufferSize];

#endif /* SYSTEM_VARS_H_ */
