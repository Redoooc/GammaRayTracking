#include "main.h"
#include "tcp.h"

int main() {
    DWORD threadId; // 用于存储线程ID的变量
    HANDLE threadHandle = CreateThread(NULL,           // 默认安全属性
                                       0,              // 使用默认堆栈大小
                                       TCPthread, // 线程函数
                                       NULL,           // 传递给ThreadFunction的参数
                                       0,              // 默认创建标志
                                       &threadId);     // 返回线程ID
 
    if (threadHandle == NULL) {
        printf("CreateThread failed: %d\n", GetLastError());
        return 1; // 如果创建失败，返回错误代码
    } else {
        printf("Thread created successfully with ID: %lu\n", threadId);
    }
    
    for(int i = 0; i < SEND_RETRYTIMES; i++){
        int num;
        printf("请输入1：\n");
        scanf("%d", &num);
        if (num == 1) {
            u_send_B1(0,16000,60);
        }
    }

    // 等待线程结束（可选）
    WaitForSingleObject(threadHandle, INFINITE); // 等待直到线程结束
    CloseHandle(threadHandle); // 关闭线程句柄
    return 0;
}

DWORD WINAPI TCPthread(LPVOID lpParam){
    tcp_server();
    return 0;
}