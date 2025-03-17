#include "tracking.h"
#include "recv_analyze.h"
#include "tcp.h"
#include "collect_test_data.h"

recv_msg track_share;

int tracking(){
    while(1){
        WaitForSingleObject(isSpecWaitingForProcess, INFINITE);
        if(readBuffer(&clc_buf, &track_share)){
            printf("横坐标为%d，纵坐标为%d，spec1%d，2:%d，3:%d，4:%d\n",track_share.angle_x,track_share.angle_z,track_share.spec1_sum,track_share.spec2_sum,track_share.spec3_sum,track_share.spec4_sum);
        }else{
            continue;
        }
        isAllowRecord = 1;
    }
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