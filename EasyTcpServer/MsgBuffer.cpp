//
//  MsgBuffer.cpp
//  EasyTcpServer
//
//  Created by stone
//

#include "MsgBuffer.hpp"
#include "CellLog.hpp"
MsgBuffer::MsgBuffer(int nSize)
{
    _nLast = 0;
    _bufferFullCount = 0;
    _nSize = nSize;
    _pBuffer = new char[_nSize];
}
MsgBuffer::~MsgBuffer()
{
    if(_pBuffer)
    {
        delete [] _pBuffer;
        _pBuffer = nullptr;
    }
}
bool MsgBuffer::push(const char *pData, int nLen)
{
    
    if (_nLast + nLen <= _nSize)
    {
        //将要发送的数据 拷贝到发送缓冲区尾部
        memcpy(_pBuffer + _nLast, pData, nLen);
        //计算数据尾部位置
        _nLast += nLen;
        
        if(_nLast == _nSize)
        {
            ++_bufferFullCount;
        }
        
        return true;
    }
    else
    {
        ++_bufferFullCount;
    }
    return  false;
}

int MsgBuffer::writeToSocket(SOCKET sockfd)
{
    int ret = 0;
    if(_nLast>0 && INVALID_SOCKET!=sockfd)
    {
        ret = (int)send(sockfd, _pBuffer, _nLast, 0);
        _nLast = 0;
        _bufferFullCount = 0;
    }
    return ret;
}

int MsgBuffer::readForSocket(SOCKET sockfd)
{
    if(_nSize-_nLast>0)
    {
        char* szRecv = _pBuffer+_nLast;
        int nLen = (int)recv(sockfd, szRecv, _nSize-_nLast, 0);
        if (nLen <= 0)
        {
            CellLog_Debug("nLen=%d",nLen);
            return nLen;
        }
        //消息缓冲区尾部位置后移
        _nLast+=nLen;
        return nLen;
    }
    return 0;
}

bool MsgBuffer::hasMsg()
{
    //判断消息缓冲区的数据长度大于消息头DataHeader长度
    if (_nLast >= sizeof(DataHeader))
    {
        //这时就可以知道当前消息的长度
        DataHeader* header = (DataHeader*)_pBuffer;
        //判断消息缓冲区的数据长度大于消息长度
        return _nLast >= header->dataLength;
    }
    return false;
}

char* MsgBuffer::data()
{
    return _pBuffer;
}

void MsgBuffer::pop(int nLen)
{
    int n = _nLast - nLen;
    if(n>0)
    {
        memcmp(_pBuffer, _pBuffer+nLen, n);
    }
    _nLast = n;
    if(_bufferFullCount>0)
    {
        --_bufferFullCount;
    }
}
