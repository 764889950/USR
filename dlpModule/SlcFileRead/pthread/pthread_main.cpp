#include "data_global.h"
#include "pthread_main.h"

pthread_t id_websocket,
	      id_fifo;


			
void ReleaseResource (int signo)
{
	// pthread_cancel (id_websocket);
	// pthread_cancel (id_uart_cmd);
	printf ("All quit\n");
	exit(0);
}



int pthread_main(void)
{
	//signal (SIGINT, ReleaseResource);
	// pthread_create (&id_websocket, 0, pthread_websocket, NULL);
	pthread_create (&id_fifo, 0, pthread_fifo, NULL);

	// pthread_join (id_websocket, NULL);
	// pthread_join (id_fifo, NULL);

	return 0;
}
