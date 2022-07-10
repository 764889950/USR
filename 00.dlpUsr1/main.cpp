#include "wiringSerial.h"
#include "DEV_Config.h"
#include "DEV_Config_2.h"//其他设备初始化
#include "parameter.h"
#include "HMI-Screen.h"
#include "file.h"
#include "marlinHost.h"
#include "SLC_Read.h"

#include "data_global.h"



using namespace std;

// void  Handler(int signo)
// {
    // //System Exit
    // printf("\r\nHandler:Motor Stop\r\n");
    // DRV8825_SelectMotor(MOTOR1);
    // DRV8825_Stop();
    // DRV8825_SelectMotor(MOTOR2);
    // DRV8825_Stop();
    // DEV_ModuleExit();
	
	// serialClose(fd_serial);
	// serialClose(fd_ttyUSB0);
	// serialClose(fd_ttyUSB1);
	// serialClose(fd_ttyUSB2);
	// serialClose(fd_ttyUSB3);	

    // exit(0);
// }


int main(void)
{
	// if(strstr("N6 M107*35","M107")!=NULL)
	// {
		// cout<<"M ok"<<endl;
		
	// }

// cout<<"Last Line:"<<Line_Last<<endl;

    //1.System Initialization
    if(DEV_ModuleInit())
        exit(0);
	
    // // Exception handling:ctrl + c
    // signal(SIGINT, Handler);
    
    // // /*
    // // # 1.8 degree: nema23, nema14
    // // # softward Control :
    // // # 'fullstep': A cycle = 200 steps
    // // # 'halfstep': A cycle = 200 * 2 steps
    // // # '1/4step': A cycle = 200 * 4 steps
    // // # '1/8step': A cycle = 200 * 8 steps
    // // # '1/16step': A cycle = 200 * 16 steps
    // // # '1/32step': A cycle = 200 * 32 steps
    // // */
    // // DRV8825_SelectMotor(MOTOR1);
    // // DRV8825_SetMicroStep(HARDWARD, (char *)"fullstep");
    // // DRV8825_TurnStep(BACKWARD, 200, 2);
    // // DRV8825_Stop();

    // // /*
    // // # 28BJY-48:
    // // # softward Control :
    // // # 'fullstep': A cycle = 2048 steps
    // // # 'halfstep': A cycle = 2048 * 2 steps
    // // # '1/4step': A cycle = 2048 * 4 steps
    // // # '1/8step': A cycle = 2048 * 8 steps
    // // # '1/16step': A cycle = 2048 * 16 steps
    // // # '1/32step': A cycle = 2048 * 32 steps
    // // */
    // // DRV8825_SelectMotor(MOTOR2);
    // // DRV8825_SetMicroStep(SOFTWARD, (char *)"fullstep");
    // // DRV8825_TurnStep(BACKWARD, 2048, 2);
	// // DRV8825_Stop();



	printcfg_read();//读取公共配置
	// printf("print_work_level_z=%lf,print_work_level_Pedal=%lf\r\n",print_work_level_z,print_work_level_Pedal);
	// printf("z_rpo=%lf,Pedal_rpo=%lf\r\n",z_rpo,Pedal_rpo);
	// printf("dir_z=%ld,dir_Pedal=%ld\r\n",dir_z,dir_Pedal);
	// printf("speed_z=%ld,speed_Pedal=%ld\r\n",speed_z,speed_Pedal);
	// printf("led_choose=%ld\r\n",led_choose);
	// printf("print_style=%ld\r\n",print_style);
	// printf("buttom=%ld\r\n",buttom);






	printf("hardware_init_main\r\n");
	hardware_init_main();//外设硬件初始化，包括显示屏，所以要放在前面
	file_main();//文件系统初始化
	screen_main();//显示初始化
	slc_main();//SLC文件读取初始化
	motor_main();

	while(1);

    //3.System Exit
    DEV_ModuleExit();
    return 0;
}


    // pinMode(0,OUTPUT);
