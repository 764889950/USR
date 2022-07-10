#include <wiringSerial.h> 
#include "DEV_Config_2.h"//其他设备初始化
#include "DEV_Config.h"
#include "parameter.h"
#include "HMI-Screen.h"//fd_serial
#include "data_global.h"

#include "file.h"

long usb_flag=0;//u盘插入拔出标识
long file_change_flag=0;//u盘读取内容发生变化标识

//-----------------------------------------------
//读取serial.cfg文件并进行配置
//-----------------------------------------------
void printcfg_read(void)
{
	FILE *print_fp;
	// printf("read printcfg\n");
	static char com[100];
	sprintf(com,"%s/print.cfg", print_cfg_path);
	print_fp = fopen(com,"r");

	if(NULL == print_fp)
	{
		printf("can't open print.cfg");
	}
	else
	{
		fscanf(print_fp, "print_work_level_z=%lf\n", &print_work_level_z);
		fscanf(print_fp, "print_work_level_Pedal=%lf\n", &print_work_level_Pedal);
		fscanf(print_fp, "z_rpo=%lf\n", &z_rpo);
		fscanf(print_fp, "Pedal_rpo=%lf\n", &Pedal_rpo);
		fscanf(print_fp, "dir_z=%ld\n", &dir_z);
		fscanf(print_fp, "dir_Pedal=%ld\n", &dir_Pedal);
		fscanf(print_fp, "speed_z=%ld\n", &speed_z);
		fscanf(print_fp, "speed_Pedal=%ld\n", &speed_Pedal);	
		fscanf(print_fp, "led_choose=%ld\n", &led_choose);
		fscanf(print_fp, "print_style=%ld\n", &print_style);
		fscanf(print_fp, "buttom=%ld\n", &buttom);
	}
	fclose(print_fp);
}


//-----------------------------------------------
//写入参数到print.cfg文件
//-----------------------------------------------
void printcfg_write(void)
{
	FILE *print_fp;
	// printf("writeprintcfg\n");
	static char com[100];

	sprintf(com,"%s/print.cfg", print_cfg_path);
	print_fp = fopen(com,"w+");

	if(NULL == print_fp)
	{
		printf("can't open print.cfg");
	}
	else
	{
		fprintf(print_fp, "print_work_level_z=%lf\n", print_work_level_z);
		fprintf(print_fp, "print_work_level_Pedal=%lf\n", print_work_level_Pedal);
		fprintf(print_fp, "z_rpo=%lf\n", z_rpo);
		fprintf(print_fp, "Pedal_rpo=%lf\n", Pedal_rpo);
		fprintf(print_fp, "dir_z=%ld\n",dir_z);
		fprintf(print_fp, "dir_Pedal=%ld\n",dir_Pedal);
		fprintf(print_fp, "speed_z=%ld\n",speed_z);
		fprintf(print_fp, "speed_Pedal=%ld\n",speed_Pedal);	
		fprintf(print_fp, "led_choose=%ld\n",led_choose);
		fprintf(print_fp, "print_style=%ld\n",print_style);
		fprintf(print_fp, "buttom=%ld\n",buttom);
	}
	fclose(print_fp);
}



//-----------------------------------------------
//保存读取到的slc文件数据到file文件并读取文件数量
//-----------------------------------------------
void read_print_file(void)
{
	FILE *fp;
	char str[256]={0};
	char buf[1050];//文件深度默认为4
	
	static char com[100];
	
	// fp = popen("sudo find /media/pi/usb1 -name '*.slc'", "r");//查找/media/pi/usb1目录及分支目录下的slc文件
	fp = popen("sudo find /media/pi/ -maxdepth 2 -name '*.slc'", "r");//-maxdepth 2表示查找深度为2
	FILE *fp_temp;  //将数据保存到文本区
	//保存数据到文件
	sprintf(com,"%s/file.cfg", print_cfg_path);
	fp_temp=fopen(com, "w+");
	while(memset(buf, 0, sizeof(buf)), fgets(buf, sizeof(buf) - 1, fp) != 0 )
	{
		fprintf(fp_temp,"%s",buf);
	}
	fclose(fp_temp);
	pclose(fp);				
	
	// printf("begin read file_num\r\n");
	
	//读取文件数量
	sprintf(com,"sudo grep -o '.slc' %s/file.cfg | wc -l", print_cfg_path);
	fp = popen(com, "r");
	memset(str, '\0', sizeof(str));
	fgets(str, sizeof(str), fp);
	file_num = atoi(str);//将字符数字转化成数

	// printf("file_num：%ld\r\n", file_num);
	pclose(fp);	
	
}


//用自定义函数Mystrcmp的方法比较大小（模拟实现strcmp的作用）
int Mystrcmp(char*str1,char*str2)
{
	while(*str1!='\0'||*str2!='\0')//遍历两个字符串到'\0'，用||逻辑是为了防止两个字符串不一样长如str3和str4
	{
		if(*str1==*str2)
		{
		str1++;
		str2++;
		}
			else if(*str1>*str2)
			{
			return 1;
			}
				else 
				{
				return -1;
				}
			
	}
	return 0;//循环结束意味着两个字符串的数值大小相同
}

static void *thread_1(void *args)//其他
{
	/*
	其他
	*/
	FILE *fp;
	char str[256]={0};
	// char buf[1050];//文件深度默认为4

	static char u_num=0;
	static char buf_usb[4][10];
	static char buf_usb_temp[10];

	while(1)//检查U盘挂载情况
	{		
		usleep(500000);
		// serialPrintf(fd_serial,"U check\r\n");
		
		fp = popen("sudo find /dev/ -name \"sd*[1-9]*\" |wc -l", "r");//查看sd*[1-inf]文件的数量
		fgets(str, sizeof(str), fp);
		u_num=atoi(str);
		// serialPrintf(fd_serial,"U NUM=%d\r\n",u_num);
		pclose(fp);

		if(u_num!=usb_flag)
		{
			if((print_flag==1)|(print_flag==2))
			{
				if(u_num<usb_flag)
				{
					file_change_flag=0;//允许顺序去执行usb_read中的内容
					// printf("U盘状态发生变化\r\n");
				}
			}

			if(file_change_flag==0)//usb_read
			{
				strcpy(buf_usb_temp,buf_usb[0]);
				//更新挂载
				//卸载
				fp = popen("sudo umount -l /media/pi/usb0 >/dev/null 2>&1", "r");
				pclose(fp);

				fp = popen("sudo umount -l /media/pi/usb1 >/dev/null 2>&1", "r");
				pclose(fp);
				// printf("已执行卸载命令\r\n");

				if(print_flag==0)
				{
					if(u_num!=0)
					{
						// serialPrintf(fd_serial,"begin mount\r\n");
						fp = popen("sudo find /dev/ -name \"sd*[1-9]*\"", "r");//查看sd*[1-inf]文件
						for(size_t i = 0;i<u_num;i++)
						{
							fgets(str, sizeof(str)-1, fp);
							
							if((str[strlen(str)-1]=='\r')|(str[strlen(str)-1]=='\n'))
							str[strlen(str)-1]='\0';
							if((str[strlen(str)-2]=='\r')|(str[strlen(str)-2]=='\n'))
							str[strlen(str)-2]='\0';
							if((str[strlen(str)-3]=='\r')|(str[strlen(str)-3]=='\n'))
							str[strlen(str)-3]='\0';
						
							// serialPrintf(fd_serial,"U: %s\r\n",str);
							memset(buf_usb[i], '\0', sizeof(buf_usb[i]));
							strcpy(buf_usb[i],str);
						}
						pclose(fp);
						
						
						for(size_t i = 0;i<u_num;i++)
						{
							sprintf(str,"sudo mount -o iocharset=utf8 %s /media/pi/usb%d >/dev/null 2>&1",buf_usb[i],i);
							//挂载
							fp=popen(str, "r");
							pclose(fp);
							// printf("%s\r\n",str);
						}
						
						// printf("挂载的u盘数为:%d\r\n",u_num);
						
						// printf("执行的挂载命令为:%s\r\n",str);
						// printf("已执行挂载命令\r\n");
					}
				}
				
				if(print_flag==0)
				{
					page_menu=1;
					//清空缓冲区
					menu_show_clear();						
					read_print_file();
					file_show();
				}
			}
			
			usb_flag=u_num;
		}

		static int print_flag_last=0;
		if(print_flag_last!=print_flag)
		{
			print_flag_last=print_flag;
			if(print_flag==0)
			{
				fp = popen("omxplayer -o local /home/pi/wiringpi/0001.wav", "r");
				pclose(fp);
			}
		}		

	}
	return NULL;
}

int file_main(void)
{
	int ret=0;
	pthread_t id1;
	
	ret=pthread_create(&id1,NULL,thread_1,NULL);//开启线程	
	if(ret)
	{
		printf("create pthread error!\n");
		return -1; 
	}

	return 0;
}




