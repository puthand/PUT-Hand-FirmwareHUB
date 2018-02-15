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

#ifdef TestSetup_1
	const uint8_t drvList_Finger[] = {0};
	const uint8_t drvList_Lin[] = {};
	const uint8_t drvList_Thumb[] = {};

	const uint8_t drvList_Addresses[MotorDriver_Count] = {0x01};
#endif

#ifdef TestSetup_3
	const uint8_t drvList_Finger[] = {0, 2};
	const uint8_t drvList_Lin[] = {1};
	const uint8_t drvList_Thumb[] = {};

	const uint8_t drvList_Addresses[MotorDriver_Count] = {0x02, 0x03, 0x04};
#endif

#ifdef TestSetup_9
	const uint8_t drvList_Finger[] = {0, 1, 2, 3, 4, 5, 6, 7, 8};
	const uint8_t drvList_Lin[] = {};
	const uint8_t drvList_Thumb[] = {};

	const uint8_t drvList_Addresses[MotorDriver_Count] = {0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01};
#endif

#ifdef Pazdzierz_One
	const uint8_t drvList_Finger[] = {1, 2, 4, 6};
	const uint8_t drvList_Lin[] = {3, 5};
	const uint8_t drvList_Thumb[] = {0};

	const uint8_t drvList_Addresses[MotorDriver_Count] = {0x07, 0x04, 0x05, 0x03, 0x02, 0x06, 0x01};
#endif

int main(void)
{
	MotorDriver_Polling = MotorDriver_List;

	for(int i=0; i<MotorDriver_Count; i++)
	{
		MotorDriver_List[i].Address = drvList_Addresses[i];

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
		MotorDriver_List[i].PID_Kp = 0;
		MotorDriver_List[i].PID_Ki = 0;
		MotorDriver_List[i].PID_Kd = 0;
		MotorDriver_List[i].PID_AWlimit = 0;
	}

	for(int i=0; i<sizeof(drvList_Finger); i++)
	{
		MotorDriver_List[drvList_Finger[i]].PID_Kp = Kp_Finger;
		MotorDriver_List[drvList_Finger[i]].PID_Ki = Ki_Finger;
		MotorDriver_List[drvList_Finger[i]].PID_Kd = Kd_Finger;
		MotorDriver_List[drvList_Finger[i]].PID_AWlimit = AWlimit_Finger;
	}

	for(int i=0; i<sizeof(drvList_Lin); i++)
	{
		MotorDriver_List[drvList_Lin[i]].PID_Kp = Kp_Lin;
		MotorDriver_List[drvList_Lin[i]].PID_Ki = Ki_Lin;
		MotorDriver_List[drvList_Lin[i]].PID_Kd = Kd_Lin;
		MotorDriver_List[drvList_Lin[i]].PID_AWlimit = AWlimit_Lin;
	}

	for(int i=0; i<sizeof(drvList_Thumb); i++)
	{
		MotorDriver_List[drvList_Thumb[i]].PID_Kp = Kp_Thumb;
		MotorDriver_List[drvList_Thumb[i]].PID_Ki = Ki_Thumb;
		MotorDriver_List[drvList_Thumb[i]].PID_Kd = Kd_Thumb;
		MotorDriver_List[drvList_Thumb[i]].PID_AWlimit = AWlimit_Thumb;
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
