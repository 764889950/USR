/*
 * ESPRESSIF MIT License
 *
 * Copyright (c) 2016 <ESPRESSIF SYSTEMS (SHANGHAI) PTE LTD>
 *
 * Permission is hereby granted for use on ESPRESSIF SYSTEMS ESP8266 only, in which case,
 * it is free of charge, to any person obtaining a copy of this software and associated
 * documentation files (the "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished
 * to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#include "ets_sys.h"
#include "osapi.h"
#include "ip_addr.h"
#include "espconn.h"
#include "mem.h"
#include "user_interface.h"
#include "smartconfig.h"
#include "airkiss.h"
#include "c_types.h"
#include "upgrade.h"

#define fileBinPath  "xuhong/8266NewBin/"

LOCAL esp_udp ssdp_udp;
LOCAL struct espconn pssdpudpconn;
LOCAL os_timer_t ssdp_time_serv;


void ICACHE_FLASH_ATTR ota_finished_callback(void *arg)
{
	struct upgrade_server_info *update = arg;
	if (update->upgrade_flag == true){
		os_printf("OTA  Success ! rebooting!\n");
		system_upgrade_reboot();
	}else{
		os_printf("OTA failed!\n");
	}

	os_free(update->pespconn);
	os_free(update->url);
	os_free(update);
}

/**
 * server_ip: 服务器地址
 * port:服务器端口
 * path:文件
 */
void ICACHE_FLASH_ATTR ota_start_Upgrade(const char *server_ip, uint16_t port,const char *path) {
	const char* file;
	//获取系统的目前加载的是哪个bin文件
	uint8_t userBin = system_upgrade_userbin_check();

	switch (userBin) {

	//如果检查当前的是处于user1的加载文件，那么拉取的就是user2.bin
	case UPGRADE_FW_BIN1:
		file = "user2.4096.new.4.bin";
		break;

		//如果检查当前的是处于user2的加载文件，那么拉取的就是user1.bin
	case UPGRADE_FW_BIN2:
		file = "user1.4096.new.4.bin";
		break;

		//如果检查都不是，可能此刻不是OTA的bin固件
	default:
		os_printf("Fail read system_upgrade_userbin_check! \n\n");
		return;
	}

	struct upgrade_server_info* update =
			(struct upgrade_server_info *) os_zalloc(
					sizeof(struct upgrade_server_info));
	update->pespconn = (struct espconn *) os_zalloc(sizeof(struct espconn));
	//设置服务器地址
	os_memcpy(update->ip, server_ip, 4);
	//设置服务器端口
	update->port = port;
	//设置OTA回调函数
	update->check_cb = ota_finished_callback;
	//设置定时回调时间
	update->check_times = 10000;
	//从 4M *1024 =4096申请内存
	update->url = (uint8 *)os_zalloc(4096);

	//打印下請求地址
	os_printf("Http Server Address:%d.%d.%d.%d ,port: %d,filePath: %s,fileName: %s \n",
			IP2STR(update->ip), update->port, path, file);

	//拼接完整的 URL去请求服务器
	os_sprintf((char*) update->url, "GET /%s%s HTTP/1.1\r\n"
			"Host: "IPSTR":%d\r\n"
	"Connection: keep-alive\r\n"
	"\r\n", path, file, IP2STR(update->ip), update->port);

	if (system_upgrade_start(update) == false) {
		os_printf("[OTA]Could not start upgrade\n");
		//释放资源
		os_free(update->pespconn);
		os_free(update->url);
		os_free(update);
	} else {
		os_printf("[OTA]Upgrading...\n");
	}
}

void ICACHE_FLASH_ATTR
smartconfig_done(sc_status status, void *pdata){
    switch(status) {
        case SC_STATUS_WAIT:
            os_printf("SC_STATUS_WAIT\n");
            break;
        case SC_STATUS_FIND_CHANNEL:
            os_printf("SC_STATUS_FIND_CHANNEL\n");
            break;
        case SC_STATUS_GETTING_SSID_PSWD:
            os_printf("SC_STATUS_GETTING_SSID_PSWD\n");
			sc_type *type = pdata;
            if (*type == SC_TYPE_ESPTOUCH) {
                os_printf("SC_TYPE:SC_TYPE_ESPTOUCH\n");
            } else {
                os_printf("SC_TYPE:SC_TYPE_AIRKISS\n");
            }
            break;
        case SC_STATUS_LINK:
            os_printf("SC_STATUS_LINK\n");
            struct station_config *sta_conf = pdata;
	
	        wifi_station_set_config(sta_conf);
	        wifi_station_disconnect();
	        wifi_station_connect();
            break;
        case SC_STATUS_LINK_OVER:
            os_printf("SC_STATUS_LINK_OVER\n");
            if (pdata != NULL) {
				//SC_TYPE_ESPTOUCH
                uint8 phone_ip[4] = {0};

                os_memcpy(phone_ip, (uint8*)pdata, 4);
			    os_printf("Phone ip: %d.%d.%d.%d\n", phone_ip[0], phone_ip[1],
					phone_ip[2], phone_ip[3]);
                os_printf("ready to upgrade\r\n");

            char serverip[] = { 192, 168, 0, 104 };
			uint16_t port = 80; //本地端口或者远程服务器端口
			ota_start_Upgrade(serverip, port, fileBinPath); //无线升级开始

            } else {
            	//SC_TYPE_AIRKISS - support airkiss v2.0
			//airkiss_start_discover();
            }
            smartconfig_stop();
            break;
    }
	
}

/******************************************************************************
 * FunctionName : user_rf_cal_sector_set
 * Description  : SDK just reversed 4 sectors, used for rf init data and paramters.
 *                We add this function to force users to set rf cal sector, since
 *                we don't know which sector is free in user's application.
 *                sector map for last several sectors : ABCCC
 *                A : rf cal
 *                B : rf init data
 *                C : sdk parameters
 * Parameters   : none
 * Returns      : rf cal sector
*******************************************************************************/
uint32 ICACHE_FLASH_ATTR
user_rf_cal_sector_set(void)
{
    enum flash_size_map size_map = system_get_flash_size_map();
    uint32 rf_cal_sec = 0;

    switch (size_map) {
        case FLASH_SIZE_4M_MAP_256_256:
            rf_cal_sec = 128 - 5;
            break;

        case FLASH_SIZE_8M_MAP_512_512:
            rf_cal_sec = 256 - 5;
            break;

        case FLASH_SIZE_16M_MAP_512_512:
        case FLASH_SIZE_16M_MAP_1024_1024:
            rf_cal_sec = 512 - 5;
            break;

        case FLASH_SIZE_32M_MAP_512_512:
        case FLASH_SIZE_32M_MAP_1024_1024:
            rf_cal_sec = 1024 - 5;
            break;

        case FLASH_SIZE_64M_MAP_1024_1024:
            rf_cal_sec = 2048 - 5;
            break;
        case FLASH_SIZE_128M_MAP_1024_1024:
            rf_cal_sec = 4096 - 5;
            break;
        default:
            rf_cal_sec = 0;
            break;
    }

    return rf_cal_sec;
}


void ICACHE_FLASH_ATTR
user_rf_pre_init(void)
{
}

void ICACHE_FLASH_ATTR
user_init(void)
{

	os_printf("\n*********************************\r\n");
	os_printf("now user bin:%d\r\n", system_upgrade_userbin_check());
	os_printf("SDK version:%s\r\n", system_get_sdk_version());
	os_printf("Build Datetime:%s %s\r\n", __DATE__, __TIME__);
	os_printf("Chip ID:%d\r\n", system_get_chip_id());
	os_printf("CPU freq:%d\r\n", system_get_cpu_freq());
	os_printf("Free heap size:%d\r\n", system_get_free_heap_size());
	os_printf("\n*********************************\r\n");

	os_printf("\n1111111111111111\r\n");
	smartconfig_set_type(SC_TYPE_ESPTOUCH);
    wifi_set_opmode(STATION_MODE);
    smartconfig_start(smartconfig_done);
}

//波特率 74880
