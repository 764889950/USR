#include "data_global.h"
#include "pthread_main.h"
#include "../cJSON-master/cJSON.h"


//linux查看json文件
//sudo apt install jq
//jq . file.json

char str_fifo[100]={0};
char fifoRecv=0;

void *pthread_fifo_recv_data (void *arg)
{
	int fd;  
	FILE *fp;
	char buf[256];
	char strs[fileDepth];

	if(mkfifo(FIFO_REC, 0777))//创建管道文件
	{  
		// perror("Mkfifo error");  
	}  
	
	sprintf(strs,"sudo chmod 777 -R %s",FIFO_REC);
	fp = popen(strs, "r");
	pclose(fp);	
	
	// printf("open for reading... \n");  
	fd=open(FIFO_REC,O_RDONLY|O_NONBLOCK);//非阻塞 
	/* 
	1：open(fifo, O_RDONLY);  
	       open(fifo, O_WRONLY); 
		不管先运行哪个，都会等另一个进程把对应连接打开时候才open结束 
		  reader 的 open会等到writer的open  运行起来才open结束, 
		  反之亦然 
	    2：open(fifo, O_RDONLY|O_NONBLOCK );  就不等待writer打开文件了 
	    3：open(fifo , O_WRONLY|O_NONBLOCK)： 
	     如果没有reader打开该管道文件的话，就直接报错，退出进程 
	     用perror去抓信息，得到的会是No such device or address 
	    得先运行reader 
	*/  
	if(fd<0)  
	{  
		perror("Failed to open fifo");  
		return NULL;  
	}  
	// printf("opened ... \n");  
	
	int count;
	while(1)
	{
		count=read(fd,buf,127);
		//要用底层io。read()会返回实际读到的字节数
		if(count>0)
		{
			buf[count]=0;//结束符，也可以='\0';
			// printf("recv:%s\n",buf);//打印从FIFO收到的数据
			strcpy(str_fifo,buf);
			fifoRecv=1;
		}
	}
	close(fd);
	return NULL; 
}

void *pthread_fifo (void *arg)
{
	pthread_t id_fifo_recv_data;

	pthread_create (&id_fifo_recv_data, 0, pthread_fifo_recv_data, NULL);

	return NULL;  
}
