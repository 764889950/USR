/*****************************************************************************
* | File        :   DRV8825.h
* | Author      :   Waveshare team
* | Function    :   Drive DRV8825
* | Info        :
*                The DRV8825 provides an integrated motor driver solution for
*                printers, scanners, and other automated equipment applications.
*                The device has two H-bridge drivers and a microstepping indexer,
*                and is intended to drive a bipolar stepper motor.
*----------------
* |	This version:   V1.0
* | Date        :   2018-10-13
* | Info        :   Basic version
*
******************************************************************************/
#ifndef __MARLINHOST_
#define __MARLINHOST_


void time_calculate(void);

void home_z(void);
void workPoint_z(void);//运动到工作原点

int motor_main(void);//初始化



#endif
