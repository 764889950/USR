#include "parameter.h"
#include <stdio.h>  

//打印参数
double thickness=0;//读。打印层厚(mm)，初始为0
long totalLayers=0;//读。打印总层数，初始为0

int pixel_bits_x=1920;//图片的X轴分辨率，初始为1920
int pixel_bits_y=1080;//图片的Y轴分辨率，初始为1080
float zoom_x=24;//X轴放大比例，初始为20
float zoom_y=24;//Y轴放大比例，初始为20

long LayerCurrent=0;//当前解析层，初始为0
char mirrorBmpFlag=0;//	图像翻转标识，等于0默认翻转，当等于1时不翻转



//读取并送显的默认位图文件路径设置
char bmp_path[]="/home/pi/wiringpi";




