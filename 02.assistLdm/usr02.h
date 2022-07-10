#include <stdio.h>      //printf()
#include <stdlib.h>     //exit()
#include <unistd.h> 	//usleep
#include <string.h>     //strlen
#include <ctype.h>      //tolower
#include <stdarg.h>		//va_list  ...


#include <vector>
#include <string>//c++ string类
#include <iostream>

//公共数据头文件
#include "../pthread/pthread_main.h"
#include "../pthread/data_global.h"
#include "../cJSON-master/cJSON.h"
#include "../libcrc-master/include/checksum.h"
#include "../octoModule/octoMovement.h"
#include "../octoModule/wiringSerial.h"


#define FIFO_Ultrasound "/home/ReadFifoUltrasound"
//辅助文件路径，用于控制CGI返回,通常位于用户目录
#define FIFO_UltrasoundBACK "/home/ReadFifoUltrasoundBack"

#define FIFO_Reset "/home/ReadFifoReset"
//辅助文件路径，用于控制CGI返回,通常位于用户目录
#define FIFO_ResetBACK "/home/ReadFifoResetBack"

#define FIFO_Pressure "/home/ReadFifoPressure"
//辅助文件路径，用于控制CGI返回,通常位于用户目录
#define FIFO_PressureBACK "/home/ReadFifoPressureBack"



using namespace std;



double Pressure = 0;//压力值



static void *pthread_fifo_Ultrasound (void *arg)
{
	FILE *fp;
	char strs[fileDepth];
	int flag=0;	
	char buf[256];
	if(mkfifo(FIFO_Ultrasound, 0777))//创建管道文件
	{
		// perror("Mkfifo error");
	} 	
	sprintf(strs,"sudo chmod 777 %s",FIFO_Ultrasound);
	fp = popen(strs, "r");
	pclose(fp);

	int fdUlt;
	fdUlt=open(FIFO_Ultrasound,O_RDONLY|O_NONBLOCK);//非阻塞 	
	int count;

	int a=0;
	
	while(1)
	{
		count=read(fdUlt,buf,127);
		//要用底层io。read()会返回实际读到的字节数
		if(count>0)
		{
			buf[count]=0;//结束符，也可以='\0';
			// printf("recv:%s\n",buf);//打印从FIFO收到的数据
			printf("fdUlt\n");
			
			fp=fopen("/home/outfileUltrasound", "w+");
			if(flag==0)
			{
				flag=1;
				fputs("0",fp);
				printf("fputs:0000\n");
				a++;
			}
			else
			{
				flag=0;
				fputs("1",fp);
				printf("fputs:0001\n");
				a++;
			}
			fclose(fp);

			sprintf(strs,"sudo chmod 777 %s","/home/outfileUltrasound");
			fp = popen(strs, "r");
			pclose(fp);

			usleep(10000);
			
			rename("/home/outfileUltrasound",FIFO_UltrasoundBACK);
			
			printf("a:%d\n",a);
		}
	}
	close(fdUlt);
	return NULL;
}



static void *pthread_fifo_Reset (void *arg)
{
	FILE *fp;
	char strs[fileDepth];
	int flag=0;	
	char buf[256];	
	if(mkfifo(FIFO_Reset, 0777))//创建管道文件
	{
		// perror("Mkfifo error");
	} 
	sprintf(strs,"sudo chmod 777 %s",FIFO_Reset);
	fp = popen(strs, "r");
	pclose(fp);
	
	int fdRes;
	
	fdRes=open(FIFO_Reset,O_RDONLY|O_NONBLOCK);//非阻塞 
	int count;
	
	while(1)
	{
		count=read(fdRes,buf,127);
		//要用底层io。read()会返回实际读到的字节数
		if(count>0)
		{
			buf[count]=0;//结束符，也可以='\0';
			printf("recv:%s\n",buf);//打印从FIFO收到的数据
			printf("fdRes\n");
			
			fp=fopen("/home/outfileReset", "w+");
			if(flag==0)
			{
				flag=1;
				fputs("0",fp);
				printf("fputs:0000\n");
			}
			else
			{
				flag=0;
				fputs("1",fp);
				printf("fputs:0001\n");
			}
			fclose(fp);

			sprintf(strs,"sudo chmod 777 %s","/home/outfileReset");
			fp = popen(strs, "r");
			pclose(fp);
			
			// usleep(10000);

			rename("/home/outfileReset",FIFO_ResetBACK);	
			
			
			
		}		
	}
	close(fdRes);
	return NULL;
}



static void *pthread_fifo_Pressure (void *arg)
{
	FILE *fp;
	char strs[fileDepth];
	int flag=0;	
	char buf[256];

	if(mkfifo(FIFO_Pressure, 0777))//创建管道文件
	{
		// perror("Mkfifo error");
	}
	

	sprintf(strs,"sudo chmod 777 %s",FIFO_Pressure);
	fp = popen(strs, "r");
	pclose(fp);

	int fdPre;
	

	fdPre=open(FIFO_Pressure,O_RDONLY|O_NONBLOCK);//非阻塞 

	int count;

	
	while(1)
	{

		count=read(fdPre,buf,127);
		//要用底层io。read()会返回实际读到的字节数
		if(count>0)
		{
			buf[count]=0;//结束符，也可以='\0';
			// printf("fdPre\n");
			
			fp=fopen("/home/outfilePressure", "w+");
			if(flag==0)
			{
				flag=1;
				fputs("0",fp);
				
			}
			else
			{
				flag=0;
				fputs("1",fp);				
			}

			fclose(fp);

			sprintf(strs,"sudo chmod 777 %s","/home/outfilePressure");
			fp = popen(strs, "r");
			pclose(fp);

			rename("/home/outfilePressure",FIFO_PressureBACK);
		}
	}

	close(fdPre);
	
		// readUltrasound:			usleep(100000);
		// }
		// get_netInfConnect();
		// scram();//急停
	
	return NULL; 
}

int fd;
char tty_flags[5]={0};//串口有效标识，等于0无效(串口打开失败)
//套接字变量
int fd_serial;
int fd_ttyUSB0;
int fd_ttyUSB1;
int fd_ttyUSB2;
int fd_ttyUSB3;

static void open_tty(void)
{
	if(tty_flags[0]==0)
	{	
		//打开硬件串口    ->1、关闭蓝牙sudo systemctl page_set_disable hciuart  2、串口对调 sudo nano /boot/config.txt 增加dtoverlay=pi3-miniuart-bt
		if((fd_serial = serialOpen("/dev/ttyAMA0", 115200)) < 0)
		{
			printf("串口硬件初始化错误\r\n");
			serialClose(fd_serial);	
			tty_flags[0]=-1;
		}	
	}
	
	if(tty_flags[1]==0)
	{	
		//打开接口
		if((fd_ttyUSB0 = serialOpen("/dev/ttyUSB0", 115200)) < 0)
		{
			printf("ttyUSB0初始化失败!\r\n");
			serialClose(fd_ttyUSB0);
			tty_flags[1]=-1;
		}
	}

	if(tty_flags[2]==0)
	{
		if((fd_ttyUSB1 = serialOpen("/dev/ttyUSB1", 115200)) < 0)
		{
			printf("ttyUSB1初始化失败!\r\n");
			serialClose(fd_ttyUSB1);	
			tty_flags[2]=-1;
		}
	}

	if(tty_flags[3]==0)
	{		
		if((fd_ttyUSB2 = serialOpen("/dev/ttyUSB2", 115200)) < 0)
		{
			printf("ttyUSB2初始化失败!\r\n");
			serialClose(fd_ttyUSB2);
			tty_flags[3]=-1;
		}
	}
	
	if(tty_flags[4]==0)
	{		
		if((fd_ttyUSB3 = serialOpen("/dev/ttyUSB3", 115200)) < 0)
		{
			printf("ttyUSB3初始化失败!\r\n");
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
				printf("关闭fd_serial \r\n");
			}
		}
		
		if(i==1)
		{
			if(tty_flags[i]!=1)
			{
				serialClose(fd_ttyUSB0);
				printf("关闭fd_ttyUSB0 \r\n");
			}				
			
		}
		
		if(i==2)
		{
			if(tty_flags[i]!=1)
			{
				serialClose(fd_ttyUSB1);
				printf("关闭fd_ttyUSB1 \r\n");
			}
		}
		
		if(i==3)
		{
			if(tty_flags[i]!=1)
			{
				serialClose(fd_ttyUSB2);
				printf("关闭fd_ttyUSB2 \r\n");
			}
		}
		
		if(i==4)
		{
			if(tty_flags[i]!=1)
			{
				serialClose(fd_ttyUSB3);
				printf("关闭fd_ttyUSB3 \r\n");
			}
		}
	}	
}


extern int assistLdmUsr02(void)
{	
	/*  高能打印机辅助配套控制   */
	
	FILE *fp;	
	char buf[100];
	char com[100];
	uint16_t  crc_16_modbus_val_rec;
	uint16_t  crc_16_modbus_val_send;
	uint8_t data_rec[100];
	
	int ttyUSB_num;
	
	init:
	ttyUSB_num=0;
	
	fp = popen("sudo find /dev/ -name \"ttyUSB[0-9]*\" |wc -l", "r");//查看sd*[1-inf]文件的数量
	fgets(buf, sizeof(buf), fp);
	ttyUSB_num=atoi(buf);
	pclose(fp);
	
	printf("ttyUSB_num:%d\r\n",ttyUSB_num);
	
	
	
	
	pthread_t id_fifo_Ultrasound;
	pthread_t id_fifo_Reset;
	pthread_t id_fifo_Pressure;
	
	pthread_create (&id_fifo_Ultrasound, 0, pthread_fifo_Ultrasound, NULL);
	pthread_create (&id_fifo_Reset, 0, pthread_fifo_Reset, NULL);
	pthread_create (&id_fifo_Pressure, 0, pthread_fifo_Pressure, NULL);
	
	// open_tty();
	// close_tty();
	// return 0;
		
	while(1);
	
	
	char recIndex=0;//接收数据下标





	if((fd = serialOpen("/dev/ttyUSB0", 19200)) < 0)
	{
		printf("ttyUSB0初始化失败!\r\n");
		serialClose(fd);	
		return 0;
	}
	else
	{
		printf("ttyUSB0初始化成功!\r\n");
	}
	
	

	while(1)
	{
		
		crc_16_modbus_val_send=0xF8C6;
		//发送命令读取压力数据
		readUltrasound:
		serialPutchar (fd_ttyUSB0, 0x01);
		serialPutchar (fd_ttyUSB0, 0x03);
		serialPutchar (fd_ttyUSB0, 0x0f);
		serialPutchar (fd_ttyUSB0, 0xb0);
		serialPutchar (fd_ttyUSB0, 0x00);
		serialPutchar (fd_ttyUSB0, 0x02);
		serialPutchar (fd_ttyUSB0, 0xc6);
		serialPutchar (fd_ttyUSB0, 0xf8);
		
		printf("d7:%x,d8:%x\r\n",(uint8_t)(0x00ff&crc_16_modbus_val_send),(uint8_t)(0x00ff&(crc_16_modbus_val_send>>8)));
		
		//获取接收数据校验
		usleep(1000000);
		while(serialDataAvail(fd_ttyUSB0) > 0)
		{
			data_rec[recIndex]=(uint8_t)serialGetchar(fd_ttyUSB0);
			recIndex++;
		}
		
		sprintf(com,"./libcrc-master/tstcrc %x %x %x %x %x %x %x", data_rec[0],data_rec[1],data_rec[2],data_rec[3],data_rec[4],data_rec[5],data_rec[6]);
		printf("com:%s\n",com);
		fp=popen(com, "r");
		while(memset(buf, 0, sizeof(buf)), fgets(buf, sizeof(buf) - 1, fp) != 0 )//以换行结尾
		{
			printf("buf:%s\n",buf);//分别输出当前返回内容
			crc_16_modbus_val_rec = strtol(buf, NULL, 16);
			printf("crc_16_modbus_val_rec:%x\n",crc_16_modbus_val_rec);//分别输出当前返回内容
		}
		
		//校验值位置为 data_rec[2]+3(低) data_rec[2]+3+1(高)
		if(crc_16_modbus_val_rec!=((data_rec[8]<<8)|data_rec[7]))
			goto readUltrasound;
		
		recIndex=0;//接收下标清空
		
		
		int16_t data=(data_rec[3]<<8)|data_rec[4];
		
		double J = 1.72;
		
		if(data<0x39)
		{
			J=1.72;
		}
		else
		{
			J=-0.0031*data+1.8992;	
		}
		
		Pressure = J*data;
		
		
		
		printf("data=%d\n",data);
		printf("J=%lf\n",J);
		printf("Pressure=%lf\n",Pressure);

	}

	serialClose(fd);
	return 0;
}


//memset(str, '\0', sizeof(com));
/*
fp=popen(com, "r");			
while(memset(buf, 0, sizeof(buf)), fgets(buf, sizeof(buf) - 1, fp) != 0 )//以换行结尾
{
printf("buf:%s\n",buf);//分别输出当前返回内容
crc16_send = strtol(buf, NULL, 16);
printf("crc16_send:%x\n",crc16_send);//分别输出当前返回内容
// if(my_strstr(buf,"GetCurrent=",sizeof(buf))!=NULL)
// {
	// if((buf[strlen(buf)-1] == 0x0a)|(buf[strlen(buf)-1] == 0x0d))
	// buf[strlen(buf)-1]='\0';
	// if((buf[strlen(buf)-2] == 0x0a)|(buf[strlen(buf)-2] == 0x0d))
	// buf[strlen(buf)-2]='\0';
	// if((buf[strlen(buf)-3] == 0x0a)|(buf[strlen(buf)-3] == 0x0d))
	// buf[strlen(buf)-3]='\0';

	// _temp=atoi(my_strstr(buf,"GetCurrent=",sizeof(buf))+11);


	// // printf("读取的电流值为：%d\r\n",_temp);
	// if(_temp!=set_temp)//读取值不等于设置值
	// {
		// //添加硬件控制，重启光机
		// //待添加并延时

	// }
	// else//功率设置成功
	// {
		// // printf("成功设置成功,电流_temp=%d\r\n",_temp);
	// }
// }
}
pclose(fp);

*/



