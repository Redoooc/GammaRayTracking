#include <stdio.h>
#include <winsock2.h>
#include <windows.h>

#pragma comment(lib, "ws2_32.lib")  // 链接Winsock库

#define SERVER_ADRESS "192.168.137.1"
#define CLIENT_ADRESS "192.168.137.14"
#define PORT 6000
#define TCP_RECV_BUFFER_SIZE 20000
#define SEND_RETRYTIMES 5 // 发送指令最大允许重试次数
#define SEND_RETRY_INTERVAL_TIME 10 // 发送指令重试间隔时间（单位：毫秒）

extern unsigned char tcpRecvBuffer[TCP_RECV_BUFFER_SIZE];
extern HANDLE isRecvWaitingForAnalyze;

int tcp_server();
int u_send_B1(int direction, int angle, int speed);
int u_send_B2(int channel, int status);
int u_send_B6(int angle_x, int angle_z, int speed_x, int speed_z, int direction_z);
int u_send_B9(int direction, int angle_x, int angle_z, int speed_x, int speed_z);