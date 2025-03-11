#include "tcp.h"
#include "recv_analyze.h"

WSADATA wsa;
SOCKET server_fd, client_fd;
struct sockaddr_in server_addr, client_addr;
int client_addr_len = sizeof(client_addr);
unsigned char tcpRecvBuffer[TCP_RECV_BUFFER_SIZE] = {0};

// 连接状态标志
volatile int isconnect = 0;
// 待处理接收消息标志
HANDLE isRecvWaitingForAnalyze = NULL;

/*TCP通讯线程函数
 */
int tcp_server() {    
    // 初始化待处理接收消息标志
    isRecvWaitingForAnalyze = CreateEvent(NULL, FALSE, FALSE, NULL);
    
    // 初始化Winsock
    if (WSAStartup(MAKEWORD(2,2), &wsa) != 0) {
        printf("WSAStartup失败，错误码：%d\n", WSAGetLastError());
        return 1;
    }

    // 创建Socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        printf("Socket创建失败，错误码：%d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    // 设置服务器地址配置
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(SERVER_ADRESS);
    server_addr.sin_port = htons(PORT);

    // 设置地址重用选项
    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, (const char*)&opt, sizeof(opt));
    
    // 绑定Socket
    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        printf("绑定失败，错误码：%d\n", WSAGetLastError());
        closesocket(server_fd);
        WSACleanup();
        return 1;
    }

    // 开始监听
    if (listen(server_fd, 5) == SOCKET_ERROR) {
        printf("监听失败，错误码：%d\n", WSAGetLastError());
        closesocket(server_fd);
        WSACleanup();
        return 1;
    }

    printf("服务器已启动，正在监听 %s:%d...\n", 
           inet_ntoa(server_addr.sin_addr), 
           ntohs(server_addr.sin_port));

    // 接受客户端连接
    while(1) {
        if(!isconnect){
            client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_addr_len);
            if (client_fd == INVALID_SOCKET) {
                printf("接受连接失败，错误码：%d\n", WSAGetLastError());
                continue;
            }
            printf("客户端已连接：%s:%d\n", 
                inet_ntoa(client_addr.sin_addr), 
                ntohs(client_addr.sin_port));
 
            // 标记完成连接
            isconnect = 1;
        }

        // 接收数据
        int recv_size = recv(client_fd, tcpRecvBuffer, TCP_RECV_BUFFER_SIZE, 0);
        if (recv_size > 0) {
            printf("收到 %d 字节数据：%s\n", recv_size, tcpRecvBuffer);
            SetEvent(isRecvWaitingForAnalyze);
            continue;
        } else if (recv_size == 0) {
            isconnect = 0;
            printf("连接关闭\n");
        } else {
            isconnect = 0;
            printf("接收失败，错误码：%d\n", WSAGetLastError());
        }

        closesocket(client_fd);
        printf("客户端连接已关闭\n");
    }

    // 清理资源（通常不会执行到这里）
    closesocket(server_fd);
    WSACleanup();
    return 0;
}

/*B1指令发送函数（单向旋转）
 *direction:方向选择：0水平顺时针，1水平逆时针，2垂直向上，3垂直向下。
 *angle:角度：旋转角度为真实角度乘上100，水平：0-36000，垂直，0-5500。
 *speed:速度：为云台真实速度的10倍，水平：0-60，垂直：0-25。
 *return:0：发送完成，-1：客户端未连接，-2：发送超时，-3：云台正在转动，转动指令发送冲突。
 */
int u_send_B1(int direction, int angle, int speed){
    if(!isNotRotating){
        printf("云台正在转动，转动指令B1发送冲突！\n");
        return -3;
    }
    if(isconnect){
        UINT8 data[] = {0x55, 0x00, 0x21, 0xAA, 0xAA, 0xAA, 0xAA, 0x01, 0xB1, 0x00, 0x14, 0x00, 0xB1, 0x00, (UINT8)direction, (UINT16)angle >> 8, (UINT16)angle, 0x00, (UINT8)speed, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x23};
        if(send(client_fd, data, sizeof(data), 0) != sizeof(data)){
            for(int i = 0; i < SEND_RETRYTIMES; i++){
                if(send(client_fd, data, sizeof(data), 0) == sizeof(data)){
                    printf("B1指令发送完成\n");
                    return 0;
                }
                Sleep(SEND_RETRY_INTERVAL_TIME);
            }
            printf("B1指令发送超时\n");
            return -2;
        }else{
            printf("B1指令发送完成\n");
            return 0;
        }
    }else{
        printf("客户端未连接，B1指令发送失败！\n");
        return -1;
    }
}

/*B2指令发送函数（能谱开关）
 *channel:1-5，0001只配置1通道，0002只配置2通道，0003只配置3通道，0004只配置4通道，0005同时配置4个通道。
 *status:开关选择：0000关闭，0001开启。
 *return:0：发送完成，-1：客户端未连接，-2：发送超时。
 */
int u_send_B2(int channel, int status){
    if(isconnect){
        UINT8 data[] = {0x55, 0x00, 0x21, 0xAA, 0xAA, 0xAA, 0xAA, 0x01, 0xB2, 0x00, 0x14, 0x00, 0xB2, 0x00, (UINT8)channel, 0x00, (UINT8)status, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x23};
        if(send(client_fd, data, sizeof(data), 0) != sizeof(data)){
            for(int i = 0; i < SEND_RETRYTIMES; i++){
                if(send(client_fd, data, sizeof(data), 0) == sizeof(data)){
                    printf("B2指令发送完成\n");
                    return 0;
                }
                Sleep(SEND_RETRY_INTERVAL_TIME);
            }
            printf("B2指令发送超时\n");
            return -2;
        }else{
            printf("B2指令发送完成\n");
            return 0;
        }
    }else{
        printf("客户端未连接，B2指令发送失败！\n");
        return -1;
    }
}

/*B6指令发送函数（复位/单向二次旋转）
 *angle_x:水平角度：旋转位置为真实角度乘上100，水平旋转角度范围0-36000。（注意为绝对角度）
 *angle_z:垂直角度：旋转位置为真实角度乘上100，垂直旋转角度范围0-5500。（注意为绝对角度）
 *speed_x:水平速度：为实际转速放大10倍后取整的值，水平速度范围0-60，也就是0-6r/min，0-36°/s。
 *speed_z:垂直速度：为实际转速放大10倍后取整的值，垂直速度范围0-25，也就是0-2.5r/min，0-15°/s。
 *direction_z:垂直方向：1表示负数；0表示正数；
 *return:0：发送完成，-1：客户端未连接，-2：发送超时，-3：云台正在转动，转动指令发送冲突。
 */
int u_send_B6(int angle_x, int angle_z, int speed_x, int speed_z, int direction_z){
    if(!isNotRotating){
        printf("云台正在转动，转动指令B6发送冲突！\n");
        return -3;
    }
    if(isconnect){
        UINT8 data[] = {0x55, 0x00, 0x21, 0xAA, 0xAA, 0xAA, 0xAA, 0x01, 0xB6, 0x00, 0x14, 0x00, 0xB6, (UINT16)angle_x >> 8, (UINT16)angle_x, (UINT16)angle_z >> 8, (UINT16)angle_z, 0x00, (UINT8)speed_x, 0x00, (UINT8)speed_z, 0x00, (UINT8)direction_z, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x23};
        if(send(client_fd, data, sizeof(data), 0) != sizeof(data)){
            for(int i = 0; i < SEND_RETRYTIMES; i++){
                if(send(client_fd, data, sizeof(data), 0) == sizeof(data)){
                    printf("B6指令发送完成\n");
                    return 0;
                }
                Sleep(SEND_RETRY_INTERVAL_TIME);
            }
            printf("B6指令发送超时\n");
            return -2;
        }else{
            printf("B6指令发送完成\n");
            return 0;
        }
    }else{
        printf("客户端未连接，B6指令发送失败！\n");
        return -1;
    }
}

/*B9指令发送函数（斜向旋转）
 *direction:方向选择：0000斜向逆时针向上，0001斜向顺时针向上，0002斜向逆时针向下，0003斜向顺时针向下。
 *angle_x:水平角度：旋转位置为真实角度乘上100，水平旋转角度范围0-36000。（注意为绝对角度）
 *angle_z:垂直角度：旋转位置为真实角度乘上100，垂直旋转角度范围0-5500。（注意为绝对角度）
 *speed_x:水平速度：为实际转速放大10倍后取整的值，水平速度范围0-60，也就是0-6r/min，0-36°/s。
 *speed_z:垂直速度：为实际转速放大10倍后取整的值，垂直速度范围0-25，也就是0-2.5r/min，0-15°/s。
 *return:0：发送完成，-1：客户端未连接，-2：发送超时，-3：云台正在转动，转动指令发送冲突。
 */
int u_send_B9(int direction, int angle_x, int angle_z, int speed_x, int speed_z){
    if(!isNotRotating){
        printf("云台正在转动，转动指令B9发送冲突！\n");
        return -3;
    }
    if(isconnect){
        UINT8 data[] = {0x55, 0x00, 0x21, 0xAA, 0xAA, 0xAA, 0xAA, 0x01, 0xB9, 0x00, 0x14, 0x00, 0xB9, 0x00, (UINT8)direction, (UINT16)angle_x >> 8, (UINT16)angle_x, (UINT16)angle_z >> 8, (UINT16)angle_z, 0x00, (UINT8)speed_x, 0x00, (UINT8)speed_z, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x23};
        if(send(client_fd, data, sizeof(data), 0) != sizeof(data)){
            for(int i = 0; i < SEND_RETRYTIMES; i++){
                if(send(client_fd, data, sizeof(data), 0) == sizeof(data)){
                    printf("B9指令发送完成\n");
                    return 0;
                }
                Sleep(SEND_RETRY_INTERVAL_TIME);
            }
            printf("B9指令发送超时\n");
            return -2;
        }else{
            printf("B9指令发送完成\n");
            return 0;
        }
    }else{
        printf("客户端未连接，B9指令发送失败！\n");
        return -1;
    }
}

/*BA指令发送函数（中途停止转动）
 *return:0：发送完成，-1：客户端未连接，-2：发送超时。
 */
int u_send_BA(){
    if(isconnect){
        UINT8 data[] = {0x55, 0x00, 0x21, 0xAA, 0xAA, 0xAA, 0xAA, 0x01, 0xBA, 0x00, 0x14, 0x00, 0xBA, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x23};
        if(send(client_fd, data, sizeof(data), 0) != sizeof(data)){
            for(int i = 0; i < SEND_RETRYTIMES; i++){
                if(send(client_fd, data, sizeof(data), 0) == sizeof(data)){
                    printf("BA指令发送完成\n");
                    return 0;
                }
                Sleep(SEND_RETRY_INTERVAL_TIME);
            }
            printf("BA指令发送超时\n");
            return -2;
        }else{
            printf("BA指令发送完成\n");
            return 0;
        }
    }else{
        printf("客户端未连接，BA指令发送失败！\n");
        return -1;
    }
}