#include "collect_test_data.h"
#include "file_read&write.h"
#include "tcp.h"
#include "recv_analyze.h"
#include "tracking.h"

volatile int isAllowRecord = 0;

int aim_x = 0;
int aim_y = 0;

/*操控云台收集以放射源为中心的上下55度和左右90度范围的伽马数据
 */
void collect_test_data(){
    char addr[] = "../../ExperimentalData/example.txt";
    appendOpenFile(addr);
    for(int x = -90; x <= 90; x++){
        for(int y = -55; y <= 55; y++){
            while(!isNotRotating);
            u_send_B2(5,1);
            for(int cnt = 0; cnt < 5; cnt++){
                while(!isAllowRecord);
                int msg[] = {abs(track_share.angle_x-aim_x)<10000?track_share.angle_x-aim_x:((aim_x-track_share.angle_x)/abs(track_share.angle_x-aim_x))*(36000-abs(track_share.angle_x-aim_x)),track_share.angle_z-aim_y,track_share.spec1_sum,track_share.spec2_sum,track_share.spec3_sum,track_share.spec4_sum};
                char *char_msg = array_int_to_char(msg, sizeof(msg)/sizeof(msg[0]));
                writeFile(char_msg);
                free(char_msg);
                char *char_spec1_detail = array_int_to_char(track_share.spec1_detail, sizeof(track_share.spec1_detail)/sizeof(track_share.spec1_detail[0]));
                writeFile(char_spec1_detail);
                free(char_spec1_detail);
                char *char_spec2_detail = array_int_to_char(track_share.spec2_detail, sizeof(track_share.spec2_detail)/sizeof(track_share.spec2_detail[0]));
                writeFile(char_spec2_detail);
                free(char_spec2_detail);
                char *char_spec3_detail = array_int_to_char(track_share.spec3_detail, sizeof(track_share.spec3_detail)/sizeof(track_share.spec3_detail[0]));
                writeFile(char_spec3_detail);
                free(char_spec3_detail);
                char *char_spec4_detail = array_int_to_char(track_share.spec4_detail, sizeof(track_share.spec4_detail)/sizeof(track_share.spec4_detail[0]));
                writeFile(char_spec4_detail);
                free(char_spec4_detail);
                isAllowRecord = 0;
            }
            u_send_B2(5,0);
            rotate((aim_x+(x*100)+36000)%36000, aim_y+(y*100), 60, 25, 1);
        }
    }
}

void collect_test_data_horizontal(){
    char addr[] = "../../ExperimentalData/horizontal.txt";
    appendOpenFile(addr);
    for(int x = -90; x < 90; x++){
        rotate((aim_x+(x*100)+36000)%36000, aim_y, 60, 25, 1);
        while(!isNotRotating);
        u_send_B2(5,1);
        for(int cnt = 0; cnt < 5; cnt++){
            while(!isAllowRecord);
            int msg[] = {abs(track_share.angle_x-aim_x)<10000?track_share.angle_x-aim_x:((aim_x-track_share.angle_x)/abs(track_share.angle_x-aim_x))*(36000-abs(track_share.angle_x-aim_x)),track_share.angle_z-aim_y,track_share.spec1_sum,track_share.spec2_sum,track_share.spec3_sum,track_share.spec4_sum};
            char *char_msg = array_int_to_char(msg, sizeof(msg)/sizeof(msg[0]));
            writeFile(char_msg);
            free(char_msg);
            char *char_spec1_detail = array_int_to_char(track_share.spec1_detail, sizeof(track_share.spec1_detail)/sizeof(track_share.spec1_detail[0]));
            writeFile(char_spec1_detail);
            free(char_spec1_detail);
            char *char_spec2_detail = array_int_to_char(track_share.spec2_detail, sizeof(track_share.spec2_detail)/sizeof(track_share.spec2_detail[0]));
            writeFile(char_spec2_detail);
            free(char_spec2_detail);
            char *char_spec3_detail = array_int_to_char(track_share.spec3_detail, sizeof(track_share.spec3_detail)/sizeof(track_share.spec3_detail[0]));
            writeFile(char_spec3_detail);
            free(char_spec3_detail);
            char *char_spec4_detail = array_int_to_char(track_share.spec4_detail, sizeof(track_share.spec4_detail)/sizeof(track_share.spec4_detail[0]));
            writeFile(char_spec4_detail);
            free(char_spec4_detail);
            isAllowRecord = 0;
        }
        u_send_B2(5,0);
    }
}

void collect_test_data_vertical(){
    char addr[] = "../../ExperimentalData/vertical.txt";
    appendOpenFile(addr);
    for(int y = -55; y < 55; y++){
        rotate(aim_x, aim_y+(y*100), 60, 25, 1);
        while(!isNotRotating);
        u_send_B2(5,1);
        for(int cnt = 0; cnt < 5; cnt++){
            while(!isAllowRecord);
            int msg[] = {abs(track_share.angle_x-aim_x)<10000?track_share.angle_x-aim_x:((aim_x-track_share.angle_x)/abs(track_share.angle_x-aim_x))*(36000-abs(track_share.angle_x-aim_x)),track_share.angle_z-aim_y,track_share.spec1_sum,track_share.spec2_sum,track_share.spec3_sum,track_share.spec4_sum};
            char *char_msg = array_int_to_char(msg, sizeof(msg)/sizeof(msg[0]));
            writeFile(char_msg);
            free(char_msg);
            char *char_spec1_detail = array_int_to_char(track_share.spec1_detail, sizeof(track_share.spec1_detail)/sizeof(track_share.spec1_detail[0]));
            writeFile(char_spec1_detail);
            free(char_spec1_detail);
            char *char_spec2_detail = array_int_to_char(track_share.spec2_detail, sizeof(track_share.spec2_detail)/sizeof(track_share.spec2_detail[0]));
            writeFile(char_spec2_detail);
            free(char_spec2_detail);
            char *char_spec3_detail = array_int_to_char(track_share.spec3_detail, sizeof(track_share.spec3_detail)/sizeof(track_share.spec3_detail[0]));
            writeFile(char_spec3_detail);
            free(char_spec3_detail);
            char *char_spec4_detail = array_int_to_char(track_share.spec4_detail, sizeof(track_share.spec4_detail)/sizeof(track_share.spec4_detail[0]));
            writeFile(char_spec4_detail);
            free(char_spec4_detail);
            isAllowRecord = 0;
        }
        u_send_B2(5,0);
    }
}

void collect_test_data_30degree(){
    char addr[] = "../../ExperimentalData/30degree.txt";
    appendOpenFile(addr);
    for(int x = -90; x < 90; x++){
        rotate((aim_x+(x*100)+36000)%36000, aim_y+(x*50), 60, 25, 1);
        while(!isNotRotating);
        u_send_B2(5,1);
        for(int cnt = 0; cnt < 5; cnt++){
            while(!isAllowRecord);
            int msg[] = {abs(track_share.angle_x-aim_x)<10000?track_share.angle_x-aim_x:((aim_x-track_share.angle_x)/abs(track_share.angle_x-aim_x))*(36000-abs(track_share.angle_x-aim_x)),track_share.angle_z-aim_y,track_share.spec1_sum,track_share.spec2_sum,track_share.spec3_sum,track_share.spec4_sum};
            char *char_msg = array_int_to_char(msg, sizeof(msg)/sizeof(msg[0]));
            writeFile(char_msg);
            free(char_msg);
            char *char_spec1_detail = array_int_to_char(track_share.spec1_detail, sizeof(track_share.spec1_detail)/sizeof(track_share.spec1_detail[0]));
            writeFile(char_spec1_detail);
            free(char_spec1_detail);
            char *char_spec2_detail = array_int_to_char(track_share.spec2_detail, sizeof(track_share.spec2_detail)/sizeof(track_share.spec2_detail[0]));
            writeFile(char_spec2_detail);
            free(char_spec2_detail);
            char *char_spec3_detail = array_int_to_char(track_share.spec3_detail, sizeof(track_share.spec3_detail)/sizeof(track_share.spec3_detail[0]));
            writeFile(char_spec3_detail);
            free(char_spec3_detail);
            char *char_spec4_detail = array_int_to_char(track_share.spec4_detail, sizeof(track_share.spec4_detail)/sizeof(track_share.spec4_detail[0]));
            writeFile(char_spec4_detail);
            free(char_spec4_detail);
            isAllowRecord = 0;
        }
        u_send_B2(5,0);
    }
}

void collect_test_data_45degree(){
    char addr[] = "../../ExperimentalData/45degree.txt";
    appendOpenFile(addr);
    for(int x = -58; x < 58; x++){
        rotate((aim_x+(x*100)+36000)%36000, aim_y+(x*100), 60, 25, 1);
        while(!isNotRotating);
        u_send_B2(5,1);
        for(int cnt = 0; cnt < 5; cnt++){
            while(!isAllowRecord);
            int msg[] = {abs(track_share.angle_x-aim_x)<10000?track_share.angle_x-aim_x:((aim_x-track_share.angle_x)/abs(track_share.angle_x-aim_x))*(36000-abs(track_share.angle_x-aim_x)),track_share.angle_z-aim_y,track_share.spec1_sum,track_share.spec2_sum,track_share.spec3_sum,track_share.spec4_sum};
            char *char_msg = array_int_to_char(msg, sizeof(msg)/sizeof(msg[0]));
            writeFile(char_msg);
            free(char_msg);
            char *char_spec1_detail = array_int_to_char(track_share.spec1_detail, sizeof(track_share.spec1_detail)/sizeof(track_share.spec1_detail[0]));
            writeFile(char_spec1_detail);
            free(char_spec1_detail);
            char *char_spec2_detail = array_int_to_char(track_share.spec2_detail, sizeof(track_share.spec2_detail)/sizeof(track_share.spec2_detail[0]));
            writeFile(char_spec2_detail);
            free(char_spec2_detail);
            char *char_spec3_detail = array_int_to_char(track_share.spec3_detail, sizeof(track_share.spec3_detail)/sizeof(track_share.spec3_detail[0]));
            writeFile(char_spec3_detail);
            free(char_spec3_detail);
            char *char_spec4_detail = array_int_to_char(track_share.spec4_detail, sizeof(track_share.spec4_detail)/sizeof(track_share.spec4_detail[0]));
            writeFile(char_spec4_detail);
            free(char_spec4_detail);
            isAllowRecord = 0;
        }
        u_send_B2(5,0);
    }
}

void collect_test_data_60degree(){
    char addr[] = "../../ExperimentalData/60degree.txt";
    appendOpenFile(addr);
    for(int x = -55; x < 55; x++){
        rotate((aim_x+(x*50)+36000)%36000, aim_y+(x*100), 60, 25, 1);
        while(!isNotRotating);
        u_send_B2(5,1);
        for(int cnt = 0; cnt < 5; cnt++){
            while(!isAllowRecord);
            int msg[] = {abs(track_share.angle_x-aim_x)<10000?track_share.angle_x-aim_x:((aim_x-track_share.angle_x)/abs(track_share.angle_x-aim_x))*(36000-abs(track_share.angle_x-aim_x)),track_share.angle_z-aim_y,track_share.spec1_sum,track_share.spec2_sum,track_share.spec3_sum,track_share.spec4_sum};
            char *char_msg = array_int_to_char(msg, sizeof(msg)/sizeof(msg[0]));
            writeFile(char_msg);
            free(char_msg);
            char *char_spec1_detail = array_int_to_char(track_share.spec1_detail, sizeof(track_share.spec1_detail)/sizeof(track_share.spec1_detail[0]));
            writeFile(char_spec1_detail);
            free(char_spec1_detail);
            char *char_spec2_detail = array_int_to_char(track_share.spec2_detail, sizeof(track_share.spec2_detail)/sizeof(track_share.spec2_detail[0]));
            writeFile(char_spec2_detail);
            free(char_spec2_detail);
            char *char_spec3_detail = array_int_to_char(track_share.spec3_detail, sizeof(track_share.spec3_detail)/sizeof(track_share.spec3_detail[0]));
            writeFile(char_spec3_detail);
            free(char_spec3_detail);
            char *char_spec4_detail = array_int_to_char(track_share.spec4_detail, sizeof(track_share.spec4_detail)/sizeof(track_share.spec4_detail[0]));
            writeFile(char_spec4_detail);
            free(char_spec4_detail);
            isAllowRecord = 0;
        }
        u_send_B2(5,0);
    }
}

void collect_test_data_static(){
    char addr[] = "../../ExperimentalData/static.txt";
    appendOpenFile(addr);
    u_send_B2(5,1);
    rotate(aim_x, aim_y, 60, 25, 1);
    while(!isNotRotating);
    while(1){
        while(!isAllowRecord);
        int msg[] = {abs(track_share.angle_x-aim_x)<10000?track_share.angle_x-aim_x:((aim_x-track_share.angle_x)/abs(track_share.angle_x-aim_x))*(36000-abs(track_share.angle_x-aim_x)),track_share.angle_z-aim_y,track_share.spec1_sum,track_share.spec2_sum,track_share.spec3_sum,track_share.spec4_sum};
        char *char_msg = array_int_to_char(msg, sizeof(msg)/sizeof(msg[0]));
        writeFile(char_msg);
        free(char_msg);
        char *char_spec1_detail = array_int_to_char(track_share.spec1_detail, sizeof(track_share.spec1_detail)/sizeof(track_share.spec1_detail[0]));
        writeFile(char_spec1_detail);
        free(char_spec1_detail);
        char *char_spec2_detail = array_int_to_char(track_share.spec2_detail, sizeof(track_share.spec2_detail)/sizeof(track_share.spec2_detail[0]));
        writeFile(char_spec2_detail);
        free(char_spec2_detail);
        char *char_spec3_detail = array_int_to_char(track_share.spec3_detail, sizeof(track_share.spec3_detail)/sizeof(track_share.spec3_detail[0]));
        writeFile(char_spec3_detail);
        free(char_spec3_detail);
        char *char_spec4_detail = array_int_to_char(track_share.spec4_detail, sizeof(track_share.spec4_detail)/sizeof(track_share.spec4_detail[0]));
        writeFile(char_spec4_detail);
        free(char_spec4_detail);
        isAllowRecord = 0;
    }
}

void always_record(){
    char addr[100];
    
    time_t now;
    time(&now);
    struct tm *local = localtime(&now);
    char timeString[100];
    strftime(timeString, sizeof(timeString), "%Y年%m月%d日%H时%M分%S秒", local);

    sprintf(addr,"../../ExperimentalData/Log/%s.txt",timeString);
    appendOpenFile(addr);
    while(1){
        while(!isAllowRecord);
        int msg[] = {track_share.angle_x,track_share.angle_z,track_share.spec1_sum,track_share.spec2_sum,track_share.spec3_sum,track_share.spec4_sum};
        char *char_msg = array_int_to_char(msg, sizeof(msg)/sizeof(msg[0]));
        writeFile(char_msg);
        free(char_msg);
        char *char_spec1_detail = array_int_to_char(track_share.spec1_detail, sizeof(track_share.spec1_detail)/sizeof(track_share.spec1_detail[0]));
        writeFile(char_spec1_detail);
        free(char_spec1_detail);
        char *char_spec2_detail = array_int_to_char(track_share.spec2_detail, sizeof(track_share.spec2_detail)/sizeof(track_share.spec2_detail[0]));
        writeFile(char_spec2_detail);
        free(char_spec2_detail);
        char *char_spec3_detail = array_int_to_char(track_share.spec3_detail, sizeof(track_share.spec3_detail)/sizeof(track_share.spec3_detail[0]));
        writeFile(char_spec3_detail);
        free(char_spec3_detail);
        char *char_spec4_detail = array_int_to_char(track_share.spec4_detail, sizeof(track_share.spec4_detail)/sizeof(track_share.spec4_detail[0]));
        writeFile(char_spec4_detail);
        free(char_spec4_detail);
        isAllowRecord = 0;
    }
}

/*将整型数组转化为字符串
 *注意：1.输入的array_len应当为sizeof(msg)/sizeof(msg[0])
 *     2.存在内存分配，需要及时释放
 */
char *array_int_to_char(int *array, int array_len){
    char *ret_str = (char *)malloc(array_len*16 + 1);
    int pos = 0; // 用于跟踪字符串的当前位置
 
    // 开始构建字符串
    for (int i = 0; i < array_len; i++) {
        pos += sprintf(ret_str + pos, "%d ", array[i]); // 将每个元素转换为字符串并追加到str中
    }
 
    // 添加换行与字符串结束字符
    if (pos > 0) {
        ret_str[pos - 1] = '\n'; 
        ret_str[pos - 0] = '\0'; // 将最后一个空格替换为字符串结束符
    }

    return ret_str;
}