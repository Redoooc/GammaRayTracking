#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include <windows.h>
#include "recv_analyze.h"

#define SMALLEST_ROTATE_ANGLE 5 // 最小旋转角度（单位0.01度）（可以为0）

extern recv_msg track_share;

int tracking();
void rotate(int angle_x, int angle_z, int speed_x, int speed_z, int model);
