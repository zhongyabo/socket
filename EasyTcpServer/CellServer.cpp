#include "CellServer.hpp"
std::mutex CellServer::_mutex;
CellServer::CellServer(int id)
{
    _id = id;
    _clients_change = false;
    _pNetEvent = nullptr;
    _taskServer._serverId = id;
    _oldTime= CellTime::getNowTimeInMilliSec();
}

CellServer::~CellServer()
{
    CellLog::info("CellServer<%d>::~CellServer() start...\n",_id);
    Close();
    CellLog::info("CellServer<%d>::~CellServer() end...\n",_id);
}

void CellServer::setEventObj(INetEvent* event)
{
    _pNetEvent = event;
}

void CellServer::Close()
{
    CellLog::info("CellServer<%d>::Close() start...\n",_id);
    _taskServer.Close();
    _thread.close();
    CellLog::info("CellServer<%d>::Close() end...\n",_id);
}

void CellServer::clearClients()
{
    for (auto iter : _clients)
    {
        delete iter.second;
    }
    _clients.clear();
        
    for (auto iter : _clientsBuff)
    {
        delete iter;
    }
    _clientsBuff.clear();
}

void CellServer::OnRun(CellThread* pThread)
{
    while (pThread->isRun())
    {
        if (!_clientsBuff.empty())
        {//从缓冲队列里取出客户数据
            std::lock_guard<std::mutex> lock(_mutex);
            for (auto pClient : _clientsBuff)
            {
                _clients[pClient->sockfd()]=pClient;
                pClient->serverId = _id;
                if(_pNetEvent)
                    _pNetEvent->OnNetJoin(pClient);
                
            }
            _clientsBuff.clear();
            _clients_change=true;
        }

        //如果没有需要处理的客户端，就跳过
        if (_clients.empty())
        {
            std::chrono::milliseconds t(1);
            std::this_thread::sleep_for(t);
            _oldTime = CellTime::getNowTimeInMilliSec();
            continue;
        }

        //checkTime();

        //伯克利套接字 BSD socket
        fd_set fdRead;//描述符（socket） 集合
        fd_set fdWrite;
        //fd_set fdExc;
        if(_clients_change)
        {
            _clients_change = false;
            //清理集合
            FD_ZERO(&fdRead);
            //将描述符（socket）加入集合
            _maxSock = _clients.begin()->second->sockfd();
            for (auto iter : _clients)
            {
                FD_SET(iter.second->sockfd(), &fdRead);
                if (_maxSock < iter.second->sockfd())
                {
                    _maxSock = iter.second->sockfd();
                }
            }
            memcpy(&_fdRead_bak, &fdRead, sizeof(fd_set));
        }
        else
        {
            memcpy(&fdRead, &_fdRead_bak, sizeof(fd_set));
        }
        
        FD_ZERO(&fdWrite);
        bool bNeedWrite = false;
        //检测是否需要写数据给客户端
        for(auto iter : _clients)
        {
            if(iter.second->needWrite())
            {
                bNeedWrite = true;
                FD_SET(iter.second->sockfd(),&fdWrite);
            }
            
        }

        //memcpy(&fdWrite, &_fdRead_bak, sizeof(fd_set));
        ///nfds 是一个整数值 是指fd_set集合中所有描述符(socket)的范围，而不是数量
        ///既是所有文件描述符最大值+1 在Windows中这个参数可以写0
        timeval t={0,1};
        int ret = 0;
        if(bNeedWrite)
        {
            ret = select(_maxSock + 1, &fdRead, &fdWrite, nullptr, &t);
        }
        else
            ret = select(_maxSock + 1, &fdRead, nullptr, nullptr, &t);
        
        if (ret < 0)
        {
            CellLog::info("cellServer<%d>.onRun.select error. exit\n",_id);
            pThread->exit();
            break;
        }
        else if(ret == 0)
        {
            continue;
        }
        
        ReadData(fdRead);
        writeData(fdWrite);
//        writeData(fdExc);
//        CellLog::info("CellServer<%d>.OnRun.select: fdRead<%d>,fdWrite<%d>\n",_id,fdRead.fds_bits,fdWrite.fds_bits);
//        if(fdExc.fds_bits)
//        {
//            CellLog::info("###fdExc<%d>",fdExc.fds_bits);
//        }
    }
    CellLog::info("CEllServer<%d>::OnRun exit\n",_id);
}

void CellServer::checkTime()
{
    auto nowTime = CellTime::getNowTimeInMilliSec();
    
    auto dt = nowTime-_oldTime;
    _oldTime = nowTime;
    for(auto iter = _clients.begin();iter!=_clients.end();)
    {
        //心跳检测
        if(iter->second->checkHeart(dt))
        {
            if(_pNetEvent)
                _pNetEvent->OnNetLeave(iter->second);
            _clients_change = true;
            delete iter->second;
            auto iterOld = iter++;
            _clients.erase(iterOld);
            continue;
        }
        
//        //定时发送检测
//        iter->second->checkSend(dt);
        
        iter++;
    }
}

void CellServer::onClientLeave(ClientSocket* pClient)
{
    if(_pNetEvent)
        _pNetEvent->OnNetLeave(pClient);
    _clients_change = true;
    delete pClient;
}
void CellServer::writeData(fd_set& fdWrite)
{
#ifdef _WIN32
        for(int n=0;n<fdWrite.fd_count;n++)
        {
            auto iter = _clients.find(fdWrite.fd_array[n]);
            if(iter!=_clients.end())
            {
                if (-1 == iter->second->SendDataNow())
                {
                    onClientLeave(iter->second);
                    _clients.erase(iter);
                }
            }
        }
#else
    for (auto iter = _clients.begin();iter!=_clients.end();)
        {
            if (iter->second->needWrite() && FD_ISSET(iter->second->sockfd(), &fdWrite))
            {
                if (-1 == iter->second->SendDataNow())
                {
                    onClientLeave(iter->second);
                    auto iterOld = iter;
                    iter++;
                    _clients.erase(iterOld);
                    continue;
                }
            }
            iter++;
        }
#endif
}

void CellServer::ReadData(fd_set& fdRead)
{
#ifdef _WIN32
        for(int n=0;n<fdRead.fd_count;n++)
        {
            auto iter = _clients.find(fdRead.fd_array[n]);
            if(iter!=_clients.end())
            {
                if (-1 == RecvData(iter->second))
                {
                    onClientLeave(iter->second);
                    _clients.erase(iter);
                }
            }
        }
#else
    for (auto iter = _clients.begin();iter!=_clients.end();)
        {
            if (FD_ISSET(iter->second->sockfd(), &fdRead))
            {
                if (-1 == RecvData(iter->second))
                {
                    onClientLeave(iter->second);
                    auto iterOld = iter;
                    iter++;
                    _clients.erase(iterOld);
                    continue;
                }
            }
            iter++;
        }
#endif
}

int CellServer::RecvData(ClientSocket* pClient)
{
    // 5 接收客户端数据
    int nLen = pClient->recvData();
    if (nLen <= 0)
    {
        return -1;
    }
    //接受网络数据事件
    _pNetEvent->OnNetRecv(pClient);
    //循环处理消息
    while (pClient->hasMsg())
    {
        //处理消息
        OnNetMsg(pClient, pClient->front_msg());
        //移除处理完的数据
        pClient->pop_front_msg();
    }
    return 0;
}

void CellServer::OnNetMsg(ClientSocket* pClient, DataHeader* header)
{
    _pNetEvent->OnNetMsg(this,pClient, header);
}

void CellServer::addClient(ClientSocket* pClient)
{
    std::lock_guard<std::mutex> lock(_mutex);
    //_mutex.lock();
    _clientsBuff.push_back(pClient);
    //_mutex.unlock();
}

void CellServer::Start()
{
    _taskServer.Start();
    _thread.start(
                  //onCreate
                  nullptr,
                  //onRun
                  [this](CellThread* pThread){
                        OnRun(&_thread);},
                  //onDestory
                  [this](CellThread* pThread){
                        clearClients();}
                  );
}

size_t CellServer::getClientCount()
{
    return _clients.size() + _clientsBuff.size();
}

void CellServer::addSendTask(ClientSocket* pClient,DataHeader* header)
{
    _taskServer.addTask([pClient,header]()
    {
        pClient->SendData(header);
        delete header;
    });
}
