/**
  ******************************************************************************
  * @file    main.c
  * @author  Ac6
  * @version V1.0
  * @date    01-December-2013
  * @brief   Default main function.
  ******************************************************************************
*/

#include "main.h"

int main(void)
{
	MotorDriver_Polling = MotorDriver_List;
#ifdef TestSetup_9
	MotorDriver_List[0].Address = 0x01;
	MotorDriver_List[1].Address = 0x01;
	MotorDriver_List[2].Address = 0x01;
	MotorDriver_List[3].Address = 0x01;
	MotorDriver_List[4].Address = 0x01;
	MotorDriver_List[5].Address = 0x01;
	MotorDriver_List[6].Address = 0x01;
	MotorDriver_List[7].Address = 0x01;
	MotorDriver_List[8].Address = 0x01;
#endif

#ifdef TestSetup_1
	MotorDriver_List[0].Address = 0x01;
#endif

	for(int i=0; i<MotorDriver_Count; i++)
	{
		MotorDriver_List[i].PWM = 0;
		MotorDriver_List[i].Direction = Dir_Positive;
		MotorDriver_List[i].FreeDrive = FreeDrive_DIS;
		MotorDriver_List[i].Current = 0;
		MotorDriver_List[i].PositionCurrent = 0;
		MotorDriver_List[i].PositionSet = 20000;
		MotorDriver_List[i].MotorDriverOperation = Operation_OK;
		MotorDriver_List[i].ResetFaultFlag = FaultFlag_Keep;

		MotorDriver_List[i].PID_Integral = 0;
		MotorDriver_List[i].PID_PrevErr = 0;
		MotorDriver_List[i].PID_Kp = 1000;
		MotorDriver_List[i].PID_Ki = 50;
		MotorDriver_List[i].PID_Kd = 1000;
	}

	CurrentError = ERROR_OK;
	CalibrationProcedure = CALIBRATION_Disabled;
	SystemOperationMode = MODE_IDLE;
	SendSatusToPC = SendSatusToPC_DISABLED;
	SendingStatusState = SendingStatusState_SEND;

	for(int i=0; i<0xFFF; i++);//delay to initialize all slaves (MotorDrivers)

	Init();

	SysTick_Conf();
	GPIO_Conf();
	CRC_Conf();
	DMA_Conf();
	TIM_Conf();
	USART_Conf();

	//SystemOperationMode = MODE_INT_REGULATOR;
	//CalibrationProcedure = CALIBRATION_Perform;

	//SendSatusToPC = SendSatusToPC_ENABLED;

	while(1)
	{

	}
}
