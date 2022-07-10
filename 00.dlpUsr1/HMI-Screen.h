#ifndef __HMI_SCREEN_
#define __HMI_SCREEN_

extern long page_menu;
extern char reboot_flag;

int screen_main(void);
void hex_send(char com[],char data_length);
void page_projector_init_fail_switch(void);
void page_Z_init_fail_switch(void);
void page_init_switch(void);
void page_home_switch(void);
void page_model_switch(void);
void page_model_save_succeed_switch(void);
void page_model_save_fail_switch(void);
void page_model_delet_fail_switch(void);
void page_error_switch(void);
void page_admin_config_switch(void);
void page_supper_admin_config_switch(void);
void page_print_switch(void);
void page_print_load_switch(void);
void menu_show_clear(void);
void file_show(void);
void file_get(void);
void show_print_inf_init(void);
void show_inf_init(void);
void show_print_inf_print(void);

#endif