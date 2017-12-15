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

volatile uint8_t USART_RS485_TX_buffer[USART_RS485_MaxBufferSize];
volatile uint8_t USART_RS485_RX_Ptr;

volatile uint8_t USART_RS485_WaitResponse_Flag;

typedef enum
{
	FreeDrive_EN  = 0x01,
	FreeDrive_DIS = 0x00
}FreeDrive_State;

typedef enum
{
	Dir_Positive =  0x02,
	Dir_Negative = 0x00
}Direction_State;

typedef struct
{
	uint8_t 			Address;
	uint16_t 			PWM;
	Direction_State		Direction;
	FreeDrive_State		FreeDrive;
	uint16_t			Current;
	uint16_t			Position;
}MotorDriver_Values_TypeDef;

#ifdef TestSetup
#define MotorDriver_Count					1
#endif
volatile MotorDriver_Values_TypeDef MotorDriver_List[MotorDriver_Count];
volatile MotorDriver_Values_TypeDef* MotorDriver_Polling;

#endif /* SYSTEM_VARS_H_ */
