//
//  CellTask.hpp
//  EasyTcpServer
//
//  Created by stone
//

#ifndef _CellTask_hpp
#define _CellTask_hpp
#include <thread>
#include <mutex>
#include <list>
#include <memory>
#include <functional>
#include "CellThread.hpp"
class CellTaskServer
{
    typedef std::function<void()> CellTask;
public:
    CellTaskServer();
    
    ~CellTaskServer();
    
    //添加任务
    void addTask(CellTask task);
    
    //启动服务
    void Start();
    
    void Close();
private:
    //工作函数
    void OnRun(CellThread* pThread);
private:
    //任务数据
    std::list<CellTask> _tasks;
    //任务数据缓冲区
    std::list<CellTask> _tasksBuf;
    //改变数据缓冲区时需要加锁
    std::mutex _mutex;
    //
    CellThread _thread;
public:
    int _serverId;
    bool _isWaitExit=false;
};

#endif /* CellTask_hpp */
