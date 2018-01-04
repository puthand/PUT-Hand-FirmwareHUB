/*
 * crc_ccitt.c
 *
 *  Created on: 04.01.2018
 *      Author: Tomasz
 */
#include "crc_ccitt.h"

uint8_t CRC8_CCITT_Calc(uint8_t inCrc, uint8_t inData)
{
   uint8_t i;
   uint8_t data;
   data = inCrc ^ inData;
   for ( i = 0; i < 8; i++ )
   {
        if (( data & 0x80 ) != 0 )
        {
            data <<= 1;
            data ^= 0x07;
        }
        else
        {
            data <<= 1;
        }
   }
   return data;
}

