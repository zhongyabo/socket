#include "EasyTcpClient.hpp"
EasyTcpClient::EasyTcpClient()
{
    _sock = INVALID_SOCKET;
    _isConnect = false;
}

EasyTcpClient::~EasyTcpClient()
{
    Close();
}

void EasyTcpClient::InitSocket()
{
#ifdef _WIN32
    //启动Windows socket 2.x环境
    WORD ver = MAKEWORD(2, 2);
    WSADATA dat;
    WSAStartup(ver, &dat);
#endif
    if (INVALID_SOCKET != _sock)
    {
        printf("<socket=%d>关闭旧连接...\n", _sock);
        Close();
    }
    _sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (INVALID_SOCKET == _sock)
    {
        printf("错误，建立Socket失败...\n");
    }
    else
    {
        //        printf("建立Socket=<%d>成功...\n", _sock);
    }
}

int EasyTcpClient::Connect(const char *ip, unsigned short port)
{
    if (INVALID_SOCKET == _sock)
    {
        InitSocket();
    }
    // 2 连接服务器 connect
    sockaddr_in _sin = {};
    _sin.sin_family = AF_INET;
    _sin.sin_port = htons(port);
#ifdef _WIN32
    _sin.sin_addr.S_un.S_addr = inet_addr(ip);
#else
    _sin.sin_addr.s_addr = inet_addr(ip);
#endif
    //printf("<socket=%d>正在连接服务器<%s:%d>...\n", _sock, ip, port);
    int ret = connect(_sock, (sockaddr *)&_sin, sizeof(sockaddr_in));
    if (SOCKET_ERROR == ret)
    {
        printf("<socket=%d>错误，连接服务器<%s:%d>失败...\n", _sock, ip, port);
    }
    else
    {
        _isConnect = true;
        //printf("<socket=%d>连接服务器<%s:%d>成功...\n",_sock, ip, port);
    }
    return ret;
}

void EasyTcpClient::Close()
{
    if (_sock != INVALID_SOCKET)
    {
#ifdef _WIN32
        closesocket(_sock);
        //清除Windows socket环境
        WSACleanup();
#else
        close(_sock);
#endif
        _sock = INVALID_SOCKET;
    }
    _isConnect = false;
}

bool EasyTcpClient::OnRun()
{
    if (isRun())
    {
        fd_set fdReads;
        FD_ZERO(&fdReads);
        FD_SET(_sock, &fdReads);
        timeval t = {0, 0};
        int ret = select(_sock + 1, &fdReads, 0, 0, nullptr);
        //printf("select ret=%d count=%d\n", ret, _nCount++);
        if (ret < 0)
        {
            printf("<socket=%d>select任务结束1\n", _sock);
            Close();
            return false;
        }
        if (FD_ISSET(_sock, &fdReads))
        {
            FD_CLR(_sock, &fdReads);
            if (-1 == RecvData(_sock))
            {
                printf("<socket=%d>select任务结束2\n", _sock);
                Close();
                return false;
            }
        }
        return true;
    }
    return false;
}

bool EasyTcpClient::isRun()
{
    return _sock != INVALID_SOCKET && _isConnect;
}

int EasyTcpClient::RecvData(SOCKET cSock)
{
    // 5 接收数据
    int nLen = (int)recv(cSock, _szRecv, RECV_BUFF_SZIE, 0);
    //printf("nLen=%d\n", nLen);
    if (nLen <= 0)
    {
        printf("<socket=%d>与服务器断开连接，任务结束。\n", cSock);
        return -1;
    }
    //将收取到的数据拷贝到消息缓冲区
    memcpy(_szMsgBuf+_lastPos, _szRecv, nLen);
    //消息缓冲区的数据尾部位置后移
    _lastPos += nLen;
    //判断消息缓冲区的数据长度大于消息头DataHeader长度
    while (_lastPos >= sizeof(DataHeader))
    {
        //这时就可以知道当前消息的长度
        DataHeader* header = (DataHeader*)_szMsgBuf;
        //判断消息缓冲区的数据长度大于消息长度
        if (_lastPos >= header->dataLength)
        {
            //消息缓冲区剩余未处理数据的长度
            int nSize = _lastPos - header->dataLength;
            //处理网络消息
            OnNetMsg(header);
            //将消息缓冲区剩余未处理数据前移
            memcpy(_szMsgBuf, _szMsgBuf + header->dataLength, nSize);
            //消息缓冲区的数据尾部位置前移
            _lastPos = nSize;
        }
        else {
            //消息缓冲区剩余数据不够一条完整消息
            break;
        }
    }
    return 0;
}

void EasyTcpClient::OnNetMsg(DataHeader *header)
{
    switch (header->cmd)
    {
    case CMD_LOGIN_RESULT:
    {

        LoginResult *login = (LoginResult *)header;
        //printf("<socket=%d>收到服务端消息：CMD_LOGIN_RESULT,数据长度：%d\n", _sock, login->dataLength);
    }
    break;
    case CMD_LOGOUT_RESULT:
    {
        LogoutResult *logout = (LogoutResult *)header;
        //printf("<socket=%d>收到服务端消息：CMD_LOGOUT_RESULT,数据长度：%d\n", _sock, logout->dataLength);
    }
    break;
    case CMD_NEW_USER_JOIN:
    {
        NewUserJoin *userJoin = (NewUserJoin *)header;
        //printf("<socket=%d>收到服务端消息：CMD_NEW_USER_JOIN,数据长度：%d\n",_sock, userJoin->dataLength);
    }
    break;
    case CMD_HEART:
    {
    }
    case CMD_ERROR:
    {
        printf("<socket=%d>收到服务端消息：CMD_ERROR,数据长度：%d\n", _sock, header->dataLength);
    }
    break;

    default:
    {
        printf("<socket=%d>收到未定义消息,数据长度：%d\n", _sock, header->dataLength);
    }
    }
}
#include <sstream>
int EasyTcpClient::SendData(DataHeader *header, int nLen)
{
    int ret = SOCKET_ERROR;
    if (isRun() && header)
    {
        //ret =send(_sock, (const char *)header, nLen, 0);
        std::stringstream ss;
        ss << "hello "<<_sock<<"\r\n";
        ret = send(_sock, ss.str().c_str(), ss.str().length(), 0);
        if (SOCKET_ERROR == ret)
        {
            Close();
        }
    }
    return ret;
}
