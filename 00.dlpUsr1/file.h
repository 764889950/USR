#ifndef __FILE_
#define __FILE_

extern long usb_flag;//U盘变化标识,U盘插入,该标志会置成非0
extern long file_change_flag;//u盘读取内容发生变化标识

void printcfg_read(void);
void printcfg_write(void);
void read_print_file(void);
int file_main(void);


#endif