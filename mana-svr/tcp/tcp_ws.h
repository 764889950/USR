#ifndef __TCP_WS_H__
#define __TCP_WS_H__

/**握手协议*/
void process_ws_shake_hand(int sock, char* http_str);

/**发送信息给客户端*/
void ws_send_data(struct session* s, unsigned char* pkg_data, unsigned int pkg_len);

/**解析包*/
int on_ws_recv_data(struct session* s, unsigned char*pkg_data, int pkg_len, char* msg);

#endif
