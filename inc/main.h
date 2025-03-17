#include <windows.h>
#include <stdio.h>

#define MAX_THREAD_NUM 12

DWORD WINAPI TCPthread(LPVOID lpParam);
DWORD WINAPI RECV_ANALYZEthread(LPVOID lpParam);
DWORD WINAPI TRACKINGthread(LPVOID lpParam);