#include "collect_test_data.h"
#include "file_read&write.h"
#include "tcp.h"
#include "recv_analyze.h"

int aim_x = 9010;
int aim_y = 2010;

/*操控云台收集以放射源为中心的上下55度和左右90度范围的伽马数据
 */
void collect_test_data(){
    initBuffer(&clc_buf);
    char addr[] = "../../ExperimentalData/example.txt";
    appendOpenFile(addr);
    u_send_B2(5,1);
    for(int x = -90; x <= 90; x++){
        for(int y = -55; y <= 55; y++){
            while(!isNotRotating);
            for(int cnt = 0; cnt < 5; cnt++){
                recv_msg temp;
                if(readBuffer(&clc_buf, &temp)){
                    printf("横坐标为%d，纵坐标为%d，spec1%d，2:%d，3:%d，4:%d\n",temp.angle_x,temp.angle_z,temp.spec1,temp.spec2,temp.spec3,temp.spec4);
                }else{
                    cnt--;
                }
                int msg[] = {temp.angle_x,temp.angle_z,temp.spec1,temp.spec2,temp.spec3,temp.spec4};
                char *char_msg = array_int_to_char(msg, sizeof(msg)/sizeof(msg[0]));
                writeFile(char_msg);
                free(char_msg);
            }
            u_send_B6(aim_x+(x*100), abs(aim_y+(y*100)), 60, 25, aim_y+(y*100)/abs(aim_y+(y*100))==-1?1:0);
        }
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