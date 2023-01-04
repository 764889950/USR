#include "data_global.h"
#include "pthread_main.h"
#include "../mana-svr/tcp/tcp_epoll.h"/* start_server函数，str_net[]接收变量，recv_ok接收标识*/


// void *pthread_data_procese (void *arg)
// {
	// while(1)
	// {

		
	// }
	// return NULL;
// }

//用于启动websocket
void *pthread_websocket(void *arg)
{
	// pthread_t id_pthread_data_procese;
	// pthread_create (&id_pthread_data_procese, 0, pthread_data_procese, NULL);

	start_server(9523);//启动websocket，准备接收数据,阻塞
	while(1);

	return NULL;
}


