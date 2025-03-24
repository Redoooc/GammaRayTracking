#include <stdlib.h>
#include <stdio.h>

#ifndef KALMAN_FILTER_H
#define KALMAN_FILTER_H
typedef struct {
    float x;    // 状态估计
    float P;    // 估计误差协方差
    float Q;    // 过程噪声方差
    float R;    // 测量噪声方差
    float K;    // 卡尔曼增益（持久化存储用于调试）
    int initialized; // 初始化标志
} KalmanFilterStruct;
#endif

KalmanFilterStruct* kalman_filter_init(float Q, float R, float initial_value);
void kalman_filter_free(KalmanFilterStruct* kf);
float kalman_filter_update(KalmanFilterStruct* kf, float measurement);