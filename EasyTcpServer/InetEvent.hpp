//
//  InetEvent.h
//  EasyTcpServer
//
//  Created by stone
//

#ifndef _InetEvent_hpp_
#define _InetEvent_hpp_
#include "MessageHeader.hpp"
class CellServer;
class INetEvent
{
public:
    //纯虚函数
    //客户端加入事件
    virtual void OnNetJoin(ClientSocket*& pClient) = 0;
    //客户端离开事件
    virtual void OnNetLeave(ClientSocket*& pClient) = 0;
    //客户端离开事件
    virtual void OnNetMsg(CellServer* pCellServer,ClientSocket*& pClient, DataHeader* header) = 0;
    //recv事件
    virtual void OnNetRecv(ClientSocket*& pClient)=0;
private:

};
#endif /* InetEvent_h */
