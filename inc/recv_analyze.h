#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define RECV_CIRCULARBUFFER_SIZE 20  // 定义缓冲区大小

typedef struct {
    int angle_x;
    int angle_z;
    int spec1;
    int spec2;
    int spec3;
    int spec4;
} recv_msg;

typedef struct {
    recv_msg buffer[RECV_CIRCULARBUFFER_SIZE];
    int read_index;  // 读指针
    int write_index; // 写指针
    int count;       // 缓冲区中当前元素的数量
} CircularBuffer;

extern volatile int isNotRotating;
extern CircularBuffer clc_buf;

int recv_analyze();
void recv_process_B2();
void recv_process_B6();
void recv_process_BA();
void recv_process_C2();
void recv_process_C3();
void recv_process_C6();
void recv_process_D1();
int decode_coordinates_x(int num);
int decode_coordinates_z(int num);
void divide_spec_data();
void initRecvShare();
bool checkSpecAllWritten();
void initBuffer(CircularBuffer *cb);
bool writeBuffer(CircularBuffer *cb, recv_msg data);
bool readBuffer(CircularBuffer *cb, recv_msg *data);