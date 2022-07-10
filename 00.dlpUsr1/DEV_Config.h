/*****************************************************************************
* | File        :   DEV_Config.h
* | Author      :   Waveshare team
* | Function    :   Hardware underlying interface
* | Info        :
*                Used to shield the underlying layers of each master 
*                and enhance portability
*----------------
* |	This version:   V1.0
* | Date        :   2018-10-15
* | Info        :   Basic version
*
******************************************************************************/
#ifndef __DEV_CONFIG_H_
#define __DEV_CONFIG_H_

#include <wiringPi.h>
#include <stdint.h>

/**
 * data
**/
#define UBYTE   uint8_t
#define UWORD   uint16_t
#define UDOUBLE uint32_t

/**
 * GPIO
**/
// #define M1_ENABLE_PIN 12


#define DEV_Digital_Write(_pin, _value) digitalWrite(_pin, _value)

/*------------------------------------------------------------------------------------------------------*/

UBYTE DEV_ModuleInit(void);
void DEV_ModuleExit(void);

void DEV_Delay_ms(UDOUBLE xms);
void DEV_Delay_us(UDOUBLE xus);

#endif