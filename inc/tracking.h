#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include <windows.h>
#include "recv_analyze.h"
#include <math.h>

#define SIMULATION_MODE 1       // 模拟追踪模式

#define SMALLEST_ROTATE_ANGLE 5 // 最小旋转角度（单位0.01度）（可以为0）
#define BASIC_ROTATE_ANGLE 3500 // 基础旋转角度，相对角度旋转函数旋转的角度将此值乘以系数计算移动
#define BASIC_ROTATE_SPEED 16   // 基础旋转速度，相对角度旋转函数旋转的速度将此值乘以系数计算移动
#define OFFSET_THRESHOLD 3.1415926/9 // 偏移超出该角度则停止转动重新计算转动方向。
#define SOURCE_MISS_TTHRESHOLD 50     // 放射源追踪丢失判断阈值，若所有探测器计数和小于该值则认为源追踪丢失，停止转动并停止滤波和数据处理
#define STOP_THRESHOLD 0.05     // 偏移量均小于该值时停止转动并认为已经完成追踪
#define FILTER_THRESHOLD 0.3    // 复合滤波器滤波方式转变阈值（低于该值采用中值平均滤波，反之采用卡尔曼滤波）
#define MEDIAN_FILTER_WINDOW_LENGTH 5 // 中值平均滤波器窗口大小
#define KALMAN_FILTER_PROCESS_NOISE 0.01f // 卡尔曼滤波过程噪声
#define KALMAN_FILTER_MEASURE_NOISE 0.1f  // 卡尔曼滤波测量噪声
#define MAX_SQRT_BUFFER 20      // 允许同时存在的SQRT判断个数
#define LINEARIZATION_PARAMETERS    1/1.75 // 差比和公式线性化处理次方
#define SLOPE_LINEARIZATION_PARAMETERS 1/0.595387797271777 // 斜率线性化处理次方

extern recv_msg track_share;

int tracking();
void rotate(int angle_x, int angle_z, int speed_x, int speed_z, int model);
void rotate_angle(float rotate_x, float rotate_z);
float combine_filter(float filter_input);
float combine_filter_slope(float filter_input_x, float filter_input_y);
bool sprt_detect(bool input, int true_times, int reserve_times, int sqrtIndex);
int hold_minus_int(double hold_input);