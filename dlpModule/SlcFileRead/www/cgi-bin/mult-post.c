#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>  
#include <sys/types.h>  
#include <sys/stat.h>  
#include <string.h>  
#include <ctype.h>      //tolower

#define time_out 500  	//超时时间（秒）
#define recPer_length 256	//接收数据的单行长度
#define rec_length 1024		//接收数据的总长度

#define lenth_path 100

#define FIFO_REC "/SLC_ReadFifoRec"

//辅助文件路径，用于控制CGI返回,通常位于用户目录
#define FIFO_BACK "/home/gcp/SLC_ReadFifoBack"

int fifo_write(char * fifoName,char *buf)
{
	int fd;  
	//printf("open for writing ... \n"); 
	fd=open(fifoName,O_WRONLY|O_NONBLOCK);// 如果写端设置成非阻塞，不能先于读端运行，否则 open失败  
	if(fd<0)  
	{  
	    perror("Failed to open fifo:");  
	    return -1;  
	}  
	write(fd,buf,strlen(buf));//把缓存写入
	// printf("writing ok \n"); 
	return 0;
}



int main(void){
	char file_dir[lenth_path]={0},fifo_dir[lenth_path]={0};
	long len;//允许单个json文件最大32-1位长度,最大2147483647
	char *lenstr;// lenstr=(char *)malloc(sizeof(char) * 10);//内存申请,允许单个json文件最大32-1位长度，最大2147483647

	printf("Content-Type:text/html\n\n");
	lenstr=getenv("CONTENT_LENGTH");
	if(lenstr == NULL)
	{
		printf("<DIV STYLE=\"COLOR:RED\">Error parameters should be entered!</DIV>\n");
	}	
	else
	{
		len=atol(lenstr);
	}
	

	char *poststr=NULL;
	
	poststr=(char *)malloc(sizeof(char) * len);//内存申请
	if (poststr == NULL)
	{
		printf("Fail to allocate memory to wid\r\n");
		fflush(stdout);
		return -1;
	}
	fgets(poststr,len+1,stdin);
		
	if(fifo_write(FIFO_REC,poststr))
	{
		goto end;
	}

	// FILE *fp = NULL;
	// fp = fopen("/home/gcp/samba_share/info_out.txt", "w");
	// fputs(poststr, fp);
	// fclose(fp);	
	
	int fd;
	FILE *fp = NULL;
	char buf[recPer_length]={0};
	char strs[rec_length]={0};
	
	int times=0;
	
	juge:
	times++;
	
	fd=access(FIFO_BACK,F_OK);
	
	if(fd)//文件不存在
	{
		sleep(1);
		if(times<time_out)
		{
			goto juge;
		}
	}
	else if(fd==0)//文件存在
	{
		
		usleep(50000);
		fp = fopen(FIFO_BACK, "r");
		while(fgets(buf,recPer_length,fp))
		{
			strncat(strs,buf,recPer_length);
		}
		fclose(fp);
		// printf("read ok\n");
		remove(FIFO_BACK);
		
		
	}
	
	printf("%s",strs);
	
	// printf("access:%d\n",access(FIFO_BACK,F_OK));
	
	// printf("<html><body>");
	// printf("<a href=\"/index.html\">return index.html</a>");
	// printf("</body></html>");
	
	end:
	
	fflush(stdout);
	
	// free(lenstr);//释放内存
	free(poststr);//释放内存
	
	return 0;
	
}

