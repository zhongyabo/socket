//
//  CellTask.cpp
//  EasyTcpServer
//
//  Created by stone
//

#include "CellTask.hpp"
CellTaskServer::CellTaskServer()
{
    _serverId = -1;
}
CellTaskServer::~CellTaskServer()
{
    
}
void CellTaskServer::addTask(CellTask task)
{
    std::lock_guard<std::mutex> lock(_mutex);
    _tasksBuf.push_back(task);
}
void CellTaskServer::Start()
{
    _thread.start(nullptr,[this](CellThread* pThread){
        OnRun(&_thread);
    });
}
void CellTaskServer::Close()
{
    //CellLog::info("CellTaskServer<%d>::Close() start...\n",_serverId);
    _thread.close();
   // CellLog::info("CellTaskServer<%d>::Close() end...\n",_serverId);
}
void CellTaskServer::OnRun(CellThread* pThread)
{
    while(pThread->isRun())
    {
        //从缓冲区取出数据
        if(!_tasksBuf.empty())
        {
            std::lock_guard<std::mutex> lock(_mutex);
            for(auto pTask : _tasksBuf)
            {
                _tasks.push_back(pTask);
            }
            _tasksBuf.clear();
        }
        //如果没有任务
        if(_tasks.empty())
        {
            std::chrono::milliseconds t(1);
            std::this_thread::sleep_for(t);
            continue;
        }
        //处理任务
        for(auto pTask : _tasks)
        {
            pTask();
        }
        _tasks.clear();
    }
    for(auto pTask : _tasksBuf)
    {
        pTask();
    }
   // CellLog::info("CellTaskServer<%d>.OnRun exit\n",_serverId);
    
}
