#include <stdio.h>      //printf()
#include <stdlib.h>     //exit()
#include <unistd.h> 	//usleep
#include <string.h>     //strlen
#include <ctype.h>      //tolower
#include <stdarg.h>		//va_list  ...

#include <vector>
#include <string>//c++ string类
#include <iostream>

//公共数据头文件
#include "./pthread/pthread_main.h"
#include "./pthread/data_global.h"
#include "./cJSON-master/cJSON.h"
#include "./libcrc-master/include/checksum.h"
/////////////////////

using namespace std;


/* ****************************************************** */
/* 工程选择 */
/*01 FDM单机自动打印*/
//#define autoFdm 	 

/*02 高能打印机辅助配套控制*/
//#define assistLdm


//各工程头文件
#ifdef autoFdm    /*  FDM单机自动打印   */
	#include "./octoModule/octoMovement.h"
	#include "./octoModule/wiringSerial.h"
	#include "./01.autoFdm/usr01.h"
#endif
	
#ifdef assistLdm	/*  高能打印机辅助配套控制   */
	#include "./02.assistLdm/usr02.h"
#endif	

/////////////////////////////////////////////

/* ****************************************************** */


int main(int argc, char **argv)
{
	//FDM单机自动打印
	#ifdef autoFdm
		autoFdmUsr01();
	#endif	
	
	/*  高能打印机辅助配套控制   */
	#ifdef assistLdm
		assistLdmUsr02();
	#endif

	
	return 0;
}



