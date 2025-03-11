#include "recv_analyze.h"
#include "tcp.h"

// 允许发送旋转指令标志（云台旋转时不能发送旋转指令）
volatile int isNotRotating = 0;
CircularBuffer clc_buf;
recv_msg recv_share;

int recv_analyze(){
    while(1){
        WaitForSingleObject(isRecvWaitingForAnalyze, INFINITE);
        if(tcpRecvBuffer[0] == 0x55){
            switch (tcpRecvBuffer[8])
            {
            case 0xB2:  //能谱关闭反馈
                recv_process_B2();
                break; 
            
            case 0xB6:  //复位完成反馈
                recv_process_B6();
                break; 

            case 0xBA:  //中途通过指令停止转动反馈
                recv_process_BA();
                break; 

            case 0xC2:  //设备状态反馈
                recv_process_C2();
                break;  

            case 0xC3:  //旋转完成反馈
                recv_process_C3();
                break;

            case 0xC6:  //云台自检完成反馈
                recv_process_C6();
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

void recv_process_B2(){

}

void recv_process_B6(){
    isNotRotating = 1;
}

void recv_process_BA(){
    isNotRotating = 1;
}

void recv_process_C2(){

}

void recv_process_C3(){
    isNotRotating = 1;
}

void recv_process_C6(){
    isNotRotating = 1;
    initBuffer(&clc_buf);
}

void recv_process_D1(){
    if(!checkSpecAllWritten() && recv_share.angle_x == decode_coordinates_x(0) && recv_share.angle_z == decode_coordinates_z(0)){ //上个数据包没有包含所有探测器数据且当前数据包为上个数据包的续传（续传判断：探测坐标位置相同）
        divide_spec_data();
    }else{
        initRecvShare();
        recv_share.angle_x = decode_coordinates_x(0);
        recv_share.angle_z = decode_coordinates_z(0);
        divide_spec_data();
    }
    if(checkSpecAllWritten()){
        writeBuffer(&clc_buf, recv_share);
        initRecvShare();
    }
    // recv_msg temp;
    // if(readBuffer(&clc_buf, &temp)){
    //     printf("横坐标为%d，纵坐标为%d，spec1%d，2:%d，3:%d，4:%d\n",temp.angle_x,temp.angle_z,temp.spec1,temp.spec2,temp.spec3,temp.spec4);
    // }
}

// 计算横坐标
int decode_coordinates_x(int num){
    int raw_angle_x = 0;
    raw_angle_x = (int)((tcpRecvBuffer[(num*4146)+4108] << 8) | (tcpRecvBuffer[(num*4146)+4109]));
    return raw_angle_x;
}

// 计算纵坐标
int decode_coordinates_z(int num){
    int raw_angle_z = 0;
    if(tcpRecvBuffer[(num*4146)+4111]!=0x00){
        raw_angle_z = (int)((tcpRecvBuffer[(num*4146)+4110] << 8) | (tcpRecvBuffer[(num*4146)+4111]));
    }else if (tcpRecvBuffer[(num*4146)+4113]!=0x00){
        raw_angle_z = -(int)((tcpRecvBuffer[(num*4146)+4112] << 8) | (tcpRecvBuffer[(num*4146)+4113]));
    }else{
        raw_angle_z = 0;
    }
    return raw_angle_z;
}

// 分割数据
void divide_spec_data(){
    for(int i = 0; i < 4; i++){ 
        if(tcpRecvBuffer[i*4146] == 0x55 && recv_share.angle_x == decode_coordinates_x(i) && recv_share.angle_z == decode_coordinates_z(i)){ // 分割数据包并确保四个探测器的数据在同一处测得
            switch (tcpRecvBuffer[i*4146 + 4107])
            {
                case 0XF1:
                recv_share.spec1 = (int)(tcpRecvBuffer[i*4146 + 15] << 24 | tcpRecvBuffer[i*4146 + 16] << 16 | tcpRecvBuffer[i*4146 + 17] << 8 | tcpRecvBuffer[i*4146 + 18]);
                break;

                case 0XF2:
                recv_share.spec2 = (int)(tcpRecvBuffer[i*4146 + 15] << 24 | tcpRecvBuffer[i*4146 + 16] << 16 | tcpRecvBuffer[i*4146 + 17] << 8 | tcpRecvBuffer[i*4146 + 18]);
                break;

                case 0XF3:
                recv_share.spec3 = (int)(tcpRecvBuffer[i*4146 + 15] << 24 | tcpRecvBuffer[i*4146 + 16] << 16 | tcpRecvBuffer[i*4146 + 17] << 8 | tcpRecvBuffer[i*4146 + 18]);
                break;

                case 0XF4:
                recv_share.spec4 = (int)(tcpRecvBuffer[i*4146 + 15] << 24 | tcpRecvBuffer[i*4146 + 16] << 16 | tcpRecvBuffer[i*4146 + 17] << 8 | tcpRecvBuffer[i*4146 + 18]);
                break;

                default:
                printf("接收到代码为0X%X的探测器\n",tcpRecvBuffer[i*4146 + 4107]);
                break;
            }
        }else{
            break;
        }
    }
}

void initRecvShare(){
    recv_share.angle_x = 0;
    recv_share.angle_z = 0;
    recv_share.spec1 = -1;
    recv_share.spec2 = -1;
    recv_share.spec3 = -1;
    recv_share.spec4 = -1;
}

// 检查是否有任意Spec数据未被写入（即是否有任意Spec == -1） return为true时所有数据均被写入
bool checkSpecAllWritten(){
    if(recv_share.spec1 != -1 && recv_share.spec2 != -1 && recv_share.spec3 != -1 && recv_share.spec4 != -1){
        return true;
    }else{
        return false;
    }
}

// 初始化环形缓冲区
void initBuffer(CircularBuffer *cb) {
    cb->read_index = 0;
    cb->write_index = 0;
    cb->count = 0;
}
 
// 向缓冲区写入数据
bool writeBuffer(CircularBuffer *cb, recv_msg data) {
    if (cb->count == RECV_CIRCULARBUFFER_SIZE) { // 缓冲区已满
        return false; // 返回失败标志
    }
    cb->buffer[cb->write_index] = data;
    cb->write_index = (cb->write_index + 1) % RECV_CIRCULARBUFFER_SIZE; // 更新写指针，考虑循环
    cb->count++; // 增加缓冲区中的元素数量
    return true; // 返回成功标志
}
 
// 从缓冲区读取数据
bool readBuffer(CircularBuffer *cb, recv_msg *data) {
    if (cb->count == 0) { // 缓冲区为空
        return false; // 返回失败标志
    }
    *data = cb->buffer[cb->read_index];
    cb->read_index = (cb->read_index + 1) % RECV_CIRCULARBUFFER_SIZE; // 更新读指针，考虑循环
    cb->count--; // 减少缓冲区中的元素数量
    return true; // 返回成功标志
}