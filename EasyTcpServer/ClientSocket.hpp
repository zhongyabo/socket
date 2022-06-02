//
//  ClientSocket.hpp
//  EasyTcpServer
//
//  Created by stone
//

#ifndef ClientSocket_hpp
#define ClientSocket_hpp
#include "Configuration.hpp"
#include "MessageHeader.hpp"
#include "MsgBuffer.hpp"
#include "CellLog.hpp"
//#include "objectPoolBase.hpp"

//客户端心跳检测死亡时间
#define CLIENT_HEART_DEAD_TIME  30000
//指定时间发送缓冲区内的数据
#define CLIENT_SEND_BUFF_TIME  500
class ClientSocket //: public ObjectPoolBase<ClientSocket, 1000>
{
public:
    int id = -1;
    int serverId = -1;
public:
    ClientSocket(SOCKET sockfd = INVALID_SOCKET):
                _sendBuff(SEND_BUFF_SZIE),
                _recvBuff(RECV_BUFF_SZIE)
    {
        static int n=0;
        id = n++;
        _sockfd = sockfd;
        resetDTHeart();
        resetDTSend();
    }
    ~ClientSocket()
    {
        CellLog_Debug("Serverid=%d,ClientSocket<%d>::~ClientSocket()\n",serverId,id);
        if(INVALID_SOCKET!=_sockfd)
        {
#ifdef _WIN32
        closesocket(_sockfd);
#else
        close(_sockfd);
#endif
        _sockfd = INVALID_SOCKET;
        }
    }
    
    bool needWrite()
    {
        
        return true;
    }
    
    SOCKET sockfd()
    {
        return _sockfd;
    }
    
    bool hasMsg()
    {
        return _recvBuff.hasMsg();
    }
    
    DataHeader* front_msg()
    {
        return (DataHeader*)_recvBuff.data();
    }
    
    void pop_front_msg()
    {
        if(hasMsg())
            _recvBuff.pop(front_msg()->dataLength);
    }
    
    int recvData()
    {
        return _recvBuff.readForSocket(_sockfd);
    }
    //缓冲区的控制根据业务需求的差异调整
    //发送数据
    int SendData(DataHeader* header)
        {
            if(_sendBuff.push((const char*) header, header->dataLength))
            {
                return header->dataLength;
            }
            return SOCKET_ERROR;
        }
    //立即将缓冲区数据发送到客户端
    int SendDataNow()
    {
        resetDTSend();
        return _sendBuff.writeToSocket(_sockfd);
        
    }
    
    void resetDTHeart()
    {
        _dtHeart = 0;
    }

    void resetDTSend()
    {
        _dtSend = 0;
    }
    //–ƒÃ¯ºÏ≤‚
    bool checkHeart(time_t dt)
    {
        _dtHeart += dt;
        if (_dtHeart >= CLIENT_HEART_DEAD_TIME)
        {
            CellLog::info("checkHeart dead:s=%d,time=%d\n",_sockfd, _dtHeart);
            return true;
        }
        return false;
    }
    
    
    bool checkSend(time_t dt)
    {
        _dtSend += dt;
        if (_dtSend >= CLIENT_SEND_BUFF_TIME)
        {
            CellLog::info("checkSend:s=%d,time=%d\n",_sockfd, _dtSend);
            SendDataNow();
            resetDTSend();
            return true;
            
        }
        return false;
    }
private:
    // socket fd_set  file desc set
    SOCKET _sockfd;
    //接受缓冲区
    MsgBuffer _recvBuff;
    //发送缓冲区
    MsgBuffer _sendBuff;
    //心跳死亡倒计时
    time_t _dtHeart;
    //上次发送时间
    time_t _dtSend;
};
typedef std::shared_ptr<ClientSocket> ClientSocketPtr;
#endif /* ClientSocket_hpp */
