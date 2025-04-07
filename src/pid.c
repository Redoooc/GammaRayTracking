#include "pid.h"

double integral_error_save[INTEGRAL_SAVE_CNT] = {};

void DualPID_Init(DualPIDController *pid, 
                  double Kp, double Ki, double Kd,
                  double min_Kp, double max_Kp,
                  double alpha, double dead_zone) 
{
    pid->Kp_base = Kp;
    pid->Ki = Ki;
    pid->Kd = Kd;
    pid->integral = 0.0;
    pid->prev_error = 0.0;

    pid->min_Kp = min_Kp;
    pid->max_Kp = max_Kp;
    pid->alpha = alpha;
    pid->dead_zone = dead_zone;
}

void DualPID_Update(DualPIDController *pid, 
                    double *val1, double *val2,
                    double dt) 
{
    // 计算联合误差（向量模长）
    double error = sqrt(pow(*val1, 2) + sqrt(pow(*val2, 2)));

    // 死区处理
    if(fabs(*val1) < pid->dead_zone && fabs(*val2) < pid->dead_zone){
        memset(integral_error_save, 0, INTEGRAL_SAVE_CNT);
        pid->integral = 0;
        return;
    }

    // 动态增益计算
    double Kp_adaptive = pid->Kp_base * (1 + pid->alpha * error);
    Kp_adaptive = fmax(fmin(Kp_adaptive, pid->max_Kp), pid->min_Kp);

    // PID计算
    double derivative = (error - pid->prev_error) / dt;
    pid->integral += error * dt;

    // 只保留最近几次误差进行积分
    pid->integral -= integral_error_save[0];
    for(int i = 0; i < INTEGRAL_SAVE_CNT-1; i++){
        integral_error_save[i] = integral_error_save[i+1];
    }
    integral_error_save[INTEGRAL_SAVE_CNT-1] = error * dt;

    // 微分滤波
    static double prev_deriv = 0;
    derivative = 0.6*derivative + 0.4*prev_deriv;
    prev_deriv = derivative;

    // 总控制量计算
    double control = Kp_adaptive * error 
        + pid->Ki * pid->integral
        + pid->Kd * derivative;

    // 按当前值比例分配控制量（关键步骤）
    double total = fabs(*val1) + fabs(*val2);
    if(total > 1e-6) {  // 防除零保护
    *val1 += control * (*val1 / total) * dt;
    *val2 += control * (*val2 / total) * dt;
    }

    pid->prev_error = error;
}
