#include "simulation.h"
#include "recv_analyze.h"
#include "tracking.h"

int simSourceX = 2000;
int simSourceY = 2000;
int simSourceAimX = 10000;
int simSourceAimY = 4000;
int simSourceSpeedX = 8;
int simSourceSpeedY = 2;
int simDetectorLocationX = 0;
int simDetectorLocationY = 0;
int simDetectorAimX = 0;
int simDetectorAimY = 0;
int simDetectorSpeedX = 0;
int simDetectorSpeedY = 0;
int simCorrectRelativeX = 0;
volatile int simIsRotateDone = 1;

CircularBuffer sim_clc_buf;
recv_msg sim_share;
// 待处理能谱消息标志
HANDLE isSpecWaitingForProcess_SIM = NULL;

int sim_thread(){
    isSpecWaitingForProcess_SIM = CreateEvent(NULL, FALSE, FALSE, NULL);
    initBuffer(&sim_clc_buf);
    sim_initRecvShare(sim_share);
    sim_share.spec1_sum = 0;
    sim_share.spec2_sum = 0;
    sim_share.spec3_sum = 0;
    sim_share.spec4_sum = 0;
    writeBuffer(&sim_clc_buf, sim_share);
    SetEvent(isSpecWaitingForProcess_SIM);
    Sleep(UPDATE_INTERVAL * 1000);
    printf("[模拟]初始化完成。\n");
    while(1){
        sim_initRecvShare(sim_share);
        sim_update_location();
        sim_update_source_location();
        sim_update_spec();
        writeBuffer(&sim_clc_buf, sim_share);
        SetEvent(isSpecWaitingForProcess_SIM);
        Sleep(UPDATE_INTERVAL * 1000);
    }
}

void sim_rotate_relative(int angle_x, int angle_z, int speed_x, int speed_z){
    simDetectorAimX = simDetectorLocationX + angle_x;
    simDetectorAimY = 1 == abs(simDetectorLocationY + angle_z)/5800?hold_minus_int(simDetectorLocationY + angle_z)*5800:simDetectorLocationY + angle_z;
    int speed_x_raw = (float)abs(angle_x)/speed_x > (float)abs(angle_z)/speed_z?speed_x:(int)round((((float)abs(angle_x)/speed_x)/((float)abs(angle_z)/speed_z))*speed_x);
    int speed_z_raw = (float)abs(angle_x)/speed_x < (float)abs(angle_z)/speed_z?speed_z:(int)round((((float)abs(angle_z)/speed_z)/((float)abs(angle_x)/speed_x))*speed_z);
    simDetectorSpeedX = hold_minus_int(angle_x) * speed_x_raw;
    simDetectorSpeedY = hold_minus_int(angle_z) * speed_z_raw;
    simIsRotateDone = 0;
    printf("[模拟]开始模拟旋转，目标水平角：%d，垂直角：%d，水平速度：%d，垂直速度：%d。\n",simDetectorAimX,simDetectorAimY,simDetectorSpeedX,simDetectorSpeedY);
}

void sim_stop(){
    simDetectorAimX = simDetectorLocationX;
    simDetectorAimY = simDetectorLocationY;
    simDetectorSpeedX = 0;
    simDetectorSpeedY = 0;
    simIsRotateDone =1;
    printf("[模拟]立即停止模拟转动。\n");
}

void sim_update_location(){
    if(simIsRotateDone)return;
    if(simDetectorSpeedX > 0 && simDetectorLocationX < simDetectorAimX){
        if(simDetectorSpeedY > 0 && simDetectorLocationY < simDetectorAimY){
            simDetectorLocationX += (int)(60 * UPDATE_INTERVAL * simDetectorSpeedX); // 一个速度挡位对应0.6度每秒
            simDetectorLocationY += (int)(60 * UPDATE_INTERVAL * simDetectorSpeedY); // 一个速度挡位对应0.6度每秒
            return;
        }else if(simDetectorSpeedY < 0 && simDetectorLocationY > simDetectorAimY){
            simDetectorLocationX += (int)(60 * UPDATE_INTERVAL * simDetectorSpeedX); // 一个速度挡位对应0.6度每秒
            simDetectorLocationY += (int)(60 * UPDATE_INTERVAL * simDetectorSpeedY); // 一个速度挡位对应0.6度每秒
            return;
        }else{
            simDetectorLocationX += (int)(60 * UPDATE_INTERVAL * simDetectorSpeedX); // 一个速度挡位对应0.6度每秒
            return;
        }
    }else if(simDetectorSpeedX < 0 && simDetectorLocationX > simDetectorAimX){
        if(simDetectorSpeedY > 0 && simDetectorLocationY < simDetectorAimY){
            simDetectorLocationX += (int)(60 * UPDATE_INTERVAL * simDetectorSpeedX); // 一个速度挡位对应0.6度每秒
            simDetectorLocationY += (int)(60 * UPDATE_INTERVAL * simDetectorSpeedY); // 一个速度挡位对应0.6度每秒
            return;
        }else if(simDetectorSpeedY < 0 && simDetectorLocationY > simDetectorAimY){
            simDetectorLocationX += (int)(60 * UPDATE_INTERVAL * simDetectorSpeedX); // 一个速度挡位对应0.6度每秒
            simDetectorLocationY += (int)(60 * UPDATE_INTERVAL * simDetectorSpeedY); // 一个速度挡位对应0.6度每秒
            return;
        }else{
            simDetectorLocationX += (int)(60 * UPDATE_INTERVAL * simDetectorSpeedX); // 一个速度挡位对应0.6度每秒
            return;
        }
    }else{
        if(simDetectorSpeedY > 0 && simDetectorLocationY < simDetectorAimY){
            simDetectorLocationY += (int)(60 * UPDATE_INTERVAL * simDetectorSpeedY); // 一个速度挡位对应0.6度每秒
            return;
        }else if(simDetectorSpeedY < 0 && simDetectorLocationY > simDetectorAimY){
            simDetectorLocationY += (int)(60 * UPDATE_INTERVAL * simDetectorSpeedY); // 一个速度挡位对应0.6度每秒
            return;
        }
    }
    if(!simIsRotateDone){
        simDetectorLocationX = (simDetectorLocationX + 36000) % 36000;
        simDetectorAimX = (simDetectorAimX + 36000) % 36000;
        simIsRotateDone = 1;
    }
}

void sim_update_source_location(){
    if(simSourceSpeedX > 0 && simSourceX < simSourceAimX){
        if(simSourceSpeedY > 0 && simSourceY < simSourceAimY){
            simSourceX += (int)(60 * UPDATE_INTERVAL * simSourceSpeedX); // 一个速度挡位对应0.6度每秒
            simSourceY += (int)(60 * UPDATE_INTERVAL * simSourceSpeedY); // 一个速度挡位对应0.6度每秒
        }else if(simSourceSpeedY < 0 && simSourceY > simSourceAimY){
            simSourceX += (int)(60 * UPDATE_INTERVAL * simSourceSpeedX); // 一个速度挡位对应0.6度每秒
            simSourceY += (int)(60 * UPDATE_INTERVAL * simSourceSpeedY); // 一个速度挡位对应0.6度每秒
        }else{
            simSourceX += (int)(60 * UPDATE_INTERVAL * simSourceSpeedX); // 一个速度挡位对应0.6度每秒
        }
        printf("[模拟]当前模拟源位置：水平：%d，垂直：%d。\n",simSourceX,simSourceY);
        return;
    }else if(simSourceSpeedX < 0 && simSourceX > simSourceAimX){
        if(simSourceSpeedY > 0 && simSourceY < simSourceAimY){
            simSourceX += (int)(60 * UPDATE_INTERVAL * simSourceSpeedX); // 一个速度挡位对应0.6度每秒
            simSourceY += (int)(60 * UPDATE_INTERVAL * simSourceSpeedY); // 一个速度挡位对应0.6度每秒
        }else if(simSourceSpeedY < 0 && simSourceY > simSourceAimY){
            simSourceX += (int)(60 * UPDATE_INTERVAL * simSourceSpeedX); // 一个速度挡位对应0.6度每秒
            simSourceY += (int)(60 * UPDATE_INTERVAL * simSourceSpeedY); // 一个速度挡位对应0.6度每秒
        }else{
            simSourceX += (int)(60 * UPDATE_INTERVAL * simSourceSpeedX); // 一个速度挡位对应0.6度每秒
        }
        printf("[模拟]当前模拟源位置：水平：%d，垂直：%d。\n",simSourceX,simSourceY);
        return;
    }else{
        if(simSourceSpeedY > 0 && simSourceY < simSourceAimY){
            simSourceY += (int)(60 * UPDATE_INTERVAL * simSourceSpeedY); // 一个速度挡位对应0.6度每秒
            printf("[模拟]当前模拟源位置：水平：%d，垂直：%d。\n",simSourceX,simSourceY);
            return;
        }else if(simSourceSpeedY < 0 && simSourceY > simSourceAimY){
            simSourceY += (int)(60 * UPDATE_INTERVAL * simSourceSpeedY); // 一个速度挡位对应0.6度每秒
            printf("[模拟]当前模拟源位置：水平：%d，垂直：%d。\n",simSourceX,simSourceY);
            return;
        }
    }
    simSourceX = (simSourceX + 36000) % 36000;
    simSourceAimX = (simSourceAimX + 36000) % 36000;
    simSourceAimX = 10000;
    simSourceAimY = -4000;
    simSourceSpeedX = 0;
    simSourceSpeedY = -5;
}

void sim_update_spec(){
    simCorrectRelativeX = abs(simSourceX - simDetectorLocationX) < 18000?simSourceX - simDetectorLocationX:-hold_minus_int(simSourceX - simDetectorLocationX)*(36000 - abs(simSourceX - simDetectorLocationX));
    
    sim_share.angle_x = (simDetectorLocationX + 36000) % 36000;
    sim_share.angle_z = simDetectorLocationY;
    
    float slope;
    if(0 == simSourceX - simDetectorLocationX){
        slope = ((float)simDetectorLocationY - simSourceY)/0.000001;
    }else{
        slope = ((float)simDetectorLocationY - simSourceY)/simCorrectRelativeX;
    }

    sim_share.spec1_sum = (int)round(gaussian_value(CENTER_SPEC_CNT, sim_calculate_fwhm(atanf(slope), 1), sim_calculate_x0(atanf(slope), 1), (double)sim_convert_angles(-simCorrectRelativeX, simDetectorLocationY - simSourceY)/100));
    sim_share.spec2_sum = (int)round(gaussian_value(CENTER_SPEC_CNT, sim_calculate_fwhm(atanf(slope), 2), sim_calculate_x0(atanf(slope), 2), (double)sim_convert_angles(-simCorrectRelativeX, simDetectorLocationY - simSourceY)/100));
    sim_share.spec3_sum = (int)round(gaussian_value(CENTER_SPEC_CNT, sim_calculate_fwhm(atanf(slope), 3), sim_calculate_x0(atanf(slope), 3), (double)sim_convert_angles(-simCorrectRelativeX, simDetectorLocationY - simSourceY)/100));
    sim_share.spec4_sum = (int)round(gaussian_value(CENTER_SPEC_CNT, sim_calculate_fwhm(atanf(slope), 4), sim_calculate_x0(atanf(slope), 4), (double)sim_convert_angles(-simCorrectRelativeX, simDetectorLocationY - simSourceY)/100));
}

void sim_initRecvShare(){
    sim_share.angle_x = 0;
    sim_share.angle_z = 0;
    sim_share.spec1_sum = -1;
    memset(sim_share.spec1_detail, 0, SPEC_DETAIL_SIZE);
    sim_share.spec2_sum = -1;
    memset(sim_share.spec2_detail, 0, SPEC_DETAIL_SIZE);
    sim_share.spec3_sum = -1;
    memset(sim_share.spec3_detail, 0, SPEC_DETAIL_SIZE);
    sim_share.spec4_sum = -1;
    memset(sim_share.spec4_detail, 0, SPEC_DETAIL_SIZE);
}

int sim_convert_angles(int theta_x100, int phi_x100) {    
    // 将输入的角度转换为度数
    double theta_deg = theta_x100 / 100.0;
    double phi_deg = phi_x100 / 100.0;
    
    // 转换为弧度
    double theta_rad = theta_deg * MY_PI / 180.0;
    double phi_rad = phi_deg * MY_PI / 180.0;
    
    // 计算cos(theta) * cos(phi)
    double product = cos(theta_rad) * cos(phi_rad);
    
    // 处理浮点运算的精度问题，确保值域有效
    product = fmax(fmin(product, 1.0), -1.0);
    
    // 计算实际角度的弧度值
    double alpha_rad = acos(product);
    
    // 转换回度数并乘以100，四舍五入为整数
    double alpha_deg = alpha_rad * 180.0 / MY_PI;
    int result = (int)round(alpha_deg * 100.0);
    
    if(abs(theta_x100) < 5){
        return -result;
    }
    return hold_minus_int(theta_x100)*result;
}

float sim_calculate_x0(float angle2x0, int detectorNo){
    switch (detectorNo)
    {
    case 1:
        return -12.5*sinf(angle2x0+(MY_PI/4));
        break;
    
    case 2:
        return 12.5*cosf(angle2x0+(MY_PI/4));
        break;
    
    case 3:
        return -12.5*cosf(angle2x0+(MY_PI/4));
        break;
    
    case 4:
        return 12.5*sinf(angle2x0+(MY_PI/4));
        break;
    
    default:
        printf("[模拟]计算x0时输入错误探测器编号！\n");
        break;
    }
    return 0;
}

float sim_calculate_fwhm(float angle2fwhm, int detectorNo){
    switch (detectorNo)
    {
    case 1:
        return sim_calculate_fwhm_angle_part(angle2fwhm);
        break;
    
    case 2:
        return (2*FWHM_BASIC_VALUE)-sim_calculate_fwhm_angle_part(angle2fwhm);
        break;
    
    case 3:
        return (2*FWHM_BASIC_VALUE)-sim_calculate_fwhm_angle_part(angle2fwhm);
        break;
    
    case 4:
        return sim_calculate_fwhm_angle_part(angle2fwhm);
        break;
    
    default:
        printf("[模拟]计算FWHM时输入错误探测器编号！\n");
        break;
    }
    return 0;
}

float sim_calculate_fwhm_angle_part(float angle2fwhmAnglePart) {
    if (angle2fwhmAnglePart > MY_PI / 2 || angle2fwhmAnglePart < -MY_PI / 2) {
        printf("[模拟]警告：输入值超出定义域（-pi/2, pi/2）\n");
    }

    float y;
    
    if (angle2fwhmAnglePart > 0 && angle2fwhmAnglePart <= MY_PI / 4) {
        // y = 7 * (x^1.75) + FWHM_BASIC_VALUE
        y = 7 * pow(angle2fwhmAnglePart, 1.75) + FWHM_BASIC_VALUE;
    } else if (angle2fwhmAnglePart > -MY_PI / 4 && angle2fwhmAnglePart <= 0) {
        // 关于(0, FWHM_BASIC_VALUE)中心对称
        y = 2 * FWHM_BASIC_VALUE - (7 * pow(-angle2fwhmAnglePart, 1.75) + FWHM_BASIC_VALUE);
    } else if (angle2fwhmAnglePart > MY_PI / 4 && angle2fwhmAnglePart <= MY_PI / 2) {
        // 关于x = pi/4轴对称
        y = 7 * pow(MY_PI / 4 * 2 - angle2fwhmAnglePart, 1.75) + FWHM_BASIC_VALUE;
    } else if (angle2fwhmAnglePart >= -MY_PI / 2 && angle2fwhmAnglePart <= -MY_PI / 4) {
        // 关于x = -pi/4轴对称，并且是关于(0, FWHM_BASIC_VALUE)中心对称的延伸
        y = 2 * FWHM_BASIC_VALUE - (7 * pow(-(-MY_PI / 4 * 2 - angle2fwhmAnglePart), 1.75) + FWHM_BASIC_VALUE);
    } else {
        // 这个else理论上不会执行，因为已经在main中判断并输出警告
        y = 0; // 或者可以返回一个特定的错误值
    }
    return y;
}

/*通过FWHM，X0，以及探测器处于目标位置时的理论计数拟合探测器的各处计数值  部分开始*/
double gaussian_value(double y0, double w, double x0, double x) {
    double A, sigma;
    calculate_A_sigma(w, x0, y0, &A, &sigma);
    double exponent = -pow(x - x0, 2) / (2 * pow(sigma, 2));
    return A * exp(exponent) - 10000 > 0?A * exp(exponent) - 10000:0;
}

double equation(double sigma, double W, double x0, double y0) {
    double A = (y0 + 10000) * exp(x0 * x0 / (2 * sigma * sigma));
    double term = (A + 10000) / (2 * A);
    if (term <= 0) return NAN;
    return W - 2 * sqrt(-2 * sigma * sigma * log(term));
}

double newton_raphson(double W, double x0, double y0, double initial_guess) {
    double sigma = initial_guess;
    double f, df, h = 1e-6;
    int iter = 0;

    do {
        f = equation(sigma, W, x0, y0);
        if (isnan(f)) break;

        double f_plus_h = equation(sigma + h, W, x0, y0);
        df = (f_plus_h - f) / h;

        if (fabs(df) < 1e-12) break;
        sigma -= f / df;
        iter++;
    } while (fabs(f) > GAUSS_TOLERANCE && iter < GAUSS_MAX_ITER);

    return (iter >= GAUSS_MAX_ITER || sigma <= 0) ? -1 : sigma;
}

void calculate_A_sigma(double W, double x0, double y0, double *A, double *sigma) {
    double initial_guess = sqrt(x0 * x0 / (2 * log(2)));
    *sigma = newton_raphson(W, x0, y0, initial_guess);
    *A = (*sigma > 0) ? (y0 + 10000) * exp(x0 * x0 / (2 * (*sigma) * (*sigma))) : -1;
}
/*通过FWHM，X0，以及探测器处于目标位置时的理论计数拟合探测器的各处计数值  部分结束*/