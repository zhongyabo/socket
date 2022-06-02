#ifndef _EasyTcpClient_hpp_
#define _EasyTcpClient_hpp_

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <WinSock2.h>
#pragma comment(lib, "ws2_32.lib")
#else
#include <unistd.h> //uni std
#include <arpa/inet.h>
#include <string.h>

#define SOCKET int
#define INVALID_SOCKET (SOCKET)(~0)
#define SOCKET_ERROR (-1)
#endif

//缓冲区最小单元大小
#ifndef RECV_BUFF_SZIE
#define RECV_BUFF_SZIE 10000
#endif // !RECV_BUFF_SZIE

#ifndef SEND_BUFF_SZIE
#define SEND_BUFF_SZIE 10000
#endif // !RECV_BUFF_SZIE

#include <stdio.h>
#include "MessageHeader.hpp"

class EasyTcpClient
{
private:
    SOCKET _sock;
    bool _isConnect;

public:
    int _nCount = 0;
    //第二缓冲区 消息缓冲区
    char _szMsgBuf[RECV_BUFF_SZIE*10];
    //消息缓冲区的数据尾部位置
    int _lastPos = 0;
//    //接收缓冲区
    char _szRecv[RECV_BUFF_SZIE];
    //发送缓冲区
    char _szSendBuf[SEND_BUFF_SZIE];
    int _lastSendPos;
    //心跳死亡计时
    time_t _dtHeart;
    //上次发送时间
    time_t _dtSend;

public:
    EasyTcpClient();

    virtual ~EasyTcpClient();

    //初始化socket
    void InitSocket();

    //连接服务器
    int Connect(const char *ip, unsigned short port);

    //关闭套节字closesocket
    void Close();

    //处理网络消息
    bool OnRun();

    //是否工作中
    bool isRun();

    //接收数据 处理粘包 拆分包
    int RecvData(SOCKET cSock);

    //响应网络消息
    virtual void OnNetMsg(DataHeader *header);

    //发送数据
    int SendData(DataHeader *header,int nLen);
    
    void resetDTHeart();
    
    bool isAlive(time_t dt);
    
    
};

#endif
