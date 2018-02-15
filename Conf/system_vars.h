/*
 * system_vars.h
 *
 *  Created on: 06.12.2017
 *      Author: Tomasz
 */

#ifndef SYSTEM_VARS_H_
#define SYSTEM_VARS_H_

//#define TestSetup_1
//#define TestSetup_3
//#define TestSetup_9

#define Pazdzierz_One

#ifdef TestSetup_1
	#define MotorDriver_Count					1
#endif

#ifdef TestSetup_3
	#define MotorDriver_Count					3
#endif

#ifdef TestSetup_9
	#define MotorDriver_Count					9
#endif

#ifdef Pazdzierz_One
	#define MotorDriver_Count					7
#endif

#define Kp_Finger	2000
#define Ki_Finger	50
#define Kd_Finger	8000
#define AWlimit_Finger 100000


#define Kp_Lin		6000
#define Ki_Lin		150
#define Kd_Lin		8000
#define AWlimit_Lin 100000

#define Kp_Thumb	4000
#define Ki_Thumb	50
#define Kd_Thumb	8000
#define AWlimit_Thumb 100000

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
}USART_RS485_State_Type;
volatile USART_RS485_State_Type USART_RS485_State;

#define USART_FT232_MaxBufferSize			200
#define USART_FT232_DecodedBufferSize		USART_FT232_MaxBufferSize * 6 / 8
volatile uint8_t USART_FT232_TX_buffer[USART_FT232_MaxBufferSize];
volatile uint16_t USART_FT232_TX_Ptr;

typedef enum
{
	FreeDrive_EN  = 0x01,
	FreeDrive_DIS = 0x00
}FreeDrive_Type;

typedef enum
{
	Dir_Positive = 0x02,
	Dir_Negative = 0x00
}Direction_Type;

typedef enum
{
	FaultFlag_Keep = 0x00,
	FaultFlag_Reset = 0x04
}ResetFaultFlag_Type;

typedef enum
{
	Operation_OK = 0x00,
	Operation_Fault = 0x04
}MotorDriverOperation_Type;

typedef struct
{
	uint8_t 					Address;
	uint16_t 					PWM;
	Direction_Type				Direction;
	FreeDrive_Type				FreeDrive;
	uint16_t					Current;
	uint16_t					PositionCurrent;
	uint16_t					PositionSet;
	MotorDriverOperation_Type	MotorDriverOperation;
	ResetFaultFlag_Type			ResetFaultFlag;

	int32_t						PID_Kp;
	int32_t						PID_Ki;
	int32_t						PID_Kd;
	int32_t						PID_AWlimit;
	int32_t						PID_Integral;
	int32_t						PID_PrevErr;
}MotorDriver_Type;

volatile MotorDriver_Type MotorDriver_List[MotorDriver_Count];
volatile MotorDriver_Type* MotorDriver_Polling;

typedef enum
{
	ERROR_OK = 0x00,
	ERROR_RS485_TIMEOUT = 0x01,
	ERROR_MOTOR_FAULT = 0x02,
	ERROR_RS485_CRC = 0x04,
	ERROR_FT232_CRC = 0x08,
}CurrentError_Type;

CurrentError_Type CurrentError;

#define TIM_MotorPolling_WatchDog_Prescaler			191 //Inc 250 000 Hz
#define TIM_MotorPolling_WatchDog_Period			249 //Rel 249 = 1 ms = 1000 Hz

#define TIM_MotorCalibration_WatchDog_Prescaler		47999 //Inc 1 000 Hz
#define TIM_MotorCalibration_WatchDog_Period		19999 //Rel 19999 = 0,05 Hz = 20s


typedef enum{
	MODE_IDLE = 0x00,
	MODE_INT_REGULATOR = 0x01,
	MODE_EXT_REGULATOR = 0x02,
}SystemOperationMode_Type;

SystemOperationMode_Type SystemOperationMode;

typedef enum{
	CALIBRATION_Disabled,
	CALIBRATION_Perform,
	CALIBRATION_Perform_NextCycle
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

typedef enum
{
    FT232_CMD_EnableStatusUpdate = 0x01,
    FT232_CMD_DisableStatusUpdate = 0x02,
    FT232_CMD_CalibrationProcedureEnable = 0x03,
    FT232_CMD_ResetErrors = 0x04,
    FT232_CMD_IdleMode = 0x05,
    FT232_CMD_IntRegulatorMode = 0x06,
    FT232_CMD_ExtRegulatorMode = 0x07
}FT232_CMD_Type;

#endif /* SYSTEM_VARS_H_ */
