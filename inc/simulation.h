#include <stdio.h>
#include <math.h>
#include <winsock2.h>
#include <windows.h>
#include "recv_analyze.h"
#include <stdbool.h>

#define MY_PI 3.141593
#define FWHM_BASIC_VALUE 58.5 // 拟合探测器技术规律高斯分布曲线正值FWHM的基础值
#define UPDATE_INTERVAL 0.1   // 更新间隔（单位为秒）
#define CENTER_SPEC_CNT 800   // 模拟探测器恰好指向放射源时的计数
#define GAUSS_MAX_ITER  1000  // 单次拟合高斯函数允许最大拟合次数
#define GAUSS_TOLERANCE 1e-6  // 拟合高斯函数的sigma参数的判断误差（小于该误差则认为完成拟合）

extern HANDLE isSpecWaitingForProcess_SIM;
extern CircularBuffer sim_clc_buf;
extern volatile int simIsRotateDone;

int sim_thread();
void sim_rotate_relative(int angle_x, int angle_z, int speed_x, int speed_z);
void sim_stop();
void sim_update_location();
void sim_update_source_location();
void sim_update_spec();
void sim_initRecvShare();
int sim_convert_angles(int theta_x100, int phi_x100);
float sim_calculate_x0(float angle2x0, int detectorNo);
float sim_calculate_fwhm(float angle2fwhm, int detectorNo);
float sim_calculate_fwhm_angle_part(float angle2fwhmAnglePart);
double gaussian_value(double y0, double w, double x0, double x);
double equation(double sigma, double W, double x0, double y0);
double newton_raphson(double W, double x0, double y0, double initial_guess);
void calculate_A_sigma(double W, double x0, double y0, double *A, double *sigma);