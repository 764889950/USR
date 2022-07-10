#ifndef __DATA_GLOBAL__H__
#define __DATA_GLOBAL__H__


#include <stdio.h>//定义输入／输出函数
#include <errno.h>//定义错误码(Linus从minix中引进的)。
#include <unistd.h>//符号常量unix
#include <signal.h>//信号机制支持
#include <pthread.h>//线程
#include <sys/ipc.h>//IPC(命名管道)
#include <sys/shm.h>//共享存储
#include <sys/sem.h>//信号量

#include <iconv.h> 
#include <fcntl.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <syscall.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <linux/fs.h>
#include <linux/ioctl.h>
#include <stdarg.h>
#include <stdint.h>
#include <math.h> 
#include <vector>
#include <iostream> 
#include <string>


#include "Debug.h"  //DEBUG()



//命令
typedef struct idx
{
	char command[512];

}Goods;

//详情
typedef struct trade_set
{
	char platform[10];	//平台类型
	char order_no[50];	//编号
	int pocket_Num;		//口袋种类数
	struct idx *goods;	//各口袋信息块
	char order_status;	//读入状态，为0未读入，为1已读入
	struct trade_set *next;
}Node,*nodeList;


//查看ubuntu位数 -> #getconf LONG_BIT

#endif
