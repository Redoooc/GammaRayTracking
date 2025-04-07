#include "kalman_filter.h"

// 初始化卡尔曼滤波器
KalmanFilterStruct* kalman_filter_init(float Q, float R, float initial_value) {
    KalmanFilterStruct* kf = malloc(sizeof(KalmanFilterStruct));
    kf->Q = Q;
    kf->R = R;
    kf->x = initial_value;
    kf->P = 1.0f;  // 初始估计误差
    kf->initialized = 1;
    return kf;
}

// 释放滤波器资源
void kalman_filter_free(KalmanFilterStruct* kf) {
    free(kf);
}

// 卡尔曼滤波更新（传入新测量值，返回滤波结果）
float kalman_filter_update(KalmanFilterStruct* kf, float measurement) {
    // 首次测量初始化
    if (!kf->initialized) {
        kf->x = measurement;
        kf->P = 1.0f;
        kf->initialized = 1;
        return measurement;
    }

    /* 预测阶段 */
    const float F = 10.0f; // 状态转移矩阵（一维简化）
    const float H = 1.0f; // 观测矩阵（一维简化）
    
    // 状态预测：x = F * x
    // 协方差预测：P = F * P * F + Q
    kf->P = F * kf->P * F + kf->Q;

    /* 更新阶段 */
    // 计算残差
    const float y = measurement - H * kf->x;
    
    // 创新协方差
    const float S = H * kf->P * H + kf->R;
    
    // 卡尔曼增益计算
    kf->K = (kf->P * H) / S;
    
    // 状态更新
    kf->x = kf->x + kf->K * y;
    
    // 协方差更新
    kf->P = (1 - kf->K * H) * kf->P;

    return kf->x;
}