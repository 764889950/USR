#include <wiringSerial.h>
#include "DEV_Config_2.h"//其他设备初始化
#include "SLC_Read.h"
#include "HMI-Screen.h"
#include "parameter.h"
#include "strstr.h"
#include "marlinHost.h"
#include "file.h"

#include "data_global.h"


/*-------------------------------------------------------------------------------------
	'函数名:hex_send
    '功能说明
    '-------------------------------------------------------------------------------------
    '简述:实现发送系列控制码
    '引用方法:
    '解释:------
    '=====================================================================================
    '参数说明
    '-------------------------------------------------------------------------------------
    '传入参数:
    '参数1       com 要发送的控制码

    '函数参数:
    '参数2       data_length 发送的控制码个数
	'=====================================================================================
*/
void hex_send(char com[],char data_length)
{
	int i=0;
	for(i=0;i<data_length;i++)
	{
		serialPutchar (fd_HMI, com[i]);
		// printf("%x ",com[i]);
	}
	// printf("\r\n");
}


//切换到光机自检失败界面：5A A5 04 80 03 0033
void page_projector_init_fail_switch(void)
{
	char com[10]={0};
	com[0]=0x5A; com[1]=0xA5; com[2]=0x04; com[3]=0x80; com[4]=0x03; com[5]=0x00; com[6]=0x33;
	hex_send(com,7);
}

//切换到Z轴自检失败界面：5A A5 04 80 03 0034
void page_Z_init_fail_switch(void)
{
	char com[10]={0};
	com[0]=0x5A; com[1]=0xA5; com[2]=0x04; com[3]=0x80; com[4]=0x03; com[5]=0x00; com[6]=0x34;
	hex_send(com,7);
}

//切换到初始页界面：5A A5 04 80 03 0000
void page_init_switch(void)
{
	char com[10]={0};
	com[0]=0x5A; com[1]=0xA5; com[2]=0x04; com[3]=0x80; com[4]=0x03; com[5]=0x00; com[6]=0x00;
	hex_send(com,7);
}


//切换到首页：5A A5 07 82 0084 5A01 0002
void page_home_switch(void)
{
	char com[10]={0};
	// com[0]=0x5A; com[1]=0xA5; com[2]=0x04; com[3]=0x80; com[4]=0x03; com[5]=0x00; com[6]=0x02;
	com[0]=0x5A; com[1]=0xA5; com[2]=0x07; com[3]=0x82; com[4]=0x00; com[5]=0x84; com[6]=0x5A; com[7]=0x01; com[8]=0x00; com[9]=0x02;
	hex_send(com,10);
	
	// printf("切换到首页\r\n");
}


//切换到模型页指令为：5A A5 07 82 0084 5A01 0006
void page_model_switch(void)
{
	char com[10]={0};
	com[0]=0x5A; com[1]=0xA5; com[2]=0x07; com[3]=0x82; com[4]=0x00; com[5]=0x84; com[6]=0x5A; com[7]=0x01; com[8]=0x00; com[9]=0x06;
	hex_send(com,10);
}


//切换到模型保存成功页 5A A5 04 80 03 0026
void page_model_save_succeed_switch(void)
{
	char com[10]={0};
	com[0]=0x5A; com[1]=0xA5; com[2]=0x04; com[3]=0x80; com[4]=0x03; com[5]=0x00; com[6]=0x26;
	hex_send(com,7);
}


//切换到模型保存失败页 5A A5 04 80 03 0027
void page_model_save_fail_switch(void)
{
	char com[10]={0};
	com[0]=0x5A; com[1]=0xA5; com[2]=0x04; com[3]=0x80; com[4]=0x03; com[5]=0x00; com[6]=0x27;
	hex_send(com,7);
}

//切换到模型删除失败页指令 5A A5 04 80 03 0029
void page_model_delet_fail_switch(void)
{
	char com[10]={0};
	com[0]=0x5A; com[1]=0xA5; com[2]=0x04; com[3]=0x80; com[4]=0x03; com[5]=0x00; com[6]=0x29;
	hex_send(com,7);
}


//切换到系统异常界面   5A A5 04 80 03 0005
void page_error_switch(void)
{
	char com[10]={0};
	com[0]=0x5A; com[1]=0xA5; com[2]=0x04; com[3]=0x80; com[4]=0x03; com[5]=0x00; com[6]=0x05;
	hex_send(com,7);
}


//切换到管理员配置页面 5A A5 04 80 03 000C
void page_admin_config_switch(void)
{
	char com[10]={0};
	com[0]=0x5A; com[1]=0xA5; com[2]=0x04; com[3]=0x80; com[4]=0x03; com[5]=0x00; com[6]=0x0C;
	hex_send(com,7);
}


//切换到超级管理员配置页面5A A5 07 82 0084 5A01 001C
void page_supper_admin_config_switch(void)
{
	char com[10]={0};
	com[0]=0x5A; com[1]=0xA5; com[2]=0x07; com[3]=0x82; com[4]=0x00; com[5]=0x84; com[6]=0x5A; com[7]=0x01; com[8]=0x00; com[9]=0x1C;
	hex_send(com,10);
}


//切换到打印加工页：5A A5 07 82 0084 5A01 0003
void page_print_switch(void)
{
	char com[10]={0};
	com[0]=0x5A; com[1]=0xA5; com[2]=0x07; com[3]=0x82; com[4]=0x00; com[5]=0x84; com[6]=0x5A; com[7]=0x01; com[8]=0x00; com[9]=0x03;
	hex_send(com,10);
}

//切换到打印加工加载页：5A A5 07 82 0084 5A01 0005
void page_print_load_switch(void)
{
	char com[10]={0};
	com[0]=0x5A; com[1]=0xA5; com[2]=0x07; com[3]=0x82; com[4]=0x00; com[5]=0x84; com[6]=0x5A;; com[7]=0x01; com[8]=0x00; com[9]=0x05;
	hex_send(com,10);
	file_change_flag=1;//等于1不允许u盘读取内容发生变化
	
	time_print=0;
	
	//恢复"暂停与开始"按钮状态
	serialPutchar (fd_HMI, 0x5A);
	serialPutchar (fd_HMI, 0xA5);
	serialPutchar (fd_HMI, 0x05);
	serialPutchar (fd_HMI, 0x82);
	serialPutchar (fd_HMI, 0x40);
	serialPutchar (fd_HMI, 0x1D);
	serialPutchar (fd_HMI, 0x00);
	serialPutchar (fd_HMI, 0x00);
	
}

//清空模型列表缓冲区内容 
void menu_show_clear(void)
{
	unsigned int i=0;	
	char com[10]={0};
	
	com[0]=0x5A; com[1]=0xA5; com[2]=203; com[3]=0x82;
	//清空缓冲区
	// com[4]=0x01; com[5]=0x10;//第一个地址
	com[4]=0x30; com[5]=0x10;//第一个地址
	hex_send(com,6);
	for(i=0;i<200;i++)
	{
		serialPutchar(fd_HMI,0);
	}

	// com[4]=0x01; com[5]=0x20;//第二个地址
	com[4]=0x30; com[5]=0x30;//第二个地址
	hex_send(com,6);
	for(i=0;i<200;i++)
	{
		serialPutchar(fd_HMI,0);
	}
	
	// com[4]=0x01; com[5]=0x30;//第三个地址
	com[4]=0x30; com[5]=0x50;//第三个地址
	hex_send(com,6);
	for(i=0;i<200;i++)
	{
		serialPutchar(fd_HMI,0);
	}
	
	// com[4]=0x01; com[5]=0x40;//第四个地址
	com[4]=0x30; com[5]=0x70;//第四个地址
	hex_send(com,6);
	for(i=0;i<200;i++)
	{
		serialPutchar(fd_HMI,0);
	}
	
	// com[4]=0x02; com[5]=0x20;//第五个地址
	com[4]=0x30; com[5]=0x90;//第五个地址
	hex_send(com,6);
	for(i=0;i<200;i++)
	{
		serialPutchar(fd_HMI,0);
	}

	// com[4]=0x02; com[5]=0x30;//第六个地址
	com[4]=0x30; com[5]=0xB0;//第六个地址
	hex_send(com,6);
	for(i=0;i<200;i++)
	{
		serialPutchar(fd_HMI,0);
	}

	// com[4]=0x02; com[5]=0x40;//第七个地址
	com[4]=0x30; com[5]=0xD0;//第七个地址
	hex_send(com,6);
	for(i=0;i<200;i++)
	{
		serialPutchar(fd_HMI,0);
	}

	// com[4]=0x02; com[5]=0x30;//第八个地址
	com[4]=0x30; com[5]=0xF0;//第八个地址
	hex_send(com,6);
	for(i=0;i<200;i++)
	{
		serialPutchar(fd_HMI,0);
	}
	
}



//将文件输出显示在屏幕上
void file_show(void)
{
	// usleep(100000);
	unsigned int i=0;
	unsigned int dex_i=0;
	char *p;
	char buf[256]={0};
	char str[256]={0};
	char com[10]={0};
	FILE *file_fp;				//文件指针
	char buffer[1050]={0};		//全路径
	char file_name_temp[256]={0};	//文件名
	
	static char com_2[100];

	sprintf(com_2,"%s/file.cfg", print_cfg_path);
	
	file_fp = fopen(com_2,"r");	
	
	
	for(i=0;i<(unsigned int)(page_menu-1)*8;i++)
    {
        fgets(buffer, sizeof(buffer) - 1, file_fp);
    }
	for(dex_i=0;dex_i<8;dex_i++)
	{
		fgets(buffer, sizeof(buffer) - 1, file_fp);
		if((buffer[strlen(buffer)-1]=='\r')|(buffer[strlen(buffer)-1]=='\n'))
			buffer[strlen(buffer)-1]='\0';
		if((buffer[strlen(buffer)-2]=='\r')|(buffer[strlen(buffer)-2]=='\n'))
			buffer[strlen(buffer)-2]='\0';
		if((buffer[strlen(buffer)-3]=='\r')|(buffer[strlen(buffer)-3]=='\n'))
			buffer[strlen(buffer)-3]='\0';
		
		if((page_menu-1)*8+dex_i>=(unsigned int)file_num)
			buffer[0]='\0';

		// printf("buffer：%s\r\n",buffer);   
		//上句假设以某种方式获得的全文件名在pathname中,"..."中只是举例
		strcpy(buf,(p=strrchr(buffer,'/')) ? p+1 :buffer);
		sprintf(str,"%s",buf);
		
		// printf("buf: %s\n", buf);
		u2g(str, strlen(str), buf, sizeof(buf));
		// usleep(1000);     
		strcpy(file_name_temp,buf);
		
		// printf("buf: %s\n", buf);
		// printf("file_name_temp: %s\n", file_name_temp);
		if(dex_i==0)
		{
			com[0]=0x5A; com[1]=0xA5; com[3]=0x82;
			// com[4]=0x01; com[5]=0x10;//第一个地址
			com[4]=0x30; com[5]=0x10;//第一个地址
			
			com[2]=strlen(file_name_temp)+3;
			if(com[2]>30)
			{
				com[2]=30;
				for(size_t k=0,t=0,s=0;;k++)
				{
					if(file_name_temp[k]>0x80)
					{
						t=1;
					}
					if(t==1)
					{
						s++;
					}
					if(k>=com[2])
					{
						if(s%2==1)
						{
							com[2]=k;
							break;
						}
					}
				}
				
				hex_send(com,6);
				for(i=0;i<(unsigned int)(com[2]-3-3);i++)//剩下3个用于发送...
				{
					serialPutchar(fd_HMI,file_name_temp[i]);
				}
				serialPutchar(fd_HMI,'.');
				serialPutchar(fd_HMI,'.');
				serialPutchar(fd_HMI,'.');
			}
			else
			{
				hex_send(com,6);
				for(i=0;i<strlen(file_name_temp);i++)
				{
					serialPutchar(fd_HMI,file_name_temp[i]);
				}
			}
		}
		if(dex_i==1)
		{
			com[0]=0x5A; com[1]=0xA5; com[3]=0x82;
			// com[4]=0x01; com[5]=0x20;//第二个地址
			com[4]=0x30; com[5]=0x30;//第二个地址
			com[2]=strlen(file_name_temp)+3;
			if(com[2]>30)
			{
				com[2]=30;
				for(size_t k=0,t=0,s=0;;k++)
				{
					if(file_name_temp[k]>0x80)
					{
						t=1;
					}
					if(t==1)
					{
						s++;
					}
					if(k>=com[2])
					{
						if(s%2==1)
						{
							com[2]=k;
							break;
						}
					}
				}
				
				hex_send(com,6);
				for(i=0;i<(unsigned int)(com[2]-3-3);i++)//剩下3个用于发送...
				{
					serialPutchar(fd_HMI,file_name_temp[i]);
				}
				serialPutchar(fd_HMI,'.');
				serialPutchar(fd_HMI,'.');
				serialPutchar(fd_HMI,'.');
			}
			else
			{
				hex_send(com,6);
				for(i=0;i<strlen(file_name_temp);i++)
				{
					serialPutchar(fd_HMI,file_name_temp[i]);
				}
			}
		}
		if(dex_i==2)
		{
			com[0]=0x5A; com[1]=0xA5; com[3]=0x82;
			// com[4]=0x01; com[5]=0x30;//第三个地址
			com[4]=0x30; com[5]=0x50;//第三个地址
			com[2]=strlen(file_name_temp)+3;
			if(com[2]>30)
			{
				com[2]=30;
				for(size_t k=0,t=0,s=0;;k++)
				{
					if(file_name_temp[k]>0x80)
					{
						t=1;
					}
					if(t==1)
					{
						s++;
					}
					if(k>=com[2])
					{
						if(s%2==1)
						{
							com[2]=k;
							break;
						}
					}
				}
				
				hex_send(com,6);
				for(i=0;i<(unsigned int)(com[2]-3-3);i++)//剩下3个用于发送...
				{
					serialPutchar(fd_HMI,file_name_temp[i]);
				}
				serialPutchar(fd_HMI,'.');
				serialPutchar(fd_HMI,'.');
				serialPutchar(fd_HMI,'.');
			}
			else
			{
				hex_send(com,6);
				for(i=0;i<strlen(file_name_temp);i++)
				{
					serialPutchar(fd_HMI,file_name_temp[i]);
				}
			}
		}
		if(dex_i==3)
		{
			com[0]=0x5A; com[1]=0xA5; com[3]=0x82;
			// com[4]=0x01; com[5]=0x40;//第四个地址
			com[4]=0x30; com[5]=0x70;//第四个地址
			com[2]=strlen(file_name_temp)+3;
			if(com[2]>30)
			{
				com[2]=30;
				for(size_t k=0,t=0,s=0;;k++)
				{
					if(file_name_temp[k]>0x80)
					{
						t=1;
					}
					if(t==1)
					{
						s++;
					}
					if(k>=com[2])
					{
						if(s%2==1)
						{
							com[2]=k;
							break;
						}
					}
				}
				
				hex_send(com,6);
				for(i=0;i<(unsigned int)(com[2]-3-3);i++)//剩下3个用于发送...
				{
					serialPutchar(fd_HMI,file_name_temp[i]);
				}
				serialPutchar(fd_HMI,'.');
				serialPutchar(fd_HMI,'.');
				serialPutchar(fd_HMI,'.');
			}
			else
			{
				hex_send(com,6);
				for(i=0;i<strlen(file_name_temp);i++)
				{
					serialPutchar(fd_HMI,file_name_temp[i]);
				}
			}
		}
		
		
		if(dex_i==4)
		{
			com[0]=0x5A; com[1]=0xA5; com[3]=0x82;
			// com[4]=0x02; com[5]=0x20;//第五个地址
			com[4]=0x30; com[5]=0x90;//第五个地址
			com[2]=strlen(file_name_temp)+3;
			if(com[2]>30)
			{
				com[2]=30;
				for(size_t k=0,t=0,s=0;;k++)
				{
					if(file_name_temp[k]>0x80)
					{
						t=1;
					}
					if(t==1)
					{
						s++;
					}
					if(k>=com[2])
					{
						if(s%2==1)
						{
							com[2]=k;
							break;
						}
					}
				}
				
				hex_send(com,6);
				for(i=0;i<(unsigned int)(com[2]-3-3);i++)//剩下3个用于发送...
				{
					serialPutchar(fd_HMI,file_name_temp[i]);
				}
				serialPutchar(fd_HMI,'.');
				serialPutchar(fd_HMI,'.');
				serialPutchar(fd_HMI,'.');
			}
			else
			{
				hex_send(com,6);
				for(i=0;i<strlen(file_name_temp);i++)
				{
					serialPutchar(fd_HMI,file_name_temp[i]);
				}
			}		
		}		
		
		if(dex_i==5)
		{
			com[0]=0x5A; com[1]=0xA5; com[3]=0x82;
			// com[4]=0x02; com[5]=0x30;//第六个地址
			com[4]=0x30; com[5]=0xB0;//第六个地址
			com[2]=strlen(file_name_temp)+3;
			if(com[2]>30)
			{
				com[2]=30;
				for(size_t k=0,t=0,s=0;;k++)
				{
					if(file_name_temp[k]>0x80)
					{
						t=1;
					}
					if(t==1)
					{
						s++;
					}
					if(k>=com[2])
					{
						if(s%2==1)
						{
							com[2]=k;
							break;
						}
					}
				}
				
				hex_send(com,6);
				for(i=0;i<(unsigned int)(com[2]-3-3);i++)//剩下3个用于发送...
				{
					serialPutchar(fd_HMI,file_name_temp[i]);
				}
				serialPutchar(fd_HMI,'.');
				serialPutchar(fd_HMI,'.');
				serialPutchar(fd_HMI,'.');
			}
			else
			{
				hex_send(com,6);
				for(i=0;i<strlen(file_name_temp);i++)
				{
					serialPutchar(fd_HMI,file_name_temp[i]);
				}
			}
		}
		
		if(dex_i==6)
		{
			com[0]=0x5A; com[1]=0xA5; com[3]=0x82;
			// com[4]=0x02; com[5]=0x40;//第七个地址
			com[4]=0x30; com[5]=0xD0;//第七个地址
			com[2]=strlen(file_name_temp)+3;
			if(com[2]>30)
			{
				com[2]=30;
				for(size_t k=0,t=0,s=0;;k++)
				{
					if(file_name_temp[k]>0x80)
					{
						t=1;
					}
					if(t==1)
					{
						s++;
					}
					if(k>=com[2])
					{
						if(s%2==1)
						{
							com[2]=k;
							break;
						}
					}
				}
				
				hex_send(com,6);
				for(i=0;i<(unsigned int)(com[2]-3-3);i++)//剩下3个用于发送...
				{
					serialPutchar(fd_HMI,file_name_temp[i]);
				}
				serialPutchar(fd_HMI,'.');
				serialPutchar(fd_HMI,'.');
				serialPutchar(fd_HMI,'.');
			}
			else
			{
				hex_send(com,6);
				for(i=0;i<strlen(file_name_temp);i++)
				{
					serialPutchar(fd_HMI,file_name_temp[i]);
				}
			}
		}

		if(dex_i==7)
		{
			com[0]=0x5A; com[1]=0xA5; com[3]=0x82;
			// com[4]=0x02; com[5]=0x50;//第八个地址
			com[4]=0x30; com[5]=0xF0;//第八个地址
			com[2]=strlen(file_name_temp)+3;
			if(com[2]>30)
			{
				com[2]=30;
				for(size_t k=0,t=0,s=0;;k++)
				{
					if(file_name_temp[k]>0x80)
					{
						t=1;
					}
					if(t==1)
					{
						s++;
					}
					if(k>=com[2])
					{
						if(s%2==1)
						{
							com[2]=k;
							break;
						}
					}
				}
				
				hex_send(com,6);
				for(i=0;i<(unsigned int)(com[2]-3-3);i++)//剩下3个用于发送...
				{
					serialPutchar(fd_HMI,file_name_temp[i]);
				}
				serialPutchar(fd_HMI,'.');
				serialPutchar(fd_HMI,'.');
				serialPutchar(fd_HMI,'.');
			}
			else
			{
				hex_send(com,6);
				for(i=0;i<strlen(file_name_temp);i++)
				{
					serialPutchar(fd_HMI,file_name_temp[i]);
				}
			}
		}
	}
	fclose(file_fp);
}

void file_get(void)
{
	// usleep(50000);   
	unsigned int i=0;
	char *p;
	char buf[256]={0};
	char str[256]={0};
	char buffer[1050]={0};		//全路径
	FILE *file_fp;				//文件指针
	
	static char com[100];

	sprintf(com,"%s/file.cfg", print_cfg_path);
	
	file_fp = fopen(com,"r");

	for(i=0;i<(unsigned int)file;i++)
	{

		fgets(buffer, sizeof(buffer) - 1, file_fp);
		if((buffer[strlen(buffer)-1]=='\r')|(buffer[strlen(buffer)-1]=='\n'))
			buffer[strlen(buffer)-1]='\0';
		if((buffer[strlen(buffer)-2]=='\r')|(buffer[strlen(buffer)-2]=='\n'))
			buffer[strlen(buffer)-2]='\0';
		if((buffer[strlen(buffer)-3]=='\r')|(buffer[strlen(buffer)-3]=='\n'))
			buffer[strlen(buffer)-3]='\0';
	}
	strcpy(print_path,buffer);
	// printf("打印文件为：%s\r\n",print_path);
	
	//上句假设以某种方式获得的全文件名在pathname中,"..."中只是举例
	strcpy(buf,(p=strrchr(buffer,'/')) ? p+1 :buffer);
	
	strcpy(file_name_UTF_8,buf);
	
	sprintf(str,"%s",buf);
	u2g(str, strlen(str), buf, sizeof(buf));
	// usleep(1000);
	strcpy(file_name,buf);
	
	fclose(file_fp);
}


void show_print_inf_init(void)
{
	unsigned int i=0;	
	char str_temp[50]={0};
	char buf[50]={0};
	char com[10]={0};

	//发送初始化值
	//发送文件名
	//清空缓冲区
	com[0]=0x5A; com[1]=0xA5; com[2]=203; com[3]=0x82;
	com[4]=0x31; com[5]=0x10;//第一个地址
	hex_send(com,6);
	for(i=0;i<200;i++)
	{
		serialPutchar(fd_HMI,0);
	}
	
	//发送文件名
	com[2]=strlen(file_name)+3;
	if(com[2]>33)
	{
		com[2]=33;
		for(size_t k=0,t=0,s=0;;k++)
		{
			if(file_name[k]>0x80)
			{
				t=1;
			}
			if(t==1)
			{
				s++;
			}
			if(k>=com[2])
			{
				if(s%2==1)
				{
					com[2]=k;
					break;
				}
			}
		}
		
		hex_send(com,6);
		for(i=0;i<(unsigned int)(com[2]-3-3);i++)//剩下3个用于发送...
		{
			serialPutchar(fd_HMI,file_name[i]);
		}
		serialPutchar(fd_HMI,'.');
		serialPutchar(fd_HMI,'.');
		serialPutchar(fd_HMI,'.');
	}
	else
	{
		hex_send(com,6);
		for(i=0;i<strlen(file_name);i++)
		{
			serialPutchar(fd_HMI,file_name[i]);
		}
	}


	//清空缓冲区（剩余时间显示区域）
	com[0]=0x5A; com[1]=0xA5; com[2]=203; com[3]=0x82;
	com[4]=0x31; com[5]=0x30;//第六个地址
	hex_send(com,6);
	for(i=0;i<200;i++)
	{
		serialPutchar(fd_HMI,0);
	}	
	
	//预估剩余时间
	// 此处进行剩余时间预估
	time_calculate();
	sprintf(buf,"%ld min                   ",time_print_res/60);
	u2g(buf, strlen(buf), str_temp, sizeof(str_temp));
	com[2]=strlen(str_temp)+3; 
	com[4]=0x31; com[5]=0x30;//第六个地址
	hex_send(com,6);	
	for(i=0;i<strlen(str_temp);i++)
	{
		serialPutchar(fd_HMI,str_temp[i]);
	}
	// printf("剩余时间time_print_res为:%ld\r\n",time_print_res);



	//打印时间
	sprintf(str_temp,"0:0                    ");
	com[2]=strlen(str_temp)+3;
	com[4]=0x31; com[5]=0x70;//第六个地址
	hex_send(com,6);
	for(i=0;i<strlen(str_temp);i++)
	{
		serialPutchar(fd_HMI,str_temp[i]);
	}

	//当前打印层
	sprintf(str_temp,"1                    ");
	com[2]=strlen(str_temp)+3;
	com[4]=0x31; com[5]=0x90;//第六个地址
	hex_send(com,6);
	for(i=0;i<strlen(str_temp);i++)
	{
		serialPutchar(fd_HMI,str_temp[i]);
	}
	
	//打印进度
	sprintf(str_temp,"0%%                    ");
	com[2]=strlen(str_temp)+3;
	com[4]=0x31; com[5]=0xB0;//第六个地址
	hex_send(com,6);
	for(i=0;i<strlen(str_temp);i++)
	{
		serialPutchar(fd_HMI,str_temp[i]);
	}


	//读取层厚与层数信息
	ReadSLC_inf(print_path);
	sprintf(str_temp,"%.2lfmm                    ",level_z);
	com[2]=strlen(str_temp)+3;
	com[4]=0x31; com[5]=0xD0;//第六个地址
	hex_send(com,6);
	for(i=0;i<strlen(str_temp);i++)
	{
		serialPutchar(fd_HMI,str_temp[i]);
	}
	//打印总层数
	sprintf(str_temp,"%ld                    ",layer_z);
	com[2]=strlen(str_temp)+3; 
	com[4]=0x31; com[5]=0x50;//第六个地址
	hex_send(com,6);
	for(i=0;i<strlen(str_temp);i++)
	{
		serialPutchar(fd_HMI,str_temp[i]);
	}
}


void show_print_inf_print(void)
{
	unsigned int i=0;	
	char str_temp[50]={0};
	char buf[50]={0};
	char com[10]={0};

	com[0]=0x5A; com[1]=0xA5; com[3]=0x82;
	//预估剩余时间
	sprintf(buf,"%ld min                   ",time_print_res/60);
	u2g(buf, strlen(buf), str_temp, sizeof(str_temp));	
	com[2]=strlen(str_temp)+3; 
	com[4]=0x31; com[5]=0x30;//第一个地址
	hex_send(com,6);	
	for(i=0;i<strlen(str_temp);i++)
	{
		serialPutchar(fd_HMI,str_temp[i]);
	}


	//打印时间
	sprintf(str_temp,"%ld:%ld                    ",time_print/60,time_print%60);
	com[2]=strlen(str_temp)+3; 
	com[4]=0x31; com[5]=0x70;//第二个地址
	hex_send(com,6);
	for(i=0;i<strlen(str_temp);i++)
	{
		serialPutchar(fd_HMI,str_temp[i]);
	}
	

	//当前打印层
	sprintf(str_temp,"%ld                    ",print_z);
	com[2]=strlen(str_temp)+3; 
	com[4]=0x31; com[5]=0x90;//第三个地址
	hex_send(com,6);
	for(i=0;i<strlen(str_temp);i++)
	{
		serialPutchar(fd_HMI,str_temp[i]);
	}
	// printf("print_z:%ld\r\n",print_z);
	
	//打印进度
	sprintf(str_temp,"%ld%%                    ",sch);
	com[2]=strlen(str_temp)+3;
	com[4]=0x31; com[5]=0xB0;//第四个地址
	hex_send(com,6);
	for(i=0;i<strlen(str_temp);i++)
	{
		serialPutchar(fd_HMI,str_temp[i]);
	}

}


