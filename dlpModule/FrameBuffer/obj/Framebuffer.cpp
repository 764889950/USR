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
#define FBDEVICE "/dev/fb0"
#include "Framebuffer.h"
#include "parameter.h"


using namespace cv;
using namespace std;

/************************************************************************/
/****************读取FrameBuffer信息，即像素信息*************************/
typedef struct pixel{int x;int y;int bits_per_pix;} pixel_Framebuffer;
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
	// // xres_virtual yres_virtual定义了framebuffer内存中一帧的尺寸。xres_virtual yres_virtual必定大于或者等于xres yres，
	// printf("显示信息->xres：%d\n",vinfo.xres);
	// printf("显示信息->yres：%d\n",vinfo.yres);
	// printf("显示信息->xres_virtual：%d\n",vinfo.xres_virtual);
	// printf("显示信息->yres_virtual：%d\n",vinfo.yres_virtual);
	// printf("显示信息->颜色深度：%d\n",vinfo.bits_per_pixel);

	temp.x=vinfo.xres;
	temp.y=vinfo.yres;
	temp.bits_per_pix=vinfo.bits_per_pixel;

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


int show_bmp(unsigned char *pfb, unsigned int width, unsigned int height,char dir[])
{
	FILE *fp;
	unsigned char pix=0;
	int rc;
	int line_x, line_y;
	long int location = 0;//, BytesPerLine = 0;
	int pix_c=4;

	fp = fopen(dir, "rb" );
	
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
	
	fclose(fp);
	
	// printf("show_bmp function over\n");
	
	return(0 );
}


void inf(void)
{
	read_Framebuffer(pixel_bits);
	int pixel_bits_x=pixel_bits.x;
	int pixel_bits_y=pixel_bits.y;
	int bits_per_pixel=pixel_bits.bits_per_pix;	
	
	printf("pixel_bits_x:%d\r\n",pixel_bits_x);
	printf("pixel_bits_y:%d\r\n",pixel_bits_y);
	printf("bits_per_pixel:%d\r\n",bits_per_pixel);

}

void curr(void)
{
	int fb_fd; 			//fb文件描述符
	char show_back = 0; //是否为BMP标识
	read_Framebuffer(pixel_bits);
	int pixel_bits_x=pixel_bits.x;
	int pixel_bits_y=pixel_bits.y;
	int bits_per_pixel=pixel_bits.bits_per_pix;
	
	char str[100];
 	sprintf(str,"%s/dst.bmp",bmp_path);
	// imwrite((const String&)str,dst);
	///////////////////////////////////////////////////
	//Framebuffr初始化并打开
    fb_fd = open(FBDEVICE, O_RDWR);
	//内存映射
	screensize = pixel_bits_x * pixel_bits_y * bits_per_pixel / 8;//除8表示每个字节8位，bits_per_pixel单位为位
	pfb = (unsigned char *)mmap(NULL, screensize, PROT_READ | PROT_WRITE, MAP_SHARED, fb_fd, 0);
	////////////////////////////////////////////////////	
	show_back=show_bmp(pfb, pixel_bits_x, pixel_bits_y,str);
	if(show_back == -3)//像素值显示失败，it's not a BMP file
	{
		printf("show bmp fail\r\n");
		// 添加其他函数，将其他格式的图片文件写入到FrameBuffer文件中
	}	
	munmap(pfb, screensize);
	close(fb_fd);				//关闭Frambuffer文件
}




void bmp_255(void)
{
	int fb_fd; //fb文件描述符
	char show_back = 0; //是否为BMP标识
	
	char str[100];
	sprintf(str,"%s/dst.bmp",bmp_path);
	read_Framebuffer(pixel_bits);
	int pixel_bits_x=pixel_bits.x;
	int pixel_bits_y=pixel_bits.y;
	int bits_per_pixel=pixel_bits.bits_per_pix;
	vector<Point> contour;       	 //单个轮廓坐标值
	vector<vector<Point>> v_contour; //当前层所有轮廓集合
	Mat dst_basic = Mat::zeros(pixel_bits_y, pixel_bits_x, CV_8UC1);//生成的图片，其分辨率由实际的FrameBuffer来决定
	CvScalar color=cvScalar(255);//填充最大值
	contour = {{0,0},{0,pixel_bits_y},{pixel_bits_x,pixel_bits_y},{pixel_bits_x,0}};
	v_contour.push_back(contour);//追加当前轮廓数据到当前层容器变量中
	drawContours(dst_basic,v_contour ,0, color, CV_FILLED );
	imwrite(str,dst_basic);
	
	///////////////////////////////////////////////////
	//Framebuffr初始化并打开s
    fb_fd = open(FBDEVICE, O_RDWR);
	//内存映射
	screensize = pixel_bits_x * pixel_bits_y * bits_per_pixel / 8;//除8表示每个字节8位，bits_per_pixel单位为位
	pfb = (unsigned char *)mmap(NULL, screensize, PROT_READ | PROT_WRITE, MAP_SHARED, fb_fd, 0);
	////////////////////////////////////////////////////	
	show_back= show_bmp(pfb, pixel_bits_x, pixel_bits_y,str);
	if(show_back == -3)//像素值显示失败，it's not a BMP file
	{
		printf("show bmp fail\r\n");
		// 添加其他函数，将其他格式的图片文件写入到FrameBuffer文件中
	}
	munmap(pfb, screensize);	
	close(fb_fd);				//关闭Frambuffer文件		
	
	contour.clear();//删除容器中的所有元素
	v_contour.clear();//删除容器中的所有元素，这里的元素是同一层中所有轮廓数据
}

void bmp_0(void)
{
	int fb_fd; //fb文件描述符
	char show_back = 0; //是否为BMP标识
	
	char str[100];
	sprintf(str,"%s/dst.bmp",bmp_path);
	
	read_Framebuffer(pixel_bits);
	int pixel_bits_x=pixel_bits.x;
	int pixel_bits_y=pixel_bits.y;
	int bits_per_pixel=pixel_bits.bits_per_pix;
	
	vector<Point> contour;       	 //单个轮廓坐标值
	vector<vector<Point>> v_contour; //当前层所有轮廓集合
	Mat dst_basic = Mat::zeros(pixel_bits_y, pixel_bits_x, CV_8UC1);//生成的图片，其分辨率由实际的FrameBuffer来决定
	CvScalar color=cvScalar(0);//填充最小值
	contour = {{0,0},{0,pixel_bits_y},{pixel_bits_x,pixel_bits_y},{pixel_bits_x,0}};
	v_contour.push_back(contour);//追加当前轮廓数据到当前层容器变量中
	drawContours(dst_basic,v_contour ,0, color, CV_FILLED );
	imwrite(str,dst_basic);
	
	///////////////////////////////////////////////////
	//Framebuffr初始化并打开s
    fb_fd = open(FBDEVICE, O_RDWR);
	//内存映射
	screensize = pixel_bits_x * pixel_bits_y * bits_per_pixel / 8;//除8表示每个字节8位，bits_per_pixel单位为位
	pfb = (unsigned char *)mmap(NULL, screensize, PROT_READ | PROT_WRITE, MAP_SHARED, fb_fd, 0);
	////////////////////////////////////////////////////	
	show_back= show_bmp(pfb, pixel_bits_x, pixel_bits_y,str);
	if(show_back == -3)//像素值显示失败，it's not a BMP file
	{
		printf("show bmp fail\r\n");
		// 添加其他函数，将其他格式的图片文件写入到FrameBuffer文件中
	}
	munmap(pfb, screensize);	
	close(fb_fd);				//关闭Frambuffer文件	
	
	contour.clear();//删除容器中的所有元素
	v_contour.clear();//删除容器中的所有元素，这里的元素是同一层中所有轮廓数据
}


int demo(int argc, char **argv)
{
	int fb_fd;				//fb文件描述符
	char show_back = 0; 	//是否为BMP标识
	int ret=0;
	read_Framebuffer(pixel_bits);
	int pixel_bits_x=pixel_bits.x;
	int pixel_bits_y=pixel_bits.y;
	int bits_per_pixel=pixel_bits.bits_per_pix;	
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
							// printf("str_v=%s\n",str);
							
							///////////////////////////////////////////////////
							//Framebuffr初始化并打开
							fb_fd = open(FBDEVICE, O_RDWR);
							//内存映射
							screensize = pixel_bits_x * pixel_bits_y * bits_per_pixel / 8;//除8表示每个字节8位，bits_per_pixel单位为位
							pfb = (unsigned char *)mmap(NULL, screensize, PROT_READ | PROT_WRITE, MAP_SHARED, fb_fd, 0);
							////////////////////////////////////////////////////
							show_back= show_bmp(pfb, pixel_bits_x, pixel_bits_y,str);
							if(show_back == -3)//像素值显示失败，it's not a BMP file
							{
								printf("show bmp fail\r\n");
								// 添加其他函数，将其他格式的图片文件写入到FrameBuffer文件中
							}
							munmap(pfb, screensize);
							close(fb_fd);				//关闭Frambuffer文件
							
							goto demo_end;
						}
						else
						{
							ret=-1;
						}
					}
					else
					{
						ret=-1;
					}
					
					break;
				}
			}
		}
		demo_end:;
	}
	
	
	return ret;
	
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


	




