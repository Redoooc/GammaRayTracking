#include <stdio.h>
#include <winsock2.h>
#include <windows.h>

#pragma comment(lib, "ws2_32.lib")  // 链接Winsock库

#define SERVER_ADRESS "192.168.137.1"
#define CLIENT_ADRESS "192.168.137.14"
#define PORT 6000
#define BUFFER_SIZE 1024
#define SEND_RETRYTIMES 5 // 发送指令最大允许重试次数

extern unsigned char buffer[BUFFER_SIZE];
extern HANDLE isRecvWaitingForAnalyze;

int tcp_server();
int u_send_B1(int direction, int angle, int speed);