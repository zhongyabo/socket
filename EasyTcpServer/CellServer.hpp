//
//  CellServer.hpp
//  EasyTcpServer
//
//  Created by stone
//

#ifndef CellServer_hpp
#define CellServer_hpp
#include <vector>
#include <map>
#include <mutex>
#include <thread>
#include "ClientSocket.hpp"
#include "Configuration.hpp"
#include "InetEvent.hpp"
#include "CellTask.hpp"
#include "CellTimestamp.hpp"
#include "CellSemaphore.hpp"
class CellServer
{
public:
    CellServer(int id);

    ~CellServer();

    void setEventObj(INetEvent* event);

    //关闭Socket
    void Close();


    //处理网络消息
    void OnRun(CellThread* pThread);
    //ª∫≥Â«¯
    //char _szRecv[RECV_BUFF_SZIE] = {};
    //Ω” ’ ˝æ› ¥¶¿Ì’≥∞¸ ≤∑÷∞¸
    int RecvData(ClientSocket* pClient);

    void writeData(fd_set& fdWrite);
    
    //œÏ”¶Õ¯¬Áœ˚œ¢
    virtual void OnNetMsg(ClientSocket* pClient, DataHeader* header);
    void addClient(ClientSocket* pClient);

    void Start();

    size_t getClientCount();
    
    void onClientLeave(ClientSocket* pClient);
    void addSendTask(ClientSocket* pClient,DataHeader* header);
    
    void checkTime();
    
    void ReadData(fd_set& fdRead);
    
    void clearClients();
private:
    //’˝ ΩøÕªß∂”¡–
    std::map<SOCKET,ClientSocket*> _clients;
    //ª∫≥ÂøÕªß∂”¡–
    std::vector<ClientSocket*> _clientsBuff;
    static std::mutex _mutex;
    INetEvent* _pNetEvent;
    
private:
    //备份客户socket fd_set
    fd_set _fdRead_bak;
    //客户列表是否有变化
    bool _clients_change;
    //
    CellThread _thread;
    
    int _id;
    SOCKET _maxSock;
    CellTaskServer _taskServer;
    
    time_t _oldTime;
};

#endif /* CellServer_hpp */
