//
//  MyServer.cpp
//  EasyTcpServer
//
//  Created by stone
//

#include "MyServer.hpp"
MyServer::MyServer()
{
    
}
MyServer::~MyServer()
{
    
}
void MyServer::OnNetJoin(ClientSocket*& pClient)
{
    EasyTcpServer::OnNetJoin(pClient);
}
void MyServer::OnNetMsg(CellServer* pCellServer,ClientSocket*& pClient, DataHeader *header)
{
    EasyTcpServer::OnNetMsg(pCellServer,pClient, header);
    switch (header->cmd)
    {
    case CMD_LOGIN:
    {

        pClient->resetDTHeart();
        //Login* login = (Login*)header;
        LoginResult ret;
        if(0 == pClient->SendData(&ret))
        {
            
            //发送缓冲区满了,消息没发出去
            CellLog::info("socket<%d> Send Buff Full.\n",pClient->sockfd());
        }
        //CellLog::info("收到客户端<Socket=%d>请求：CMD_LOGIN,数据长度：%d,userName=%s PassWord=%s\n", cSock, login->dataLength, login->userName, login->PassWord);
        //忽略判断用户密码是否正确的过程
        //auto ret = std::make_shared<LoginResult>();
        //LoginResult* ret = new LoginResult();
        //pCellServer->addSendTask(pClient, ret);
    }
    break;
    case CMD_LOGOUT:
    {
       // Logout* logout = (Logout*)header;
        //CellLog::info("收到客户端<Socket=%d>请求：CMD_LOGOUT,数据长度：%d,userName=%s \n", cSock, logout->dataLength, logout->userName);
        //忽略判断用户密码是否正确的过程
        //LogoutResult ret;
        //SendData(cSock, &ret);
    }
    break;
    case CMD_HEART:
    {
        pClient->resetDTHeart();
        Heart ret;
        pClient->SendData(&ret);
    }
    default:
    {
        CellLog::info("<socket=%d>收到未定义消息,数据长度：%d\n", pClient->sockfd(), header->dataLength);
        //DataHeader ret;
        //SendData(cSock, &ret);
    }
    break;
    }
}
void MyServer::OnNetLeave(ClientSocket*& pClient)
{
    EasyTcpServer::OnNetLeave(pClient);
}
void MyServer::OnNetRecv(ClientSocket*& pClient)
{
    EasyTcpServer::OnNetRecv(pClient);
}
