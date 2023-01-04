//OpenCV的轮廓查找和填充  https://blog.csdn.net/garfielder007/article/details/50866101
//opencv findContours和drawContours使用方法  https://blog.csdn.net/ecnu18918079120/article/details/78428002
//OpenCV关于容器的介绍  https://blog.csdn.net/Ahuuua/article/details/80593388

//opencv
#include "opencv2/highgui/highgui.hpp"  
#include "opencv2/imgproc/imgproc.hpp" 

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
#include <pthread.h>  	//线程
#include <string>

//////////////////////////////////////
#define FBDEVICE "/dev/fb0"
#include "parameter.h"
#include "SLC_Read.h"
#include "HMI-Screen.h"
#include "file.h"
#include "wiringSerial.h"



using namespace cv;
using namespace std;


FILE *fd_temp;  //将数据保存到文本区


/************************************************************************/
/****************读取FrameBuffer信息，即像素信息*************************/
pixel_Framebuffer pixel_bits;
static int screensize = 0;
unsigned char *pfb = NULL;



void read_Framebuffer(pixel_Framebuffer &temp)
{
	static int fd;
    int ret;
    struct fb_fix_screeninfo finfo;
    struct fb_var_screeninfo vinfo;
    fd = open(FBDEVICE, O_RDWR);
    if (fd < 0)
    {
        perror("open");
    }
    // printf("open %s success \n", FBDEVICE);

   
    ret = ioctl(fd, FBIOGET_FSCREENINFO, &finfo);
    if (ret < 0)
    {
        perror("ioctl");
    }

    ret = ioctl(fd, FBIOGET_VSCREENINFO, &vinfo);
    if (ret < 0)
    {
        perror("ioctl");
    }
    
	
	//计算屏幕的总大小（字节）
	screensize = vinfo.xres * vinfo.yres * vinfo.bits_per_pixel / 8;//除8表示每个字节8位，bits_per_pixel单位为位
	
	// printf("屏幕的总大小screensize=%d byte\n",screensize);

	// xres_virtual yres_virtual定义了framebuffer内存中一帧的尺寸。xres_virtual yres_virtual必定大于或者等于xres yres，
	printf("显示信息->xres：%d\n",vinfo.xres);
	printf("显示信息->yres：%d\n",vinfo.yres);
	printf("显示信息->xres_virtual：%d\n",vinfo.xres_virtual);
	printf("显示信息->yres_virtual：%d\n",vinfo.yres_virtual);
	printf("显示信息->颜色深度：%d\n",vinfo.bits_per_pixel);

	temp.x=vinfo.xres;
	temp.y=vinfo.yres;
	temp.bits_per_pix=vinfo.bits_per_pixel;
	
/*	if(led_choose==0)
	{
		temp.x=1280;
		temp.y=800;
		temp.bits_per_pix=32;		
	}
	
	if(led_choose==1)
	{
		temp.x=1920;
		temp.y=1080;
		temp.bits_per_pix=32;			
	}	
*/	
	close(fd);
}


//14byte文件头
typedef struct
{
	char cfType[2];//文件类型，"BM"(0x4D42)
	long cfSize;//文件大小（字节）
	long cfReserved;//保留，值为0
	long cfoffBits;//数据区相对于文件头的偏移量（字节）
}__attribute__((packed)) BITMAPFILEHEADER;
//__attribute__((packed))的作用是告诉编译器取消结构在编译过程中的优化对齐
 
//40byte信息头
typedef struct
{
	char ciSize[4];//BITMAPFILEHEADER所占的字节数
	long ciWidth;//宽度
	long ciHeight;//高度
	char ciPlanes[2];//目标设备的位平面数，值为1
	int ciBitCount;//每个像素的位数
	char ciCompress[4];//压缩说明
	char ciSizeImage[4];//用字节表示的图像大小，该数据必须是4的倍数
	char ciXPelsPerMeter[4];//目标设备的水平像素数/米
	char ciYPelsPerMeter[4];//目标设备的垂直像素数/米
	char ciClrUsed[4]; //位图使用调色板的颜色数
	char ciClrImportant[4]; //指定重要的颜色数，当该域的值等于颜色数时（或者等于0时），表示所有颜色都一样重要
}__attribute__((packed)) BITMAPINFOHEADER;
 
typedef struct
{
	unsigned short blue;
	unsigned short green;
	unsigned short red;
	unsigned short reserved;
}__attribute__((packed)) PIXEL;//颜色模式RGB
 
BITMAPFILEHEADER FileHead;
BITMAPINFOHEADER InfoHead;


int show_bmp(unsigned char *pfb, unsigned int width, unsigned int height)
{
	FILE *fp;
	unsigned char pix=0;
	int rc;
	int line_x, line_y;
	long int location = 0;//, BytesPerLine = 0;
	int pix_c=4;
	char str[100];
 
 	sprintf(str,"%s/dst.bmp",bmp_path);
	
	// printf("dst路径:%s\r\n",str);
	
	fp = fopen(str, "rb" );
	
	if (fp == NULL)
	{
		// printf("file is NULL\n");
		return(-1 );
	}
 
	rc = fread(&FileHead, sizeof(BITMAPFILEHEADER),1, fp );
	if (rc != 1)
	{
		// printf("read header error!\n");
		fclose(fp );
		return(-2 );
	}
 
	//检测是否是bmp图像
	if (memcmp(FileHead.cfType, "BM", 2) != 0)
	{
		// printf("it's not a BMP file\n");
		fclose(fp );
		return(-3 );
	}
 
	rc = fread((char *)&InfoHead, sizeof(BITMAPINFOHEADER),1, fp );
	if (rc != 1)
	{
		// printf("read infoheader error!\n");
		fclose(fp );
		return(-4 );
	}
 
	//跳转的数据区
	fseek(fp, FileHead.cfoffBits, SEEK_SET);
	//每行字节数
	// BytesPerLine = InfoHead.ciWidth * InfoHead.ciBitCount / 8;
	
	// 图片信息
	// printf("图片宽度（单位为一个像素）InfoHead.ciWidth：%ld\n",InfoHead.ciWidth);
	// printf("图片的高度:%ld,图片的宽度:%ld\n",InfoHead.ciWidth,InfoHead.ciHeight);
	// printf("图片每个像素的位数InfoHead.ciBitCount：%d\n",InfoHead.ciBitCount);
	// printf("图片每行字节数：%ld\n",BytesPerLine);
	
	line_x = line_y = 0;
	//向framebuffer中写BMP图片
	while(!feof(fp))
	{	
		if(InfoHead.ciBitCount == 32)//假如颜色深度是32（4个字节），像素值依次带入
		{
			rc = fread(&pix, 1, 1, fp);
			// if (rc != sizeof(pix))
				// break;
			if (line_x == InfoHead.ciWidth*4)//Framebuffer为4位字节表示，所以ciWidth乘4
			{
				line_x = 0;
				line_y++;
				if(line_y == InfoHead.ciHeight)
					break;
			}
			location = line_x + (InfoHead.ciHeight - line_y - 1) * width * 4;//Framebuffer为4位字节表示，所以width乘4
			//显示每一个像素
			*(pfb + location)=pix;
			line_x++;
		}
		else
		{
			if(InfoHead.ciBitCount == 8)//假如颜色深度是8（1个字节），R=G=B=pix
			{
				rc = fread(&pix, 1, 1, fp);
				// if (rc != sizeof(pix))
					// break;
				pix_c=4;
				while(pix_c--)
				{
					if (line_x == InfoHead.ciWidth*4)//Framebuffer为4位字节表示，所以ciWidth乘4
					{
						line_x = 0;
						line_y++;
						if(line_y == InfoHead.ciHeight)
							break;
					}
					location = line_x + (InfoHead.ciHeight - line_y - 1) * width * 4;//Framebuffer为4位字节表示，所以width乘4
					//显示每一个像素
					if(pix_c==0)
					{
						*(pfb + location)=255;//透明度
					}
					else
					{
						*(pfb + location)=pix;
					}
					line_x++;
				}
			}
		}
	}
	
	fclose(fp );
	
	// printf("show_bmp function over\n");
	
	return(0 );
}


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



void curr_picture(void)
{
	FILE *fp;
	int fb_fd; //fb文件描述符
	char show_back = 0; //是否为BMP标识

	read_Framebuffer(pixel_bits);
	int pixel_bits_y=pixel_bits.y;
	int pixel_bits_x=pixel_bits.x;
	int bits_per_pixel=pixel_bits.bits_per_pix;
	
	// printf("led_choose=%ld\r\n",led_choose);
	
	char str[100]={0};
	
	if(led_choose==0)
	{
		sprintf(str,"sudo cp %s/dst_1280_800.bmp %s/dst.bmp",bmp_path,bmp_path);
		fp = popen(str, "r");
	}
	else
	{
		if(led_choose==1)
		{
			sprintf(str,"sudo cp %s/dst_1920_1080.bmp %s/dst.bmp",bmp_path,bmp_path);
			fp = popen(str, "r");
		}
	}
	
	pclose(fp);	//图片分辨率要匹配，否则无法写入
	///////////////////////////////////////////////////
	//Framebuffr初始化并打开s
    fb_fd = open(FBDEVICE, O_RDWR);
	//内存映射
	screensize = pixel_bits_x * pixel_bits_y * bits_per_pixel / 8;//除8表示每个字节8位，bits_per_pixel单位为位
	pfb = (unsigned char *)mmap(NULL, screensize, PROT_READ | PROT_WRITE, MAP_SHARED, fb_fd, 0);
	////////////////////////////////////////////////////	
	show_back= show_bmp(pfb, pixel_bits_x, pixel_bits_y);
	if(show_back == -3)//像素值显示失败，it's not a BMP file
	{
		printf("show bmp fail\r\n");
		// 添加其他函数，将其他格式的图片文件写入到FrameBuffer文件中
	}
	munmap(pfb, screensize);	
	close(fb_fd);				//关闭Frambuffer文件	
}

void curr(void)
{
	int fb_fd; //fb文件描述符
	char show_back = 0; //是否为BMP标识
	vector<Point> contour;       	 //单个轮廓坐标值
	vector<vector<Point>> v_contour; //当前层所有轮廓集合
	read_Framebuffer(pixel_bits);
	int pixel_bits_y=pixel_bits.y;
	int pixel_bits_x=pixel_bits.x;
	int bits_per_pixel=pixel_bits.bits_per_pix;
	Mat dst = Mat::zeros(pixel_bits_y, pixel_bits_x, CV_8UC1);//生成的图片，其分辨率由实际的FrameBuffer来决定
	CvScalar color=cvScalar(255);
	contour={{0,0},{0, pixel_bits_y},{ pixel_bits_x, pixel_bits_y},{pixel_bits_x,0}};
	v_contour.push_back(contour);//追加当前轮廓数据到当前层容器变量中
	drawContours(dst,v_contour ,0, color, CV_FILLED );
	contour.clear();//删除容器中的所有元素		
	v_contour.clear();//删除容器中的所有元素，这里的元素是同一层中所有轮廓数据
	
	char str[100];
 	sprintf(str,"%s/dst.bmp",bmp_path);
	imwrite((const String&)str,dst);
	///////////////////////////////////////////////////
	//Framebuffr初始化并打开
    fb_fd = open(FBDEVICE, O_RDWR);
	//内存映射
	screensize = pixel_bits_x * pixel_bits_y * bits_per_pixel / 8;//除8表示每个字节8位，bits_per_pixel单位为位
	pfb = (unsigned char *)mmap(NULL, screensize, PROT_READ | PROT_WRITE, MAP_SHARED, fb_fd, 0);
	////////////////////////////////////////////////////	
	show_back=show_bmp(pfb, pixel_bits_x, pixel_bits_y);
	if(show_back == -3)//像素值显示失败，it's not a BMP file
	{
		printf("show bmp fail\r\n");
		// 添加其他函数，将其他格式的图片文件写入到FrameBuffer文件中
	}	
	munmap(pfb, screensize);
	close(fb_fd);				//关闭Frambuffer文件	
}


//异或
int xor_personal(string command_str)
{
	char result=0;
	
	for(unsigned int i=0;i<command_str.length();i++)
	{
		result ^= command_str[i];
	}

	// printf("result:%d\r\n",result);
	return result;
}

//自加1，大小无限制，受硬件容量影响
string self_imposed_one(string num_str)
{
	for (int i=(num_str.length()-1);i>=0;i--)
	{
		if(i==(int)(num_str.length()-1))
		{
			num_str[i]+=1;
		}
		
		if(i>0)
		{
			if((num_str[i]-48)>9)
			{
				num_str[i-1]+=1;
			}
		}
		
		if(i==0)
		{
			if((num_str[i]-48)>9)
			{
				num_str[i]=0+48;
				num_str="1"+num_str;
			}
		}

		if((num_str[i]-48)>9)
		{
			num_str[i]=0+48;
		}
	}
	
	return num_str;
}


void GMCreat(long layer_z)
{
	FILE* fps = NULL; // 文件指针
	char srts[500]={0};
	char fileName[500]={0};	
	
	
	sprintf(fileName,"%s/GMfile/G-M.gcode",print_cfg_path);
	fps = fopen(fileName,"w+");
	
	
	string N_str="1";
	string _str="";
	string str="";
	unsigned int iPos;
	
	fputs("N1 M110 N1*125\n", fps);	//设置行号
	N_str=self_imposed_one(N_str);
	fputs("N2 G28 Z0*91\n", fps);		//Z轴回零
	N_str=self_imposed_one(N_str);

	sprintf(srts,"G0 Z%.3lf\r\n",print_work_level_z);//工作原点
	str=srts;
	iPos = str.find('\r')<str.find("\n")?str.find("\r"):str.find("\n");
	iPos = iPos<str.find(';',1)?iPos:str.find(';');
	_str="N" + N_str + " " + str.substr(0, iPos);
	_str=_str+"*"+to_string(xor_personal(_str))+"\n";	
	N_str=self_imposed_one(N_str);
	fputs(_str.c_str(), fps);


	sprintf(srts,"M114\r\n");//查询当前指令是否完成
	str=srts;
	iPos = str.find('\r')<str.find("\n")?str.find("\r"):str.find("\n");
	iPos = iPos<str.find(';',1)?iPos:str.find(';');
	_str="N" + N_str + " " + str.substr(0, iPos);
	_str=_str+"*"+to_string(xor_personal(_str))+"\n";	
	N_str=self_imposed_one(N_str);
	fputs(_str.c_str(), fps);
	
	str="M106\r\n";//开启LED
	iPos = str.find('\r')<str.find("\n")?str.find("\r"):str.find("\n");
	iPos = iPos<str.find(';',1)?iPos:str.find(';');
	_str="N" + N_str + " " + str.substr(0, iPos);
	_str=_str+"*"+to_string(xor_personal(_str))+"\n";
	N_str=self_imposed_one(N_str);
	fputs(_str.c_str(), fps);
	str="M107\r\n";//关闭LED
	iPos = str.find('\r')<str.find("\n")?str.find("\r"):str.find("\n");
	iPos = iPos<str.find(';',1)?iPos:str.find(';');
	_str="N" + N_str + " " + str.substr(0, iPos);
	_str=_str+"*"+to_string(xor_personal(_str))+"\n";
	N_str=self_imposed_one(N_str);
	fputs(_str.c_str(), fps);
	// fputs("M106\r\n", fps);//开启LED
	// fputs("M107\r\n", fps);//关闭LED
	
	double height;
	height=print_work_level_z;
	
	printf("layer_z:%ld\r\n",layer_z);
	for(long k=0;k<layer_z;k++)
	{
		//print_back_level_z
		sprintf(srts,"G0 Z%.3lf\r\n",height-print_back_level_z);
		str=srts;
		iPos = str.find('\r')<str.find("\n")?str.find("\r"):str.find("\n");
		iPos = iPos<str.find(';',1)?iPos:str.find(';');
		_str="N" + N_str + " " + str.substr(0, iPos);
		_str=_str+"*"+to_string(xor_personal(_str))+"\n";	
		N_str=self_imposed_one(N_str);
		fputs(_str.c_str(), fps);		
		// fputs(srts, fps);

		height-=level_z;//更新高度
		sprintf(srts,"G0 Z%.3lf\r\n",height); //speed_z
		str=srts;
		iPos = str.find('\r')<str.find("\n")?str.find("\r"):str.find("\n");
		iPos = iPos<str.find(';',1)?iPos:str.find(';');
		_str="N" + N_str + " " + str.substr(0, iPos);
		_str=_str+"*"+to_string(xor_personal(_str))+"\n";	
		N_str=self_imposed_one(N_str);
		fputs(_str.c_str(), fps);
		
		sprintf(srts,"M114\r\n");//查询当前指令是否完成
		str=srts;
		iPos = str.find('\r')<str.find("\n")?str.find("\r"):str.find("\n");
		iPos = iPos<str.find(';',1)?iPos:str.find(';');
		_str="N" + N_str + " " + str.substr(0, iPos);
		_str=_str+"*"+to_string(xor_personal(_str))+"\n";	
		N_str=self_imposed_one(N_str);
		fputs(_str.c_str(), fps);

		str="M106\r\n";//开启LED
		iPos = str.find('\r')<str.find("\n")?str.find("\r"):str.find("\n");
		iPos = iPos<str.find(';',1)?iPos:str.find(';');
		_str="N" + N_str + " " + str.substr(0, iPos);
		_str=_str+"*"+to_string(xor_personal(_str))+"\n";	
		N_str=self_imposed_one(N_str);
		fputs(_str.c_str(), fps);
		
		str="M107\r\n";//关闭LED
		iPos = str.find('\r')<str.find("\n")?str.find("\r"):str.find("\n");
		iPos = iPos<str.find(';',1)?iPos:str.find(';');
		_str="N" + N_str + " " + str.substr(0, iPos);
		_str=_str+"*"+to_string(xor_personal(_str))+"\n";	
		N_str=self_imposed_one(N_str);
		fputs(_str.c_str(), fps);
		
	}

	sprintf(srts,"G28 Z0\r\n");//Z轴回零
	str=srts;
	iPos = str.find('\r')<str.find("\n")?str.find("\r"):str.find("\n");
	iPos = iPos<str.find(';',1)?iPos:str.find(';');
	_str="N" + N_str + " " + str.substr(0, iPos);
	_str=_str+"*"+to_string(xor_personal(_str))+"\n";
	N_str=self_imposed_one(N_str);
	fputs(_str.c_str(), fps);
	
	// 最后不要忘了,关闭打开的文件~~~
	fclose(fps);	
}

void ReadSLC_inf(const char file_dir[])
{	
	int fd; 		//打印文件描述符

	/************处理文件头的字符变量，解析完毕后，关闭当前文件*********************/
	//关于文件的文件头变量
	char str_temp[300];
	float minx, maxx, miny, maxy, minz, maxz;

	FILE * fid = fopen(file_dir,"r");//用于处理文件头的信息
	if(fid == NULL)
    {
		fclose(fid);	
        return;
    }
	// printf("文件打开成功\r\n");
	//此处添加文件头处理方式
	fread(str_temp,sizeof(char),300,fid);
	//-EXTENTS <minx,maxx miny,maxy minz,maxz> CAD模型 x,y,z轴的范围
	int ssss = FindString(str_temp,300,(char *)"-EXTENTS",8);
	strncpy(str_temp,str_temp+ssss,100);//提取出XYZ的范围数据
	
	//提取XYZ的范围（前6个浮点值）
	sscanf(str_temp, "%*[(A-Z)|^-]%f%*[(^ )|(^,)]%f%*[(^ )|(^,)]%f%*[(^ )|(^,)]%f%*[(^ )|(^,)]%f%*[(^ )|(^,)]%f%*[(A-Z)|^*]", &minx, &maxx, &miny, &maxy, &minz, &maxz);
	// printf("%f %f %f %f %f %f\r\n", minx,maxx,miny,maxy, minz, maxz);
    
	fclose(fid);
	/*********************************************************************************/

	char m_data;	//读取到的数据


	fd = open(file_dir, O_RDONLY);
	//////////////////////////////

	unsigned int i=0;
	unsigned int j=0;

	float   n_float;

    /**************************处理头文件部分**************************/
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
		level_z=n_float;//读取层厚
		// printf("Layer Thickness=%.5f\r\n",n_float);
		
		// n_totalLayers=(int)((zmax-zmin)/n_float);    //计算出来的总层数
		read(fd, &n_float, 4);    //Line Width Compensation
		read(fd, &n_float, 4);    //Reserved
		m_data--;
	}
	// layer_z=(unsigned long)((maxz-minz)/level_z+0.5)+buttom;//读取层数	
	layer_z=(unsigned long)((maxz-minz)/level_z)+buttom;//读取层数
	close(fd);					//关闭打印文件
	
	GMCreat(layer_z);
}


void bmp_255(void)
{
	char str[100];
	sprintf(str,"%s/dst.bmp",bmp_path);
	read_Framebuffer(pixel_bits);
	int pixel_bits_y=pixel_bits.y;
	int pixel_bits_x=pixel_bits.x;
	vector<Point> contour;       	 //单个轮廓坐标值
	vector<vector<Point>> v_contour; //当前层所有轮廓集合
	Mat dst_basic = Mat::zeros(pixel_bits_y, pixel_bits_x, CV_8UC1);//生成的图片，其分辨率由实际的FrameBuffer来决定
	CvScalar color=cvScalar(255);
	contour = {{0,0},{0,pixel_bits_y},{pixel_bits_x,pixel_bits_y},{pixel_bits_x,0}};
	v_contour.push_back(contour);//追加当前轮廓数据到当前层容器变量中
	drawContours(dst_basic,v_contour ,0, color, CV_FILLED );	
	imwrite(str,dst_basic);
	show_bmp(pfb, pixel_bits_x, pixel_bits_y);
	contour.clear();//删除容器中的所有元素		  
	v_contour.clear();//删除容器中的所有元素，这里的元素是同一层中所有轮廓数据
}

void OpenSLC(const char file_dir[])
{
	//FILE *fp;
	int fb_fd; //fb文件描述符
	char show_back = 0; //是否为BMP标识
	
	static int fd; 		//打印文件描述符
	int dx=0,dy=0;
	
	char str[100]={0};
	
	//保存调试数据到文件夹
	sprintf(str,"%s/temp.txt",bmp_path);
	fd_temp=fopen(str,"w+");
	
	/************处理文件头的字符变量，解析完毕后，关闭当前文件*********************/
	//关于文件的文件头变量
	char str_temp[300];
	float minx, maxx, miny, maxy, minz, maxz;

	FILE * fid = fopen(file_dir,"r");//用于处理文件头的信息
	if(fid == NULL)
    {
		file_continue_flag = 0;    //使下次处于一个阻态
		print_flag = 0;            //结束打印
		printf("open file failed\r\n");
		fclose(fid);
		//关闭调试输出的数据文件
		fclose(fd_temp);
        return;
    }
	// printf("文件打开成功,开始读取文件头内容\r\n");
	//此处添加文件头处理方式
	fread(str_temp,sizeof(char),300,fid);
	//-EXTENTS <minx,maxx miny,maxy minz,maxz> CAD模型 x,y,z轴的范围
	int ssss = FindString(str_temp,300,(char *)"-EXTENTS",8);
	strncpy(str_temp,str_temp+ssss,100);//提取出XYZ的范围数据
	
	//提取XYZ的范围（前6个浮点值）
	sscanf(str_temp, "%*[(A-Z)|^-]%f%*[(^ )|(^,)]%f%*[(^ )|(^,)]%f%*[(^ )|(^,)]%f%*[(^ )|(^,)]%f%*[(^ )|(^,)]%f%*[(A-Z)|^*]", &minx, &maxx, &miny, &maxy, &minz, &maxz);
	// printf("%f %f %f %f %f %f\r\n", minx,maxx,miny,maxy, minz, maxz);
	// printf("%s\r\n", str_temp);
	
	fprintf(fd_temp,"%s\r\n",str_temp);
	fclose(fid);
	/*********************************************************************************/

	char m_data;	//读取到的数据
	float d = 0;    //d > 0 从上往下看是逆时针
	
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
	read_Framebuffer(pixel_bits);
	int pixel_bits_y=pixel_bits.y;
	int pixel_bits_x=pixel_bits.x;
	int bits_per_pixel=pixel_bits.bits_per_pix;
	
	///////////////////////////////////////////////////
	//Framebuffr初始化并打开
    fb_fd = open(FBDEVICE, O_RDWR);
	//内存映射
	screensize = pixel_bits_x * pixel_bits_y * bits_per_pixel / 8;//除8表示每个字节8位，bits_per_pixel单位为位
	pfb = (unsigned char *)mmap(NULL, screensize, PROT_READ | PROT_WRITE, MAP_SHARED, fb_fd, 0);
	////////////////////////////////////////////////////	
	
	
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
	

	long print_level=0;
	/******************************************************************/
	/***************************处理基底层***************************/

	for(i=0;i<(unsigned int)buttom;i++)
	{
		if(led_choose==0)
		{
			//sprintf(str_1,"sudo cp %s/dst_1280_800_print.bmp %s/dst.bmp",bmp_path,bmp_path);
			//fp = popen(str_1, "r");
			// pclose(fp);	//图片分辨率要匹配，否则无法写入
			bmp_255();
			
		}
		else if(led_choose==1)
		{
			//sprintf(str_1,"sudo cp %s/dst_1920_1080_print.bmp %s/dst.bmp",bmp_path,bmp_path);
			//fp = popen(str_1, "r");
			// pclose(fp);	//图片分辨率要匹配，否则无法写入
			bmp_255();
		}
	
		printf("BMP_OK\r\n");

		while(!file_continue_flag)
        {
            if(print_flag == 0)
            {
				file_continue_flag = 0;		//使下次处于一个阻态
				//删除对象映射
				munmap(pfb, screensize);
				fclose(fd_temp);			//关闭调试输出文件
				close(fd);					//关闭打印文件
				close(fb_fd);				//关闭Frambuffer文件
				return;
            }
        }

		// show_framebuffer();//读图显示
		show_back=show_bmp(pfb, pixel_bits_x, pixel_bits_y);
		if(show_back == -3)//像素值显示失败，it's not a BMP file
		{
			printf("show bmp fail\r\n");
			//添加其他函数，将其他格式的图片文件写入到FrameBuffer文件中
		}
		
		file_continue_flag = 0;    // 使下次处于一个阻态
		
		print_level++;
		sch=(long)(100*print_level/layer_z);
		print_z=print_level;		
		printf("print_level：第%ld层\r\n",print_level);
		
		// fprintf(fd_temp,"第%ld层\r\n",print_level);
		// printf("第%ld层\r\n",print_level);
	}


	if(access(file_dir,0)<0)
	{
		file_continue_flag = 0;		//使下次处于一个阻态
		print_flag = 0;				//结束打印
		//删除对象映射
		munmap(pfb, screensize);
		fclose(fd_temp);			//关闭调试输出文件
		close(fd);					//关闭打印文件
		close(fb_fd);				//关闭Frambuffer文件
		
		usb_flag=-1;
		menu_show_clear();
		page_model_switch();
		
		return;
	}		
		
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
	
	// static char set_print_flag=0;
	// /******************************************************************/
	// /*************************处理轮廓数据部分*************************/
	while(1)
	{
		read(fd, &n_layer, 4);

		fprintf(fd_temp,"Z轴高度=%.5f\r\n",n_layer);
		
		read(fd, &n_boundary, 4);

		if(access(file_dir,0)<0)
		{
			file_continue_flag = 0;		//使下次处于一个阻态
			print_flag = 0;				//结束打印
			//删除对象映射
			munmap(pfb, screensize);
			fclose(fd_temp);			//关闭调试输出文件
			close(fd);					//关闭打印文件
			close(fb_fd);				//关闭Frambuffer文件
			
			usb_flag=-1;
			menu_show_clear();
			page_model_switch();
			
			return;
		}		

		fprintf(fd_temp,"轮廓数=%d\r\n",n_boundary);

		if(n_boundary==0xFFFFFFFF)  //结束符
		{
			
			printf("曝光完成\r\n");
			sch=100;
			time_print_res=0;
			//发送窗口最后信息
			show_print_inf_print();
						
			file_continue_flag = 0;		//使下次处于一个阻态
			//删除对象映射
			munmap(pfb, screensize);
			fclose(fd_temp);			//关闭调试输出文件
			close(fd);					//关闭打印文件
			close(fb_fd);				//关闭Frambuffer文件

			printf("关闭硬件操作句柄，回收\r\n");
			
			return;			
			

		}
		
		for(i=0;i<n_boundary;i++)   //把同一层多个轮廓都放在同一容器中，
		{                           //显示跟数据处理时 要根据起始点和同轮廓的终点相等来判断是否为同一轮廓  
			read(fd, &n_vertices, 4);//一个轮廓环中的点数

			fprintf(fd_temp,"第%d个轮廓环中的点数=%d\r\n",i+1,n_vertices);
			
			read(fd, &n_gaps, 4);
			
			if(access(file_dir,0)<0)
			{
				file_continue_flag = 0;		//使下次处于一个阻态
				print_flag = 0;				//结束打印
				//删除对象映射
				munmap(pfb, screensize);
				fclose(fd_temp);			//关闭调试输出文件
				close(fd);					//关闭打印文件
				close(fb_fd);				//关闭Frambuffer文件
				
				usb_flag=-1;
				menu_show_clear();
				page_model_switch();
				
				return;
			}		

			contour.clear();//删除容器中的所有元素
			for(j=0;j<n_vertices;j++)
			{
				read(fd, &n_polylineX, 4);
				read(fd, &n_polylineY, 4);
				
				if(access(file_dir,0)<0)
				{
					file_continue_flag = 0;		//使下次处于一个阻态
					print_flag = 0;				//结束打印
					//删除对象映射
					munmap(pfb, screensize);
					fclose(fd_temp);			//关闭调试输出文件
					close(fd);					//关闭打印文件
					close(fb_fd);				//关闭Frambuffer文件
					
					usb_flag=-1;
					menu_show_clear();
					page_model_switch();
					
					return;
				}		
				
				dx=pixel_bits_x/2-zoom_x*10*(minx+maxx)/2;
				dy=pixel_bits_y/2-zoom_y*10*(miny+maxy)/2;
				contour.push_back(Point((long)(pixel_bits_x-(n_polylineX*zoom_x*10+dx)),(long)((n_polylineY*zoom_y*10+dy)))); //向轮廓坐标尾部添加点坐标，坐标关于某轴进行翻转时，顺逆指针会发生变化

				// printf("lineX=%.5f   ",n_polylineX);
				// printf("lineY=%.5f\r\n",n_polylineY);
				fprintf(fd_temp,"{%ld,",(long)(pixel_bits_x-(n_polylineX*zoom_x*10+dx)));
				fprintf(fd_temp,"%ld}\r\n",(long)((n_polylineY*zoom_y*10+dy)));
				// printf("contour.size()=%d\r\n",contour.size());//查看容器的大小
				// printf("v_contour.size()=%d\r\n",v_contour.size());
			}

			v_contour.push_back(contour);//追加当前轮廓数据到当前层容器变量中
			contour.clear();//删除容器中的所有元素		  
		}
		

////////////////////////////////////////////////////////////////////////////////////////////////
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
						//printf("有存在交叉现象\r\n");
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
            if(d > 0)
			{
				//cout << "逆时针：counterclockwise"<< endl;
				fprintf(fd_temp,"逆时\r\n\r\n");
				//填充白色
                // color=cvScalar(255);
				color=cvScalar(0);
			}
			else
			{
				//cout << "顺时针：clockwise" << endl;
				fprintf(fd_temp,"顺时\r\n\r\n");
				//填充黑色
                // color=cvScalar(0);
				color=cvScalar(255);
			}
			drawContours(dst,v_contour ,i, color, CV_FILLED );
		}
		
		char str_2[100]={0};
		sprintf(str_2,"%s/dst.bmp",bmp_path);
		
		// printf("imwrite dst路径:%s\r\n",str_2);
		// imwrite("/home/pi/wiringpi/dst.bmp",dst);
		imwrite((const String&)str_2,dst);			
		
		v_contour.clear();//删除容器中的所有元素，这里的元素是同一层中所有轮廓数据

		// fprintf(fd_temp,"第%ld层\r\n",print_level);
		printf("BMP_OK\r\n");
		
		while(!file_continue_flag)
		{
			if(print_flag == 0)
			{
				file_continue_flag = 0;	//使下次处于一个阻态
				//删除对象映射
				munmap(pfb, screensize);
				fclose(fd_temp);			//关闭调试输出文件
				close(fd);					//关闭打印文件
				close(fb_fd);				//关闭Frambuffer文件
				return;
			}
		}
				
		// show_framebuffer();//读图显示
		show_back=show_bmp(pfb, pixel_bits_x, pixel_bits_y);
		if(show_back == -3)//像素值显示失败，it's not a BMP file
		{
			printf("show bmp fail\r\n");
			// 添加其他函数，将其他格式的图片文件写入到FrameBuffer文件中
		}

		file_continue_flag = 0;    // 使下次处于一个阻态
		
		if(print_level < layer_z)
		{
			print_level++;
		}
		sch=(long)(100*print_level/layer_z);
		print_z=print_level;

		// fprintf(fd_temp,"第%ld层\r\n",print_level);
		printf("print_level：第%ld层\r\n",print_level);
	}
	
}


static void *thread_1(void *args)//文件处理
{
	print_flag=0;
	file_continue_flag=0;


	while(1)
	{
		//OpenSLC函数一般情况下为阻态
		while(!print_flag);
		printf("打印文件为:%s\r\n",print_path);

		OpenSLC(print_path);
		
		
		printf("解析结束\r\n");
		// serialPrintf(fd_serial,"SLC OVER\r\n");
		
	}
	return NULL;
}

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

int slc_main(void)
{
	int ret=0;
	pthread_t id1;//,id2;
	ret=pthread_create(&id1,NULL,thread_1,NULL);//开启线程	
	if(ret)
	{
		printf("create pthread error!\n");
		return -1; 
	}

	// ret=pthread_create(&id2,NULL,thread_2,NULL);//开启线程
	// if(ret)
	// {
		// printf("create pthread error!\n");
		// return  -1; 
	// }
	return 0;
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


	




