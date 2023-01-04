/*****************************************************************************
* | File        :   DEV_Config.c
* | Author      :   Waveshare team
* | Function    :   Hardware underlying interface
* | Info        :
*                Used to shield the underlying layers of each master
*                and enhance portability
*----------------
* |	This version:   V1.0
* | Date        :   2018-09-03
* | Info        :   Basic version
*
******************************************************************************/
#include "DEV_Config.h"
#include "DEV_Config_2.h"
#include "parameter.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>  //usleep
#include "wiringSerial.h"
#include <stdlib.h>     //exit()
#include <pthread.h>  	//包线程要包含
#include "strstr.h"
#include "parameter.h"
#include "DEV_Config_2.h"//其他设备初始化
#include "HMI-Screen.h"
#include "marlinHost.h"
#include "SLC_Read.h"

long Pedal_ON_OFF_flag=0;
 
 
int fd_PDC03;//光机接口套接字
int fd_HMI;//显示屏接口套接字
int fd_Marlin;//显示屏接口套接字

//套接字变量
int fd_serial;
int fd_ttyUSB0;
int fd_ttyUSB1;
int fd_ttyUSB2;
int fd_ttyUSB3;

char tty_flags[5]={0};//串口有效标识，等于0无效(串口打开失败)，1-显示屏，2-PDC03光机
	
char uart1_sign=0;           //显示屏串口接收完成标志
char RevBuf1[Data_SIZE]={0}; //数据接收缓冲区
char uart2_sign=0;           //光机串口接收完成标志
char RevBuf2[Data_SIZE]={0};      //数据接收缓冲区
char uart3_sign=0;           //光机串口接收完成标志
char RevBuf3[Data_SIZE]={0};      //数据接收缓冲区

char uart_init_flag=0;

unsigned char set_temp_extern=0;





void Uart_REC(void)
{
	char data=0;//接收的数据
	
	static int index=0;
	static int data_count_pdc03=0;//低分辨率光机数据下标
	//显示屏接口控制参数
	static int data_count1=0;    //数据下标
	static char end_flag1=0,end_flag2=0;
	/////////////

	uart_init_flag=1;
	
	while(1)
	{
		if(uart1_sign == 0)
		{
			// 串口接收数据
			if(serialDataAvail(fd_HMI) > 0)
			{
				// putchar(serialGetchar(fd_HMI));
				data=serialGetchar(fd_HMI);
				// printf("%x ", data);
				// printf("%c", data);
				
							
				// // 以附加方式打开可读/写的文件, 如果没有此文件则会进行创建，然后以附加方式打开可读/写的文件
				// fps = fopen("/home/pi/wiringpi/log_2.txt","a+");
				// sprintf(srts,"%x ",data);
				// // 将追加内容写入文件指针当前的位置
				// fputs(srts, fps);
				// // 最后不要忘了,关闭打开的文件~~~
				// fclose(fps);	

				if((data!='\n')|(end_flag1==1)|(end_flag2==1)) //判断是否接收到结束符
				{
					RevBuf1[data_count1]=data;// 否，就存到RevBuf【】数组中
					data_count1++;
					if(end_flag2==1)
					{
						if(data==0xff)
						{
							// printf("receive2:%x ",data);
							// RevBuf1[data_count1]='\0';
							// RevBuf1[data_count1-1]='\0';
							
							// fps = fopen("/home/pi/wiringpi/log_2.txt","a+");
							// sprintf(srts,"\r\n");
							// // 将追加内容写入文件指针当前的位置
							// fputs(srts, fps);
							// // 最后不要忘了,关闭打开的文件~~~
							// fclose(fps);

							end_flag1=0;
							end_flag2=0;
							data_count1=0; //下标清零
							uart1_sign = 1;
						}
					}
					
					if((end_flag1==1)&(end_flag2!=1))
					{
						if(data==0xff)
						{
							// printf("receive1:%x ",data);
							end_flag2=1;
						}
					}	
					else
					{
						end_flag1=0;
						end_flag2=0;
					}
					
					
					if(data_count1 >= Data_SIZE)
					{
						data_count1=Data_SIZE;
					}
					
				}
				else
				{
					// 新增
					RevBuf1[data_count1]='\n';
					// printf("receive:%x ",RevBuf1[data_count1]);
					data_count1++;
					end_flag1=1;
				}
			}
			//数据接收格式：xxxx...\n + 0xff 0xff
			// 以换行和0xff和0xff结尾表示数据接收完毕
		}
		
		
		if(uart2_sign == 0)
		{
			if(serialDataAvail(fd_PDC03) > 0)
			{
				data=serialGetchar(fd_PDC03);
				//printf("%d",data);

				RevBuf2[data_count_pdc03]=data;// 否，就存到RevBuf【】数组中
				
				data_count_pdc03++;
				
				if (my_strstr(RevBuf2,"efg",Data_SIZE)!=NULL)
				{
					// printf("RevBuf2:%s\r\n", RevBuf2);
					data_count_pdc03=0;
					uart2_sign = 1;
				}
				
				
				if(data_count_pdc03 >= Data_SIZE)
				{
					data_count_pdc03=Data_SIZE;
				}
			}		
		}			
		
		if(uart2_sign == 2)
		{
			if(serialDataAvail(fd_PDC03) > 0)
			{
				set_temp_extern=serialGetchar(fd_PDC03);
				// printf("%d",set_temp_extern);
			}
		}
				
				
				
			/*PDC03
			//串口接收数据
			if(serialDataAvail(fd_PDC03) > 0)
			{
				data=serialGetchar(fd_PDC03);
				// printf("%c",data);

				RevBuf2[data_count_pdc03]=data;// 否，就存到RevBuf【】数组中
				
				data_count_pdc03++;
				
				if (my_strstr(RevBuf2,"ShakeHands",Data_SIZE)!=NULL)
				{
					data_count_pdc03=0;
					uart2_sign = 1;
				}
				
				else
				{
					static char times_rec=0;
					if(data=='\n')//返回的电流值
					{
						times_rec++;
						if(times_rec>=4)
						{
							uart2_sign = 1;
							data_count_pdc03=0;
						}
					}
				}
				
				if(data_count_pdc03 >= Data_SIZE)
				{
					data_count_pdc03=Data_SIZE;
				}
			}		
		}
		else
		{
			if(serialDataAvail(fd_PDC03) > 0)
			{
				serialGetchar(fd_PDC03);
			}
			
		}
		*/

		if(uart3_sign==0)
		{
			while (serialDataAvail (fd_Marlin))
			{
				data=serialGetchar(fd_Marlin);
				// printf ("%c", data);
				if(data!='\n')
				{
					RevBuf3[index]=data;
					index++;
				}
				else
				{
					index=0;
					uart3_sign=1;
					break;
				}
			}
		}
	}
}


static void open_tty(void)
{
	if(tty_flags[0]==0)
	{	
		//打开硬件串口    ->1、关闭蓝牙sudo systemctl page_set_disable hciuart  2、串口对调 sudo nano /boot/config.txt 增加dtoverlay=pi3-miniuart-bt
		if((fd_serial = serialOpen("/dev/ttyAMA0", 115200)) < 0)
		{
			// printf("串口硬件初始化错误\r\n");
			serialClose(fd_serial);	
			tty_flags[0]=-1;
		}	
	}
	
	if(tty_flags[1]==0)
	{	
		//打开接口
		if((fd_ttyUSB0 = serialOpen("/dev/ttyUSB0", 115200)) < 0)
		{
			// printf("ttyUSB0初始化失败!\r\n");
			serialClose(fd_ttyUSB0);
			tty_flags[1]=-1;
		}
	}

	if(tty_flags[2]==0)
	{
		if((fd_ttyUSB1 = serialOpen("/dev/ttyUSB1", 115200)) < 0)
		{
			// printf("ttyUSB1初始化失败!\r\n");
			serialClose(fd_ttyUSB1);	
			tty_flags[2]=-1;
		}
	}

	if(tty_flags[3]==0)
	{		
		if((fd_ttyUSB2 = serialOpen("/dev/ttyUSB2", 115200)) < 0)
		{
			// printf("ttyUSB2初始化失败!\r\n");
			serialClose(fd_ttyUSB2);
			tty_flags[3]=-1;
		}
	}
	
	if(tty_flags[4]==0)
	{		
		if((fd_ttyUSB3 = serialOpen("/dev/ttyUSB3", 115200)) < 0)
		{
			// printf("ttyUSB3初始化失败!\r\n");
			serialClose(fd_ttyUSB3);
			tty_flags[4]=-1;
		}
	}		
}

static void close_tty(void)
{
	for(size_t i=0;i<5;i++)//判断串口接哪个屏
	{
		if(i==0)
		{
			if(tty_flags[i]!=1)
			{
				serialClose(fd_serial);
				// printf("关闭fd_serial \r\n");
			}
		}
		
		if(i==1)
		{
			if(tty_flags[i]!=1)
			{
				serialClose(fd_ttyUSB0);
				// printf("关闭fd_ttyUSB0 \r\n");
			}				
			
		}
		
		if(i==2)
		{
			if(tty_flags[i]!=1)
			{
				serialClose(fd_ttyUSB1);
				// printf("关闭fd_ttyUSB1 \r\n");
			}
		}
		
		if(i==3)
		{
			if(tty_flags[i]!=1)
			{
				serialClose(fd_ttyUSB2);
				// printf("关闭fd_ttyUSB2 \r\n");
			}
		}
		
		if(i==4)
		{
			if(tty_flags[i]!=1)
			{
				serialClose(fd_ttyUSB3);
				// printf("关闭fd_ttyUSB3 \r\n");
			}
		}
	}	
}

static void MarlinBoard_find(void)
{
	open_tty();
	for(size_t i=0;i<10;i++)//判断串口接哪个屏
	{

		static char times=0;//等待时间次数
		
		
		if(tty_flags[0]==0)
		{		
			fd_Marlin=fd_serial;
			//判断是否为Marlin主板指令
			serialPrintf (fd_Marlin,"M107\r\n");
			// printf("h1\r\n");
			rec1:
			usleep(500000);
			if(uart3_sign == 1)
			{
				//printf("h1\r\n");
				tty_flags[0]=1;
				memset(RevBuf3, '\0', sizeof(RevBuf3));
				uart3_sign = 0;
				//break;
				goto Marlin_find_end;
			}
			else
			{
				if(times<5)
				{
					// printf("h1_\r\n");
					times++;
					goto rec1;
				}
				times=0;
			}		
		}		

		if(tty_flags[1]==0)
		{		
			fd_Marlin=fd_ttyUSB0;
			//判断是否为Marlin主板指令
			serialPrintf (fd_Marlin,"M107\r\n");
			// printf("h2\r\n");
			rec2:
			usleep(500000);
			if(uart3_sign == 1)
			{
				//printf("h2\r\n");
				tty_flags[1]=1;
				memset(RevBuf3, '\0', sizeof(RevBuf3));
				uart3_sign = 0;
				//break;
				goto Marlin_find_end;
			}
			else
			{
				if(times<5)
				{
					// printf("h2_\r\n");
					times++;
					goto rec2;
				}
				times=0;
			}
		}

		if(tty_flags[2]==0)
		{
			fd_Marlin=fd_ttyUSB1;
			//判断是否为Marlin主板指令
			serialPrintf (fd_Marlin,"M107\r\n");
			// printf("h3\r\n");
			rec3:
			usleep(500000);
			if(uart3_sign == 1)
			{
				//printf("h3\r\n");
				tty_flags[2]=1;
				memset(RevBuf3, '\0', sizeof(RevBuf3));
				uart3_sign = 0;
				//break;
				goto Marlin_find_end;
			}
			else
			{
				if(times<5)
				{
					// printf("h3_\r\n");
					times++;
					goto rec3;
				}
				times=0;
			}
		}
		
		
		if(tty_flags[3]==0)
		{
			fd_Marlin=fd_ttyUSB2;
			//判断是否为Marlin主板指令
			serialPrintf (fd_Marlin,"M107\r\n");
			// printf("h4\r\n");
			rec4:
			usleep(500000);
			if(uart3_sign == 1)
			{
				//printf("h4\r\n");
				tty_flags[3]=1;
				memset(RevBuf3, '\0', sizeof(RevBuf3));
				uart3_sign = 0;
				//break;
				goto Marlin_find_end;
			}
			else
			{
				if(times<5)
				{
					// printf("h4_\r\n");
					times++;
					goto rec4;
				}
				times=0;
			}
		}

		if(tty_flags[4]==0)
		{
			fd_Marlin=fd_ttyUSB3;
			//判断是否为Marlin主板指令
			serialPrintf (fd_Marlin,"M107\r\n");
			// printf("h5\r\n");
			rec5:
			usleep(500000);
			if(uart3_sign == 1)
			{
				//printf("h5\r\n");
				tty_flags[4]=1;
				memset(RevBuf3, '\0', sizeof(RevBuf3));
				uart3_sign = 0;
				//break;
				goto Marlin_find_end;
			}
			else
			{
				if(times<5)
				{
					// printf("h5_\r\n");
					times++;
					goto rec5;
				}
				times=0;
			}
		}		

		if(i>9)
		{
			reboot();
		}
	}
	
	Marlin_find_end:;
	
	close_tty();
}

void juge_HMI(void)
{
	//5AA5 0B 82 00d4 5AA5 0004 0026 0258
	
	serialPutchar (fd_HMI, 0x5A);
	serialPutchar (fd_HMI, 0xA5);
	serialPutchar (fd_HMI, 0x0B);
	serialPutchar (fd_HMI, 0x82);
	serialPutchar (fd_HMI, 0x00);
	serialPutchar (fd_HMI, 0xD4);
	serialPutchar (fd_HMI, 0x5A);
	serialPutchar (fd_HMI, 0xA5);
	serialPutchar (fd_HMI, 0x00);
	serialPutchar (fd_HMI, 0x04);
	serialPutchar (fd_HMI, 0x00);
	serialPutchar (fd_HMI, 0x26);
	serialPutchar (fd_HMI, 0x02);
	serialPutchar (fd_HMI, 0x58);
	// printf("判断是否到首页\r\n");
}


//寻找显示屏
void HMI_find(void)
{
	open_tty(); 
	for(size_t i=0;i<10;i++)//判断串口接哪个屏
	{

		static char times=0;//等待时间次数
		
		
		if(tty_flags[0]==0)
		{		
			fd_HMI=fd_serial;
			//跳转到首页
			if(reboot_flag==0)
			{
				 page_home_switch();
			}
			usleep(500000);
			//判断是否为屏幕指令
			juge_HMI();
			// printf("juge_h1\r\n");
			rec1:
			usleep(500000);
			
			if(uart1_sign == 1)
			{
				//printf("h1\r\n");
				tty_flags[0]=1;
				memset(RevBuf1, '\0', sizeof(RevBuf1));
				uart1_sign = 0;
				//break;
				// if()
				goto HMI_find_end;
			}
			else
			{
				if(times<5)
				{
					//printf("h1_\r\n");
					times++;
					goto rec1;
				}
				times=0;
			}		
		}		

		if(tty_flags[1]==0)
		{		
			fd_HMI=fd_ttyUSB0;
			//跳转到首页
			if(reboot_flag==0)
			{
				 page_home_switch();
			}
			usleep(500000);
			//判断是否为屏幕指令
			juge_HMI();
			// printf("juge_h2\r\n");
			rec2:
			usleep(500000);
				
			if(uart1_sign == 1)
			{
				//printf("h2\r\n");
				tty_flags[1]=1;
				memset(RevBuf1, '\0', sizeof(RevBuf1));
				uart1_sign = 0;
				//break;
				goto HMI_find_end;
			}
			else
			{
				if(times<5)
				{
					//printf("h2_\r\n");
					times++;
					goto rec2;
				}
				times=0;
			}		
		}
		
		if(tty_flags[2]==0)
		{
			fd_HMI=fd_ttyUSB1;
			//跳转到首页
			if(reboot_flag==0)
			{
				 page_home_switch();
			}
			usleep(500000);
			//判断是否为屏幕指令
			juge_HMI();
			// printf("juge_h3\r\n");
			rec3:
			usleep(500000);
			
			if(uart1_sign == 1)
			{
				//printf("h3\r\n");
				tty_flags[2]=1;
				memset(RevBuf1, '\0', sizeof(RevBuf1));
				uart1_sign = 0;
				//break;
				goto HMI_find_end;
			}
			else
			{
				if(times<5)
				{
					//printf("h3_\r\n");
					times++;
					goto rec3;
				}
				times=0;
			}
		}
		
		
		if(tty_flags[3]==0)
		{
			fd_HMI=fd_ttyUSB2;
			//跳转到首页
			if(reboot_flag==0)
			{
				 page_home_switch();
			}
			usleep(500000);
			//判断是否为屏幕指令
			juge_HMI();
			// printf("juge_h4\r\n");
			rec4:
			usleep(500000);
			
			if(uart1_sign == 1)
			{
				//printf("h4\r\n");
				tty_flags[3]=1;
				memset(RevBuf1, '\0', sizeof(RevBuf1));
				uart1_sign = 0;
				//break;
				goto HMI_find_end;
			}
			else
			{
				if(times<5)
				{
					//printf("h4_\r\n");
					times++;
					goto rec4;
				}
				times=0;
			}
		}

		if(tty_flags[4]==0)
		{
			fd_HMI=fd_ttyUSB3;
			//跳转到首页
			if(reboot_flag==0)
			{
				 page_home_switch();
			}
			usleep(500000);
			//判断是否为屏幕指令
			juge_HMI();
			// printf("juge_h5\r\n");
			rec5:
			usleep(500000);
			
			if(uart1_sign == 1)
			{
				//printf("h5\r\n");
				tty_flags[4]=1;
				memset(RevBuf1, '\0', sizeof(RevBuf1));
				uart1_sign = 0;
				//break;
				goto HMI_find_end;
			}
			else
			{
				if(times<5)
				{
					//printf("h5_\r\n");
					times++;
					goto rec5;
				}
				times=0;
			}
		}		

		// printf("i:%d\r\n",i);
		
		if(i>9)
		{
			reboot();
		}
	}
	
	HMI_find_end:;
	
	close_tty();

}



char PDC_Uart_find(void)
{
	char status=0;//返回值
	
	open_tty();
	
	for(size_t i=0;i<10;i++)//判断串口接哪个屏
	{

		static char times=0;//等待时间次数
		
		if(i==2)
		{
			printf("PDC失败\r\n");
			reboot_flag=1;//光机报错标识
			status=-1;
			
			break;
		}		
		
		
		if(tty_flags[0]==0)
		{
			fd_PDC03=fd_serial;
			//usleep(500000);
			//判断是否为光机指令
			serialPutchar (fd_serial, 0xE0);
			serialPutchar (fd_serial, 0x05);
			serialPutchar (fd_serial, 0x44);
			serialPutchar (fd_serial, 0x00);
			serialPutchar (fd_serial, 0xD7);
			// printf("PDC:%d\r\n",i);
			rec1:
			usleep(100000);
			
			if(uart2_sign == 1)
			{
				// printf("h11\r\n");
				tty_flags[0]=1;
				memset(RevBuf2, '\0', sizeof(RevBuf2));
				uart2_sign = 2;
				//break;
				goto PDC_Uart_find_end;
			}
			else
			{
				if(times<5)
				{
					// printf("h11_\r\n");
					times++;
					goto rec1;
				}
				times=0;
			}
		}

		if(tty_flags[1]==0)
		{		
			fd_PDC03=fd_ttyUSB0;
			//usleep(500000);
			//判断是否为光机指令
			serialPutchar (fd_ttyUSB0, 0xE0);
			serialPutchar (fd_ttyUSB0, 0x05);
			serialPutchar (fd_ttyUSB0, 0x44);
			serialPutchar (fd_ttyUSB0, 0x00);
			serialPutchar (fd_ttyUSB0, 0xD7);
			
			rec2:
			usleep(100000);
			
			if(uart2_sign == 1)
			{
				// printf("h22\r\n");
				tty_flags[1]=1;
				memset(RevBuf2, '\0', sizeof(RevBuf2));
				uart2_sign = 2;
				//break;
				goto PDC_Uart_find_end;
			}
			else
			{
				if(times<5)
				{
					// printf("h22_\r\n");
					times++;
					goto rec2;
				}
				times=0;
			}		
		}
		
		if(tty_flags[2]==0)
		{
			fd_PDC03=fd_ttyUSB1;
			//usleep(500000);
			//判断是否为光机指令
			serialPutchar (fd_ttyUSB1, 0xE0);
			serialPutchar (fd_ttyUSB1, 0x05);
			serialPutchar (fd_ttyUSB1, 0x44);
			serialPutchar (fd_ttyUSB1, 0x00);
			serialPutchar (fd_ttyUSB1, 0xD7);
			
			rec3:
			usleep(100000);
			
			if(uart2_sign == 1)
			{
				// printf("h33\r\n");
				tty_flags[2]=1;
				memset(RevBuf2, '\0', sizeof(RevBuf2));
				uart2_sign = 2;				
				//break;
				goto PDC_Uart_find_end;
			}
			else
			{
				if(times<5)
				{
					// printf("h33_\r\n");
					times++;
					goto rec3;
				}
				times=0;
			}
		}
		
		if(tty_flags[3]==0)
		{
			fd_PDC03=fd_ttyUSB2;
			//usleep(500000);
			//判断是否为光机指令
			serialPutchar (fd_ttyUSB2, 0xE0);
			serialPutchar (fd_ttyUSB2, 0x05);
			serialPutchar (fd_ttyUSB2, 0x44);
			serialPutchar (fd_ttyUSB2, 0x00);
			serialPutchar (fd_ttyUSB2, 0xD7);
			
			rec4:
			usleep(100000);
			
			if(uart2_sign == 1)
			{
				// printf("h44\r\n");
				tty_flags[3]=1;
				memset(RevBuf2, '\0', sizeof(RevBuf2));
				uart2_sign = 2;
				//break;
				goto PDC_Uart_find_end;
			}
			else
			{
				if(times<5)
				{
					// printf("h44_\r\n");
					times++;
					goto rec4;
				}
				times=0;
			}
		}

		if(tty_flags[4]==0)
		{
			fd_PDC03=fd_ttyUSB3;
			//usleep(500000);
			//判断是否为光机指令
			serialPutchar (fd_ttyUSB3, 0xE0);
			serialPutchar (fd_ttyUSB3, 0x05);
			serialPutchar (fd_ttyUSB3, 0x44);
			serialPutchar (fd_ttyUSB3, 0x00);
			serialPutchar (fd_ttyUSB3, 0xD7);
	
			rec5:
			usleep(100000);
			
			if(uart2_sign == 1)
			{
				// printf("h55\r\n");
				tty_flags[4]=1;
				memset(RevBuf2, '\0', sizeof(RevBuf2));
				uart2_sign = 2;
				//break;
				goto PDC_Uart_find_end;
			}
			else
			{
				if(times<5)
				{
					// printf("h55_\r\n");
					times++;
					goto rec5;
				}
				times=0;
			}
		}
	}
	
	
	return status;
	PDC_Uart_find_end:;
	
	close_tty();
	
	return status;
}

char PDC_Uart_find2(void)
{
	char status=0;//返回值
	
	open_tty();
	
	for(size_t i=0;i<10;i++)//判断光机哪个串口接
	{

		static char times=0;//等待时间次数
		
		if(i==5)
		{
			//printf("PDC失败\r\n");
			//reboot_flag=1;//光机报错标识
			//break;
			status=-1;//将返回-1，寻找光机失败，将重新进行光机的开机操作，开机时间将延长
		}		
		
		
		if(tty_flags[0]==0)
		{
			fd_PDC03=fd_serial;
			//usleep(500000);
			//判断是否为光机指令
			serialPutchar (fd_serial, 0x65);

			// printf("PDC:%d\r\n",i);
			rec1:
			usleep(100000);
			
			if(uart2_sign == 1)
			{
				//printf("h11\r\n");
				tty_flags[0]=1;
				memset(RevBuf2, '\0', sizeof(RevBuf2));
				uart2_sign = 2;
				//break;
				goto PDC_Uart_find2_end;
			}
			else
			{
				if(times<5)
				{
					//printf("h11_\r\n");
					times++;
					goto rec1;
				}
				times=0;
			}
		}

		if(tty_flags[1]==0)
		{		
			fd_PDC03=fd_ttyUSB0;
			//usleep(500000);
			//判断是否为光机指令
			serialPutchar (fd_ttyUSB0, 0x65);
	
			rec2:
			usleep(100000);
			
			if(uart2_sign == 1)
			{
				//printf("h22\r\n");
				tty_flags[1]=1;
				memset(RevBuf2, '\0', sizeof(RevBuf2));
				uart2_sign = 2;
				//break;
				goto PDC_Uart_find2_end;
			}
			else
			{
				if(times<5)
				{
					//printf("h22_\r\n");
					times++;
					goto rec2;
				}
				times=0;
			}
		}
		
		if(tty_flags[2]==0)
		{
			fd_PDC03=fd_ttyUSB1;
			//usleep(500000);
			//判断是否为光机指令
			serialPutchar (fd_ttyUSB1, 0x65);
			
			rec3:
			usleep(100000);
			
			if(uart2_sign == 1)
			{
				//printf("h33\r\n");
				tty_flags[2]=1;
				memset(RevBuf2, '\0', sizeof(RevBuf2));
				uart2_sign = 2;				
				//break;
				goto PDC_Uart_find2_end;
			}
			else
			{
				if(times<5)
				{
					//printf("h33_\r\n");
					times++;
					goto rec3;
				}
				times=0;
			}
		}
		
		if(tty_flags[3]==0)
		{
			fd_PDC03=fd_ttyUSB2;
			//usleep(500000);
			//判断是否为光机指令
			serialPutchar (fd_ttyUSB2, 0x65);
			
			rec4:
			usleep(100000);
			
			if(uart2_sign == 1)
			{
				//printf("h44\r\n");
				tty_flags[3]=1;
				memset(RevBuf2, '\0', sizeof(RevBuf2));
				uart2_sign = 2;
				//break;
				goto PDC_Uart_find2_end;
			}
			else
			{
				if(times<5)
				{
					//printf("h44_\r\n");
					times++;
					goto rec4;
				}
				times=0;
			}
		}

		if(tty_flags[4]==0)
		{
			fd_PDC03=fd_ttyUSB3;
			//usleep(500000);
			//判断是否为光机指令
			serialPutchar (fd_ttyUSB3, 0x65);
	
			rec5:
			usleep(100000);
			
			if(uart2_sign == 1)
			{
				//printf("h55\r\n");
				tty_flags[4]=1;
				memset(RevBuf2, '\0', sizeof(RevBuf2));
				uart2_sign = 2;
				//break;
				goto PDC_Uart_find2_end;
			}
			else
			{
				if(times<5)
				{
					//printf("h55_\r\n");
					times++;
					goto rec5;
				}
				times=0;
			}
		}
	}


	PDC_Uart_find2_end:;
	
	close_tty();
	
	return status;
}


static void *thread_1(void *args)//其他
{
	Uart_REC();
	return NULL;
}


void GPIO_Init(void)
{
	//待加
	
}


int time_add=4;//初始按键等待时间为4秒
int reboot_=0;//判断是否加1标识
//-----------------------------------------------
//写入参数到print.cfg文件
//-----------------------------------------------
void rebootcfg_write(void)
{
	FILE *print_fp;
	printf("writeprintcfg\n");
	static char com[100];

	sprintf(com,"%s/reboot.cfg", print_cfg_path);
	print_fp = fopen(com,"w");

	if(NULL == print_fp)
	{
		printf("can't open reboot.cfg");
	}
	else
	{
		fprintf(print_fp, "time_add=%d\n", time_add);
		fprintf(print_fp, "reboot_=%d\n", reboot_);
		printf("写入成功\n");
	}
	fclose(print_fp);
}

//-----------------------------------------------
//读取serial.cfg文件并进行配置
//-----------------------------------------------
void rebootcfg_read(void)
{
	FILE *print_fp;
	// printf("read printcfg\n");
	static char com[100];
	sprintf(com,"%s/reboot.cfg", print_cfg_path);
	print_fp = fopen(com,"r");

	if(NULL == print_fp)
	{
		rebootcfg_write();
		printf("can't open reboot.cfg");
	}
	else
	{
		fscanf(print_fp, "time_add=%d\n", &time_add);
		fscanf(print_fp, "reboot_=%d\n", &reboot_);//reboot_ 重启后的标识，为1为重启操作进入当前操作
	}
	fclose(print_fp);
}


int hardware_init_main(void)
{	
	pixel_Framebuffer pixel_bits_test;
	char flag_pdc=0;
	int ret=0;
	FILE *fp;
	pthread_t id1;
	ret=pthread_create(&id1,NULL,thread_1,NULL);//开启线程	
	if(ret)
	{
		printf("create pthread error!\n");
		return -1; 
	}

	GPIO_Init();
   
	while(uart_init_flag==0);
	
	if(led_choose==0)
	{
		//PDC_Uart_find();//寻找低分辨光机
	}
	
	
	read_Framebuffer(pixel_bits_test);
	if(((pixel_bits_test.x==720)&&(pixel_bits_test.y==480))|(pixel_bits_test.x<=1000))//小于1000一般不正常
	{
		rebootcfg_read();

		printf("time_add:%d\n",time_add);
		printf("reboot_:%d\n",reboot_);
		
		if(time_add>8)//冗余
			time_add=1;//冗余
		
		if(reboot_)
		{
			//printf("time_add+1\n");
			time_add++;
			if(time_add>8)
				time_add=1;
			rebootcfg_write();				
		}
		
		//开启光机电源
		//printf("打开光机操作，打开过程时间为%d\n",time_add);
		fp = popen("sudo /home/pi/wiringpi/Relay.sh CH1 ON >/dev/null 2>&1", "r");
		pclose(fp);
		sleep(time_add);
		fp = popen("sudo /home/pi/wiringpi/Relay.sh CH1 OFF >/dev/null 2>&1", "r");
		pclose(fp);
		//或者在上处引入G代码代替
	
		reboot_=1;
		rebootcfg_write();	
		sleep(20);
		// printf("正在重启\n"); 
		reboot();	
	}
	reboot_=0;
	rebootcfg_write();
	
	// flag_pdc=PDC_Uart_find2();//寻找低分辨光机
	if(reboot_flag==1)
	{
		// 切换到光机自检失败界面
		page_projector_init_fail_switch();
		page_projector_init_fail_switch();
		return -1;
	}
	
	if(flag_pdc==-1)
	{
		reboot();
	}


	printf("开始寻找电机控制板\r\n");
	memset(RevBuf3, '\0', sizeof(RevBuf3));
	MarlinBoard_find();//寻找电机控制板
	printf("寻找电机控制板结束\r\n");
	
	
	printf("开始寻找屏幕\r\n");
	memset(RevBuf1, '\0', sizeof(RevBuf1));
	HMI_find();//寻找屏幕
	printf("寻找屏幕结束\r\n");


	for(size_t i=0;i<5;i++)//判断串口接哪个
	{
		printf("%d\r\n",tty_flags[i]);
	}

	return 0;
}




