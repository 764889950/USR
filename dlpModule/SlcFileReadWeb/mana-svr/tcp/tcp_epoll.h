#ifndef __TCP_EPOLL_H__
#define __TCP_EPOLL_H__

#ifdef  __cplusplus  
extern "C" {  
#endif  

extern char str_net[100]; //由TCP事务进行阻塞，websocket接收到数据，将赋值给str_net
extern char websocketRecv;
		
void start_server(int port);

#ifdef  __cplusplus  
}  
#endif  /* end of __cplusplus */  

#endif
