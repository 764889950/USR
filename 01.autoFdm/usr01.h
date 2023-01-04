//Linux下编程获取本地IP地址的常见方法  https://www.cnblogs.com/baiduboy/p/7287026.html
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


#include "../cJSON-master/cJSON.h"
#include "../octoModule/octoMovement.h"

using namespace std;



//#define connectName0 "ens33"//网络名
#define connectName1 "eth0"//网络名
#define connectName2 "wlan0"//网络名

//指令集
#define LoopFlie "/api/files/local/loop" //读取与处理的文件夹

//指令令牌号码
#define headerAuthorization "Bearer 4554E6F2DB404CFA8511BF2F648F3755"




vector<string> fileName;//定义个一个字符串容器,用于存放文件名

/*文件数量获取*/
int cjson_pocketsNumber(char *dir)
{
	FILE *fp = NULL;
	char *order_str=NULL;
	int fSize,Buff_Length;

	fp=fopen(dir,"r");
	fseek(fp,0,SEEK_END);//文件指针指向文件尾部
	fSize=ftell(fp);  	 //得知该文件大小
	rewind(fp);  					//指向文件头部
	order_str =(char*)malloc(sizeof(char)*fSize);//内存申请
	if(order_str==NULL)
	{
		printf("申请内存失败！\n");
	}
	Buff_Length=fread(order_str,1,fSize,fp);//写入内存成功会返回数据尺寸
	if(Buff_Length!=fSize)
	{
		printf("json文件获取错误\n");
		free(order_str);//内存释放
		return -1;
	}
	fclose(fp);

	size_t pocket_Num=0;//该json文件中表示的口袋数
	cJSON *root;
	cJSON *order_detail,*order_detail_Item;
	root = cJSON_Parse((const char*)order_str);
	if(!root)
	{
		printf("JSON格式错误:%s \n", cJSON_GetErrorPtr());
		cJSON_Delete(root);
		free(order_str);//内存释放
		return -1;
	}
	else
	{
		// printf("JSON格式正确:\n%s\n",cJSON_Print(root));
		order_detail =  cJSON_GetObjectItem(root, "children");  //获取children键对应的值
		size_t index=0;
		if(order_detail)
		{
			for(;;index++)
			{
				order_detail_Item = cJSON_GetArrayItem(order_detail, index);
				if(NULL==order_detail_Item)
				{
					break;
				}
			}
			pocket_Num=index;
		}
	}
	cJSON_Delete(root);
	free(order_str);//内存释放
	return pocket_Num;
}


/*文件获取处理*/
static int cjson_analysis(char *dir)
{
	FILE *fp = NULL;
	char *order_str=NULL;
	int fSize,Buff_Length;

	fp=fopen(dir,"r");
	fseek(fp,0,SEEK_END); 	//文件指针指向文件尾部
	fSize=ftell(fp);  		//得知该文件大小
	rewind(fp);  			//指向文件头部
	order_str =(char*)malloc(sizeof(char)*fSize);//内存申请
	if(order_str==NULL)
	{
		printf("申请内存失败！\n");
	}
			
	Buff_Length=fread(order_str,1,fSize,fp);//写入内存成功会返回数据尺寸
	if(Buff_Length==fSize)
	{
		
	}
	else
	{
		printf("json文件获取错误\n");
		free(order_str);//内存释放
		return -1;
	}
	fclose(fp);

	cJSON *root;
	
	char *name=NULL;//文件名
	
	cJSON *order_detail,*order_detail_Item;

	
	root = cJSON_Parse((const char*)order_str);
	if(!root)
	{
		printf("JSON格式错误:%s \n", cJSON_GetErrorPtr());
		cJSON_Delete(root);
		free(order_str);//内存释放
		return -1;
	}
	else
	{
		// //printf("JSON格式正确:\n%s\n",cJSON_Print(root));

		order_detail =  cJSON_GetObjectItem(root, "children");  //获取children键对应的值

		if(order_detail)
		{
			for(size_t i=0;;i++)
			{
				order_detail_Item = cJSON_GetArrayItem(order_detail, i);
				if(NULL==order_detail_Item)
				{
					break;
				}
				
				name = cJSON_GetObjectItem(order_detail_Item,"name")->valuestring;//platform键值对
				// printf("fileName:%s\n",name);
				fileName.push_back(name);
			}
		}
	}
	cJSON_Delete(root);
	free(order_str);//内存释放
	return 0;
}


/*文件获取处理  返回1表示loop目录下的文件正在打印，返回0表示loop目录下的文件正在结束打印或者处于非打印转态*/
int cjson_analysisState(char *dir)
{
	
	static int fileState = 0;
	FILE *fp = NULL;
	char *order_str=NULL;
	int fSize,Buff_Length;

	fp=fopen(dir,"r");
	fseek(fp,0,SEEK_END); 	//文件指针指向文件尾部
	fSize=ftell(fp);  		//得知该文件大小
	rewind(fp);  			//指向文件头部
	order_str =(char*)malloc(sizeof(char)*fSize);//内存申请
	if(order_str==NULL)
	{
		printf("申请内存失败！\n");
	}
			
	Buff_Length=fread(order_str,1,fSize,fp);//写入内存成功会返回数据尺寸
	if(Buff_Length==fSize)
	{
		
	}
	else
	{
		printf("json文件获取错误\n");
		free(order_str);//内存释放
		return -1;
	}
	fclose(fp);

	cJSON *root;
	
	char *state=NULL,*fileDir=NULL;
	
	cJSON *order_detail,*order_detail_Item;

	
	root = cJSON_Parse((const char*)order_str);
	if(!root)
	{
		printf("JSON格式错误:%s \n", cJSON_GetErrorPtr());
		cJSON_Delete(root);
		free(order_str);//内存释放
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
			free(order_str);//内存释放			
			return -1;//连接错误或正常不在线
		}		
		
		
		order_detail =  cJSON_GetObjectItem(root, "job");  //获取对象
		
		order_detail_Item = cJSON_GetObjectItem(order_detail, "file");  //获取对象
		fileDir = cJSON_GetObjectItem(order_detail_Item,"path")->valuestring;//platform键值对
		if (fileDir == NULL)
		{
			printf("文件未加载\n");
			cJSON_Delete(root);
			free(order_str);//内存释放
			return 0;	
		}			
		
		// printf("fileDir:%s\n",fileDir);
		
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
	free(order_str);//内存释放
	
	printf("fileState:%d\n",fileState);
	 
	return fileState;
}

/*文件获取处理 获取具体的打印转态，返回0表示不在打印，返回1表示在打印中*/
static int cjson_analysisPrintState(char *dir)
{
	
	static int fileState = 0;
	FILE *fp = NULL;
	char *order_str=NULL;
	int fSize,Buff_Length;

	fp=fopen(dir,"r");
	fseek(fp,0,SEEK_END); 	//文件指针指向文件尾部
	fSize=ftell(fp);  		//得知该文件大小
	rewind(fp);  			//指向文件头部
	order_str =(char*)malloc(sizeof(char)*fSize);//内存申请
	if(order_str==NULL)
	{
		printf("申请内存失败！\n");
	}
			
	Buff_Length=fread(order_str,1,fSize,fp);//写入内存成功会返回数据尺寸
	if(Buff_Length==fSize)
	{
		
	}
	else
	{
		printf("json文件获取错误\n");
		free(order_str);//内存释放
		return -1;
	}
	fclose(fp);

	cJSON *root;
	
	char *state=NULL;
	

	
	root = cJSON_Parse((const char*)order_str);
	if(!root)
	{
		printf("JSON格式错误:%s \n", cJSON_GetErrorPtr());
		cJSON_Delete(root);
		free(order_str);//内存释放
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
			free(order_str);//内存释放
			return -1;//连接错误或正常不在线
		}		
		

		if (stringcasecompare(state, "Printing") == 0)
		{
			// printf("正在打印中\n");
			fileState = 1;
		}
		else if(stringcasecompare(state, "Finishing") == 0)
		{
			fileState = 1;
		}
		else if(stringcasecompare(state, "Operational") == 0)
		{
			// printf("可操作状态\n");
			fileState = 0;
		}
			
		
		
	}
	cJSON_Delete(root);
	free(order_str);//内存释放
	 
	return fileState;
}


//主程序
int autoFdmUsr01(void)
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
			
			if(strcmp(connectName1,ifreq->ifr_name)==0)
			{
				//char *strcpy(char *dest,const char *src);
				// strcpy(connectIP,inet_ntoa(((struct sockaddr_in*)&(ifreq->ifr_addr))->sin_addr));
				sprintf(connectIP,"%s:5000",inet_ntoa(((struct sockaddr_in*)&(ifreq->ifr_addr))->sin_addr));
				printf("connectIP =[%s]\n" , connectIP);
				break;
			}
			
			if(strcmp(connectName2,ifreq->ifr_name)==0)
			{
				//char *strcpy(char *dest,const char *src);
				// strcpy(connectIP,inet_ntoa(((struct sockaddr_in*)&(ifreq->ifr_addr))->sin_addr));
				sprintf(connectIP,"%s:5000",inet_ntoa(((struct sockaddr_in*)&(ifreq->ifr_addr))->sin_addr));
				printf("connectIP =[%s]\n" , connectIP);
				break;
			}
			
			
            ifreq++;
        }
    }
    
    // getchar();//system("pause");//not used in linux 

	int ttyUSB_num;
	
	init:
	ttyUSB_num=0;
	
	fp = popen("sudo find /dev/ -name \"ttyUSB[0-9]*\" |wc -l", "r");//查看sd*[1-inf]文件的数量
	fgets(buf, sizeof(buf), fp);
	ttyUSB_num=atoi(buf);
	pclose(fp);
	
	printf("ttyUSB_num:%d\r\n",ttyUSB_num);
	
	
	for(int i=0;i<ttyUSB_num;i++)
	{
		sprintf(buf,"sudo chmod 777 -R /dev/ttyUSB%d",i);
		printf("buf:%s\r\n",buf);
		fp = popen(buf, "r");
		pclose(fp);
	}
	
	int fileNum = 0, fileNumLast = 0;
	while(1)
	{
		sleep(5);
		sprintf(buf,"curl --header \"authorization:%s\" \"%s%s\"",headerAuthorization,connectIP,LoopFlie);
		// printf("buf:%s\r\n",buf);
		
		FILE *fpFile = NULL;
		fpFile = fopen("./01.autoFdm/fileList.json", "w+");

		// fp = popen("curl --header \"authorization:Bearer 54B4D48A3FB8481F9F8A611B8FBEF057\" 192.168.0.210:5000", "r");
		 fp = popen(buf, "r");
		while(memset(buf, 0, sizeof(buf)), fgets(buf, sizeof(buf) - 1, fp) != 0 )//以换行结尾
		{
			// printf("%s\r\n",buf);//分别输出当前返回内容
			fputs(buf, fpFile);
		}
		pclose(fp);
		fclose(fpFile);
		
		// printf("文件数量:%d\n",cjson_pocketsNumber((char*)"./01.autoFdm/fileList.json"));
		if(cjson_analysis((char*)"./01.autoFdm/fileList.json")==-1)
		{
			continue;
		}

		vector<string>::iterator iter = fileName.begin();
		
		for (size_t i=0;iter!=fileName.end();iter++,i++){
			// cout<<"vecFileName:"<<fileName[i]<<endl;
			if((decide_ext(fileName[i].c_str(),"gcode")==true)|(decide_ext(fileName[i].c_str(),"gco")==true)|(decide_ext(fileName[i].c_str(),"g")==true))
			{
				// printf("ok,后缀符合\n");
			}
			else
			{
				// printf("后缀不符合\n");
				fileName.erase(iter);
				iter--;
				i--;
			}
		}
		
		for (size_t i=0;i<fileName.size();i++){
			cout<<"fileName:"<<fileName[i]<<endl;
		}
		fileNum =fileName.size();
		
		
		static int fileIndex = 0;
		if(fileNumLast != fileNum)
		{
			printf("文件数量发生变化\n");
			fileNumLast = fileNum;
			fileIndex = 0;//文件数量发生变化，下标初始化
		}
		
		fileName.clear();//清除容器中所有数据
		
		
		static int loopFlag=0;
		//判断loop目录文件的打印状态
		sprintf(buf,"curl --header \"authorization:%s\" \"%s/api/job\"",headerAuthorization,connectIP);
		// printf("buf:%s\r\n",buf);
		
		fpFile = fopen("./01.autoFdm/printState.json", "w+");

		// fp = popen("curl --header \"authorization:Bearer 54B4D48A3FB8481F9F8A611B8FBEF057\" 192.168.0.210:5000", "r");
		 fp = popen(buf, "r");
		while(memset(buf, 0, sizeof(buf)), fgets(buf, sizeof(buf) - 1, fp) != 0 )//以换行结尾
		{
			// printf("%s\r\n",buf);//分别输出当前返回内容
			fputs(buf, fpFile);
		}
		pclose(fp);		
		fclose(fpFile);
		
		loopFlag = cjson_analysisState((char*)"./01.autoFdm/printState.json");
		
		// printf("loopFlag:%d\n",loopFlag);
		printf("fileNum:%d\n",fileNum);
		
		//如loop目录的文件处于打印状态，支持连续循环打印
		if(loopFlag==-1)
		{
			goto init;
		}
		else if(loopFlag==1)//连续循环打印
		{
			if(fileNum>0)
			{
				if(cjson_analysisPrintState((char*)"./01.autoFdm/printState.json")==0)//==0表示可操作
				{
					printf("可操作\n\n");
					
					printf("fileIndex:%d\n",fileIndex);

					sprintf(buf,"curl --header \"authorization:%s\"  -X POST \"%s%s/%s\" --header \"content-type:application/json\" -d \'{\"command\": \"select\",\"print\": true}\'",headerAuthorization,connectIP,LoopFlie,(*(fileName.begin()+fileIndex)).c_str());
					printf("buf:%s\r\n",buf);
					fp = popen(buf, "r");
					pclose(fp);					
					//操作文件进行打印，文件名以  fileIndex 为下标
					
					fileIndex++;
					if(fileIndex>=fileNum)
					{
						fileIndex=0;
					}
					
				}
			}
			
		}

	}

	return 0;
}
