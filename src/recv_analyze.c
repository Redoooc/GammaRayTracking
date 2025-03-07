#include "recv_analyze.h"
#include "tcp.h"

// 允许发送旋转指令标志（云台旋转时不能发送旋转指令）
volatile int isNotRotating = 0;

int recv_analyze(){
    while(1){
        WaitForSingleObject(isRecvWaitingForAnalyze, INFINITE);
        if(tcpRecvBuffer[0] == 0x55){
            switch (tcpRecvBuffer[8])
            {
            case 0xC2:  //设备状态反馈
                recv_process_C2();
                break;

            case 0xC3:  //旋转完成反馈
                recv_process_C3();
                break;

            case 0xD1:  //能谱数据
                recv_process_D1();
                break;
            
            default:
                printf("接收到MID为：0x%X 的数据。\n",tcpRecvBuffer[8]);
                break;
            }
        }else{
            printf("错误：接受到开头不为0x55的数据！\n");
        }
    }
}

void recv_process_C2(){

}

void recv_process_C3(){
    isNotRotating = 1;
}

void recv_process_D1(){
    printf("接收到能谱数据\n");
}