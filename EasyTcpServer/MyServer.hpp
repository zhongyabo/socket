//
//  MyServer.hpp
//  EasyTcpServer
//
//  Created by stone
//

#ifndef MyServer_hpp
#define MyServer_hpp

#include "EasyTcpServer.hpp"
class MyServer : public EasyTcpServer
{
public:
    MyServer();
    ~MyServer();
    virtual void OnNetJoin(ClientSocket*& Client) override;
    virtual void OnNetLeave(ClientSocket*& pClient) override;
    virtual void OnNetMsg(CellServer* pCellServer,ClientSocket*& pClient, DataHeader *header) override;
    virtual void OnNetRecv(ClientSocket* &pClient) override;
};
#endif /* MyServer_hpp */
