//
//  MsgBuffer.hpp
//  EasyTcpServer
//
//  Created by stone
//

#ifndef MsgBuffer_hpp
#define MsgBuffer_hpp
#include "Configuration.hpp"
#include "MessageHeader.hpp"

class MsgBuffer
{
public:
    MsgBuffer(int nSize = 8192);
    ~MsgBuffer();
    bool push(const char* pData,int nLen);
    void pop(int nLen);
    int writeToSocket(SOCKET sockfd);
    int readForSocket(SOCKET sockfd);
    bool hasMsg();
    char* data();
private:
    //缓冲区
    char* _pBuffer;
    //缓冲区尾部位置,现有数据长度
    int _nLast;
    //缓冲区大小
    int _nSize;
    //缓冲区写满计数
    int _bufferFullCount;
};

#endif /* MsgBuffer_hpp */
