#ifndef __PARAMETER_H_
#define __PARAMETER_H_
/* 版本 V1.1 */
//一
/* 打印参数 */
//1、管理员配置界面
extern double zoom_x;//读写。X缩放比例;
extern double zoom_y;//读写。y缩放比例;
extern long buttom;//读写。基底层层数
extern long buttom_mid;//读写。前几层层数
extern long t_buttom_mid;//读写。前几层曝光时间(ms)
extern long t_other;//读写。其他层曝光时间(ms)
extern long Pedal;//读写。斜拉层数
extern long t_pull_lift;//读写。提拉一层间隔出光时间(ms)
extern long print_back_level_z;//读写。Z轴脱模距离(mm)
extern long print_back_level_Pedal;//读写。斜拉轴脱模距离(mm)
extern long LED_intensity;//读写。光功率设置值


//2、管理员扩展配置界面
extern long page_set_o_stop;//读写。急停按钮标识，为1告知设备急停，急停结束时该值自动清零
extern long page_set_z_up;//读写.为1告知设备运动，结束时该值自动清零
extern long page_set_z_down;//读写.为1告知设备运动，结束时该值自动清零
extern long page_set_p_up;//读写.为1告知设备运动，结束时该值自动清零
extern long page_set_p_down;//读写.为1告知设备运动，结束时该值自动清零
extern double page_set_dis;//读写。点击按钮时单次运动的行程


//3、超级管理员基本配置界面
extern double z_rpo;//读写。Z轴1个脉冲走过的长度mm;
extern double Pedal_rpo;//读写。斜拉1脉冲走过的长度(mm);
extern long speed_z;//读写。Z轴脱模速度(mm/s);
extern long speed_Pedal;//读写。斜拉轴速度(mm/s);
extern long dir_z;//读写。Z轴方向，0为负，1为正，初始为1;是否有范围限制:是(0、1)，其他不识别
extern long dir_Pedal;//读写。斜拉轴方向，0为负，1为正，初始为1;是否有范围限制:是(0、1)，其他不识别

//4、超级管理员扩展配置界面
extern long led_choose;//读写。光机选择;

//5、其它控制参数;
extern long move_z;//读写。1为正方向运动，-1为负方向运动，运动完成后自动置0
extern long move_Pedal;//读写。1为正方向运动，-1为负方向运动，运动完成后自动置0
extern long init_flag;//读写。设备初始化完成标识，为1表示已完成初始化
extern long print_flag;//读写。打印状态，0-停止打印，1-正在打印，2-暂停打印
extern double print_work_level_z;//读写。Z轴工作原点(mm);
extern double print_work_level_Pedal;//读写。斜拉轴工作原点(mm);
//extern  double LED_intensity_set;//读写。光功率调节(mW/cm^2)，该值更新过程中会在其它位置自动保存，重启打印机数据任然保留，参数在没设置之前也会有一个初始值;是否有范围限制:是(0.01-100.00)
extern long LED;//读写。光机光源电源开关，该值断电不保存，该参数为主动参数，不考虑光机是否接收，初始为0(1为打开了光机光源，0为关闭了光机光源);是否有范围限制:是(0或1)，其他不识别
//extern long LED_add_dis;//读写。光功率调节，-1为减，1为加，加完或减完会置零，该值断电不保存，初始为0;是否有范围限制:是(-1、0、1)，其他不识别
extern long file_continue_flag; //读写。初始为0;解析标志，为1则继续解析并显示，覆盖原来的图片并使图像传输到光机
extern long home_flag_z; //读写。回零标识，为1开始回零，回零完成时会置零，该值断电不保存，初始为0;是否有范围限制:是(0、1)，其他不识别
extern long home_flag_Pedal;//读写。回零标识，为1开始回零，回零完成时会置零，该值断电不保存，初始为0;是否有范围限制:是(0、1)，其他不识别
extern long work_point_flag_z;//读写。回工作原点标识，为1开始回工作原点，完成时会置零，该值断电不保存，初始为0;是否有范围限制:是(0、1)，其他不识别
extern long work_point_flag_Pedal;//读写。回工作原点标识，为1开始回工作原点，完成时会置零，该值断电不保存，初始为0;是否有范围限制:是(0、1)，其他不识别
extern long motor_flag_stop;//读写。电机停止标识，为1停止，停止完成时会置零，该值断电不保存，初始为0;是否有范围限制:是(0、1)，其他不识别
extern double print_level_z;//读。当前Z轴高度(mm)，该值断电不保存
extern double print_level_Pedal;//读。当前斜拉高度(mm)，该值断电不保存
//extern double LED_intensity;//读。光功率密度(mW/cm^2)，该值断电不保存，初始为0;是否有范围限制:(参考极限值:0.01-100.00)
//extern long engine_init;//光机矫正，设备开机立即自动矫正，该值在其他位置自动更新，"engine_init=1"表示光机矫正失败，该值断电不保存，初始为0;是否有范围限制:是(0或1)
extern long led_choose;//光机选择，读写。0：低分辨率；1：高分辨率
extern long print_style;//打印类型。0：单幅图；1：九宫格

//二、读取打印参数
//1打印用户界面
extern double level_z;//读。打印层厚(mm)，该值断电不保存，初始为0
extern long layer_z;//读。打印总层数，该值断电不保存，初始为0
extern long time_print;//读。已打印用时(s)，该值断电不保存
extern long time_print_res;//读。剩余打印时间(s)，该值断电不保存
extern long sch;//读。打印进程，该值断电不保存，初始为0，读取文件成功后该值开始更新
extern long print_z;//读。当前所打印层，该值断电不保存，初始为0，读取文件成功后该值开始更新

//文件相关
extern char print_path[700];//读。文件深度默认最高为4，当前打印文件全路径
extern char file_name[256];//要打印的文件的文件名(GBK编码）
extern char file_name_UTF_8[256];//要打印的文件的文件名(UTF-8编码)
extern long file_num;//读。当前打印文件总数量
extern long file;//读。当前打印文件为第几个文件

//三、其他参数 料盘温度读取与设置，显示在 用户界面 上
//温度:0-70
//extern int temperature;//读。读取到的温度(单位:摄氏度)，在其他位置自动更新，周期为1s
//extern int temperature_set;//写。要设置的温度(单位:摄氏度)，在其他位置自动更新到目标部件，周期为1s

//位图文件路径与配置文件路径设置
extern char bmp_path[];
extern char print_cfg_path[];


#endif
