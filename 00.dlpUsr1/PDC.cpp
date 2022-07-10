#include "wiringSerial.h"
#include "strstr.h"
#include "parameter.h"
#include "DEV_Config_2.h"//其他设备初始化

#include "data_global.h"




//set_temp为光机设置值，sor为光机类型
void LED_P_Set(long set_temp,char sor)
{
	/*
	FILE *fp;
	char buf[512];
	// LED_intensity_temp++;
	// serialPutchar (fd_PDC03, LED_intensity_temp);
	//设置方式待添加  LED_intensity_per*LED_intensity/100.0
	
	
	static int _temp=0;
	static char times=0;//等待时间次数
	static char times_reset=0;//重发设置次数
	if(sor==0)
	{		
		reset:
		
		
		//设置功率值
		serialPutchar (fd_PDC03, 0xE0);
		serialPutchar (fd_PDC03, 0x0A);
		serialPutchar (fd_PDC03, 0x41);
		serialPutchar (fd_PDC03, 0x02);
		serialPutchar (fd_PDC03, (char)set_temp);
		serialPutchar (fd_PDC03, 0);
		serialPutchar (fd_PDC03, (char)set_temp);
		serialPutchar (fd_PDC03, 0);
		serialPutchar (fd_PDC03, (char)set_temp);
		serialPutchar (fd_PDC03, 0);
		// printf("已设置功率\r\n");
		
		rec:
		usleep(300000);
		
		// printf("延迟等待读取功率\r\n");
		
		if(uart2_sign == 1)
		{
			//提取电流设置值
			// _temp=atoi(my_strstr(RevBuf2,"Red Led Current = ",sizeof(RevBuf2))+18);//待修改，有问题
			// printf("读取的设置值为：%d\r\n", _temp);
			// printf("提取电流设置值成功\r\n");
		}
		else
		{
			if(times<3)
			{
				times++;
				// printf("hhhhh_\r\n");
				if(my_strstr(RevBuf2,"Ack",sizeof(RevBuf2)))
				{
					goto PDC_end;
				}				
				goto rec;
			}
			else
			{
				times=0;
				if(times_reset<3)
				{
					times_reset++;
					goto reset;
				}
				else
				{
					if(my_strstr(RevBuf2,"Ack",sizeof(RevBuf2))!=NULL)
					{}
					else
					{
						reboot_flag=1;
						LED=0;
						//切换到光机自检失败界面
						page_projector_init_fail_switch();
						// reboot();
					}
				}
			}
			PDC_end:
			times=0;
			times_reset=0;
		}
	}
	
	static char com[100];
	if(sor==1)//高分辨率光机功率设置
	{
		sprintf(com,"sudo python2 %s/hd6500_set.py %ld", print_cfg_path,set_temp);
		fp=popen(com, "r");
		
		printf("设置命令为：%s\r\n",com);//
		
		while(memset(buf, 0, sizeof(buf)), fgets(buf, sizeof(buf) - 1, fp) != 0 )//以换行结尾
		{
			// printf("%s\r\n",buf);//分别输出当前返回内容
			if(my_strstr(buf,"GetCurrent=",sizeof(buf))!=NULL)
			{
				if((buf[strlen(buf)-1] == 0x0a)|(buf[strlen(buf)-1] == 0x0d))
					buf[strlen(buf)-1]='\0';
				if((buf[strlen(buf)-2] == 0x0a)|(buf[strlen(buf)-2] == 0x0d))
					buf[strlen(buf)-2]='\0';
				if((buf[strlen(buf)-3] == 0x0a)|(buf[strlen(buf)-3] == 0x0d))
				buf[strlen(buf)-3]='\0';
			
				_temp=atoi(my_strstr(buf,"GetCurrent=",sizeof(buf))+11);
				
				
				// printf("读取的电流值为：%d\r\n",_temp);
				if(_temp!=set_temp)//读取值不等于设置值
				{
					//添加硬件控制，重启光机
					//待添加并延时
					
				}
				else//功率设置成功
				{
					// printf("成功设置成功,电流_temp=%d\r\n",_temp);
				}
			}
		}
		pclose(fp);
	}	
	
	*/
		
		
		if(set_temp<0) set_temp=0;
		if(set_temp>100) set_temp=100;
		
		setup:
		
		serialPutchar (fd_PDC03, (unsigned char)(set_temp));
		usleep(500000);
		serialPutchar (fd_PDC03, (unsigned char)(set_temp));
		usleep(500000);
		serialPutchar (fd_PDC03, (unsigned char)(set_temp));
		usleep(500000);
		serialPutchar (fd_PDC03, (unsigned char)(set_temp));
		usleep(500000);
		
		
		//printf("设置电流值为：%d\r\n",set_temp);
		

		if(set_temp_extern!=set_temp)
		{
			goto setup;
		}

}


