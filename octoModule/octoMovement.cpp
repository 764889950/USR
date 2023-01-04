#include <netdb.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>

#include <stdio.h>      //printf()
#include <stdlib.h>     //exit()
#include <unistd.h> 	//usleep
#include <string.h>     //strlen
#include <ctype.h>      //tolower
#include <stdarg.h>		//va_list  ...

#include <vector>
#include <string>//c++ string类
#include <iostream>

#include "octoMovement.h"

#include "../cJSON-master/cJSON.h"

using namespace std;


#define IS_BLANK(c) ((c) == ' ' || (c) == '\t')
#define IS_DIGIT(c) ((c) >= '0' && (c) <= '9')
#define IS_ALPHA(c) ( ((c) >= 'a' && (c) <= 'z') || ((c) >= 'A' && (c) <= 'Z') )
#define IS_HEX_DIGIT(c) (((c) >= 'A' && (c) <= 'F') || ((c) >= 'a' && (c) <= 'f'))

/* Whether string s is a number. 
   Returns 0 for non-number, 1 for integer, 2 for hex-integer, 3 for float */
int is_number(char * s)
{
	int base = 10;
	char *ptr;
	int type = 0;
 
	if (s==NULL) return 0;
 
	ptr = s;
 
	/* skip blank */
	while (IS_BLANK(*ptr)) {
		ptr++;
	}
 
	/* skip sign */
	if (*ptr == '-' || *ptr == '+') {
		ptr++;
	}
 
	/* first char should be digit or dot*/
	if (IS_DIGIT(*ptr) || ptr[0]=='.') {
 
		if (ptr[0]!='.') {
			/* handle hex numbers */
			if (ptr[0] == '0' && ptr[1] && (ptr[1] == 'x' || ptr[1] == 'X')) {
				type = 2;
				base = 16;
				ptr += 2;
			}
 
			/* Skip any leading 0s */
			while (*ptr == '0') {
				ptr++;
			}
 
			/* Skip digit */
			while (IS_DIGIT(*ptr) || (base == 16 && IS_HEX_DIGIT(*ptr))) {
					ptr++;
			}
		}
 
		/* Handle dot */
		if (base == 10 && *ptr && ptr[0]=='.') {
			type = 3;
			ptr++;
		}
 
		/* Skip digit */
		while (type==3 && base == 10 && IS_DIGIT(*ptr)) {
			ptr++;
		}
 
		/* if end with 0, it is number */
		if (*ptr==0) 
			return (type>0) ? type : 1;
		else
			type = 0;
	}
	return type;
}


static int stringcasecompare(const char* a, const char* b)//a和b字符串对比
{
    while(*a && *b)
    {
        if (tolower(*a) != tolower(*b))
            return tolower(*a) - tolower(*b);
        a++;
        b++;
    }
    return *a - *b;
}


bool decide_ext(const char *gname,const char *nsuff)
// gname: name of the given file 
// nsuff: suffix  you need
{
    char dot = '.';
    char suff[10] = {0};
    int c;
    int j = 0;
    c = strlen(gname);
    // std::cout<< c << '\n';
    for(int i = 0; i<c; i++)
    {
        if(gname[i] == dot)
            j = i;
    }
    int k = j;
    j = c - j - 1;
    for(int i = 0; i<j; i++)
    {
        suff[i] = gname[k+i+1];
    }
    if (0==strcmp(suff,nsuff))
        return true;
    else
        return false;
}



static void logAlways(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    #pragma omp critical
    {
        vfprintf(stderr, fmt, args);
        fflush(stderr);
    }
    va_end(args);
}


static void printHelp1()
{
    logAlways("Show this help message\n");

    logAlways("\n");
}


/*文件获取处理  返回1表示loop目录下的文件正在打印，返回0表示loop目录下的文件正在结束打印或者处于非打印转态*/
static int cjson_analysisState(char *order_str)
{
	
	static int fileState = 0;
	
	cJSON *root;
	
	char *state=NULL,*fileDir=NULL;
	
	cJSON *order_detail,*order_detail_Item;
	
	root = cJSON_Parse((const char*)order_str);
	if(!root)
	{
		printf("JSON格式错误:%s \n", cJSON_GetErrorPtr());
		cJSON_Delete(root);
		return -1;
	}
	else
	{
		// printf("JSON格式正确:\n%s\n",cJSON_Print(root));
		state = cJSON_GetObjectItem(root,"state")->valuestring;//键值对
		printf("fileState:%s\n",state);
		if(stringcasecompare(state, "Offline") == 0)
		{
			cJSON_Delete(root);		
			return -1;//连接错误或正常不在线
		}		
		
		printf("order_str:%s\n",order_str);
		
		order_detail =  cJSON_GetObjectItem(root, "job");  //获取对象
		
		order_detail_Item = cJSON_GetObjectItem(order_detail, "file");  //获取对象
		fileDir = cJSON_GetObjectItem(order_detail_Item,"path")->valuestring;//platform键值对
		if (fileDir == NULL)
		{
			printf("文件未加载\n");
			cJSON_Delete(root);
			return 0;	
		}
		
		int i=0;
		while(*(fileDir+i))
		{
			i++;
			if(*(fileDir+i)=='/')
			{
				*(fileDir+i)='\0';
			}
		}
		printf("fileDir:%s\n",fileDir);
		

		if ((stringcasecompare(state, "Printing") == 0)&&(stringcasecompare(fileDir, "loop") == 0))
		{
			printf("允许连续打印\n");
			fileState = 1;		
		}
		else if((stringcasecompare(state, "Cancelling") == 0)&&(stringcasecompare(fileDir, "loop") == 0))
		{
			printf("结束连续打印\n");
			fileState = 0;			
		}
		
	}
	cJSON_Delete(root);
	
	printf("fileStateValue:%d\n",fileState);
	 
	return fileState;
}


void get_netInfConnect(void)
{
	FILE *fp;
	char connectIP[256];
	
    int sockfd;
    struct ifconf ifconf;
    struct ifreq *ifreq;
    char buf[512];//缓冲区
    //初始化ifconf
    ifconf.ifc_len =512;
    ifconf.ifc_buf = buf;
    if ((sockfd =socket(AF_INET,SOCK_DGRAM,0))<0)
    {
        perror("socket" );
        exit(1);
    }
    ioctl(sockfd, SIOCGIFCONF, &ifconf); //获取所有接口信息

    //接下来一个一个的获取IP地址
    ifreq = (struct ifreq*)ifconf.ifc_buf;
    printf("ifconf.ifc_len:%d\n",ifconf.ifc_len);
    printf("sizeof (struct ifreq):%ld\n",sizeof (struct ifreq));

    for (int i=(ifconf.ifc_len/sizeof (struct ifreq)); i>0; i--)
    {
        if(ifreq->ifr_flags == AF_INET){ //for ipv4
            printf("name =[%s]\n" , ifreq->ifr_name);
            printf("local addr = [%s]\n" ,inet_ntoa(((struct sockaddr_in*)&(ifreq->ifr_addr))->sin_addr));
			
			if(strcmp(connectName,ifreq->ifr_name)==0)
			{
				//char *strcpy(char *dest,const char *src);
				// strcpy(connectIP,inet_ntoa(((struct sockaddr_in*)&(ifreq->ifr_addr))->sin_addr));
				sprintf(connectIP,"%s:%d",inet_ntoa(((struct sockaddr_in*)&(ifreq->ifr_addr))->sin_addr),octoPort);
				printf("connectIP =[%s]\n" , connectIP);
			}
			
            ifreq++;
        }
    }
	

	sprintf(buf,"curl --header \"authorization:%s\"  -X POST \"%s/api/connection\" --header \"content-type:application/json\" -d \'{\"autoconnect\": \"false\",\"baudrate\": 115200,\"command\": \"connect\",\"port\": \"AUTO\",\"printerProfile\": \"_default\"}\'",headerAuthorization,connectIP);

	printf("\n\n%s\n\n" , buf);
	
	fp = popen(buf, "r");
	pclose(fp);
	
	
}

void scram(void)//急停
{
	
	unsigned char timeWait = 5;//curl间隔等待时间
	
	
	FILE *fp;
	char connectIP[256];
	
    int sockfd;
    struct ifconf ifconf;
    struct ifreq *ifreq;
    char buf[512];//缓冲区
    //初始化ifconf
    ifconf.ifc_len =512;
    ifconf.ifc_buf = buf;
    if ((sockfd =socket(AF_INET,SOCK_DGRAM,0))<0)
    {
        perror("socket" );
        exit(1);
    }
    ioctl(sockfd, SIOCGIFCONF, &ifconf); //获取所有接口信息

    //接下来一个一个的获取IP地址
    ifreq = (struct ifreq*)ifconf.ifc_buf;
    printf("ifconf.ifc_len:%d\n",ifconf.ifc_len);
    printf("sizeof (struct ifreq):%ld\n",sizeof (struct ifreq));

    for (int i=(ifconf.ifc_len/sizeof (struct ifreq)); i>0; i--)
    {
        if(ifreq->ifr_flags == AF_INET){ //for ipv4
            printf("name =[%s]\n" , ifreq->ifr_name);
            printf("local addr = [%s]\n" ,inet_ntoa(((struct sockaddr_in*)&(ifreq->ifr_addr))->sin_addr));
			
			if(strcmp(connectName,ifreq->ifr_name)==0)
			{
				//char *strcpy(char *dest,const char *src);
				// strcpy(connectIP,inet_ntoa(((struct sockaddr_in*)&(ifreq->ifr_addr))->sin_addr));
				sprintf(connectIP,"%s:%d",inet_ntoa(((struct sockaddr_in*)&(ifreq->ifr_addr))->sin_addr),octoPort);
				printf("connectIP =[%s]\n" , connectIP);
			}
			
            ifreq++;
        }
    }
	
	/*
	//断开连接
	sprintf(buf,"curl --header \"authorization:%s\"  -X POST \"%s/api/connection\" --header \"content-type:application/json\" -d \'{\"command\": \"disconnect\"}\'",headerAuthorization,connectIP);
	printf("\n\n%s\n\n" , buf);
	fp = popen(buf, "r");
	pclose(fp);		
		*/
	
	sleep(timeWait);
	reconnect:
	//进行连接
	sprintf(buf,"curl --header \"authorization:%s\"  -X POST \"%s/api/connection\" --header \"content-type:application/json\" -d \'{\"autoconnect\": \"false\",\"baudrate\": 115200,\"command\": \"connect\",\"port\": \"AUTO\",\"printerProfile\": \"_default\"}\'",headerAuthorization,connectIP);
	printf("\n\n%s\n\n" , buf);
	fp = popen(buf, "r");
	pclose(fp);	

	
	sleep(timeWait++);
	if(timeWait>20)
		return;
	
	//判断连接状态
	static int loopFlag=0;
	sprintf(buf,"curl --header \"authorization:%s\" \"%s/api/job\"",headerAuthorization,connectIP);
	printf("buf:%s\r\n",buf);
	string order_str;
	fp = popen(buf, "r");
	while(memset(buf, 0, sizeof(buf)), fgets(buf, sizeof(buf) - 1, fp) != 0 )//以换行结尾
	{
		// printf("%s\r\n",buf);//分别输出当前返回内容
		order_str.append(buf);
	}
	pclose(fp);	
	//判断loop目录文件的打印状态
	loopFlag = cjson_analysisState((char*)(order_str.c_str()));
	if(loopFlag==-1)
	{
		goto reconnect;
	}
}


// //Linux下编程获取本地IP地址的常见方法  https://www.cnblogs.com/baiduboy/p/7287026.html
// #include <netdb.h>
// #include <arpa/inet.h>
// #include <sys/socket.h>
// #include <unistd.h>
// #include <sys/ioctl.h>
// #include <net/if.h>

// #include <stdio.h>
// #include <stdlib.h>

// int main(int argc,char* argv[])
// {
    // int sockfd;
    // struct ifconf ifconf;
    // struct ifreq *ifreq;
    // char buf[512];//缓冲区
    // //初始化ifconf
    // ifconf.ifc_len =512;
    // ifconf.ifc_buf = buf;
    // if ((sockfd =socket(AF_INET,SOCK_DGRAM,0))<0)
    // {
        // perror("socket" );
        // exit(1);
    // }
    // ioctl(sockfd, SIOCGIFCONF, &ifconf); //获取所有接口信息

    // //接下来一个一个的获取IP地址
    // ifreq = (struct ifreq*)ifconf.ifc_buf;
    // printf("ifconf.ifc_len:%d\n",ifconf.ifc_len);
    // printf("sizeof (struct ifreq):%d\n",sizeof (struct ifreq));

    // for (int i=(ifconf.ifc_len/sizeof (struct ifreq)); i>0; i--)
    // {
        // if(ifreq->ifr_flags == AF_INET){ //for ipv4
            // printf("name =[%s]\n" , ifreq->ifr_name);
            // printf("local addr = [%s]\n" ,inet_ntoa(((struct sockaddr_in*)&(ifreq->ifr_addr))->sin_addr));
            // ifreq++;
        // }
    // }

    // getchar();//system("pause");//not used in linux 
    // return 0;
// }


