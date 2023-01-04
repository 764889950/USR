#ifndef __PARAMETER_H_
#define __PARAMETER_H_

//打印参数
extern double thickness;//打印层厚(mm)，初始为0
extern long totalLayers;//打印总层数，初始为0

extern int pixel_bits_x;//图片的X轴分辨率
extern int pixel_bits_y;//图片的Y轴分辨率
extern float zoom_x;//X轴放大比例
extern float zoom_y;//Y轴放大比例

extern long LayerCurrent;//当前解析层
extern char mirrorBmpFlag;//图像翻转标识


//读取并送显的默认位图文件路径设置
extern char bmp_path[];

#define fileDepth 1024  //文件深度,即文件路径的长度，如"FIFO_REC"和"FIFO_BACK"长度不能超过fileDepth；以及与文件深度相关的指令,

//FIFO文件路径
#define FIFO_REC "/SLC_ReadFifoRec"//用于本工程接收FIFO

//辅助文件路径，用于控制CGI返回,通常位于用户目录
#define FIFO_BACK "/home/gcp"

#endif
