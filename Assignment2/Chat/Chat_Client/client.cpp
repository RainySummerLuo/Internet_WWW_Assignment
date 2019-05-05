#pragma clang diagnostic push
#pragma ide diagnostic ignored "hicpp-signed-bitwise"
#pragma comment(lib, "ws2_32.lib")

#include <winsock2.h>
#include <process.h>
#include <cstdio>
#include <cstdlib>
#include <conio.h>

#define RECV_OVER 1
#define RECV_YET 0

#pragma warning (disable: 4996)

char userName[16] = {0};
int iStatus = RECV_YET;


unsigned __stdcall ThreadRecv(void *param) {
    char buf[128] = {0};
    while (true) {
        int ret = recv(*(SOCKET *) param, buf, sizeof(buf), 0);
        if (ret == SOCKET_ERROR) {
            Sleep(500);
            continue;
        }
        if (strlen(buf) != 0) {
            printf("%s\n", buf);
            iStatus = RECV_OVER;
        } else {
            Sleep(100);
        }
    }
    // return 0;
}

//发送数据
unsigned __stdcall ThreadSend(void *param) {
    char buf[128] = {0};
    int ret = 0;
    while (true) {
        int c = getch();
        if (c == 72 || c == 0 || c == 68) {//为了显示美观，加一个无回显的读取字符函数
            continue;
        }                   //getch返回值我是经过实验得出如果是返回这几个值，则getch就会自动跳过，具体我也不懂。
        printf("%s: ", userName);
        gets(buf);
        ret = send(*(SOCKET *) param, buf, sizeof(buf), 0);
        if (ret == SOCKET_ERROR) {
            return 1;
        }
    }
    return 0;
}

//连接服务器
int ConnectServer() {
    WSADATA wsaData = {0};//存放套接字信息
    auto ClientSocket = INVALID_SOCKET;//客户端套接字
    SOCKADDR_IN ServerAddr = {0};//服务端地址
    USHORT uPort = 18000;//服务端端口
    //初始化套接字
    if (WSAStartup(MAKEWORD(2, 2), &wsaData)) {
        printf("WSAStartup failed with error code: %d\n", WSAGetLastError());
        return -1;
    }
    //判断套接字版本
    if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2) {
        printf("wVersion was not 2.2\n");
        return -1;
    }
    //创建套接字
    ClientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (ClientSocket == INVALID_SOCKET) {
        printf("socket failed with error code: %d\n", WSAGetLastError());
        return -1;
    }
    //输入服务器IP
    printf("Please input server IP:");
    char IP[32] = {0};
    //gets(IP);
    strcpy(IP, "127.0.0.1");
    //设置服务器地址
    ServerAddr.sin_family = AF_INET;
    ServerAddr.sin_port = htons(uPort);//服务器端口
    ServerAddr.sin_addr.S_un.S_addr = inet_addr(IP);//服务器地址

    printf("connecting......\n");
    //连接服务器
    if (SOCKET_ERROR == connect(ClientSocket, (SOCKADDR *) &ServerAddr, sizeof(ServerAddr))) {
        printf("connect failed with error code: %d\n", WSAGetLastError());
        closesocket(ClientSocket);
        WSACleanup();
        return -1;
    }
    printf("Connecting server successfully IP:%s Port:%d\n",
           IP, htons(ServerAddr.sin_port));
    printf("Please input your UserName: ");
    gets(userName);
    send(ClientSocket, userName, sizeof(userName), 0);
    printf("\n\n");
    _beginthreadex(nullptr, 0, ThreadRecv, &ClientSocket, 0, nullptr); //启动接收和发送消息线程
    _beginthreadex(nullptr, 0, ThreadSend, &ClientSocket, 0, nullptr);
    for (int k = 0; k < 1000; k++) {
        Sleep(10000000);
    }
    closesocket(ClientSocket);
    WSACleanup();
    return 0;
}

int main() {
    ConnectServer(); //连接服务器
    return 0;
}

#pragma clang diagnostic pop