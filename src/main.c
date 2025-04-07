#include "main.h"
#include "tcp.h"
#include "recv_analyze.h"
#include "file_read&write.h"
#include "collect_test_data.h"
#include "tracking.h"
#include "data_visual.h"

#if SIMULATION_MODE
#include "simulation.h"
#endif

DWORD threadId[MAX_THREAD_NUM]; // 用于存储线程ID的变量
HANDLE threadHandle[MAX_THREAD_NUM];

int isNeedAdd1 = 0;
float Data1 = 0;
int isNeedAdd2 = 0;
float Data2 = 0;

int main() {
    /*  SDL初始化部分  */
    SDL_SetMainReady(); // 明确告知 SDL 主函数已由用户提供
    // 初始化 SDL
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        SDL_Log("SDL_Init failed: %s\n", SDL_GetError());
        return -1;
    }
    // 初始化 SDL_ttf
    if (TTF_Init() != 0) {
        SDL_Log("TTF_Init failed: %s\n", TTF_GetError());
        SDL_Quit();
        return -1;
    }
    init_font();
    /*SDL初始化部分结束*/

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

    threadHandle[2] = CreateThread(NULL,           // 默认安全属性
                                   0,              // 使用默认堆栈大小
                                   TRACKINGthread,      // 线程函数
                                   NULL,           // 传递给ThreadFunction的参数
                                   0,              // 默认创建标志
                                   &threadId[2]);     // 返回线程ID

    if (threadHandle[2] == NULL) {
        printf("CreateThread2 failed: %d\n", GetLastError());
    } else {
        printf("Thread2 created successfully with ID: %lu\n", threadId[2]); 
    }

#if SIMULATION_MODE
    threadHandle[3] = CreateThread(NULL,           // 默认安全属性
                                   0,              // 使用默认堆栈大小
                                   SIMthread,      // 线程函数
                                   NULL,           // 传递给ThreadFunction的参数
                                   0,              // 默认创建标志
                                   &threadId[3]);     // 返回线程ID

    if (threadHandle[3] == NULL) {
        printf("CreateThread3 failed: %d\n", GetLastError());
    } else {
        printf("Thread3 created successfully with ID: %lu\n", threadId[3]); 
    }
#endif

    init_graph(0);
    init_graph(1);
    // init_graph(2);
    // init_graph(3);
    // init_graph(4);

    for(int i = 0; i < 1; i++){
        int num;
        printf("请输入1：\n");
        scanf("%d", &num);
        if (num == 1) {
            u_send_A8(1, 2071, 0, 1, 1, 5);
            Sleep(100);
            u_send_A8(2, 2071, 0, 1, 1, 5);
            Sleep(100);
            u_send_A8(3, 2071, 0, 1, 1, 5);
            Sleep(100);
            u_send_A8(4, 2071, 0, 1, 1, 6);
            Sleep(100);
            u_send_A3(5,100);
            Sleep(100);
            u_send_B5(1);
            Sleep(100);
            // u_send_B2(5,1);
            // Sleep(100);
            // u_send_B1(0,18000,30);
            // Sleep(100);
            // always_record();
            collect_test_data_30degree();
            // collect_test_data_horizontal();
        }
    }

    while (1)
    {
        int x_input, y_input;
        while(!isNotRotating);
        printf("请输入x坐标：\n");
        scanf("%d", &x_input);
        printf("请输入y坐标：\n");
        scanf("%d", &y_input);
        rotate(x_input,y_input,60,25,0);
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

DWORD WINAPI TRACKINGthread(LPVOID lpParam){
    tracking();
    return 0;
}

#if SIMULATION_MODE
DWORD WINAPI SIMthread(LPVOID lpParam){
    sim_thread();
    return 0;
}
#endif