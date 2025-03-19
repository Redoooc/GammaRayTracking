#include "recv_analyze.h"
#include "tcp.h"

// 允许发送旋转指令标志（云台旋转时不能发送旋转指令）
volatile int isNotRotating = 0;
CircularBuffer clc_buf;
recv_msg recv_share;
int dividedBagHeadNum[MAX_DATA_NUM_IN_A_BAG];
int D1Location;
int LocationCheckCNT = 0;

// 待处理能谱消息标志
HANDLE isSpecWaitingForProcess = NULL;

int recv_analyze(){
    // 初始化待处理能谱消息标志
    isSpecWaitingForProcess = CreateEvent(NULL, FALSE, FALSE, NULL);
    while(1){
        WaitForSingleObject(isRecvWaitingForAnalyze, INFINITE);
        divide_databbag();
        for(int i = 0; dividedBagHeadNum[i] != -1 && i < MAX_DATA_NUM_IN_A_BAG; i++){
            switch (demoRecvData[dividedBagHeadNum[i] + 8])
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
                recv_process_D1(dividedBagHeadNum[i]);
                break;
            
            default:
                printf("接收到MID为：0x%X 的数据。\n",demoRecvData[dividedBagHeadNum[i] + 8]);
                break;
            }
        }
    }
}

// 分割数据包(分割完成后在dividedBagHeadNum数组中留下每条指令的开始位置)
void divide_databbag(){
    memset(dividedBagHeadNum, 0, MAX_DATA_NUM_IN_A_BAG);
    for(int i = 0; i < MAX_DATA_NUM_IN_A_BAG; i++){
        int thisBagLength;
        if(demoRecvData[dividedBagHeadNum[i]] == 0x55){
            thisBagLength = (int)(demoRecvData[dividedBagHeadNum[i] + 1] << 8 | demoRecvData[dividedBagHeadNum[i] + 2]);
            dividedBagHeadNum[i+1] = dividedBagHeadNum[i] + thisBagLength;
        }else{
            dividedBagHeadNum[i] = -1;
            break;
        }
    }
}

void recv_process_B2(){

}

void recv_process_B6(){
    isNotRotating = 1;
    printf("接收到B6反馈\n");
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
    printf("云台初始化完成\n");
}

void recv_process_D1(int recv_D1_num){
    if(!checkSpecAllWritten() && recv_share.angle_x == decode_coordinates_x(recv_D1_num) && recv_share.angle_z == decode_coordinates_z(recv_D1_num)){ //上个数据包没有包含所有探测器数据且当前数据包为上个数据包的续传（续传判断：探测坐标位置相同）
        process_spec_data(recv_D1_num);
    }else{
        initRecvShare();
        recv_share.angle_x = decode_coordinates_x(recv_D1_num);
        recv_share.angle_z = decode_coordinates_z(recv_D1_num);
        process_spec_data(recv_D1_num);
    }
    if(checkSpecAllWritten()){
        if(recv_share.angle_z > 7000 || recv_share.angle_z < -7000 || recv_share.angle_x > 36000 || recv_share.angle_x < 0){
            printf("接收到云台坐标溢出！\n");
            initRecvShare();
            return;
        }
        writeBuffer(&clc_buf, recv_share);
        SetEvent(isSpecWaitingForProcess);
        checkIsNotRotatingThroughD1();
        initRecvShare();
    }
}

// 计算横坐标
int decode_coordinates_x(int x_coord_num){
    int raw_angle_x = 0;
    raw_angle_x = (int)((demoRecvData[(x_coord_num)+4107] << 8) | (demoRecvData[(x_coord_num)+4108]));
    return raw_angle_x;
}

// 计算纵坐标
int decode_coordinates_z(int y_coord_num){
    int raw_angle_z = 0;
    if(demoRecvData[(y_coord_num)+4110]!=0x00){
        raw_angle_z = (int)((demoRecvData[(y_coord_num)+4109] << 8) | (demoRecvData[(y_coord_num)+4110]));
    }else if (demoRecvData[(y_coord_num)+4112]!=0x00){
        raw_angle_z = -(int)((demoRecvData[(y_coord_num)+4111] << 8) | (demoRecvData[(y_coord_num)+4112]));
    }else{
        raw_angle_z = 0;
    }
    return raw_angle_z;
}

// 处理能谱数据
void process_spec_data(int spec_data_num){
    switch (demoRecvData[spec_data_num + 4106])
    {
        case 0XF1:
        recv_share.spec1_sum = (int)(demoRecvData[spec_data_num + 15] << 24 | demoRecvData[spec_data_num + 16] << 16 | demoRecvData[spec_data_num + 17] << 8 | demoRecvData[spec_data_num + 18]);
        for(int spec_cnt = 0; spec_cnt < SPEC_DETAIL_SIZE; spec_cnt++){
            recv_share.spec1_detail[spec_cnt] = (int)(demoRecvData[spec_data_num + 23 + spec_cnt*4] << 24 | demoRecvData[spec_data_num + 24 + spec_cnt*4] << 16 | demoRecvData[spec_data_num + 25 + spec_cnt*4] << 8 | demoRecvData[spec_data_num + 26 + spec_cnt*4]);
        }
        break;

        case 0XF2:
        recv_share.spec2_sum = (int)(demoRecvData[spec_data_num + 15] << 24 | demoRecvData[spec_data_num + 16] << 16 | demoRecvData[spec_data_num + 17] << 8 | demoRecvData[spec_data_num + 18]);
        for(int spec_cnt = 0; spec_cnt < SPEC_DETAIL_SIZE; spec_cnt++){
            recv_share.spec2_detail[spec_cnt] = (int)(demoRecvData[spec_data_num + 23 + spec_cnt*4] << 24 | demoRecvData[spec_data_num + 24 + spec_cnt*4] << 16 | demoRecvData[spec_data_num + 25 + spec_cnt*4] << 8 | demoRecvData[spec_data_num + 26 + spec_cnt*4]);
        }
        break;

        case 0XF3:
        recv_share.spec3_sum = (int)(demoRecvData[spec_data_num + 15] << 24 | demoRecvData[spec_data_num + 16] << 16 | demoRecvData[spec_data_num + 17] << 8 | demoRecvData[spec_data_num + 18]);
        for(int spec_cnt = 0; spec_cnt < SPEC_DETAIL_SIZE; spec_cnt++){
            recv_share.spec3_detail[spec_cnt] = (int)(demoRecvData[spec_data_num + 23 + spec_cnt*4] << 24 | demoRecvData[spec_data_num + 24 + spec_cnt*4] << 16 | demoRecvData[spec_data_num + 25 + spec_cnt*4] << 8 | demoRecvData[spec_data_num + 26 + spec_cnt*4]);
        }
        break;

        case 0XF4:
        recv_share.spec4_sum = (int)(demoRecvData[spec_data_num + 15] << 24 | demoRecvData[spec_data_num + 16] << 16 | demoRecvData[spec_data_num + 17] << 8 | demoRecvData[spec_data_num + 18]);
        for(int spec_cnt = 0; spec_cnt < SPEC_DETAIL_SIZE; spec_cnt++){
            recv_share.spec4_detail[spec_cnt] = (int)(demoRecvData[spec_data_num + 23 + spec_cnt*4] << 24 | demoRecvData[spec_data_num + 24 + spec_cnt*4] << 16 | demoRecvData[spec_data_num + 25 + spec_cnt*4] << 8 | demoRecvData[spec_data_num + 26 + spec_cnt*4]);
        }
        break;

        default:
        printf("接收到代码为0X%X的探测器\n",demoRecvData[spec_data_num + 4106]);
        break;
    }
}

void initRecvShare(){
    recv_share.angle_x = 0;
    recv_share.angle_z = 0;
    recv_share.spec1_sum = -1;
    memset(recv_share.spec1_detail, 0, SPEC_DETAIL_SIZE);
    recv_share.spec2_sum = -1;
    memset(recv_share.spec2_detail, 0, SPEC_DETAIL_SIZE);
    recv_share.spec3_sum = -1;
    memset(recv_share.spec3_detail, 0, SPEC_DETAIL_SIZE);
    recv_share.spec4_sum = -1;
    memset(recv_share.spec4_detail, 0, SPEC_DETAIL_SIZE);
}

// 检查是否有任意Spec_sum数据未被写入（即是否有任意Spec_sum == -1） return为true时所有数据均被写入
bool checkSpecAllWritten(){
    if(recv_share.spec1_sum != -1 && recv_share.spec2_sum != -1 && recv_share.spec3_sum != -1 && recv_share.spec4_sum != -1){
        return true;
    }else{
        return false;
    }
}

// 云台出现停止转动而无反馈情况处理
void checkIsNotRotatingThroughD1(){
    if(isNotRotating)return;
    if(D1Location == (recv_share.angle_x+1)*(recv_share.angle_z+10000)){
        LocationCheckCNT++;
        if(LocationCheckCNT == CHECK_TIMES){
            LocationCheckCNT = 0;
            u_send_BA();
            isNotRotating = 1;
        }
    }else{
        D1Location = (recv_share.angle_x+1)*(recv_share.angle_z+10000);
        LocationCheckCNT = 0;
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