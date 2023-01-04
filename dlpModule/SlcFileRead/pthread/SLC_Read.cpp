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
#include <stdio.h>
#include <iostream>  
#include <sys/ioctl.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <pthread.h>  	//包线程要包含

//////////////////////////////////////
#include "parameter.h"
#include "SLC_Read.h"


using namespace cv;
using namespace std;


// FILE *fd_temp;  //将数据保存到文本区


char file_continue_flag = 0; //解析标志，为1则继续解析，覆盖原来的图片


//字符数组中查找字符串或字符数组
//pSrc:原字符
//srcSize:原字符长度
//pDest：比对的字符
//dstSize:比对的字符的长度
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

/*
非线性拉伸相关资料
opencv remap基础例子: https://blog.csdn.net/newchenxf/article/details/51385797
OpenCV 图像处理——非线性拉伸: https://seanwangjs.github.io/2019/07/15/opencv-image-proccess-nonlinear-stretch.html
matlab图像局部放大算法: https://blog.csdn.net/gaoyueace/article/details/78675342
opencv 拉伸、扭曲、旋转图像-透视变换: https://blog.csdn.net/u012005313/article/details/46715397
*/
//imageRemap函数为图像线性变换demo，这里的imageRemap函数内容需要根据实际情况调整
int imageRemap(Mat mImage)
{
	float* ptrX;
	float* ptrY;
	
	if (mImage.data == 0)
	{
		cerr << "Image reading error !" << endl;
		system("pause");
		return -1;
	}

	Mat mResult = mImage.clone();
	//mMapX和mMapY的数据类型必须是 CV_32FC1
	Mat mMapX(mImage.rows, mImage.cols, CV_32FC1, Scalar(0));
	Mat mMapY(mImage.rows, mImage.cols, CV_32FC1, Scalar(0));
	int Rows = mImage.rows;
	int Cols = mImage.cols;

////////////////////////////////////////////////
//计算带入值
	float dim[Rows>Cols?Rows:Cols];
	float dim2[Rows>Cols?Rows:Cols];
	float i_temp;	
	for (int i = 0; i <(Rows>Cols?Rows:Cols); i++)
	{
		// dim[i] = pow(i,1.001);
		i_temp=((float)(i))/8750;
		dim[i] =8750*(i_temp*(i_temp*i_temp+3)/3);
	}

	for (int i = 0; i <(Rows>Cols?Rows:Cols); i++)
	{
		//yy=(k.*(k.^2 + 3))/3
		i_temp=((float)(i))/6480;
		dim2[i] =6300*(i_temp*(i_temp*i_temp+3)/3);
	}
////////////////////////////////////////////////

	// for (int i = 0; i <Rows; i++)
	// {
		// float* ptrX = mMapX.ptr<float>(i);
		// float* ptrY = mMapY.ptr<float>(i);
		// for (int j = 0; j < Cols; j++)
		// {
			//(1)左右翻转
			// ptrX[j] =(float)(Cols - j);
			// ptrY[j] = (float) i;
			
			//(2)上下翻转
			// ptrX[j] = (float)j;
			// ptrY[j] = (float)(Rows - i);
			
			//(3)横向缩放
			// ptrX[j] =(float)(2*j);
			// ptrY[j] = (float) i;
			
			//(4)纵向缩放
			// ptrX[j] =(float) j;
			// ptrY[j] = (float)(2*i);
			
			//(5)横向非线性拉伸
			// ptrX[j] =(float)(pow(j,1.01));
			// ptrY[j] = (float) i;		
			
			//(6)纵向非线性拉伸
			// ptrX[j] =(float) j;
			// ptrY[j] = (float)(pow(i,1.01));	
		// }
	// }
	// remap(mImage, mResult, mMapX, mMapY, INTER_LINEAR);//重映射
	// mImage=mResult;	
	
	for (int i = 0; i <Rows; i++)
	{
		ptrX = mMapX.ptr<float>(i);
		ptrY = mMapY.ptr<float>(i);
		for (int j = 0; j < Cols; j++)
		{
			//(1)上下翻转
			ptrX[j] = (float)j;
			ptrY[j] = (float)(Rows - i);
		}
	}
	remap(mImage, mResult, mMapX, mMapY, INTER_LINEAR);//重映射
	mImage=mResult;	
	
	for (int i = 0; i <Rows; i++)
	{
		ptrX = mMapX.ptr<float>(i);
		ptrY = mMapY.ptr<float>(i);
		for (int j = 0; j < Cols; j++)
		{
			//(2)纵向非线性拉伸
			ptrX[j] =(float) j;
			ptrY[j] = dim[i];
		}
	}
	remap(mImage, mResult, mMapX, mMapY, INTER_LINEAR);//重映射
	mImage=mResult;
	
	for (int i = 0; i <Rows; i++)
	{
		ptrX = mMapX.ptr<float>(i);
		ptrY = mMapY.ptr<float>(i);
		for (int j = 0; j < Cols; j++)
		{
			//(3)上下翻转
			ptrX[j] = (float)j;
			ptrY[j] = (float)(Rows - i);
		}
	}
	remap(mImage, mResult, mMapX, mMapY, INTER_LINEAR);//重映射
	mImage=mResult;
	
	for (int i = 0; i <Rows; i++)
	{
		ptrX = mMapX.ptr<float>(i);
		ptrY = mMapY.ptr<float>(i);
		for (int j = 0; j < Cols; j++)
		{
			//(4)横向非线性拉伸
			ptrX[j] = dim2[j];
			ptrY[j] = (float) i;	
		}
	}
	remap(mImage, mResult, mMapX, mMapY, INTER_LINEAR);//重映射
	mImage=mResult;
	
	for (int i = 0; i <Rows; i++)
	{
		ptrX = mMapX.ptr<float>(i);
		ptrY = mMapY.ptr<float>(i);
		for (int j = 0; j < Cols; j++)
		{
			//(5)左右翻转
			ptrX[j] =(float)(Cols - j);
			ptrY[j] = (float) i;
		}
	}
	remap(mImage, mResult, mMapX, mMapY, INTER_LINEAR);//重映射
	mImage=mResult;
	
	for (int i = 0; i <Rows; i++)
	{
		ptrX = mMapX.ptr<float>(i);
		ptrY = mMapY.ptr<float>(i);
		for (int j = 0; j < Cols; j++)
		{
			//(6)横向非线性拉伸
			ptrX[j] = dim2[j];
			ptrY[j] = (float) i;
		}
	}
	remap(mImage, mResult, mMapX, mMapY, INTER_LINEAR);//重映射
	mImage=mResult;
	
	for (int i = 0; i <Rows; i++)
	{
		ptrX = mMapX.ptr<float>(i);
		ptrY = mMapY.ptr<float>(i);
		for (int j = 0; j < Cols; j++)
		{
			//(7)左右翻转
			ptrX[j] =(float)(Cols - j);
			ptrY[j] = (float) i;
		}
	}		
	remap(mImage, mResult, mMapX, mMapY, INTER_LINEAR);//重映射
	mImage=mResult;
	
	imwrite("./result2.bmp",mResult);
	return 0;	
  
}

void ReadSLC_inf(const char file_dir[])
{
	/************处理文件头的字符变量，解析完毕后，关闭当前文件*********************/
	//关于文件的文件头变量
	char str_temp[300];
	float minx, maxx, miny, maxy, minz, maxz;

	FILE * fid = fopen(file_dir,"r");//用于处理文件头的信息
	// printf("文件打开成功\r\n");
	//此处添加文件头处理方式
	fread(str_temp,sizeof(char),300,fid);
	//-EXTENTS <minx,maxx miny,maxy minz,maxz> CAD模型 x,y,z轴的范围
	int offset = FindString(str_temp,300,(char *)"-EXTENTS",8);
	strncpy(str_temp,str_temp+offset,100);//提取出XYZ的范围数据
	
	//提取XYZ的范围（前6个浮点值）
	sscanf(str_temp, "%*[(A-Z)|^-]%f%*[(^ )|(^,)]%f%*[(^ )|(^,)]%f%*[(^ )|(^,)]%f%*[(^ )|(^,)]%f%*[(^ )|(^,)]%f%*[(A-Z)|^*]", &minx, &maxx, &miny, &maxy, &minz, &maxz);
	printf("minx=%f minx=%f miny=%f maxy=%f minz=%f maxz=%f ", minx,minx,miny,maxy, minz, maxz);
	fclose(fid);
	
	/*********************************************************************************/
	char m_data;	//读取到的数据

	int fd; 		//打印文件描述符
	fd = open(file_dir, O_RDONLY);
	//////////////////////////////

	unsigned int i=0;
	unsigned int j=0;

	float   n_float;

    /**************************处理文件头部分**************************/
	while(1)
	{
		i++;
		if(i==2048)
		{
			close(fd);					//关闭打印文件
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
	
	
	/******************************************************************/
	/***************************处理预留部分***************************/	
	for(i=0;i<256;i++)
	{
		read(fd, &m_data, 1);
	}
	/******************************************************************/
	/**************************处理样本表部分**************************/
	read(fd, &m_data, 1);

	while(m_data)
	{
		read(fd, &n_float, 4);//Minimum Z Level
		read(fd, &n_float, 4);//Layer Thickness
		thickness=n_float;//读取层厚
		// printf("Layer Thickness=%.5f\r\n",n_float);
		
		// n_totalLayers=(int)((zmax-zmin)/n_float);    //计算出来的总层数
		read(fd, &n_float, 4);    //Line Width Compensation
		read(fd, &n_float, 4);    //Reserved
		m_data--;
	}
	printf("thickness=%f\n",thickness);
	
	close(fd);					//关闭打印文件
	
}

void ReadSLC_layerInf(const char file_dir[])
{
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

		for(i=0;i<n_boundary;i++)   //把同一层多个轮廓都放在同一容器中，
		{                           //显示跟数据处理时 要根据起始点和同轮廓的终点相等来判断是否为同一轮廓  
			read(fd, &n_vertices, 4);//一个轮廓环中的点数
			read(fd, &n_gaps, 4);
			for(j=0;j<n_vertices;j++)
			{
				read(fd, &n_polylineX, 4);
				read(fd, &n_polylineY, 4);
			}

		}
		// printf("第%d层\r\n",dex);
		// printf("BMP_OK\r\n");	
	}
	close(fd);
	totalLayers=dex;
	printf("totalLayers=%ld\n",totalLayers);
}

void OpenSLC(const char file_dir[])
{
	int dx=0,dy=0;	
	FILE *fd_temp;  //将调试数据保存到文本区

	//保存调试数据到文件夹
	fd_temp=fopen("./slcFileInf.txt", "w+");
	
	/************处理文件头的字符变量，解析完毕后，关闭当前文件*********************/
	//关于文件的文件头变量
	char str_temp[300];
	float minx, maxx, miny, maxy, minz, maxz;

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
	printf("minx=%.3f,maxx=%.3f,miny=%.3f,maxy=%.3f,minz=%.3f,maxz=%.3f\r\n", minx,maxx,miny,maxy,minz,maxz);

	// printf("%s\r\n", str_temp);
	// fprintf(fd_temp,"%s\r\n",str_temp);
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
	
	int dex;
	
	dx=pixel_bits_x/2-zoom_x*(minx+maxx)/2;
	dy=pixel_bits_y/2-zoom_y*(miny+maxy)/2;

	char colorDir;
	
	dex=0;
				
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
		
		LayerCurrent=dex;
		
		while(file_continue_flag!=1);//等于1跳出while循环
		imwrite("./dst.bmp",dst);
		// imageRemap(dst);
		file_continue_flag = 0;    // 使下次处于一个阻态
		
		v_contour.clear();//删除容器中的所有元素，这里的元素是同一层中所有轮廓数据

		printf("第%d层\r\n",dex);
		printf("BMP_OK\r\n");	
	}
	
	fclose(fd_temp);
	close(fd);
}


void OpenSLC_1(const char file_dir[],long layer)
{
	printf("file_dir=%s\nlayer=%ld\n",file_dir,layer);
	// return;
	
	int dx=0,dy=0;
	
	/************处理文件头的字符变量，解析完毕后，关闭当前文件*********************/
	//关于文件的文件头变量
	char str_temp[300];
	float minx, maxx, miny, maxy, minz, maxz;

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
			// imageRemap(dst);
			v_contour.clear();//删除容器中的所有元素，这里的元素是同一层中所有轮廓数据
			// printf("第%d层\r\n",dex);
			printf("transition OK\r\n");
			// printf("BMP_OK\r\n");	
			break;			
		}
	}
	close(fd);
}


// static void *thread_1(void *args)//文件处理
// {
	// print_flag=0;
	// file_continue_flag=0;

	// while(1)
	// {
		// //OpenSLC函数一般情况下为阻态
		// while(!print_flag);
		// // printf("打印文件为:%s\r\n",print_path);

		// OpenSLC(print_path);
		
		// // printf("解析结束\r\n");
		// // serialPrintf(fd_serial,"SLC OVER\r\n");
		
	// }
	// return NULL;
// }

// static void *thread_2(void *args)//外界通信
// {
	// /*
	// 内容：1、询问是否解除SLC文件解析阻塞，继续生成位图（注：生成BMP位图比较耗时）
	// */
	// while(1)
	// {
		// // sleep(1);
		// getchar();
		// file_continue_flag = 1;
	// }
	// return NULL;
// }



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


	




