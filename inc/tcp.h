#include <stdio.h>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")  // 链接Winsock库

#define SERVER_ADRESS "192.168.137.150"
#define CLIENT_ADRESS "192.168.137.14"
#define PORT 6000
#define BUFFER_SIZE 1024
#define SEND_RETRYTIMES 5 // 发送指令最大允许重试次数

int tcp_server();
int u_send_B1(int direction, int angle, int speed);