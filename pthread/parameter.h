#ifndef __PARAMETER_H_
#define __PARAMETER_H_

#define fileDepth 1024  //文件深度,即文件路径的长度，如"FIFO_REC"和"FIFO_BACK"长度不能超过fileDepth；以及与文件深度相关的指令

//FIFO文件路径
#define FIFO_REC "/home/ReadFifoRec"//用于本工程接收FIFO

//辅助文件路径，包含字符信息，用于控制CGI返回,通常位于用户目录
#define FIFO_BACK "/home/ReadFifoBack"

#endif
