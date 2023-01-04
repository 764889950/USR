#ifndef __DEV_CONFIG_2_H_
#define __DEV_CONFIG_2_H_

#include <wiringPi.h>
#include <stdint.h>


extern int fd_PDC03;//光机接口套接字
extern int fd_HMI;//显示屏接口套接字
extern int fd_Marlin;//显示屏接口套接字

//套接字变量
extern int fd_serial;
extern int fd_ttyUSB0;
extern int fd_ttyUSB1;
extern int fd_ttyUSB2;
extern int fd_ttyUSB3;

extern char tty_flags[5];//串口有效标识，等于0无效(串口打开失败)，1-显示屏，2-PDC03光机

#define Data_SIZE 500 //数据长度
extern char uart1_sign;           //串口接收完成标志
extern char RevBuf1[Data_SIZE];      //数据接收缓冲区
extern char uart2_sign;           //串口接收完成标志
extern char RevBuf2[Data_SIZE];      //数据接收缓冲区
extern char uart3_sign;           //光机串口接收完成标志
extern char RevBuf3[Data_SIZE];      //数据接收缓冲区

extern unsigned char set_temp_extern;



#define DEV_Digital_Read(_pin) digitalRead(_pin)

void set_other_para(void);
int hardware_init_main(void);
void GPIO_Init(void);
void HMI_find(void);
void LED_P_Set(long set_temp,char sor);//PDC.cpp


#endif

