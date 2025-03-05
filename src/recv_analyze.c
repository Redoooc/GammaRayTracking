#include "recv_analyze.h"
#include "tcp.h"

int recv_analyze(){
    while(1){
        WaitForSingleObject(isRecvWaitingForAnalyze, INFINITE);
        printf("%x\n",buffer[3]);
    }
}