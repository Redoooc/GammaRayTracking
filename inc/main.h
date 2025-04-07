#include <winsock2.h>
#include <windows.h>
#include <stdio.h>

#define MAX_THREAD_NUM 12

DWORD WINAPI TCPthread(LPVOID lpParam);
DWORD WINAPI RECV_ANALYZEthread(LPVOID lpParam);
DWORD WINAPI TRACKINGthread(LPVOID lpParam);

// 模拟线程，若SIMULATION_MODE定义为0则没有函数定义
DWORD WINAPI SIMthread(LPVOID lpParam);
