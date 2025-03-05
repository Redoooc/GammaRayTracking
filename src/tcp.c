#include "tcp.h"

WSADATA wsa;
SOCKET server_fd, client_fd;
struct sockaddr_in server_addr, client_addr;
int client_addr_len = sizeof(client_addr);
char buffer[BUFFER_SIZE] = {0};

// 连接状态标志
int isconnect = 0;

/*TCP通讯线程函数
 */
int tcp_server() {    
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
        int recv_size = recv(client_fd, buffer, BUFFER_SIZE, 0);
        if (recv_size > 0) {
            printf("收到 %d 字节数据：%s\n", recv_size, buffer);
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

/*B1指令发送函数
 *direction:方向选择：0水平顺时针，1水平逆时针，2垂直向上，3垂直向下。
 *angle:角度：旋转角度为真实角度乘上100，水平：0-36000，垂直，0-5500。
 *speed:速度：为云台真实速度的10倍，水平：0-60，垂直：0-25。
 *return:0：发送完成，-1：客户端未连接，-2：发送超时。
 */
int u_send_B1(int direction, int angle, int speed){
    if(isconnect){
        UINT8 data[] = {0x55, 0x00, 0x21, 0xAA, 0xAA, 0xAA, 0xAA, 0x01, 0xB1, 0x00, 0x14, 0x00, 0xB1, 0x00, (UINT8)direction, (UINT16)angle >> 8, (UINT16)angle, 0x00, (UINT8)speed, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x23};
        if(send(client_fd, data, sizeof(data), 0) != sizeof(data)){
            for(int i = 0; i < SEND_RETRYTIMES; i++){
                if(send(client_fd, data, sizeof(data), 0) == sizeof(data)){
                    printf("B1指令发送完成\n");
                    return 0;
                }
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