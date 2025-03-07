#include "main.h"
#include "tcp.h"
#include "recv_analyze.h"
#include "file_read&write.h"
#include "collect_test_data.h"

DWORD threadId[MAX_THREAD_NUM]; // 用于存储线程ID的变量
HANDLE threadHandle[MAX_THREAD_NUM];

int main() {
    threadHandle[0] = CreateThread(NULL,           // 默认安全属性
                                   0,              // 使用默认堆栈大小
                                   TCPthread,      // 线程函数
                                   NULL,           // 传递给ThreadFunction的参数
                                   0,              // 默认创建标志
                                   &threadId[0]);     // 返回线程ID
 
    if (threadHandle[0] == NULL) {
        printf("CreateThread0 failed: %d\n", GetLastError());
    } else {
        printf("Thread0 created successfully with ID: %lu\n", threadId[0]);
    }
    
    threadHandle[1] = CreateThread(NULL,           // 默认安全属性
                                   0,              // 使用默认堆栈大小
                                   RECV_ANALYZEthread,      // 线程函数
                                   NULL,           // 传递给ThreadFunction的参数
                                   0,              // 默认创建标志
                                   &threadId[1]);     // 返回线程ID
 
    if (threadHandle[1] == NULL) {
        printf("CreateThread1 failed: %d\n", GetLastError());
    } else {
        printf("Thread1 created successfully with ID: %lu\n", threadId[1]);
    }

    for(int i = 0; i < SEND_RETRYTIMES; i++){
        int num;
        printf("请输入1：\n");
        scanf("%d", &num);
        if (num == 1) {
            //u_send_B1(0,16000,60);
            char addr[] = "../../ExperimentalData/example.txt";
            appendOpenFile(addr);
            int msg[] = {-1,-1,12654,13589,14568,65598};
            char *char_msg = array_int_to_char(msg, sizeof(msg)/sizeof(msg[0]));
            writeFile(char_msg);
            free(char_msg);
        }
    }

    while (1)
    {
        /* code */
    }
    

    // // 等待线程结束（可选）
    // WaitForSingleObject(threadHandle[0], INFINITE); // 等待直到线程结束
    // CloseHandle(threadHandle[0]); // 关闭线程句柄
    return 0;
}

DWORD WINAPI TCPthread(LPVOID lpParam){
    tcp_server();
    return 0;
}

DWORD WINAPI RECV_ANALYZEthread(LPVOID lpParam){
    recv_analyze();
    return 0;
}