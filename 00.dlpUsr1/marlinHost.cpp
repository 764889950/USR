/*****************************************************************************
* | File        :   marlinHost.c
* | Author      :   
* | Function    :   Drive DRV8825
* | Info        :
*                	Marlin固件上位机
*

*----------------
* |	This version:   V1.0
* | Date        :   2018-10-13
* | Info        :   Basic version
*
******************************************************************************/
#include "wiringSerial.h"
#include "DEV_Config_2.h"
#include "parameter.h"		//print_cfg_path路径变量
#include "data_global.h"

#include "marlinHost.h"

using namespace std;


Node trade_set_RT;//用于命令的更新和读取
Node *oder_head = NULL;  // 头节点
Node *oder_last = NULL;  // 尾节点

//指令发送频率
#define time_fast 200000

long t=time_fast;//定义初始指令发送时间频率



/***********************************************************************************************
*函数名 ：delGoods
*函数功能描述 ：删除信息
*函数参数 ：order_no-编号  platform-平台类型
*函数返回值 ：无
*作者 ：
*函数创建日期 ：
*函数修改日期 ：
*修改人 ：
*修改原因 ：
*版本 ：
*历史版本 ：
***********************************************************************************************/
void delGoods(char order_no[50],char platform[10])
{
	Node *c, *pre;
	pre = c = oder_head;  // 从头结点开始

	bool isFind = false;

	while(c != NULL && c -> next != NULL)
	{
		c = c -> next;
		if ((strcmp(c->platform,platform) == 0) && (strcmp(c->order_no,order_no) == 0))
		{
			if (c -> next == NULL) // 说明是最后一个节点
			{
				oder_last = pre;
			}
			pre -> next = c-> next;
			free(c->goods);
			free(c); // 释放空间
			printf("\n\n\t此条信息删除成功...");
	
			isFind = true;
			//break;
		}
		pre = c;
	}

	if (!isFind)
	{
		printf("\n\t对不起，暂无此条信息...");
	}
}


// 初始化列表
void init(void)
{
	oder_head = oder_last = (nodeList)malloc(sizeof(Node)); // 为头结点分配空间
	trade_set_RT.pocket_Num = 1;
	oder_head -> goods = oder_last -> goods = (Goods*) malloc(sizeof(Goods)*trade_set_RT.pocket_Num);
	trade_set_RT.goods = (Goods*) malloc(sizeof(Goods)*trade_set_RT.pocket_Num);
	oder_last -> next = NULL;
	oder_head -> next = NULL;
	
	trade_set_RT.order_status=0;
}

// 添加信息
void addGood(Node trade_set_info)
{
	//struct goodsInfo good; // 声明结构体变量
	Node *node; // 创建一个新节点

	//printf("trade_set_info.pocket_Num:%d\n",trade_set_info.pocket_Num);
	node = (nodeList) malloc(sizeof(Node)); // 为新节点分配空间
	node->goods = (Goods*) malloc(sizeof(Goods)*trade_set_info.pocket_Num); // 为新节点分配空间
	strcpy(node->platform,trade_set_info.platform);
	strcpy(node->order_no,trade_set_info.order_no);
	node->pocket_Num=trade_set_info.pocket_Num;
	node->order_status=trade_set_info.order_status;
	*(node->goods)=*(trade_set_info.goods);

	//node -> data = goods;
	node -> next = NULL;
	oder_last -> next = node;
	oder_last = node;
	oder_last -> next = NULL;
	//LENGTH++;
	
	//printf("添加指令成功\n");
}

// 浏览头部数据状态
int view_head(void)
{
	Node *c;
	c = oder_head; // 从头结点开始
	//printf("\n\t*********************浏览头部信息*********************");
	//printf("\n");
	if(c != NULL && c -> next != NULL){} else return 0;
	while(c != NULL && c -> next != NULL)
	{
		c = c -> next;
		//printf("c->order_status:%d   ",c->order_status);
		//printf("c->goods->command:%s",c->goods->command);
		break;

	}
	return c->order_status;
}

// 浏览头部数据
void view_command_head(char com[])
{
	Node *c;
	c = oder_head; // 从头结点开始
	//printf("\n\t*********************浏览头部信息*********************");
	//printf("\n");
	if(c != NULL && c -> next != NULL){} else return;
	while(c != NULL && c -> next != NULL)
	{
		c = c -> next;
		//printf("c->order_status:%d   ",c->order_status);
		//printf("c->goods->command:%s",c->goods->command);
		break;

	}
	strcpy(com,c->goods->command);
	// return c->goods->command;
}

/***********************************************************************************************
*函数名 ：delHeadGoods
*函数功能描述 ：删除头部信息
*函数参数 ：无
*函数返回值 ：int类型数据，0表示数据已全部完成删除
*作者 ：
*函数创建日期 ：
*函数修改日期 ：
*修改人 ：
*修改原因 ：
*版本 ：
*历史版本 ：
***********************************************************************************************/
int delHeadGoods(void)
{
	Node *c, *pre;
	pre = c = oder_head;  // 从头结点开始

	
	bool isFind = false;

	char com[50];
	
	//printf("\n\t************************删除头部信息************************\n");

	if(c != NULL && c -> next != NULL)
	{
		view_command_head(com);
		printf("删除的信息为:%s\n",com);
		
		c = c -> next;
		
		if (c -> next == NULL) // 说明是最后一个节点
		{
			oder_last = pre;
		}
		pre -> next = c-> next;
		free(c->goods);
		free(c); // 释放空间
		printf("\t此条信息删除成功...\n\n\n");

		isFind = true;
		//break;
		pre = c;
		
	}
	
	return isFind;
}

static int times=0;//发送次数计数
static int flagFrequency=0;//忙等待次数
static int flagIgnore=0;//是否无视返回的“echo:busy: processing”标识

void send(char command[])
{
	serialPrintf (fd_Marlin, "%s\n", command);
	serialFlush(fd_Marlin);
	fflush (stdin);
	fflush (stdout);	
}

// 发送头部数据
int send_head(void)
{
	Node *c;
	c = oder_head; // 从头结点开始
	//printf("\n\t*********************浏览头部信息*********************");
	//printf("\n");
	if(c != NULL && c -> next != NULL){} else return -1;
	while(c != NULL && c -> next != NULL)
	{
		c = c -> next;
		unsigned int i=0;
		while(i<sizeof(c->goods->command))
		{
			if((c->goods->command[i]=='\r')|(c->goods->command[i]=='\n'))
			{
				c->goods->command[i]=0;
				c->goods->command[i+1]=0;
			}
			i++;
		}
		//printf("c->order_status:%d   ",c->order_status);
		// printf("c->goods->command:%s\n",c->goods->command);

		if(c->order_status==1)
		{
			times++;
			if(times>30)
			{
				times=0;
				delHeadGoods();
				return 0;
			}

		}
		else
		{
			times=0;
		}
		
		c->order_status=1;
		
		if(strstr(c->goods->command,"G28") != NULL)//单独指令时间延长
		{
			//printf("时间设置成slow\r\n");
			flagIgnore=0;//返回的“echo:busy: processing”有效，能根据此变更下次发送的指令的间隔
			flagFrequency=10;//延长上位机指令发送的间隔时间
		}
		
		if((strstr(c->goods->command,"G0") != NULL)|(strstr(c->goods->command,"G1") != NULL))//发送的是G0或G1指令，无视返回的“echo:busy: processing”
		{
			 flagIgnore=1;//无视返回的“echo:busy: processing”
		}		
		
		if((c->goods->command[0]==';')|(c->goods->command[0]=='\r')|(c->goods->command[0]=='\n')|(c->goods->command[0]=='\0'))//单独指令时间缩短
		{
			//printf("时间设置成fast\r\n");
			delHeadGoods();
			return 0;			
		}
		else
		{	
			times=0;
		}	
			
		send(c->goods->command);
		break;

	}
	return c->order_status;
}


//读取运动路径
void GM_Read(void)
{
	FILE *GMfp;
	char com[500];
	printf("打开G-M.gcode\r\n");
	sprintf(com,"%s/GMfile/G-M.gcode", print_cfg_path);
	GMfp=fopen(com,"r");
	while (!feof(GMfp)) 
	{ 
		fgets(com,512,GMfp);  //读取一行
		strcpy(trade_set_RT.goods->command,com);
		addGood(trade_set_RT);
		// printf("%s\n", com); //输出
	}
	fclose(GMfp);         //关闭文件
	printf("关闭G-M.gcode\r\n");	
	
	
}

//方法调用1：
void fun1(void)
{
	file_continue_flag = 1;		//允许SLC文件继续解析与投影
}

//方法调用2：
void fun2(void)
{
	while(file_continue_flag==1);	//continue_flag=0时，fun2()才退出
}

//方法调用3：
static long layer=0;//当前打印层
void fun3(void)
{
	if(print_flag == 1)//打印
	{
		layer++;
		flagFrequency=10;
		if(layer <= buttom_mid)
		{
			printf("第%ld层曝光时间为：%ld\r\n",layer,t_buttom_mid);
			delay(t_buttom_mid);//等待的曝光时间
		}

		if(layer > buttom_mid)
		{
			printf("第%ld层曝光时间为：%ld\r\n",layer,t_other);
			delay(t_other);//等待的曝光时间
		}
		flagFrequency=1;//改变指令发送频率
	}
}


/*运动策略添加
1、需要执行等待的指令（上条指令完成时或者当前指令完成时，才会有对于当前指令ok状态的返回）；
2、用于设定指令的执行时间；
3、设定此条指令完成后，到下一条的指令发送的时间间隔；
4、需要改变指令发送频率为(1/t)(当前最大发送频率)
*/
int t_delay=0;
int compareSelf(char com[500])
{
	if((strstr(com,"M114") != NULL)|(strstr(com,"G28") != NULL))
	{
		if(strstr(com,"M114") != NULL)//优先级高的先返回,包含于上一级if语句
		{
			t_delay = t_pull_lift;
			return 3;//等于3表示需要设定此条指令完成后，到下一条的指令发送的时间间隔,以上t_pull_lift代表间隔时间；
			//然后同时执行fun2()函数，当fun2()返回时，此if语句中的指令才算真正结束
		}
		return 1;//等于1表示执行的指令为等待指令
	}
	
	if(strstr(com,"M106") != NULL)
	{
		return 2;//等于2表示需要设定指令的执行时间，执行时间一般以秒计算，通过fun2()返回时间进行执行时间的设置
	}

	if(strstr(com,"M107") != NULL)
	{
		return 4;//等于4表示需要改变指令发送频率为(1/t)(当前最大发送频率)
		//指令为"M107"，然后同时完成fun1函数的内容
	}
	return 0;
}


//以下为运动控制
void home_z(void)
{
	send((char*)"G28 Z0");
	usleep(100000);
	send((char*)"G28 Z0");
	usleep(100000);
	send((char*)"G28 Z0");
	usleep(100000);
}

void workPoint_z(void)
{
	char srts[500]={0};
	sprintf(srts,"G0 Z%.3lf",print_work_level_z);//工作原点
	
	send(srts);

}


void juge_print(void)
{
	while((print_flag == 0)|(print_flag == 2))//打印暂停与继续
	{
		if(print_flag==0)
		{
			// printf("在打印函数中退出, print_flag=%ld\r\n",print_flag);
			file_continue_flag = 0;
			home_z();		//Z轴回零
			return;
		}
	}
}


void time_calculate(void)
{
	long time_print_res_temp=0;
	// printf("开始对剩余时间进行评估\r\n");
// 此处进行剩余时间预估
	if(layer<buttom_mid)
	{
		time_print_res_temp=t_buttom_mid*(buttom_mid-layer)/1000+t_other*layer_z/1000+(2*print_back_level_z/speed_z)*(layer_z-buttom_mid)+t_pull_lift*(layer_z-layer)/1000;
	}
	else
	{
		time_print_res_temp=t_other*(layer_z-layer)/1000+(2*print_back_level_z/speed_z)*(layer_z-layer)+t_pull_lift*(layer_z-layer)/1000;
	}
	
	if(time_print_res_temp>0)
	{
		time_print_res=time_print_res_temp;
	}
	else
	{
		time_print_res=0;
	}
	// printf("总层数为:layer_z=%ld\r\n",layer_z);
	// printf("剩余时间为:time_print_res=%ld s\r\n",time_print_res);
}


void print(void)
{
	// printf("层厚level_z%lf\r\n",level_z);
	layer=0;
	time_print=0;
	
	// 此处进行剩余时间预估
	time_calculate();

	//LED_P_Set(LED_intensity,led_choose);
	// printf("光机设置完成\r\n");
					
	file_continue_flag = 1;
	
	//读取运动路径
	GM_Read();
	
	while(1)
	{
		juge_print();
		if(print_flag==0)
			break;
		
		// 此处进行剩余时间预估
		time_calculate();
		
		if(send_head()==-1)
		{
			break;
		}
		for(int i=0;i<flagFrequency;i++)
		{
			usleep(t);
		}
	}
	printf("打印结束，删除剩余中的消息队列\n");
	
	while(delHeadGoods() != false);

	print_flag = 0;
	
	printf("打印结束!\n");
	return;

}


static void *thread_1(void *args)//其他
{
	init();//链表初始化
	home_z();

	while(1)
	{
		if(home_flag_z==1)
		{
			home_z();
		}


		if(work_point_flag_z==1)
		{
			workPoint_z();
		}
			
		if(print_flag==1)
		{
			//开始打印
			print();
			//发送窗口最后信息
		}


		if(move_z==1)
		{	
			// printf("Z轴向下运动\r\n");
// DrawLine(0,0,0,0,0,(long)(page_set_dis/z_rpo));
			// printf("page_set_dis/z_rpo=%ld\r\n",(long)(page_set_dis/z_rpo));
			move_z=0;
		}
		
		if(move_z==(long)(-1))
		{
			// printf("Z轴向上运动\r\n");
// DrawLine(0,0,0,0,0,(long)(-page_set_dis/z_rpo));
			// printf("-page_set_dis/z_rpo=%ld\r\n",(long)(-page_set_dis/z_rpo));
			move_z=0;

		}
		usleep(10000);
	}

	return NULL;
}


static void *thread_2(void *args)//其他
{
	while(1)
	{
		delay(1000);
		if(print_flag == 1)
		{
			// printf("已打印时间加1,time_print=%ld\r\n",time_print);
			time_print++;
		}
		else
		{
			//不做任何操作
		}
	}
	return NULL;
}


static void *thread_3(void *args)
{
	char com[500];
	long Line_Last=0; //上条指令行号
	long _Line_Last=0; //上条指令行号
	char next_flag=0; //是否删除标志,初始化值为0，允许下一条指令
	while(1)
	{
		if(uart3_sign==1)
		{
			DEBUG("RevBuf3:%s\n",RevBuf3);
			
			if(strstr(RevBuf3,"echo:busy: processing") != NULL)
			{
				printf("echo:busy: processing!\n");
				times=0;
				flagFrequency=10;//设置进入等待状态 20*time_fast
			}

			/* 带校验的命令行，e.g.行号错误时的接收顺序为 "Line Number is not Last Line Number+1, Last Line: 1\nResend: 2\nok\nok\n" */
			/* 带校验的命令行，e.g.校验值错误时接收顺序为 "Error:checksum mismatch, Last Line: 1\nResend: 2\nok" */
			//注意：以下（1）、（2）、（3）次序不能调换
			// (1)处理 "Line Number is not Last Line Number+1, Last Line: 1\n" or "Error:checksum mismatch, Last Line: 1\n"
			if (strstr(RevBuf3,"Error:Line Number is not Last Line Number+1") != NULL)
			{
				// DEBUG("行号错误\n");
				//提取行号
				sscanf(RevBuf3, "%*[(A-Z)|(a-z)|^:|^,|^+|^ ]%*d%*[(A-Z)|(a-z)|^:|^,|^ ]%ld%*[^*]", &Line_Last);
				// DEBUG("Line_Last:%ld\n",Line_Last);
				view_command_head(com);//判断N与Line_Last是否相同，相同则正常
				
				sscanf(com, "%*[(A-Z)|(a-z)|^:|^,|^+|^ ]%ld%*[(A-Z)|(a-z)|^:|^,|^ ]", &_Line_Last);
				DEBUG("当前发送的头指令行号为:%ld\n",_Line_Last);
			}

			if (strstr(RevBuf3,"Error:checksum mismatch") != NULL)
			{
				// DEBUG("校验值错误\n");
				next_flag=1;
			}
						
			// (2)处理 Resend
			if (strstr(RevBuf3,"Resend") != NULL)
			{
				
				view_command_head(com);
				// if(_Line_Last!=_Line_Last)
				// {
					// printf("Line Error\n");
				// }
				//特殊指令处理
				if((_Line_Last!=Line_Last)|(compareSelf(com) == 1)|(compareSelf(com) == 3))
				{
					next_flag=1;
				}
			}

			// (3)处理 ok\n
			if ((strstr(RevBuf3,"ok") != NULL)&(strstr(RevBuf3,"Error") == NULL)) {
				view_command_head(com);

				if (next_flag == 0)//用于即时停止delHeadGoods操作
				{
					flagFrequency=1;
					if(view_head())
					{
						if ((compareSelf(com) == 4)&(view_head() == 1))
						{
							fun1();
						}
						
						if ((compareSelf(com) == 3)&(view_head() == 1))//距下条指令的发送间隔时间为t_pull_lift 秒
						{
							flagFrequency=1;
							delay(t_delay);
							printf("等待光机投影完成\n");
							fun2();
							printf("光机投影完成\n");
						}
						if ((compareSelf(com) == 2)&(view_head() == 1))//距下条指令的发送间隔时间为t_pull_lift 秒
						{
							fun3();
						}

						DEBUG("已完成指令的发送,准备删除!\n");
						delHeadGoods();

					}
				}
				next_flag=0;
			}
			/****************************************************************************/


			//增加连接错误冗余，包括可能的USB重连
			if (strstr(RevBuf3,"kill"))
			{
				t=time_fast;
				flagFrequency=20;//使进入等待状态 20*time_fast
			}
			memset(RevBuf3,0,sizeof(RevBuf3));
			uart3_sign=0;	
		}
	}
}

static void *thread_4(void *args)
{
	char com[500];
	while(1)
	{
		usleep(200000);
		
		if(print_flag == 1)//打印
		{
			// 浏览头部数据
			view_command_head(com);
			if ((compareSelf(com) == 4)&(view_head() == 1)) {
				flagFrequency=1;//改变指令发送频率
			}
		}
	}
}


int motor_main(void)
{
	int ret=0;
	pthread_t id1,id2,id3,id4;
	
	ret=pthread_create(&id1,NULL,thread_1,NULL);//开启线程
	if(ret)
	{
		printf("create pthread error!\n");
		return -1;
	}
	
	ret=pthread_create(&id2,NULL,thread_2,NULL);//开启线程	
	if(ret)
	{
		printf("create pthread error!\n");
		return -1; 
	}

	ret=pthread_create(&id3,NULL,thread_3,NULL);//开启线程	
	if(ret)
	{
		printf("create pthread error!\n");
		return -1; 
	}
	
	ret=pthread_create(&id4,NULL,thread_4,NULL);//开启线程	
	if(ret)
	{
		printf("create pthread error!\n");
		return -1; 
	}
	
	return 0;
	
}

