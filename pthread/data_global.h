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

#include "parameter.h"

//pthread_fifo.cpp文件中定义
extern char str_fifo[100];
extern char fifoRecv;
////////////////////////////


//查看ubuntu位数 -> #getconf LONG_BIT
extern void *pthread_websocket(void *arg);		//websocket服务器线程
extern void *pthread_fifo (void *arg);			//CGI数据处理


#endif
