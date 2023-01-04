//OpenCV的轮廓查找和填充  https://blog.csdn.net/garfielder007/article/details/50866101
//opencv findContours和drawContours使用方法  https://blog.csdn.net/ecnu18918079120/article/details/78428002
//OpenCV关于容器的介绍  https://blog.csdn.net/Ahuuua/article/details/80593388

//opencv
#include "opencv2/highgui/highgui.hpp"  
#include "opencv2/imgproc/imgproc.hpp" 

//DataProcessThread.cpp : implementation file
#include <stdio.h>
#include <stdlib.h>
#include <string.h>   
#include <fcntl.h>
#include <unistd.h>		//write read等函数
#include <sys/types.h>
#include <sys/stat.h>
#include <linux/fs.h>
#include <iostream>  
#include <sys/ioctl.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <pthread.h>  	//包线程要包含
#include <termios.h>
#include <syscall.h>
#include <sys/msg.h>
#include <linux/ioctl.h>
#include <stdarg.h>
#include <stdint.h>

#include "cJSON.h"
#include "zlib-master/zlib.h"


using namespace cv;
using namespace std;

// FILE *fd_temp;  //将数据保存到文本区

//打印参数
double thickness=0;//读。打印层厚(mm)，初始为0
long totalLayers=0;//读。打印总层数，初始为0

int pixel_bits_x=1920;//图片的X轴分辨率，初始为1920
int pixel_bits_y=1080;//图片的Y轴分辨率，初始为1080
float zoom_x=20;//X轴放大比例，初始为20
float zoom_y=20;//Y轴放大比例，初始为20
char mirrorBmpFlag=0;//	图像翻转标识，等于0默认翻转，当等于1时不翻转


//字符数组中查找字符串或字符数组
//pSrc:原字符
//srcSize:原字符长度
//pDest：比对的字符
//dstSize:比对的字符的长度
int FindString(char pSrc[], int srcSize, char pDest[], int dstSize);

///////////////////////////////////////////////////
float minx, maxx, miny, maxy, minz, maxz;//用于存储从文件头读取到的模型坐标范围
float minxReal, maxxReal, minyReal, maxyReal, minzReal, maxzReal;//用于存储模型实际坐标范围
void ReadSLC_inf(const char file_dir[]);//获取文件信息
///////////////////////////////////////////////////

void OpenSLC_1(const char file_dir[],long layer);//指定的单层解析,模型尺寸区间以文件头标识为准
void OpenSLCToResult(const char fileSourceDir[],const char fileDesDir[]);//解析文件全部内容，并在指定路径下生成执行文件，,模型尺寸区间以实际解析的数模为准

static int stringcasecompare(const char* a, const char* b);//a和b字符串对比
char* cjson_analysisState(const char *order_str, const char * key, char* value);//返回json配对值参数，value作为输出返回
int cjson_ParameterAnalysis(const char *order_str,char* fileNameDirOut);//fileNameDirOut为json中的文件名，作为输出返回，其他例如zoom_x的全部参数如果有也同步更新
void strip(char *s);//将非法字符去除，重新链接字符串，并以'\0'作为结尾 https://blog.csdn.net/GeneralJing/article/details/115012537


void OpenSLCToResult(const char fileSourceDir[],const char fileDesDir[])//解析文件全部内容，并在指定路径下生成执行文件，,模型尺寸区间以实际解析的数模为准
{
	ReadSLC_inf(fileSourceDir);
		
	FILE *fd_temp;  //将调试数据保存到文本区
	//保存调试数据到文件夹
	fd_temp=fopen("./slcFileInf.txt", "w+");
	
	int fd; 		//文件描述符
	char m_data;	//读取到的数据
	float d = 0;    //d > 0 从上往下看是逆时针
	// int size=0;		//读取到的数据长度
	
	fd = open(fileSourceDir, O_RDONLY);
	//////////////////////////////

	/*
	// O_CREAT 若欲打开的文件不存在则自动建立该文件。
	// O_RDONLY 以只读方式打开文件
	// O_WRONLY 以只写方式打开文件
	*/
	
	//【1】CV_8UC1---则可以创建----8位无符号的单通道---灰度图片------grayImg
	//【2】CV_8UC3---则可以创建----8位无符号的三通道---RGB彩色图像---colorImg 
	//【3】CV_8UC4--则可以创建-----8位无符号的四通道---带透明色的RGB图像 
	Mat dst = Mat::zeros(pixel_bits_y, pixel_bits_x, CV_8UC1);//生成的图片，其分辨率由实际的FrameBuffer来决定
	CvScalar color=cvScalar(0);
	
	vector<Point> contour;       	 //单个轮廓坐标值
	vector<vector<Point>> v_contour; //当前层所有轮廓集合

	vector<int> flag_swap_vector;	//轮廓排序用
	vector<vector<Point>> vctint;	//轮廓排序用
	float flag_swap=0;				//轮廓排序用
	

	unsigned int i=0;
	unsigned int j=0;

	unsigned int n_boundary,n_vertices,n_gaps;
	float   n_float,n_layer;

	float   n_polylineX,n_polylineY;
	
	
/*   压缩相关   */	
	FILE *inFile;
	FILE *outFile;
	char str[200];
/*    */

	
	
    /**************************处理头文件部分**************************/
	while(1)
	{
		i++;
		if(i==2048)
		{
			printf("file error\r\n");
			fprintf(fd_temp,"file error\r\n");
			close(fd);
			
			//关闭调试输出的数据文件
			fclose(fd_temp);
			//////////////////////
			return;
		}

		read(fd, &m_data, 1);
		// printf("m_data=%x\r\n", m_data);

		switch(m_data)
		{
		case 0x0d:
			j=1;
			break;
		case 0x0a:
			if(j==1)
				j=2;
			break;
		case 0x1a:
			if(j==2)
				j=3;
			break;
		default:
			j=0;
			break;
		}
		if(j==3)
			break;
	}
	
	// printf("size=%d\r\n", size);
	/******************************************************************/
	/***************************处理预留部分***************************/
	for(i=0;i<256;i++)
	{
		read(fd, &m_data, 1);
		// fprintf(fd_temp,"m_data=%x\r\n",m_data);
	}
	/******************************************************************/
	/**************************处理样本表部分**************************/
	read(fd, &m_data, 1);
	printf("Sampling Table Size=%x\r\n", m_data);
	// fprintf(fd_temp,"Sampling Table Size=%x\r\n",m_data);
	while(m_data)
	{
		read(fd, &n_float, 4);//Minimum Z Level
		read(fd, &n_float, 4);//Layer Thickness
		// printf("Layer Thickness=%.5f\r\n",n_float);
		fprintf(fd_temp,"Layer Thickness=%.5f\r\n",n_float);
		
		// m_parameter->n_HLayer=n_float;
		//n_totalLayers=(int)((zmax-zmin)/n_float);    //计算出来的总层数
		read(fd, &n_float, 4);    //Line Width Compensation
		read(fd, &n_float, 4);    //Reserved
		m_data--;
	}
	
	int dex=0;
	int dx=0,dy=0;//偏移参数
	
	
	// dx=pixel_bits_x/2-zoom_x*(minx+maxx)/2;
	// dy=pixel_bits_y/2-zoom_y*(miny+maxy)/2;
	dx=pixel_bits_x/2-zoom_x*(minxReal+maxxReal)/2;
	dy=pixel_bits_y/2-zoom_y*(minyReal+maxyReal)/2;	
	

	char colorDir;
		
	// /******************************************************************/
	// /*************************处理轮廓数据部分*************************/
	while(1)
	{	
		read(fd, &n_layer, 4);
		read(fd, &n_boundary, 4);
		if(n_boundary==0xFFFFFFFF)  //结束符
			break;
		
		dex++;
		
		fprintf(fd_temp,"第%d层\r\n",dex);
		fprintf(fd_temp,"Z轴高度=%.5f\r\n",n_layer);
		fprintf(fd_temp,"轮廓数=%d\r\n",n_boundary);
		
		for(i=0;i<n_boundary;i++)   //把同一层多个轮廓都放在同一容器中，
		{                           //显示跟数据处理时 要根据起始点和同轮廓的终点相等来判断是否为同一轮廓  
			read(fd, &n_vertices, 4);//一个轮廓环中的点数

			fprintf(fd_temp,"第%d个轮廓环中的点数=%d\r\n",i+1,n_vertices);

			read(fd, &n_gaps, 4);

			contour.clear();//删除容器中的所有元素
			for(j=0;j<n_vertices;j++)
			{
				read(fd, &n_polylineX, 4);
				read(fd, &n_polylineY, 4);
				
				fprintf(fd_temp,"{%ld,",(long)(n_polylineX*zoom_x+dx));//偏移后的坐标放大，保存调试数据到文件
				fprintf(fd_temp,"%ld}\r\n",(long)(n_polylineY*zoom_y+dy));
				
				
				if(mirrorBmpFlag==0)
				{
					colorDir=1;
					contour.push_back(Point((long)(pixel_bits_x-(n_polylineX*zoom_x+dx)),(long)(n_polylineY*zoom_y+dy))); //向轮廓坐标尾部添加点坐标，坐标关于某轴进行翻转时，顺逆指针会发生变化
				}
				else if((mirrorBmpFlag==1))
				{
					colorDir=-1;
					contour.push_back(Point((long)(n_polylineX*zoom_x+dx),(long)(n_polylineY*zoom_y+dy))); //向轮廓坐标尾部添加点坐标，坐标关于某轴进行翻转时，顺逆指针会发生变化
				}
				
			}

			v_contour.push_back(contour);//追加当前轮廓数据到当前层容器变量中
			contour.clear();//删除容器中的所有元素		  
		}
		

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//通过冒泡法实现容器中轮廓的排序，使得较小轮廓始终位于较大轮廓后，能够判断是否出现交叉异常(注：两个分离的轮廓也会进行排序，不影响填充）
		int n; //需要排序的轮廓个数
		n=v_contour.size();//获取轮廓的个数

		for(size_t cmpnum = n-1; cmpnum != 0; --cmpnum)
		{
			for (size_t i = 0; i != cmpnum; ++i)
			{	
				for(size_t k=0;k<v_contour[i+1].size();k++)
				{
					flag_swap=pointPolygonTest(v_contour[i], v_contour[i+1][k], false); // 对于每个点都去检测 
					flag_swap_vector.push_back(flag_swap);
				}

				for(size_t z=0;z<flag_swap_vector.size()-1;z++)
				{
					if(flag_swap_vector[z]!=flag_swap_vector[z+1])
					{
						// printf("有存在交叉现象\r\n");
						//这里应该去做相应的异常处理
					}
				}
				
				flag_swap_vector.clear();//删除容器中的所有元素
				
				if (flag_swap == -1)
				{
					swap(v_contour[i],v_contour[i+1]);
				}
			}
		}
			
//////////////////////////////////////////////////////////////////////////////////////////////////////////////		
			
		//清除图像
		dst.setTo(Scalar(0));//把像素点值清零
		
		for(i=0;i<n_boundary;i++)   //把同一层多个轮廓都放在同一容器中
		{                           //显示跟数据处理时 要根据起始点和同轮廓的终点相等来判断是否为同一轮廓
			d = 0;
			for (size_t j = 0; j < v_contour[i].size()-1; j++)
			{
				d += -0.5*(v_contour[i][j+1].y+v_contour[i][j].y)*(v_contour[i][j+1].x-v_contour[i][j].x);
			}
		
			// a) 存放单通道图像中像素：cvScalar(255);
			// b) 存放三通道图像中像素：cvScalar(255,255,255);
			if((d*colorDir) > 0)
			{
				//cout << "逆时针：counterclockwise"<< endl;
				fprintf(fd_temp,"逆时 \r\n");
				//填充白色
				color=cvScalar(0);
			}
			else
			{
				//cout << "顺时针：clockwise" << endl;
				fprintf(fd_temp,"顺时 \r\n");
				//填充黑色
				color=cvScalar(255);
			}	
			drawContours( dst,v_contour ,i, color, CV_FILLED );
		}

		imwrite(fileDesDir,dst);
		
///////////////////////////////////////////////////////
		uint8_t destBuf[3000000]={0};
		uLongf destLen = 3000000;
		
		inFile = fopen(fileDesDir, "rb");
		
		sprintf(str,"/mnt/hgfs/share/USR/dlpModule/webModule/www/cgi-bin/result/indexed-color-image-uncompress%d.data",dex);
		outFile	= fopen(str, "wb");
		
		fseek(inFile, 0L, SEEK_END);
		long size2 = ftell(inFile);
		fseek(inFile, 0L, SEEK_SET);
		uint8_t dataBuf2[size2];
		fread(dataBuf2, size2, 1, inFile);		
		printf("待压缩的文件大小：%ld\n", size2);
		printf("目标压缩文件名:%s\r\n",str);
		
		compress(destBuf, &destLen, dataBuf2, size2);
		fwrite(destBuf, destLen, 1, outFile);
		
		fflush(outFile);
		fclose(inFile);
		fclose(outFile);		
////////////////////////////////////////////////////////	
		destLen = 3000000;

		sprintf(str,"/mnt/hgfs/share/USR/dlpModule/webModule/www/cgi-bin/result/indexed-color-image-uncompress%d.data",dex);
		inFile = fopen(str, "rb");
		sprintf(str,"/mnt/hgfs/share/USR/dlpModule/webModule/www/cgi-bin/result/indexed-color-image-uncompress%d.png",dex);
		outFile	= fopen(str, "wb");
		
		fseek(inFile, 0L, SEEK_END);
		long size = ftell(inFile);
		fseek(inFile, 0L, SEEK_SET);
		uint8_t dataBuf[size];
		fread(dataBuf, size, 1, inFile);
		printf("待解压缩文件大小：%ld\n", size);
	
		uncompress(destBuf, &destLen, dataBuf, size);
		printf("解压后大小：%ld\n", destLen);
		fwrite(destBuf, destLen, 1, outFile);

		fflush(outFile);
		fclose(inFile);
		fclose(outFile);		
///////////////////////////////////////////////////////
		
		v_contour.clear();//删除容器中的所有元素，这里的元素是同一层中所有轮廓数据
		
		printf("第%d层\r\n",dex);
		printf("BMP_OK\r\n");	
	}
	printf("%s\r\n",fileDesDir);
	
	fclose(fd_temp);
	close(fd);
}

void printHelp()
{
    printf("Unknown command.Show json method\n");
	printf("method 1:\n");
	printf("{\"fileToResult\": \"/home/jhon/share/USR-master/dlpModule/SlcFileReadWeb/www/cgi-bin/bLayer130.slc\",\"fileNameDirOut\":\"/home/jhon/share/USR-master/dlpModule/SlcFileReadWeb/www/cgi-bin/bLayer130.bmp\",\"pixel_bits_x\":1920,\"pixel_bits_y\":1080,\"zoom_x\":20,\"zoom_y\":20,\"mirrorBmpFlag\":0}");
	printf("\n");
	printf("method 2:\n");
	printf("{\"fileInf\":\"/home/jhon/share/USR-master/dlpModule/SlcFileReadWeb/www/cgi-bin/bLayer130.slc\"}");
    printf("\n");
}


int main(void){
	long len;//允许单个json文件最大32-1位长度,最大2147483647
	char *lenstr;// lenstr=(char *)malloc(sizeof(char) * 10);//内存申请,允许单个json文件最大32-1位长度，最大2147483647

	printf("Content-Type:application/json\n\n");
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
	memset(poststr,'\0',len);

	
	
	if (poststr == NULL)
	{
		printf("Fail to allocate memory to wid\r\n");
		fflush(stdout);
		return -1;
	}

	fgets(poststr,len+1,stdin);


	// printf("%s\r\n",poststr);
	//printf("len:  %ld\r\n",len);
	// fflush(stdout);
	// free(poststr); //释放内存
	// return 0;

	char existentJSONFlag=0;
	char* value=(char *)malloc(sizeof(char) * len);//内存申请;
	char* fileNameDirOut=(char *)malloc(sizeof(char) * len);//内存申请;
	
	

	if(cjson_analysisState(poststr,"fileInf",value) != NULL)//{"fileInf": "DIRSOURCE"}
    {
		ReadSLC_inf(value);
		existentJSONFlag=1;
    }
    if(cjson_analysisState(poststr,"fileToResult",value) != NULL)//{"fileToResult":"DIRSOURCE","fileNameDirOut":"DIRRESULT"}
    {
		if(cjson_ParameterAnalysis(poststr,fileNameDirOut) != -1)//打印参数获取
		{
			OpenSLCToResult(value,fileNameDirOut);
			existentJSONFlag=1;
		}
    }

    if(existentJSONFlag == 0)//未识别到命令
    {
		printHelp();
    }
	
	fflush(stdout);
	
	free(poststr); //释放内存
	free(value);   //释放内存
	free(fileNameDirOut);   //释放内存
	
	return 0;
	
}




/*
json

{"fileToResult": "/home/jhon/share/USR-master/dlpModule/SlcFileReadWeb/www/cgi-bin/bLayer130.slc","fileNameDirOut":"/home/jhon/share/USR-master/dlpModule/SlcFileReadWeb/www/cgi-bin/bLayer130.bmp","pixel_bits_x":1920,"pixel_bits_y":1080,"zoom_x":20,"zoom_y":20,"mirrorBmpFlag":0}

{"fileInf":"/home/jhon/share/USR-master/dlpModule/SlcFileReadWeb/www/cgi-bin/bLayer130.slc"}


*/

int FindString(char pSrc[], int srcSize, char pDest[], int dstSize)
{
    int iFind = -1;
    for(size_t i=0;i<(size_t)srcSize;i++){
        int iCnt = 0;
        for (size_t j=0; j<(size_t)dstSize; j++) {
            if(pDest[j] == pSrc[i+j])
                iCnt++;
        }
        if (iCnt==dstSize) {
            iFind = i;
            break;
        }
    }
    return iFind;//返回比对的字符起始位置
}


void ReadSLC_inf(const char file_dir[])//获取文件信息
{
	/************处理文件头的字符变量，解析完毕后，关闭当前文件*********************/
	char str_temp[300];
	FILE * fid = fopen(file_dir,"r");//用于处理文件头的信息
	// printf("文件打开成功\r\n");
	//此处添加文件头处理方式
	fread(str_temp,sizeof(char),300,fid);
	//-EXTENTS <minx,maxx miny,maxy minz,maxz> CAD模型 x,y,z轴的范围
	int offset = FindString(str_temp,300,(char *)"-EXTENTS",8);
	strncpy(str_temp,str_temp+offset,100);//提取出XYZ的范围数据
	
	//提取XYZ的范围（前6个浮点值）
	sscanf(str_temp, "%*[(A-Z)|^-]%f%*[(^ )|(^,)]%f%*[(^ )|(^,)]%f%*[(^ )|(^,)]%f%*[(^ )|(^,)]%f%*[(^ )|(^,)]%f%*[(A-Z)|^*]", &minx, &maxx, &miny, &maxy, &minz, &maxz);
	fclose(fid);
	/*********************************/

	int fd; 		//文件描述符
	char m_data;	//读取到的数据
	// int size=0;		//读取到的数据长度

	fd = open(file_dir, O_RDONLY);
	//////////////////////////////

	unsigned int i=0;
	unsigned int j=0;

	unsigned int n_boundary,n_vertices,n_gaps;
	float   n_float,n_layer;

	float   n_polylineX,n_polylineY;
	
    /**************************处理头文件部分**************************/
	while(1)
	{
		i++;
		if(i==2048)
		{
			printf("file error\r\n");
			close(fd);
			return;
		}

		read(fd, &m_data, 1);
		// printf("m_data=%x\r\n", m_data);

		switch(m_data)
		{
		case 0x0d:
			j=1;
			break;
		case 0x0a:
			if(j==1)
				j=2;
			break;
		case 0x1a:
			if(j==2)
				j=3;
			break;
		default:
			j=0;
			break;
		}
		if(j==3)
			break;
	}
	
	// printf("size=%d\r\n", size);
	/******************************************************************/
	/***************************处理预留部分***************************/
	for(i=0;i<256;i++)
	{
		read(fd, &m_data, 1);
	}
	/******************************************************************/
	/**************************处理样本表部分**************************/
	read(fd, &m_data, 1);
	printf("{\r\n");
	printf("	");
	printf("\"Sampling Table Size\":%x,\r\n", m_data);
	while(m_data)
	{
		read(fd, &n_float, 4);//Minimum Z Level
		read(fd, &n_float, 4);//Layer Thickness
		// printf("Layer Thickness=%.5f\r\n",n_float);
		thickness=n_float;//读取层厚
		
		// m_parameter->n_HLayer=n_float;
		//n_totalLayers=(int)((zmax-zmin)/n_float);    //计算出来的总层数
		read(fd, &n_float, 4);    //Line Width Compensation
		read(fd, &n_float, 4);    //Reserved
		m_data--;
	}
	
	float layerInint=0;
	int dex=0;
	// /******************************************************************/
	// /*************************处理轮廓数据部分*************************/
	while(1)
	{
		read(fd, &n_layer, 4);//Z轴高度
		read(fd, &n_boundary, 4);//轮廓数
		if(n_boundary==0xFFFFFFFF)  //结束符
			break;
		dex++;
		if(dex==1) {layerInint=n_layer;}

		for(i=0;i<n_boundary;i++)   //把同一层多个轮廓都放在同一容器中，
		{                           //显示跟数据处理时 要根据起始点和同轮廓的终点相等来判断是否为同一轮廓  
			read(fd, &n_vertices, 4);//一个轮廓环中的点数
			read(fd, &n_gaps, 4);
			for(j=0;j<n_vertices;j++)
			{
				read(fd, &n_polylineX, 4);
				minxReal=(minxReal<n_polylineX?minxReal:n_polylineX);
				maxxReal=(maxxReal>n_polylineX?maxxReal:n_polylineX);
				read(fd, &n_polylineY, 4);
				minyReal=(minyReal<n_polylineY?minyReal:n_polylineY);
				maxyReal=(maxyReal>n_polylineY?maxyReal:n_polylineY);
			}
		}
		// printf("第%d层\r\n",dex);
		// printf("BMP_OK\r\n");	
	}
	close(fd);
	totalLayers=dex;
	
	minzReal = layerInint - thickness;
	maxzReal = layerInint + totalLayers*thickness;
	
	printf("	");
	printf("\"thickness\":%f,\r\n",thickness);
	
	printf("	");
	printf("\"totalLayers\":%ld,\r\n",totalLayers);
	
	printf("	");
	printf("\"layerInint\":%f,\r\n",layerInint);
	
	printf("	");
	printf("\"minx\":%f,\"maxx\":%f,\"miny\":%f,\"maxy\":%f,\"minz\":%f,\"maxz\":%f,\r\n", minx,maxx,miny,maxy, minz, maxz);
	printf("	");
	printf("\"minxReal\":%f,\"maxxReal\":%f,\"minyReal\":%f,\"maxyReal\":%f,\"minzReal\":%f,\"maxzReal\":%f\r\n", minxReal, maxxReal, minyReal, maxyReal, minzReal, maxzReal);

	printf("}\r\n");
}


void OpenSLC_1(const char file_dir[],long layer)//指定的单层解析,模型尺寸区间以文件头标识为准
{
	printf("file_dir=%s\r\nlayer=%ld\r\n",file_dir,layer);
	// return;
	
	int dx=0,dy=0;
	
	/************处理文件头的字符变量，解析完毕后，关闭当前文件*********************/
	//关于文件的文件头变量
	char str_temp[300];

	FILE * fid = fopen(file_dir,"r");//用于处理文件头的信息
	if(fid == NULL)
    {
        return;
    }
	
	//此处添加文件头处理方式
	fread(str_temp,sizeof(char),300,fid);
	//-EXTENTS <minx,maxx miny,maxy minz,maxz> CAD模型 x,y,z轴的范围
	int offset = FindString(str_temp,300,(char *)"-EXTENTS",8);
	strncpy(str_temp,str_temp+offset,300);//提取出XYZ的范围数据
	
	//提取XYZ的范围（前6个浮点值）
	char str[100];//无关变量
	sscanf(str_temp, "%[(A-Z)|^-]%f%[(^ )|(^,)]%f%[(^ )|(^,)]%f%[(^ )|(^,)]%f%[(^ )|(^,)]%f%[(^ )|(^,)]%f%[(A-Z)|^*]", str, &minx, str, &maxx, str, &miny, str, &maxy, str, &minz, str, &maxz, str);
	
	// printf("minx=%.3f,maxx=%.3f,miny=%.3f,maxy=%.3f,minz=%.3f,maxz%.3f\r\n", minx,maxx,miny,maxy,minz,maxz);

	// printf("%s\r\n", str_temp);
	fclose(fid);
	/*********************************************************************************/
	
	
	int fd; 		//文件描述符
	char m_data;	//读取到的数据
	float d = 0;    //d > 0 从上往下看是逆时针
	// int size=0;		//读取到的数据长度
	
	fd = open(file_dir, O_RDONLY);
	//////////////////////////////
	
	/*
	// O_CREAT 若欲打开的文件不存在则自动建立该文件。
	// O_RDONLY 以只读方式打开文件
	// O_WRONLY 以只写方式打开文件
	*/
	
	//【1】CV_8UC1---则可以创建----8位无符号的单通道---灰度图片------grayImg
	//【2】CV_8UC3---则可以创建----8位无符号的三通道---RGB彩色图像---colorImg 
	//【3】CV_8UC4--则可以创建-----8位无符号的四通道---带透明色的RGB图像 
	Mat dst = Mat::zeros(pixel_bits_y, pixel_bits_x, CV_8UC1);//生成的图片，其分辨率由实际的FrameBuffer来决定
	CvScalar color=cvScalar(0);
	
	vector<Point> contour;       	 //单个轮廓坐标值
	vector<vector<Point>> v_contour; //当前层所有轮廓集合
	
	
	vector<int> flag_swap_vector;	//轮廓排序用
	vector<vector<Point>> vctint;	//轮廓排序用
	
	float flag_swap=0;				//轮廓排序用
	

	unsigned int i=0;
	unsigned int j=0;

	unsigned int n_boundary,n_vertices,n_gaps;
	float   n_float,n_layer;

	float   n_polylineX,n_polylineY;
	
    /**************************处理头文件部分**************************/
	while(1)
	{
		i++;
		if(i==2048)
		{
			printf("file error\r\n");
			close(fd);
			return;
		}

		read(fd, &m_data, 1);
		// printf("m_data=%x\r\n", m_data);

		switch(m_data)
		{
		case 0x0d:
			j=1;
			break;
		case 0x0a:
			if(j==1)
				j=2;
			break;
		case 0x1a:
			if(j==2)
				j=3;
			break;
		default:
			j=0;
			break;
		}
		if(j==3)
			break;
	}
	
	// printf("size=%d\r\n", size);
	/******************************************************************/
	/***************************处理预留部分***************************/
	for(i=0;i<256;i++)
	{
		read(fd, &m_data, 1);
	}
	/******************************************************************/
	/**************************处理样本表部分**************************/
	read(fd, &m_data, 1);
	printf("Sampling Table Size=%x\r\n", m_data);
	while(m_data)
	{
		read(fd, &n_float, 4);//Minimum Z Level
		read(fd, &n_float, 4);//Layer Thickness
		// printf("Layer Thickness=%.5f\r\n",n_float);
		
		// m_parameter->n_HLayer=n_float;
		//n_totalLayers=(int)((zmax-zmin)/n_float);    //计算出来的总层数
		read(fd, &n_float, 4);    //Line Width Compensation
		read(fd, &n_float, 4);    //Reserved
		m_data--;
	}
	
	int dex;
	
	dx=pixel_bits_x/2-zoom_x*(minx+maxx)/2;
	dy=pixel_bits_y/2-zoom_y*(miny+maxy)/2;

	char colorDir;
	
	dex=0;
	// /******************************************************************/
	// /*************************处理轮廓数据部分*************************/
	while(1)
	{
		read(fd, &n_layer, 4);//Z轴高度
		read(fd, &n_boundary, 4);//轮廓数
		if(n_boundary==0xFFFFFFFF)  //结束符
			break;
		dex++;

		for(i=0;i<n_boundary;i++)   //把同一层多个轮廓都放在同一容器中，
		{                           //显示跟数据处理时 要根据起始点和同轮廓的终点相等来判断是否为同一轮廓
			read(fd, &n_vertices, 4);//一个轮廓环中的点数
			read(fd, &n_gaps, 4);
			for(j=0;j<n_vertices;j++)
			{
				read(fd, &n_polylineX, 4);
				read(fd, &n_polylineY, 4);

				if(dex==layer)
				{
					if(mirrorBmpFlag==0)
					{
						colorDir=1;
						contour.push_back(Point((long)(pixel_bits_x-(n_polylineX*zoom_x+dx)),(long)(n_polylineY*zoom_y+dy))); //向轮廓坐标尾部添加点坐标，坐标关于某轴进行翻转时，顺逆指针会发生变化
					}
					else if((mirrorBmpFlag==1))
					{
						colorDir=-1;
						contour.push_back(Point((long)(n_polylineX*zoom_x+dx),(long)(n_polylineY*zoom_y+dy))); //向轮廓坐标尾部添加点坐标，坐标关于某轴进行翻转时，顺逆指针会发生变化
					}
				}
			}
			if(dex==layer)
			{
				v_contour.push_back(contour);//追加当前轮廓数据到当前层容器变量中
				contour.clear();//删除容器中的所有元素
			}
		}
		
		if(dex==layer)
		{
			//通过冒泡法实现容器中轮廓的排序，使得较小轮廓始终位于较大轮廓后，能够判断是否出现交叉异常(注：两个分离的轮廓也会进行排序，不影响填充）
			int n; //需要排序的轮廓个数
			n=v_contour.size();//获取轮廓的个数

			for(size_t cmpnum = n-1; cmpnum != 0; --cmpnum)
			{
				for (size_t i = 0; i != cmpnum; ++i)
				{	
					for(size_t k=0;k<v_contour[i+1].size();k++)
					{
		
						flag_swap=pointPolygonTest(v_contour[i], v_contour[i+1][k], false); // 对于每个点都去检测 
						flag_swap_vector.push_back(flag_swap);
					}

					for(size_t z=0;z<flag_swap_vector.size()-1;z++)
					{
						if(flag_swap_vector[z]!=flag_swap_vector[z+1])
						{
							// printf("有存在交叉现象\r\n");
							// //这里应该去做相应的异常处理
						}
					}
					
					flag_swap_vector.clear();//删除容器中的所有元素
					
					if (flag_swap == -1)
					{
						swap(v_contour[i],v_contour[i+1]);
					}
				}
			}			
			
//////////////////////////////////////////////////////////////////////////////////////////////////////////////				
			//清除图像
			dst.setTo(Scalar(0));//把像素点值清零
			
			for(i=0;i<n_boundary;i++)   //把同一层多个轮廓都放在同一容器中，
			{                           //显示跟数据处理时 要根据起始点和同轮廓的终点相等来判断是否为同一轮廓  
				d = 0;
				for (size_t j = 0; j < v_contour[i].size()-1; j++)
				{
					d += -0.5*(v_contour[i][j+1].y+v_contour[i][j].y)*(v_contour[i][j+1].x-v_contour[i][j].x);
				}
			
				// a) 存放单通道图像中像素：cvScalar(255);
				// b) 存放三通道图像中像素：cvScalar(255,255,255);
				if((d*colorDir) > 0)
				{
					//cout << "逆时针：counterclockwise"<< endl;
					//填充白色
					color=cvScalar(0);
				}
				else
				{
					//cout << "顺时针：clockwise" << endl;
					//填充黑色
					color=cvScalar(255);
				}	
				drawContours( dst,v_contour ,i, color, CV_FILLED );		
			}

			imwrite("./dst.bmp",dst);
			v_contour.clear();//删除容器中的所有元素，这里的元素是同一层中所有轮廓数据
			// printf("第%d层\r\n",dex);
			printf("transition OK\r\n");
			// printf("BMP_OK\r\n");	
			break;			
		}
	}
	close(fd);
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

char* cjson_analysisState(const char *order_str, const char * key, char* value)//返回json配对值参数，value作为输出返回
{
	cJSON *root;
	root = cJSON_Parse((const char*)order_str);
	if(!root)
	{
		//printf("non-existent JSON:%s\r\n", cJSON_GetErrorPtr());
		cJSON_Delete(root);
		return NULL;
	}
	else
	{
		//printf("existent JSON:\r\n%s\r\n",cJSON_Print(root));
		
		if(cJSON_GetObjectItem(root,key)==NULL){
			cJSON_Delete(root);
			// printf("non-existent key\r\n");
			return NULL;
		}
		else
		{
			strcpy(value,cJSON_GetObjectItem(root,key)->valuestring);//键值对
			// if(value != NULL)
			// {
				// printf("value:%s\r\n",value);
			// }	
		}
	}
	cJSON_Delete(root);
	return value;
}

int cjson_ParameterAnalysis(const char *order_str,char* fileNameDirOut)//fileNameDirOut为json中的文件名，作为输出返回，其他例如zoom_x的全部参数如果有也同步更新
{
	cJSON *root;
	root = cJSON_Parse((const char*)order_str);
	if(!root)
	{
		printf("non-existent JSON:%s\r\n", cJSON_GetErrorPtr());
		cJSON_Delete(root);
		return -1;
	}
	else
	{
		//printf("existent JSON:\r\n%s\r\n",cJSON_Print(root));

		if(cJSON_GetObjectItem(root,"fileNameDirOut")==NULL){
			cJSON_Delete(root);
			// printf("non-existent key\r\n");
			return -1;
		}
		
		if(cJSON_GetObjectItem(root,"pixel_bits_x")==NULL){
			cJSON_Delete(root);
			// printf("non-existent key\r\n");
			return -1;
		}
		if(cJSON_GetObjectItem(root,"pixel_bits_y")==NULL){
			cJSON_Delete(root);
			// printf("non-existent key\r\n");
			return -1;
		}
		if(cJSON_GetObjectItem(root,"zoom_x")==NULL){
			cJSON_Delete(root);
			// printf("non-existent key\r\n");
			return -1;
		}
		if(cJSON_GetObjectItem(root,"zoom_y")==NULL){
			cJSON_Delete(root);
			// printf("non-existent key\r\n");
			return -1;
		}
		if(cJSON_GetObjectItem(root,"mirrorBmpFlag")==NULL){
			cJSON_Delete(root);
			// printf("non-existent key\r\n");
			return -1;
		}

		strcpy(fileNameDirOut,cJSON_GetObjectItem(root,"fileNameDirOut")->valuestring);//键值对
		//printf("fileNameDirOut:%s\r\n",fileNameDirOut);
		
		pixel_bits_x=cJSON_GetObjectItem(root,"pixel_bits_x")->valueint;//键值对
		//printf("pixel_bits_x:%d\r\n",pixel_bits_x);
		
		pixel_bits_y=cJSON_GetObjectItem(root,"pixel_bits_y")->valueint;//键值对
		//printf("pixel_bits_y:%d\r\n",pixel_bits_y);

		zoom_x=cJSON_GetObjectItem(root,"zoom_x")->valuedouble;//键值对
		//printf("zoom_x:%f\r\n",zoom_x);

		zoom_y=cJSON_GetObjectItem(root,"zoom_y")->valuedouble;//键值对
		//printf("zoom_y:%f\r\n",zoom_y);

		mirrorBmpFlag=cJSON_GetObjectItem(root,"mirrorBmpFlag")->valueint;//键值对,图像对称标识
		//printf("mirrorBmpFlag:%d\r\n",mirrorBmpFlag);
	}
	cJSON_Delete(root);
	return 0;
}



void strip(char *s)//将非法字符去除，重新链接字符串，并以'\0'作为结尾 https://blog.csdn.net/GeneralJing/article/details/115012537
{
    size_t i;
    size_t len = strlen(s);
    size_t offset = 0;
    for(i = 0; i < len; ++i){
        char c = s[i];
        if(c=='\r'||c==' '||c=='\t'||c=='\n') ++offset;
        else s[i-offset] = c;
    }
    s[len-offset] = '\0';
}



/* // 顺逆时针判断（从上往下） https://blog.csdn.net/qq_37602930/article/details/80496498  */
/*	

vector<Point> contour;       	 //单个轮廓坐标值
vector<vector<Point>> v_contour; //当前层所有轮廓集合
	
contour.clear();//删除容器中的所有元素
contour = {{300,760},{292,748},{291,747},{288,747},{279,746},{269,758},{268,758},{261,745},{260,745},
{260,744},{251,742},{249,742},{238,754},{237,754},{230,739},{229,739},{229,739},{229,738},{230,737},{242,724},
{242,724},{249,737},{263,737},{271,728},{277,737},{295,736},{300,729},{305,736},{323,737},{329,728},{337,737},
{351,737},{358,724},{358,724},{371,737},{371,738},{371,739},{371,739},{370,739},{363,754},{362,754},{351,742},
{349,742},{340,744},{340,745},{339,745},{332,758},{331,758},{321,746},{312,747},{309,747},{308,748},{300,760}};
v_contour.push_back(contour);

contour.clear();//删除容器中的所有元素
contour.push_back(Point(300,760)); //向轮廓坐标尾部添加点坐标
v_contour.push_back(contour);

printf("v_contour.size()=%d\r\n",v_contour.size());//查看容器的大小


contour.clear();//删除容器中的所有元素
// contour.push_back(Point(300,760)); //向轮廓坐标尾部添加点坐标
contour = {{300,760},{292,748},{291,747},{288,747},{279,746},{269,758},{268,758},{261,745},{260,745},
{260,744},{251,742},{249,742},{238,754},{237,754},{230,739},{229,739},{229,739},{229,738},{230,737},{242,724},
{242,724},{249,737},{263,737},{271,728},{277,737},{295,736},{300,729},{305,736},{323,737},{329,728},{337,737},
{351,737},{358,724},{358,724},{371,737},{371,738},{371,739},{371,739},{370,739},{363,754},{362,754},{351,742},
{349,742},{340,744},{340,745},{339,745},{332,758},{331,758},{321,746},{312,747},{309,747},{308,748},{300,760}};
v_contour.push_back(contour);
printf("contour.size()=%d\r\n",contour.size());//查看容器的大小
printf("v_contour.size()=%d\r\n",v_contour.size());
contour.clear();//删除容器中的所有元素

contour.clear();//删除容器中的所有元素
// contour.push_back(Point(300,760)); //向轮廓坐标尾部添加点坐标
contour = {{205,748},{199,731},{199,730},{213,718},{213,718},{217,729},{220,735},{219,736},{206,748},{205,748}};
v_contour.push_back(contour);
printf("contour.size()=%d\r\n",contour.size());//查看容器的大小
printf("v_contour.size()=%d\r\n",v_contour.size());
contour.clear();//删除容器中的所有元素

contour.clear();//删除容器中的所有元素
// contour.push_back(Point(300,760)); //向轮廓坐标尾部添加点坐标
contour = {{394,748},{381,736},{380,735},{383,728},{387,718},{389,720},{401,730},{396,743},{395,748},{394,748}};
v_contour.push_back(contour);
printf("contour.size()=%d\r\n",contour.size());//查看容器的大小
printf("v_contour.size()=%d\r\n",v_contour.size());
contour.clear();//删除容器中的所有元素


// contour.push_back(Point(300,760)); //向轮廓坐标尾部添加点坐标
// contour.clear();//删除容器中的所有元素
// contour.pop_back();//删除容器中的最后一个元素
// v_contour.insert(v_contour.begin(),contour);//在当前轮廓组的首个轮廓前插入一个轮廓
	
/ color=cvScalar(rand()&255, rand()&255, rand()&255 );
*/


	
