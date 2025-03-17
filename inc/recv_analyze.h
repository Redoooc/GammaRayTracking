#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <windows.h>

#define RECV_CIRCULARBUFFER_SIZE 20  // 定义缓冲区大小
#define MAX_DATA_NUM_IN_A_BAG 20 // 定义一个包中能分割出的最多数据条数（多于此数的数据将被忽略）
#define CHECK_TIMES 3 // 定义通过D1指令返回的坐标确定云台是否在转动的检测次数
#define SPEC_DETAIL_SIZE 1020 // 定义能谱数组大小

#ifndef RECV_ANALYZE_H
#define RECV_ANALYZE_H
typedef struct {
    int angle_x;
    int angle_z;
    int spec1_sum;
    int spec1_detail[SPEC_DETAIL_SIZE];
    int spec2_sum;
    int spec2_detail[SPEC_DETAIL_SIZE];
    int spec3_sum;
    int spec3_detail[SPEC_DETAIL_SIZE];
    int spec4_sum;
    int spec4_detail[SPEC_DETAIL_SIZE];
} recv_msg;

typedef struct {
    recv_msg buffer[RECV_CIRCULARBUFFER_SIZE];
    int read_index;  // 读指针
    int write_index; // 写指针
    int count;       // 缓冲区中当前元素的数量
} CircularBuffer;
#endif

extern volatile int isNotRotating;
extern CircularBuffer clc_buf;
extern HANDLE isSpecWaitingForProcess;

int recv_analyze();
void divide_databbag();
void recv_process_B2();
void recv_process_B6();
void recv_process_BA();
void recv_process_C2();
void recv_process_C3();
void recv_process_C6();
void recv_process_D1(int num);
int decode_coordinates_x(int num);
int decode_coordinates_z(int num);
void process_spec_data();
void initRecvShare();
bool checkSpecAllWritten();
void checkIsNotRotatingThroughD1();
void initBuffer(CircularBuffer *cb);
bool writeBuffer(CircularBuffer *cb, recv_msg data);
bool readBuffer(CircularBuffer *cb, recv_msg *data);