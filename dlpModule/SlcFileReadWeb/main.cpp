#include <stdio.h>      //printf()
#include <stdio.h>      //printf()
#include <stdlib.h>     //exit()
#include <unistd.h> 	//usleep
#include <string.h>     //strlen
#include <ctype.h>      //tolower
#include <stdarg.h>		//va_list  ...

#include "./pthread/pthread_main.h"
#include "./pthread/SLC_Read.h"
#include "./pthread/parameter.h"
#include "./pthread/data_global.h"

using namespace std;

void logAlways(const char* fmt, ...)
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


void printHelp()
{
    logAlways("Show this help message\n");
	logAlways("command: \"fileInf -d DIR\"，读取文件基本信息,DIR为文件路径\n");
	logAlways("command: \"fileLayerInf -d DIR\"，读取模型的层数,DIR为文件路径\n");
	logAlways("command: \"fileToBmp -d DIR -c layer\"，生成BMP文件,DIR为文件路径,layer为要转换的层(整数)，当-c参数省略时，SLC文件通过外部触发继续解析，触发指令关键词为readFlag、readNext、eixt。e.g. fileToBmp -d /home/share/dst.slc -c 10\n");
	logAlways("command:\"help\"，显示帮助\n");
	logAlways("\"analysisNext\"，继续解析文件，传入方式为fifo or websocket\n");
	logAlways("\"eixt\"，退出解析程序，传入方式为fifo or websocket\n");

	// logAlways("r表示(rows)行，c表示(cols)列,M、N代表对应的分辨率\n");

    logAlways("\n");
}


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


static int stringcasecompare(const char* a, const char* b)
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

int fileInf(int argc, char **argv)
{
	for(int argn = 2; argn < argc; argn++)
	{
		char* str = argv[argn];
		if (str[0] == '-')
		{
			for(str++; *str; str++)
			{
				switch(*str)
				{
				case 'd':
					if((argn+1)<argc)
					{
						argn++;
						if((strlen(str)==1)&&(strlen(argv[argn])!=0))
						{
							str = argv[argn];
							ReadSLC_inf(str);	
							goto demo_end;
						}
						else
						{
							return -1;
						}
					}
					else
					{
						return -1;
					}
					break;
				}
			}
		}
		demo_end:;
	}
	return 0;
	
}
int fileLayerInf(int argc, char **argv)
{
	for(int argn = 2; argn < argc; argn++)
	{
		char* str = argv[argn];
		if (str[0] == '-')
		{
			for(str++; *str; str++)
			{
				switch(*str)
				{
				case 'd':
					if((argn+1)<argc)
					{
						argn++;
						if((strlen(str)==1)&&(strlen(argv[argn])!=0))
						{
							str = argv[argn];
							ReadSLC_layerInf(str);
							
							goto demo_end;
						}
						else
						{
							return -1;
						}
					}
					else
					{
						return -1;
					}	
					break;
				}
			}
		}
		demo_end:;
	}
	
	return 0;
	
}

void *pthread_analyse (void *arg)
{
	char fileDir[fileDepth];
	char fileDir_new[fileDepth];
	FILE *fp = NULL;
	char strs[fileDepth];
	
	sprintf(fileDir,"%s/SLC_ReadFifoBack",FIFO_BACK);
	sprintf(fileDir_new,"%s/SLC_ReadFifoBack_new",FIFO_BACK);

	sprintf(strs,"sudo mkdir %s >/dev/null 2>&1",FIFO_BACK);
	fp = popen(strs, "r");
	pclose(fp);	
	
	sprintf(strs,"sudo chmod 777 %s",FIFO_BACK);
	fp = popen(strs, "r");
	pclose(fp);	
				
	while(1)
	{
		usleep(50000);
		if(fifoRecv == 1)//管道fifo
		{
			// printf("fifo:%s\r\n",str_fifo);
			fifoRecv = 0;
			//指令对比
			if(stringcasecompare(str_fifo, "analyseNext") == 0)
			{
				file_continue_flag=1;
				while(file_continue_flag==1);//等待解析完成
				sprintf(strs,"LayerCurrent:%ld",LayerCurrent);
				//创建 FIFO_BACK 文件
				fp = fopen(fileDir_new, "w+");
				fputs("BMP_OK\n", fp);
				fputs(strs, fp);
				fclose(fp);
				sprintf(strs,"sudo chmod 777 %s",fileDir_new);
				printf("strs:%s\r\n",strs);
				fp = popen(strs, "r");
				pclose(fp);					
				
				rename(fileDir_new,fileDir);//重命名文件
			}
			else if(stringcasecompare(str_fifo, "exit") == 0)
			{
				//创建 FIFO_BACK 文件
				fp = fopen(fileDir_new, "w+");
				fputs("exit", fp);
				fclose(fp);
				sprintf(strs,"sudo chmod 777 %s",fileDir_new);
				printf("strs:%s\r\n",strs);
				fp = popen(strs, "r");
				pclose(fp);						
				rename(fileDir_new,fileDir);//重命名文件			
				// printf("exit\n");
				
				sprintf(strs,"sudo rm %s",FIFO_REC);
				fp = popen(strs, "r");
				pclose(fp);	
				
				exit(0);
			}
		}
	}
	return NULL;
}

int fileToBmp(int argc, char **argv)
{
	int DIR_flag=0,layer_flag=0;

	char file_dir[fileDepth];
	
	char strs[fileDepth];
	
	long layer = 0;
	
	FILE *fp = NULL;
	pthread_t id_analyse;//fifo指令判断线程
	
	for(int argn = 2; argn < argc; argn++)
	{
		char* str = argv[argn];
		if (str[0] == '-')
		{
			for(str++; *str; str++)
			{
				switch(*str)
				{
				case 'd':
					if((argn+1)<argc)
					{
						argn++;
						if((strlen(str)==1)&&(strlen(argv[argn])!=0))
						{
							str = argv[argn];
							strcpy(file_dir,str);
							DIR_flag=1;

							goto demo_end;
						}
						else
						{
							return -1;
						}
					}
					else
					{
						return -1;
					}
					
					break;
					
				case 'c':
					if((argn+1)<argc)
					{
						argn++;
						if((strlen(str)==1)&&(strlen(argv[argn])!=0))
						{
							str = argv[argn];
							if(is_number(str)==1)
							{
								layer =  atol(str);//将字符串转换为长整值
								layer_flag=1;
							}
							else
							{
								return -1;
							}
							goto demo_end;
						}
						else
						{
							return -1;
						}
					}
					else
					{
						return -1;
					}
					
					break;					
					
				}
			}
		}
		demo_end:;
	}
	
	if((DIR_flag==1)&&(layer_flag==1))
	{
		OpenSLC_1(file_dir,layer);
	}

	if((DIR_flag==1)&&(layer_flag==0))
	{
		pthread_create (&id_analyse, 0, pthread_analyse, NULL);//开启线程
		pthread_main();
		OpenSLC(file_dir);
		
		sprintf(strs,"sudo rm %s",FIFO_REC);
		fp = popen(strs, "r");
		pclose(fp);	
	}

	return 0;
	
}



int main(int argc, char **argv)
{
	// char fileDir[fileDepth];
	// FILE *fp = NULL;
	// char strs[fileDepth];
	
	// sprintf(fileDir,"%s_new",FIFO_BACK);
	// sprintf(strs,"LayerCurrent:%ld",LayerCurrent);
	// //创建 FIFO_BACK 文件
	// fp = fopen(fileDir, "w+");
	// fputs("BMP_OK\n", fp);
	// fputs(strs, fp);	
	// fclose(fp);
	// rename(fileDir,FIFO_BACK);//重命名文件
	// return 0;
	
	// for (int i=0;i<argc;i++){
		// printf("程序参数%d:%s\n",i,argv[i]);
	// }

	if (argc < 2)
    {
        printHelp();
        exit(1);
    }
	else if (stringcasecompare(argv[1], "fileInf") == 0)
    {
		if(fileInf(argc,argv)==-1)
			printHelp();
		return 0;
		// logAlways("demo\n");
    }
	else if (stringcasecompare(argv[1], "fileLayerInf") == 0)
    {
		if(fileLayerInf(argc,argv)==-1)
			printHelp();
		return 0;
		// logAlways("demo\n");
    }
	else if ((stringcasecompare(argv[1], "fileToBmp") == 0)||(stringcasecompare(argv[1], "fileAuto") == 0))
    {
		if(fileToBmp(argc,argv)==-1)
			printHelp();
		return 0;
		// logAlways("demo\n");
    }
    else
    {
        logAlways("Unknown command: %s\n", argv[1]);
        exit(1);
    }

	return 0;
}
