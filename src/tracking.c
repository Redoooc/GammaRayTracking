#include "tracking.h"
#include "recv_analyze.h"
#include "tcp.h"
#include "collect_test_data.h"
#include "data_visual.h"
#include "median_filter.h"
#include "kalman_filter.h"

recv_msg track_share;
int SDL_cnt = 0;
float lastRotateX = 0;
float lastRotateZ = 0;
volatile int isTrackDone = 0;
MedianFilterStruct* medianFilter;
KalmanFilterStruct* kalmanFilter;

static bool sprt_detect_buffer[MAX_SQRT_BUFFER][5] = {false}; // 存储最近5次输入的环形缓冲区
static int count[MAX_SQRT_BUFFER] = {0};            // 当前存储的元素数量（0~5）
static int head[MAX_SQRT_BUFFER] = {0};             // 指向下一个插入位置的索引
int true_count[MAX_SQRT_BUFFER] = {0};

int tracking(){
    // 初始化中值平均滤波器
    medianFilter = median_filter_init(MEDIAN_FILTER_WINDOW_LENGTH);
    kalmanFilter = kalman_filter_init(KALMAN_FILTER_PROCESS_NOISE, KALMAN_FILTER_MEASURE_NOISE, 0);
    while(1){
        WaitForSingleObject(isSpecWaitingForProcess, INFINITE);
        if(!readBuffer(&clc_buf, &track_share)){
            continue;
        }
        printf("横坐标为%d，纵坐标为%d，spec1%d，2:%d，3:%d，4:%d\n",track_share.angle_x,track_share.angle_z,track_share.spec1_sum,track_share.spec2_sum,track_share.spec3_sum,track_share.spec4_sum);
        isAllowRecord = 1;
        // plot_show(((float)track_share.spec4_sum-track_share.spec2_sum)/(track_share.spec4_sum+track_share.spec2_sum), 0);
        // plot_show(((float)track_share.spec3_sum-track_share.spec1_sum)/(track_share.spec3_sum+track_share.spec1_sum), 1);
        // plot_show(((float)track_share.spec4_sum-track_share.spec3_sum)/(track_share.spec4_sum+track_share.spec3_sum), 2);
        // plot_show(((float)track_share.spec2_sum-track_share.spec1_sum)/(track_share.spec2_sum+track_share.spec1_sum), 3);
        // if((track_share.spec4_sum+track_share.spec2_sum+track_share.spec3_sum+track_share.spec1_sum)==0){
        //     plot_slope(0, 0, 0);
        //     continue;
        // }

        // 源追踪丢失，停止处理
        if((track_share.spec4_sum+track_share.spec2_sum+track_share.spec3_sum+track_share.spec1_sum) < SOURCE_MISS_TTHRESHOLD){
            if(!isNotRotating)u_send_BA();
            printf("放射源追踪丢失或无放射源!\n");
            continue;
        }

        // 计算偏移量
        float trk_slope_x = (((float)track_share.spec4_sum-track_share.spec3_sum+track_share.spec2_sum-track_share.spec1_sum)/(track_share.spec4_sum+track_share.spec3_sum+track_share.spec2_sum+track_share.spec1_sum));
        float trk_slope_y = (((float)track_share.spec4_sum-track_share.spec2_sum+track_share.spec3_sum-track_share.spec1_sum)/(track_share.spec4_sum+track_share.spec2_sum+track_share.spec3_sum+track_share.spec1_sum));
        
        // 对偏移量滤波
        float trk_slope_x_filtered = combine_filter(trk_slope_x);
        float trk_slope_y_filtered = combine_filter(trk_slope_y);

        // 数据可视化部分
        if(SDL_cnt == 2){
        plot_slope(trk_slope_x_filtered, trk_slope_y_filtered, 0);
        SDL_cnt = 0;
        }
        SDL_cnt++;

        if(isTrackDone){
            if(sprt_detect(fabs(trk_slope_x_filtered) > STOP_THRESHOLD || fabs(trk_slope_y_filtered) > STOP_THRESHOLD, 0)){
                printf("追踪开始\n");
                isTrackDone = 0;
            }
        }

        if(!isTrackDone){
            if(sprt_detect(fabs(trk_slope_x_filtered) < STOP_THRESHOLD && fabs(trk_slope_y_filtered) < STOP_THRESHOLD, 1)){
                if(!isNotRotating)u_send_BA();
                printf("追踪完成\n");
                isTrackDone = 1;
                continue;
            }
            if(isNotRotating){
                rotate_angle(trk_slope_x_filtered,trk_slope_y_filtered);
                lastRotateX = trk_slope_x_filtered;
                lastRotateZ = trk_slope_y_filtered;
            }else{
                if(sprt_detect(atan(trk_slope_y_filtered/trk_slope_x_filtered) > atan(lastRotateZ/lastRotateX) + OFFSET_THRESHOLD || atan(trk_slope_y_filtered/trk_slope_x_filtered) < atan(lastRotateZ/lastRotateX) - OFFSET_THRESHOLD, 3)){
                    printf("atan:%lf,atanLast:%lf\n",atan(trk_slope_y_filtered/trk_slope_x_filtered),atan(lastRotateZ/lastRotateX));
                    u_send_BA();
                }
            }
        }
    }
    median_filter_free(medianFilter);
    kalman_filter_free(kalmanFilter);
}

/*绝对坐标旋转函数
 *angle_x:水平角度：旋转位置为真实角度乘上100，水平旋转角度范围0-36000。（注意为绝对角度）
 *angle_z:垂直角度：旋转位置为真实角度乘上100，垂直旋转角度范围-5500 -- +5500。（注意为绝对角度）
 *speed_x:水平速度：为实际转速放大10倍后取整的值，水平速度范围0-60，也就是0-6r/min，0-36°/s。
 *speed_z:垂直速度：为实际转速放大10倍后取整的值，垂直速度范围0-25，也就是0-2.5r/min，0-15°/s。
 *model:模式：追踪模式（支持斜向旋转）：0，精确模式（先水平后垂直）：1。
 *注意：任意方向转动角度小于1度时自动采用精确模式。
 */
void rotate(int angle_x, int angle_z, int speed_x, int speed_z, int model){
    if(angle_x > 36000 || angle_x < 0)printf("rotate函数angle_x参数输入不合法！\n");
    if(angle_z > 5800 || angle_z < -5800)printf("rotate函数angle_z参数输入不合法！\n");
    if(speed_x > 60 || speed_x < 0)printf("rotate函数speed_x参数输入不合法！\n");
    if(speed_z > 25 || speed_z < 0)printf("rotate函数speed_z参数输入不合法！\n");
    if(abs(track_share.angle_x - angle_x) < 100 || abs(track_share.angle_z - angle_z) < 100 || abs(track_share.angle_x - angle_x) > 35900)model = 1;
    if(abs(track_share.angle_x - angle_x) <= SMALLEST_ROTATE_ANGLE && abs(track_share.angle_z - angle_z) <= SMALLEST_ROTATE_ANGLE)return;
    
    if(abs(track_share.angle_x - angle_x) <= SMALLEST_ROTATE_ANGLE){ //只垂直转
        u_send_B1(track_share.angle_z > angle_z?3:2,abs(track_share.angle_z - angle_z),speed_z);
    }else if (abs(track_share.angle_z - angle_z) <= SMALLEST_ROTATE_ANGLE) //只水平转
    {
        if(abs(track_share.angle_x - angle_x) <= 18000){
            u_send_B1(track_share.angle_x > angle_x?1:0, abs(track_share.angle_x - angle_x), speed_x);
        }else{
            u_send_B1(track_share.angle_x > angle_x?0:1, 36000 - abs(track_share.angle_x - angle_x), speed_x);
        }
    }else{ //双向转动
        if(model){ //精确模式
            u_send_B6(angle_x,abs(angle_z),speed_x,speed_z,angle_z<0?1:0);
        }else{ //追踪模式
            if(track_share.angle_z < angle_z){
                if(abs(track_share.angle_x - angle_x) <= 18000){
                    u_send_B9(track_share.angle_x > angle_x?0:1, abs(track_share.angle_x - angle_x), abs(track_share.angle_z - angle_z), speed_x, speed_z);
                }else{
                    u_send_B9(track_share.angle_x > angle_x?1:0, 36000 - abs(track_share.angle_x - angle_x), abs(track_share.angle_z - angle_z), speed_x, speed_z);
                }
            }else{
                if(abs(track_share.angle_x - angle_x) <= 18000){
                    u_send_B9(track_share.angle_x > angle_x?2:3, abs(track_share.angle_x - angle_x), abs(track_share.angle_z - angle_z), speed_x, speed_z);
                }else{
                    u_send_B9(track_share.angle_x > angle_x?3:2, 36000 - abs(track_share.angle_x - angle_x), abs(track_share.angle_z - angle_z), speed_x, speed_z);
                }
            }
        }
    }
}

/*相对角度旋转函数
 */
void rotate_angle(float rotate_x, float rotate_z){
    rotate_x = -rotate_x; // 云台朝向角度与源朝向角度相反
    
    // 确保输入角度在0-36000之内
    int rotate_x_abs;
    if(track_share.angle_x+(int)(BASIC_ROTATE_ANGLE*sqrt(rotate_x)) < 0){
        rotate_x_abs = track_share.angle_x+(int)round(BASIC_ROTATE_ANGLE*sqrt(rotate_x))+36000;
    }else{
        rotate_x_abs = track_share.angle_x+(int)round(BASIC_ROTATE_ANGLE*sqrt(rotate_x));
    }
    
    // 确保速度大于0
    int rotate_angle_speed = (int)max(round(BASIC_ROTATE_SPEED*sqrt(fabs(rotate_x))),round(BASIC_ROTATE_SPEED*sqrt(fabs(rotate_z))));
    if(rotate_angle_speed < 1)rotate_angle_speed = 1;

    rotate(rotate_x_abs,track_share.angle_z+(int)round(BASIC_ROTATE_ANGLE*sqrt(rotate_z)),rotate_angle_speed,rotate_angle_speed,0);
}

/*混合滤波器：低于阈值采用中值平均滤波降低噪声，高于阈值采用卡尔曼滤波保证响应
 */
float combine_filter(float filter_input){
    float medianFliterRes = median_filter_update(medianFilter, filter_input);
    float kalmanFilterRes = kalman_filter_update(kalmanFilter, filter_input);
    if(fabs(filter_input) < FILTER_THRESHOLD){
        return medianFliterRes;
    }else{
        return kalmanFilterRes;
    }
}

/*混合斜率滤波器：低于阈值采用中值平均滤波降低噪声，高于阈值采用卡尔曼滤波保证响应,并计算和返回斜率
 */
float combine_filter_slope(float filter_input_x, float filter_input_y){
    float medianFliterRes = median_filter_update(medianFilter, filter_input_y/filter_input_x);
    float kalmanFilterRes = kalman_filter_update(kalmanFilter, filter_input_y/filter_input_x);
    if(fabs(filter_input_x) < FILTER_THRESHOLD && fabs(filter_input_y) < FILTER_THRESHOLD){
        return medianFliterRes;
    }else{
        return kalmanFilterRes;
    }
}

/*序贯概率比检验
 */
bool sprt_detect(bool input, int sqrtIndex) { 
    // 将新输入添加到缓冲区
    sprt_detect_buffer[sqrtIndex][head[sqrtIndex]] = input;
    head[sqrtIndex] = (head[sqrtIndex] + 1) % 5;           // 环形缓冲区索引循环
    if (count[sqrtIndex] < 5) {
        count[sqrtIndex]++;                     // 仅在缓冲区未满时增加计数
    }
 
    true_count[sqrtIndex] = 0;

    // 统计最近count次输入中的true数量
    for (int i = 0; i < count[sqrtIndex]; i++) {
        int index = (head[sqrtIndex] - 1 - i + 5) % 5; // 计算第i次最近输入的索引
        if (sprt_detect_buffer[sqrtIndex][index]) {
            true_count[sqrtIndex]++;
        }
    }
 
    // 判断是否满足条件并重置状态
    bool result = false;
    if (true_count[sqrtIndex] >= 3) {
        result = true;
        // 清空历史记录
        count[sqrtIndex] = 0;
        head[sqrtIndex] = 0;
        for (int i = 0; i < 5; i++) {
            sprt_detect_buffer[sqrtIndex][i] = false;
        }
    }
 
    return result;
}