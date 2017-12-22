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
#ifdef TestSetup
	MotorDriver_List[0].Address = 0x01;
	MotorDriver_List[1].Address = 0x01;
	MotorDriver_List[2].Address = 0x01;
#endif

	for(int i=0; i<MotorDriver_Count; i++)
	{
		MotorDriver_List[i].PWM = 0;
		MotorDriver_List[i].Direction = Dir_Positive;
		MotorDriver_List[i].FreeDrive = FreeDrive_EN;
		MotorDriver_List[i].Current = 0;
		MotorDriver_List[i].PositionCurrent = 0;
		MotorDriver_List[i].PositionSet = 32768;
		MotorDriver_List[i].Operation = Operation_OK;
		MotorDriver_List[i].ResetFaultFlag = FaultFlag_Keep;
	}

	CurrentErrorType = ERROR_OK;
	CalibrationProcedure = CALIBRATION_None;
	SystemOperationMode = MODE_IDLE;

	for(int i=0; i<0xFFF; i++);//delay to initialize all slaves (MotorDrivers)

	Init();

	SysTick_Conf();
	GPIO_Conf();
	CRC_Conf();
	DMA_Conf();
	TIM_Conf();
	USART_Conf();

	//CalibrationProcedure = CALIBRATION_Perform;

	while(1)
	{

	}
}
