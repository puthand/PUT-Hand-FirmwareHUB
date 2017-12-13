/*
 * crc.c
 *
 *  Created on: 11.12.2017
 *      Author: Tomasz
 */
#include "crc.h"

void CRC_Conf()
{
	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_CRC);

	LL_CRC_SetInputDataReverseMode(CRC, LL_CRC_INDATA_REVERSE_NONE);
	LL_CRC_SetOutputDataReverseMode(CRC, LL_CRC_OUTDATA_REVERSE_NONE);
	LL_CRC_SetInitialData(CRC, LL_CRC_DEFAULT_CRC_INITVALUE);
}
