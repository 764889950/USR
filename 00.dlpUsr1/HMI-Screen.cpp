#include "wiringSerial.h"
#include "parameter.h"
#include "DEV_Config_2.h"//其他设备初始化
#include "strstr.h"
#include "file.h"
#include "marlinHost.h"
#include "SLC_Read.h"
#include "data_global.h"

#include "HMI-Screen.h"



char RevBuf1_last[Data_SIZE]={0};      //数据接收缓冲区
char reboot_flag=0;


long page_menu=1;//当前显示页码
// char print_led_flag=0;

void get_print_parameters(void)
{
	printf("zoom_x=%lf\r\n", zoom_x);
	printf("zoom_y=%lf\r\n", zoom_y);
	printf("buttom_mid=%ld\r\n", buttom_mid);
	printf("t_buttom_mid=%ld\r\n", t_buttom_mid);
	printf("Pedal=%ld\r\n", Pedal);
	// printf("t_buttom=%ld\r\n", t_buttom);
	// printf("t_mid=%ld\r\n", t_mid);
	printf("t_other=%ld\r\n", t_other);
	printf("LED_intensity=%ld\r\n", LED_intensity);
	
	printf("print_back_level_z=%ld\r\n", print_back_level_z);
	printf("print_back_level_Pedal=%ld\r\n", print_back_level_Pedal);
	printf("z_rpo=%lf\r\n", z_rpo);
	printf("Pedal_rpo=%lf\r\n", Pedal_rpo);
	printf("dir_z=%ld\r\n", dir_z);
	printf("dir_Pedal=%ld\r\n", dir_Pedal);
	printf("speed_z=%ld\r\n", speed_z);
	printf("speed_Pedal=%ld\r\n", speed_Pedal);
	
	printf("led_choose=%ld\r\n", led_choose);
	printf("print_work_level_z=%lf\r\n", print_work_level_z);
	printf("print_work_level_Pedal=%lf\r\n", print_work_level_Pedal);
	printf("print_style=%ld\r\n", print_style);
	// printf("LED_intensity_per=%ld\r\n", LED_intensity_per);	
	printf("t_pull_lift=%ld\r\n", t_pull_lift);
	printf("buttom=%ld\r\n", buttom);
}


void parameters_read_REV(void)
{
	//若接收到以下数据，更新当前分类别接收到的变量/////////////////////////////////////////////////////////////////////
	if(my_strstr(RevBuf1,"zoom_x=",Data_SIZE)!=NULL)
	{
		zoom_x=((*(my_strstr(RevBuf1,"zoom_x=",Data_SIZE)+7))<<24|(*(my_strstr(RevBuf1,"zoom_x=",Data_SIZE)+8))<<16|(*(my_strstr(RevBuf1,"zoom_x=",Data_SIZE)+9))<<8|(*(my_strstr(RevBuf1,"zoom_x=",Data_SIZE)+10)))/1000.0;
	}
	
	if(my_strstr(RevBuf1,"zoom_y=",Data_SIZE)!=NULL)
	{
		zoom_y=((*(my_strstr(RevBuf1,"zoom_y=",Data_SIZE)+7))<<24|(*(my_strstr(RevBuf1,"zoom_y=",Data_SIZE)+8))<<16|(*(my_strstr(RevBuf1,"zoom_y=",Data_SIZE)+9))<<8|(*(my_strstr(RevBuf1,"zoom_y=",Data_SIZE)+10)))/1000.0;
	}

	if(my_strstr(RevBuf1,"buttom=",Data_SIZE)!=NULL)
	{
		buttom=(*(my_strstr(RevBuf1,"buttom=",Data_SIZE)+7))<<24|(*(my_strstr(RevBuf1,"buttom=",Data_SIZE)+8))<<16|(*(my_strstr(RevBuf1,"buttom=",Data_SIZE)+9))<<8|(*(my_strstr(RevBuf1,"buttom=",Data_SIZE)+10));
	}
	
	if(my_strstr(RevBuf1,"buttom_mid=",Data_SIZE)!=NULL)
	{
		buttom_mid=(*(my_strstr(RevBuf1,"buttom_mid=",Data_SIZE)+11))<<24|(*(my_strstr(RevBuf1,"buttom_mid=",Data_SIZE)+12))<<16|(*(my_strstr(RevBuf1,"buttom_mid=",Data_SIZE)+13))<<8|(*(my_strstr(RevBuf1,"buttom_mid=",Data_SIZE)+14));
	}
	
	if(my_strstr(RevBuf1,"t_buttom_mid=",Data_SIZE)!=NULL)
	{
		t_buttom_mid=((*(my_strstr(RevBuf1,"t_buttom_mid=",Data_SIZE)+13))<<24|(*(my_strstr(RevBuf1,"t_buttom_mid=",Data_SIZE)+14))<<16|(*(my_strstr(RevBuf1,"t_buttom_mid=",Data_SIZE)+15))<<8|(*(my_strstr(RevBuf1,"t_buttom_mid=",Data_SIZE)+16)))*10;
	}
	
	if(my_strstr(RevBuf1,"Pedal=",Data_SIZE)!=NULL)
	{
		Pedal=(*(my_strstr(RevBuf1,"Pedal=",Data_SIZE)+6))<<24|(*(my_strstr(RevBuf1,"Pedal=",Data_SIZE)+7))<<16|(*(my_strstr(RevBuf1,"Pedal=",Data_SIZE)+8))<<8|(*(my_strstr(RevBuf1,"Pedal=",Data_SIZE)+9));
	}
	
	if(my_strstr(RevBuf1,"t_other=",Data_SIZE)!=NULL)
	{
		t_other=((*(my_strstr(RevBuf1,"t_other=",Data_SIZE)+8))<<24|(*(my_strstr(RevBuf1,"t_other=",Data_SIZE)+9))<<16|(*(my_strstr(RevBuf1,"t_other=",Data_SIZE)+10))<<8|(*(my_strstr(RevBuf1,"t_other=",Data_SIZE)+11)))*10;
	}
	
	
	if(my_strstr(RevBuf1,"print_back_level_z=",Data_SIZE)!=NULL)
	{
		print_back_level_z=(*(my_strstr(RevBuf1,"print_back_level_z=",Data_SIZE)+19))<<24|(*(my_strstr(RevBuf1,"print_back_level_z=",Data_SIZE)+20))<<16|(*(my_strstr(RevBuf1,"print_back_level_z=",Data_SIZE)+21))<<8|(*(my_strstr(RevBuf1,"print_back_level_z=",Data_SIZE)+22));
	}
	
	if(my_strstr(RevBuf1,"print_back_level_Pedal=",Data_SIZE)!=NULL)
	{
		print_back_level_Pedal=(*(my_strstr(RevBuf1,"print_back_level_Pedal=",Data_SIZE)+23))<<24|(*(my_strstr(RevBuf1,"print_back_level_Pedal=",Data_SIZE)+24))<<16|(*(my_strstr(RevBuf1,"print_back_level_Pedal=",Data_SIZE)+25))<<8|(*(my_strstr(RevBuf1,"print_back_level_Pedal=",Data_SIZE)+26));
	}
	

	if(my_strstr(RevBuf1,"t_pull_lift=",Data_SIZE)!=NULL)
	{
		t_pull_lift=((*(my_strstr(RevBuf1,"t_pull_lift=",Data_SIZE)+12))<<24|(*(my_strstr(RevBuf1,"t_pull_lift=",Data_SIZE)+13))<<16|(*(my_strstr(RevBuf1,"t_pull_lift=",Data_SIZE)+14))<<8|(*(my_strstr(RevBuf1,"t_pull_lift=",Data_SIZE)+15)))*10;
	}	
	
	if(my_strstr(RevBuf1,"LED_intensity=",Data_SIZE)!=NULL)
	{
		LED_intensity=(*(my_strstr(RevBuf1,"LED_intensity=",Data_SIZE)+14))<<24|(*(my_strstr(RevBuf1,"LED_intensity=",Data_SIZE)+15))<<16|(*(my_strstr(RevBuf1,"LED_intensity=",Data_SIZE)+16))<<8|(*(my_strstr(RevBuf1,"LED_intensity=",Data_SIZE)+17));
	}
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	
	if(my_strstr(RevBuf1,"z_rpo=",Data_SIZE)!=NULL)
	{
		z_rpo=((*(my_strstr(RevBuf1,"z_rpo=",Data_SIZE)+6))<<24|(*(my_strstr(RevBuf1,"z_rpo=",Data_SIZE)+7))<<16|(*(my_strstr(RevBuf1,"z_rpo=",Data_SIZE)+8))<<8|(*(my_strstr(RevBuf1,"z_rpo=",Data_SIZE)+9)))/10000.0;
	}
	
	if(my_strstr(RevBuf1,"Pedal_rpo=",Data_SIZE)!=NULL)
	{
		Pedal_rpo=((*(my_strstr(RevBuf1,"Pedal_rpo=",Data_SIZE)+10))<<24|(*(my_strstr(RevBuf1,"Pedal_rpo=",Data_SIZE)+11))<<16|(*(my_strstr(RevBuf1,"Pedal_rpo=",Data_SIZE)+12))<<8|(*(my_strstr(RevBuf1,"Pedal_rpo=",Data_SIZE)+13)))/10000.0;
	}
	
	if(my_strstr(RevBuf1,"dir_z=",Data_SIZE)!=NULL)
	{
		dir_z=(*(my_strstr(RevBuf1,"dir_z=",Data_SIZE)+6))<<24|(*(my_strstr(RevBuf1,"dir_z=",Data_SIZE)+7))<<16|(*(my_strstr(RevBuf1,"dir_z=",Data_SIZE)+8))<<8|(*(my_strstr(RevBuf1,"dir_z=",Data_SIZE)+9));
	}
	
	if(my_strstr(RevBuf1,"dir_Pedal=",Data_SIZE)!=NULL)
	{
		dir_Pedal=(*(my_strstr(RevBuf1,"dir_Pedal=",Data_SIZE)+10))<<24|(*(my_strstr(RevBuf1,"dir_Pedal=",Data_SIZE)+11))<<16|(*(my_strstr(RevBuf1,"dir_Pedal=",Data_SIZE)+12))<<8|(*(my_strstr(RevBuf1,"dir_Pedal=",Data_SIZE)+13));
	}
	
	if(my_strstr(RevBuf1,"speed_z=",Data_SIZE)!=NULL)
	{
		speed_z=(*(my_strstr(RevBuf1,"speed_z=",Data_SIZE)+8))<<24|(*(my_strstr(RevBuf1,"speed_z=",Data_SIZE)+9))<<16|(*(my_strstr(RevBuf1,"speed_z=",Data_SIZE)+10))<<8|(*(my_strstr(RevBuf1,"speed_z=",Data_SIZE)+11));
	}
	
	if(my_strstr(RevBuf1,"speed_Pedal=",Data_SIZE)!=NULL)
	{
		speed_Pedal=(*(my_strstr(RevBuf1,"speed_Pedal=",Data_SIZE)+12))<<24|(*(my_strstr(RevBuf1,"speed_Pedal=",Data_SIZE)+13))<<16|(*(my_strstr(RevBuf1,"speed_Pedal=",Data_SIZE)+14))<<8|(*(my_strstr(RevBuf1,"speed_Pedal=",Data_SIZE)+15));
	}
	
	if(my_strstr(RevBuf1,"led_choose=",Data_SIZE)!=NULL)
	{
		led_choose=(*(my_strstr(RevBuf1,"led_choose=",Data_SIZE)+11))<<24|(*(my_strstr(RevBuf1,"led_choose=",Data_SIZE)+12))<<16|(*(my_strstr(RevBuf1,"led_choose=",Data_SIZE)+13))<<8|(*(my_strstr(RevBuf1,"led_choose=",Data_SIZE)+14));
	}
	
	if(my_strstr(RevBuf1,"single_map_method",Data_SIZE)!=NULL)//打印方式设置，
	{
		print_style=0;//0：单幅图；1：九宫格
	}	
	if(my_strstr(RevBuf1,"nine_palace_style",Data_SIZE)!=NULL)//打印方式设置
	{
		print_style=1;//0：单幅图；1：九宫格
	}
}


static void *thread_1(void *args)//其他
{
	char str_temp[1050];
	char str[1050];
	
	/*
	其他
	*/
	FILE *fp;
	char buf[256]={0};
	int com_list=0;//发送指令列表
	char com[20]={0};
	char get_set_parameters_flag=0;
	

	usb_flag=-1;//更新目录
	
	// com_list=10;
	while(1)
	{	
		if(com_list==7)
		{//管理员扩展参数指令
			com[0]=0x5A; com[1]=0xA5; com[2]=0x03; com[3]=0x80; com[4]=0x4F; com[5]=0x07;
			hex_send(com,6);
			// printf("hello7\r\n");
			usleep(1000000);
		}	
		
		if(com_list==8)
		{//超级管理员基本配置参数指令
			com[0]=0x5A; com[1]=0xA5; com[2]=0x03; com[3]=0x80; com[4]=0x4F; com[5]=0x08;
			hex_send(com,6);
			// printf("hello8\r\n");
			usleep(1000000);
		}
		
		
		if(com_list==9)
		{//超级管理员扩展配置参数指令
			com[0]=0x5A; com[1]=0xA5; com[2]=0x03; com[3]=0x80; com[4]=0x4F; com[5]=0x09;
			hex_send(com,6);
			// printf("hello9\r\n");
			usleep(1000000);
		}			

		if(com_list==10)
		{//重发参数模型参数指令：5AA5 0B 82 00d4 5AA5 0004 0005 0258
			com[0]=0x5A;com[1]=0xA5;com[2]=0x0B;com[3]=0x82;com[4]=0x00;com[5]=0xD4;com[6]=0x5A;com[7]=0xA5;com[8]=0x00;com[9]=0x04;com[10]=0x00;com[11]=0x05;com[12]=0x02;com[13]=0x58;
			hex_send(com,14);
			printf("get_cfg\r\n");
			usleep(1000000);
		}
		
		static long usb_flag_last=!usb_flag;
		if(usb_flag!=usb_flag_last)
		{			
			page_menu=1;
			usb_flag_last=usb_flag;
		}

		if(uart1_sign == 1) //从串口1接收到数据
		{
			// printf("RevBuf1:%s\r\n",RevBuf1);
			// 以附加方式打开可读/写的文件, 如果没有此文件则会进行创建，然后以附加方式打开可读/写的文件
			// fps = fopen("/home/pi/wiringpi/log_2.txt","a+");
			// sprintf(srts,"%s",RevBuf1);
			// 将追加内容写入文件指针当前的位置
			//fputs(srts, fps);
				
			// 最后不要忘了,关闭打开的文件~~~
			// fclose(fps);		

			
			if (my_strstr(RevBuf1,"reboot",Data_SIZE)!=NULL)//接收到重启信息
			{
				//切换到初始页界面
				page_init_switch();
				// 重启
				reboot();
				// printf("接收到重启命令\r\n");
			
			}
			
			if (my_strstr(RevBuf1,"home_page",Data_SIZE)!=NULL)//接收到重启信息
			{
				// home_page=1;
			}
			
			
			if (my_strstr(RevBuf1,"save_model",Data_SIZE)!=NULL)//
			{
				//查看文件夹大小/////////////////////////////////////
				fp=popen("sudo du -s /media/pi/temp_model", "r");
				while(fgets(buf, sizeof(buf), fp) != NULL)
				{
					if(buf[strlen(buf)-1] == '\n')
					{
						buf[strlen(buf)-1] = '\0';
					}
				}
				
				static long folder_size=0;
				folder_size=atoi(buf);
				pclose(fp);
				//查看文件夹大小/////////////////////////////////////
				// printf("文件夹大小为：%ld\r\n",folder_size);

				if(folder_size<(1024*1024))//如果文件大小在范围内，则进行保存
				{
					if(!(access(print_path,0)<0))
					{
						// printf("要保存的文件路径为:%s\r\n",print_path);
						// printf("要保存的文件名为:%s\r\n",file_name_UTF_8);
						//复制U盘文件到指定中间目录下
						sprintf(str,"sudo cp %s /media/pi/temp_model/local:%s",print_path,file_name_UTF_8);
						fp=popen(str,"r");
						pclose(fp);
						
						//查看文件目录下是否有该文件
						static long save_file_num=0;
						sprintf(str,"sudo find /media/pi/temp_model -name \"local:%s\" |wc -l",file_name_UTF_8);				
						fp=popen(str,"r");
						while(fgets(buf, sizeof(buf), fp) != NULL)
						{
							if(buf[strlen(buf)-1] == '\n')
							{
								buf[strlen(buf)-1] = '\0';
							}
						}					
						save_file_num=atoi(buf);//save_file_num为1表示有该文件在该目录下
						pclose(fp);
						
						if(save_file_num)
						{
							//保存成功，跳转到保存成功界面
							// printf("保存模型成功,找到文件数量为:%ld\r\n",save_file_num);
							page_model_save_succeed_switch();
						}
						else
						{
							//切换到模型保存失败页
							page_model_save_fail_switch();
						}
					}
					else
					{
						//切换到模型保存失败页
						page_model_save_fail_switch();
					}
				}
			}
			
			// printf("RevBuf1=%s\r\n",RevBuf1);
			
			if (my_strstr(RevBuf1,"delete_model",Data_SIZE)!=NULL)
			{			
				//删除模型
				if((print_flag==0)&&(!(access(print_path,0)<0)))//进入删除模型
				{
					sprintf(str_temp,"sudo rm -rf %s",print_path);
					// sprintf(str_temp,"sudo rm -rf %s","/media/pi/usb1/鞋模新修改/莆田鞋底模带底板.slc");
					//去掉\r\n
					if((str_temp[strlen(str_temp)-1] == 0x0a)|(str_temp[strlen(str_temp)-1] == 0x0d))
						str_temp[strlen(str_temp)-1]='\0';
					if((str_temp[strlen(str_temp)-2] == 0x0a)|(str_temp[strlen(str_temp)-2] == 0x0d))
						str_temp[strlen(str_temp)-2]='\0';
					if((str_temp[strlen(str_temp)-3] == 0x0a)|(str_temp[strlen(str_temp)-3] == 0x0d))
						str_temp[strlen(str_temp)-3]='\0';					
					
					fp=popen(str_temp, "r");
					// printf("删除模型成功\r\n");
					pclose(fp);
					// printf("%s\r\n",str_temp);
					
					//清空缓冲区
					menu_show_clear();	
					//切换到模型页 
					page_model_switch();
					usb_flag=-1;//更新目录
					file_change_flag=0;
				}
				
				else
				{
					//界面跳转 模型删除失败
					page_model_delet_fail_switch();
				}
			}
			
			////////////////////////////////////////////////////////////////////////////////////
			if(my_strstr(RevBuf1,"page_set_z_up",Data_SIZE)!=NULL)//运动控制
			{
				move_z=-1;
				page_set_dis=((*(my_strstr(RevBuf1,"page_set_dis=",Data_SIZE)+13))<<24|(*(my_strstr(RevBuf1,"page_set_dis=",Data_SIZE)+14))<<16|(*(my_strstr(RevBuf1,"page_set_dis=",Data_SIZE)+15))<<8|(*(my_strstr(RevBuf1,"page_set_dis=",Data_SIZE)+16)))/100.0;
				// printf("page_set_dis=%lf\r\n", page_set_dis);
			}	
			if(my_strstr(RevBuf1,"page_set_z_down",Data_SIZE)!=NULL)//运动控制
			{
				move_z=1;
				page_set_dis=((*(my_strstr(RevBuf1,"page_set_dis=",Data_SIZE)+13))<<24|(*(my_strstr(RevBuf1,"page_set_dis=",Data_SIZE)+14))<<16|(*(my_strstr(RevBuf1,"page_set_dis=",Data_SIZE)+15))<<8|(*(my_strstr(RevBuf1,"page_set_dis=",Data_SIZE)+16)))/100.0;
				// printf("page_set_dis=%lf\r\n", page_set_dis);
			}
			
			
			
			if(my_strstr(RevBuf1,"page_z_go_o",Data_SIZE)!=NULL)//运动控制
			{
				work_point_flag_z=1;
			}
			if(my_strstr(RevBuf1,"page_p_go_o",Data_SIZE)!=NULL)//运动控制
			{
				work_point_flag_Pedal=1;
			}
			
			
			if(my_strstr(RevBuf1,"page_set_o_stop",Data_SIZE)!=NULL)//急停
			{
				// printf("设置急停成功\r\n");
				motor_flag_stop=1;	
			}
			
			if((my_strstr(RevBuf1,"page_set_o",Data_SIZE)!=NULL)&&(my_strstr(RevBuf1,"page_set_o_begin",Data_SIZE)==NULL)&&(my_strstr(RevBuf1,"page_set_o_end",Data_SIZE)==NULL)&&(my_strstr(RevBuf1,"page_set_o_stop",Data_SIZE)==NULL))//保存工作原点
			{
				print_work_level_z=print_level_z;
				print_work_level_Pedal=print_level_Pedal;
				printcfg_write();//保存公共配置参数
			}

			if((my_strstr(RevBuf1,"single_map_method",Data_SIZE)!=NULL)||(my_strstr(RevBuf1,"nine_palace_style",Data_SIZE)!=NULL))//保存工作原点
			{
				com_list=7;
	
				if(my_strstr(RevBuf1_last,RevBuf1,Data_SIZE)!=NULL)
				{
					com_list=0;
					// 跳转到打印加工页
					parameters_read_REV();//获取打印参数
					printcfg_write();//保存公共配置参数
					// menu_show_clear();
					// menu_show_clear();
					// file_show();
					// file_show();
					page_admin_config_switch();//切换到管理员配置页面
					// get_print_parameters();
					sleep(1);
					file_show();
					file_show();
					file_show();
					file_show();
					file_show();
				}
			}			

			if (my_strstr(RevBuf1,"z_rpo=",Data_SIZE)!=NULL)//超级管理员基本配置界面
			{
				com_list=8;
				if(my_strstr(RevBuf1_last,RevBuf1,Data_SIZE)!=NULL)
				{
					com_list=0;
					// printf("RevBuf_REC_z_rpo\r\n");
					parameters_read_REV();//获取打印参数
					printcfg_write();//保存公共配置参数
					//切换到超级管理员配置页面
					page_supper_admin_config_switch();
				}
			}

			if (my_strstr(RevBuf1,"led_choose=",Data_SIZE)!=NULL)//超级管理员基本配置界面
			{
				com_list=9;
				if(my_strstr(RevBuf1_last,RevBuf1,Data_SIZE)!=NULL)
				{
					com_list=0;
					parameters_read_REV();//获取打印参数
					printcfg_write();//保存公共配置参数
					//切换到超级管理员配置页面
					page_supper_admin_config_switch();
				}
			}
			
			if (my_strstr(RevBuf1,"page_z_go_o",Data_SIZE)!=NULL)//其他控制
			{
				work_point_flag_z=1;
			}

			
			if(my_strstr(RevBuf1,"page_file_show_end",Data_SIZE)!=NULL)
			{			
				print_flag=0;
			//添加U盘和文件显示更新禁止标识 待加
			}

			if (my_strstr(RevBuf1,"PRINT_STOP_RETURN",Data_SIZE)!=NULL)//停止并返回
			{			
				print_flag=0;            //结束打印

				file_continue_flag = 0;    //使下次处于一个阻态
				com_list=0;
				file_change_flag=0;//等于0允许u盘读取内容发生变化
				//切换到模型页 5A A5 04 80 03 0004
				page_model_switch();
				get_set_parameters_flag=0;
				
				file_change_flag=0;//等于0允许u盘读取内容发生变化
				usb_flag=-1;
				// printf("结束打印\r\n");
				// printf("print_flag=%ld\r\n",print_flag);
				//清空缓冲区
				menu_show_clear();
			}

			if((my_strstr(RevBuf1,"page_file_show",Data_SIZE)!=NULL)&(my_strstr(RevBuf1,"page_file_show_end",Data_SIZE)==NULL))
			{
				page_menu=1;
				// printf("显示第一页文件\r\n");
				print_flag=0;//关闭打印
				file_continue_flag = 0;//使下次处于一个阻态
			}

			if (my_strstr(RevBuf1,"page_menu_add",Data_SIZE)!=NULL)
			{
				if(page_menu<1)
					page_menu=1;
				page_menu++;
				if(page_menu>(file_num/8+((file_num%8)>0?1:0)))
					page_menu=file_num/8+((file_num%8)>0?1:0);

				//清空缓冲区
				menu_show_clear();

				// 添加显示条件
				if(file_num==0)
				{
					page_menu=1;
				}
				else
				{
					if(file_num>0)
					{
						file_show();
					}
				}
				// printf("page_menu=\"%.0f\"\r\n",page_menu*1.0);
			}
			if (my_strstr(RevBuf1,"page_menu_sub",Data_SIZE)!=NULL)
			{
				page_menu--;
				if(page_menu<1)
					page_menu=1;
				
				//清空缓冲区
				menu_show_clear();
				
				//添加显示条件
				if(file_num==0)
				{
					page_menu=1;
				}
				else
				{
					if(file_num>0)
					{
						file_show();
					}
				}
				
				// printf("page_menu=\"%.0f\"\r\n",page_menu*1.0);
			}
			
			if (my_strstr(RevBuf1,"file_first",Data_SIZE)!=NULL)
			{
				get_set_parameters_flag=1;
				if(8*(page_menu-1)+1<=file_num)//当前文件目录位号存在，允许返回跳转
				{
					printf("当前文件目录位号存在，允许返回跳转\r\n");
					file=8*(page_menu-1)+1;
					file_get();//获取打印文件print_path路径名称
					printf("打印文件为：%s\r\n",print_path);
					ReadSLC_inf(print_path);//获取打印文件信息
					time_calculate();					
					// 跳转到打印加工加载页
					page_print_load_switch();
					// page_print_switch();
					printf("退出\r\n");

				}
			}	

			if (my_strstr(RevBuf1,"file_second",Data_SIZE)!=NULL)
			{
				get_set_parameters_flag=1;
				if(8*(page_menu-1)+1<=file_num)//当前文件目录位号存在，允许返回跳转
				{
					file=8*(page_menu-1)+2;
					file_get();//获取打印文件信息
					ReadSLC_inf(print_path);
					time_calculate();
					
					// 跳转到打印加工加载页
					page_print_load_switch();
					// page_print_switch();
				}
			}

			if (my_strstr(RevBuf1,"file_third",Data_SIZE)!=NULL)
			{
				get_set_parameters_flag=1;
				if(8*(page_menu-1)+1<=file_num)//当前文件目录位号存在，允许返回跳转
				{
					file=8*(page_menu-1)+3;
					file_get();//获取打印文件信息
					ReadSLC_inf(print_path);
					time_calculate();					
					
					// 跳转到打印加工加载页
					page_print_load_switch();
					// page_print_switch();
				}
			}

		
			if (my_strstr(RevBuf1,"file_fourth",Data_SIZE)!=NULL)
			{
				get_set_parameters_flag=1;
				if(8*(page_menu-1)+1<=file_num)//当前文件目录位号存在，允许返回跳转
				{
					file=8*(page_menu-1)+4;
					file_get();//获取打印文件信息
					ReadSLC_inf(print_path);
					time_calculate();

					// 跳转到打印加工加载页
					page_print_load_switch();
					// page_print_switch();
				}
			}

			if (my_strstr(RevBuf1,"file_five",Data_SIZE)!=NULL)
			{
				get_set_parameters_flag=1;
				if(8*(page_menu-1)+1<=file_num)//当前文件目录位号存在，允许返回跳转
				{
					file=8*(page_menu-1)+5;
					file_get();//获取打印文件信息
					ReadSLC_inf(print_path);
					time_calculate();					
					
					// 跳转到打印加工加载页
					page_print_load_switch();
					// page_print_switch();
				}
			}

			if (my_strstr(RevBuf1,"file_six",Data_SIZE)!=NULL)
			{
				get_set_parameters_flag=1;
				if(8*(page_menu-1)+1<=file_num)//当前文件目录位号存在，允许返回跳转
				{
					file=8*(page_menu-1)+6;
					file_get();//获取打印文件信息
					ReadSLC_inf(print_path);
					time_calculate();					
					
					// 跳转到打印加工加载页
					page_print_load_switch();
					// page_print_switch();
				}
			}

			if (my_strstr(RevBuf1,"file_seven",Data_SIZE)!=NULL)
			{
				get_set_parameters_flag=1;
				if(8*(page_menu-1)+1<=file_num)//当前文件目录位号存在，允许返回跳转
				{
					file=8*(page_menu-1)+7;
					file_get();//获取打印文件信息
					ReadSLC_inf(print_path);
					time_calculate();	
					
					// 跳转到打印加工加载页
					page_print_load_switch();
					// page_print_switch();
				}
			}
			
			
			if (my_strstr(RevBuf1,"file_eight",Data_SIZE)!=NULL)
			{
				get_set_parameters_flag=1;
				if(8*(page_menu-1)+1<=file_num)//当前文件目录位号存在，允许返回跳转
				{
					file=8*(page_menu-1)+8;
					file_get();//获取打印文件信息
					ReadSLC_inf(print_path);
					time_calculate();
					
					// 跳转到打印加工加载页
					page_print_load_switch();
					// page_print_switch();
				}
			}
			
			// get_set_parameters_flag=1; 为打印参数包标识,针对不同模型
			if(get_set_parameters_flag==1)
			{
				com_list=10;

				if (my_strstr(RevBuf1,"print_cfg_1",Data_SIZE)!=NULL)//设置选择打印参数指令
				{
					if(my_strstr(RevBuf1_last,RevBuf1,Data_SIZE)!=NULL)
					{
						com_list=0;
						printf("打印参数更新\r\n");
						get_set_parameters_flag=2;
					}
				}
				
			}	
			
		
			if ((my_strstr(RevBuf1,"print_cfg",Data_SIZE)!=NULL)&&(my_strstr(RevBuf1,"zoom_x",Data_SIZE)!=NULL))//设置选择打印参数指令
			{//在打印加工页获取打印参数	
				// printf("RevBuf1:%s\r\n",RevBuf1);
				if(get_set_parameters_flag==0)//管理员配置中允许界面跳转
				{
					parameters_read_REV();//获取打印参数
					// get_print_parameters();
					LED_P_Set(LED_intensity,led_choose);
					if(reboot_flag==0)
					{
						page_admin_config_switch();//切换到管理员配置页面 
					}
					reboot_flag=0;
				}
					
				if(get_set_parameters_flag==2)
				{
					if(my_strstr(RevBuf1_last,RevBuf1,Data_SIZE)!=NULL)
					{
						LED=0;//关闭LED灯
						com_list=0;
						// 获取打印参数
						parameters_read_REV();//获取打印参数		
						// get_print_parameters();			
						printf("获取当前打印参数成功！\r\n");
						
						
						//跳转到打印加工页
						page_print_switch();
						page_print_switch();
						page_print_switch();
						// page_print_switch();
						// page_print_switch();
						
						printf("跳转到打印加工页\r\n");
						
						//打印参数初始化
						sch=0;
						time_print=0;//开始打印，打印累计时间清零
						print_z=1;
						time_calculate();
						
						show_print_inf_init();//显示打印文件名等参数信息
						show_print_inf_init();
						show_print_inf_init();
						// show_print_inf_init();
						// show_print_inf_init();
					}
				}
			}

			static long print_flag_temp=0;
			if((my_strstr(RevBuf1,"PRINT_STOP",Data_SIZE)!=NULL)&(my_strstr(RevBuf1,"PRINT_STOP_RETURN",Data_SIZE)==NULL))//打印与暂停
			{
				print_flag_temp=print_flag;//将中间变量初始化为上一次print_flag值,该行要保留，重新进入打印时"按钮"会正常
				print_flag_temp++;
				if(print_flag_temp>2)
					print_flag_temp=1;

				print_flag=print_flag_temp;

				printf("print_flag=%ld\r\n",print_flag);
				// printf("print_flag==1?1:0=%d\r\n",print_flag==1?1:0);
			}


			/////////////////////////////////
			static char sss2=1;
			if (my_strstr(RevBuf1,"LED_ON_OFF",Data_SIZE)!=NULL)//光机LED电源控制  check
			{	
				// printf("LED_ON_OFF\r\n");
				if(LED==0)
				{
					curr();
					sss2=1;
				}				
				
				LED++;
				if(LED>1)
				{
					LED=0;
				}
				//更改LED按钮状态
				// printf("LED=%ld\r\n",LED);
			}

			if (my_strstr(RevBuf1,"PIC_switch",Data_SIZE)!=NULL)//光机LED电源控制  check
			{	
				// printf("LED_ON_OFF\r\n");
				sss2++;
				if(sss2>2)
					sss2=1;
				
				if(sss2==1)
				{
					curr();
				}
				if(sss2==2)
				{
					curr_picture();
				}
			}


			if (my_strstr(RevBuf1,"LED_check_begin",Data_SIZE)!=NULL)//进入到LED控制页面
			{
				LED=0;
				//更改LED按钮状态
			}
			////////////////////////////////////////////////////////////
			memcpy(RevBuf1_last,RevBuf1,Data_SIZE);
			memset(RevBuf1, '\0', sizeof(RevBuf1));
			uart1_sign = 0;
		}
		usleep(200000);

		static long str_temp[4]={0};
		static long Z_resend_flag=0;
		static double print_level_z_last=0;

		if(print_level_z!=print_level_z_last)
		{
			if(print_flag==0)
			{
				str_temp[3]=((unsigned long)((print_level_z*10)+0.5)>>24)&0xff;
				str_temp[2]=((unsigned long)((print_level_z*10)+0.5)>>16)&0xff;
				str_temp[1]=((unsigned long)((print_level_z*10)+0.5)>>8)&0xff;
				str_temp[0]=(unsigned long)((print_level_z*10)+0.5)&0xff;

				serialPutchar (fd_HMI, 0x5A);
				serialPutchar (fd_HMI, 0xA5);
				serialPutchar (fd_HMI, 0x07);
				serialPutchar (fd_HMI, 0x82);
				serialPutchar (fd_HMI, 0x00);
				serialPutchar (fd_HMI, 0x93);

				serialPutchar (fd_HMI, str_temp[3]);
				serialPutchar (fd_HMI, str_temp[2]);
				serialPutchar (fd_HMI, str_temp[1]);
				serialPutchar (fd_HMI, str_temp[0]);

				Z_resend_flag++;
				if(Z_resend_flag>5)
					Z_resend_flag=5;
				
				// printf("print_level_z=%lf\r\n",print_level_z);
			}
		}
		else
		{
			if(print_flag==0)
			{
				if(Z_resend_flag)//重发
				{
					Z_resend_flag--;
					str_temp[3]=((unsigned long)((print_level_z*10)+0.5)>>24)&0xff;
					str_temp[2]=((unsigned long)((print_level_z*10)+0.5)>>16)&0xff;
					str_temp[1]=((unsigned long)((print_level_z*10)+0.5)>>8)&0xff;
					str_temp[0]=(unsigned long)((print_level_z*10)+0.5)&0xff;

					serialPutchar (fd_HMI, 0x5A);
					serialPutchar (fd_HMI, 0xA5);
					serialPutchar (fd_HMI, 0x07);
					serialPutchar (fd_HMI, 0x82);
					serialPutchar (fd_HMI, 0x00);
					serialPutchar (fd_HMI, 0x93);

					serialPutchar (fd_HMI, str_temp[3]);
					serialPutchar (fd_HMI, str_temp[2]);
					serialPutchar (fd_HMI, str_temp[1]);
					serialPutchar (fd_HMI, str_temp[0]);
				}
			}
		}
		
		////////////////////////////////////////////
		static long time_print_last=0;
		if(time_print!=time_print_last)
		{
			// printf("打印时间更新,time_print=%ld\r\n",time_print);
			show_print_inf_print();
			time_print_last=time_print;
		}
		////////////////////////////////////////////
		
	}
	return NULL;
}



int screen_main(void)
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

