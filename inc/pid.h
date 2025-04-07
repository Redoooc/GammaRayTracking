#include <stdio.h>
#include <math.h>
#include <string.h>

#ifndef PID_H
#define PID_H
typedef struct {
    // PID基础参数
    double Kp_base;
    double Ki, Kd;
    double integral;
    double prev_error;
    
    // 动态调节参数
    double min_Kp, max_Kp;
    double alpha;       // 增益调节系数
    double dead_zone;
} DualPIDController;

#endif

#define INTEGRAL_SAVE_CNT 30 // 积分保留次数

void DualPID_Init(DualPIDController *pid, double Kp, double Ki, double Kd,double min_Kp, double max_Kp,double alpha, double dead_zone);
void DualPID_Update(DualPIDController *pid, double *val1, double *val2,double dt);