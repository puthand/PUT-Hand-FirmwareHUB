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
#define USART_RS485_DecodedBufferSize		USART_RS485_MaxBufferSize * 6 / 8
volatile uint8_t USART_RS485_RX_buffer[USART_RS485_MaxBufferSize];
volatile uint8_t USART_RS485_TX_buffer[USART_RS485_MaxBufferSize];
volatile uint8_t USART_RS485_RX_Ptr;
volatile uint8_t USART_RS485_RX_Decoded[USART_RS485_DecodedBufferSize];
volatile uint8_t USART_RS485_RX_Decoded_Len;


typedef enum{
	USART_RS485_State_WAITING,
	USART_RS485_State_TIMEOUT,
	USART_RS485_State_NEWPACKET
}USART_RS485_State_Typedef;
volatile USART_RS485_State_Typedef USART_RS485_State;

#define USART_FT232_MaxBufferSize			200
#define USART_FT232_DecodedBufferSize		USART_FT232_MaxBufferSize * 6 / 8
volatile uint8_t USART_FT232_TX_buffer[USART_FT232_MaxBufferSize];
volatile uint16_t USART_FT232_TX_Ptr;

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

typedef enum
{
	FaultFlag_Keep = 0x00,
	FaultFlag_Reset = 0x04
}ResetFaultFlag_State;

typedef enum
{
	Operation_OK = 0x00,
	Operation_Fault = 0x04
}Operation_State;

typedef struct
{
	uint8_t 				Address;
	uint16_t 				PWM;
	Direction_State			Direction;
	FreeDrive_State			FreeDrive;
	uint16_t				Current;
	uint16_t				PositionCurrent;
	uint16_t				PositionSet;
	Operation_State			Operation;
	ResetFaultFlag_State	ResetFaultFlag;

	int32_t					PID_Kp;
	int32_t					PID_Ki;
	int32_t					PID_Kd;
	int32_t					PID_Integral;
	int32_t					PID_PrevErr;
}MotorDriver_Values_TypeDef;


#ifdef TestSetup_1
#define MotorDriver_Count					1
#endif

#ifdef TestSetup_9
#define MotorDriver_Count					9
#endif

volatile MotorDriver_Values_TypeDef MotorDriver_List[MotorDriver_Count];
volatile MotorDriver_Values_TypeDef* MotorDriver_Polling;

typedef enum
{
	ERROR_OK = 0x00,
	ERROR_RS485_TIMEOUT = 0x01,
	ERROR_MOTOR_FAULT = 0x02,
	ERROR_RS485_CRC = 0x04,
}ErrorIndicator_Type;

ErrorIndicator_Type CurrentErrorType;

#define TIM_MotorPolling_WatchDog_Prescaler			191 //Inc 250 000 Hz
#define TIM_MotorPolling_WatchDog_Period			249 //Rel 249 = 1 ms = 1000 Hz

#define TIM_MotorCalibration_WatchDog_Prescaler		9599 //Inc 5 000 Hz
#define TIM_MotorCalibration_WatchDog_Period		39999 //Rel 24999 = 0,2 Hz = 5s; Rel 39999 = 0,125 Hz = 7s


typedef enum{
	MODE_IDLE = 0x00,
	MODE_INT_REGULATOR = 0x01,
	MODE_EXT_REGULATOR = 0x02,
}SystemOperationMode_Type;

SystemOperationMode_Type SystemOperationMode;

typedef enum{
	CALIBRATION_Disabled,
	CALIBRATION_Perform
}CalibrationProcedure_Type;

volatile CalibrationProcedure_Type CalibrationProcedure;

typedef enum{
	SendSatusToPC_DISABLED,
	SendSatusToPC_ENABLED,
}SendSatusToPC_Type;

volatile SendSatusToPC_Type SendSatusToPC;

typedef enum{
	SendingStatusState_SEND,
	SendingStatusState_SENDING,
}SendingStatusState_Type;

volatile SendingStatusState_Type SendingStatusState;

#endif /* SYSTEM_VARS_H_ */
