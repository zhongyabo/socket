//
//  CellThread.hpp
//  EasyTcpServer
//
//  Created by stone
//

#ifndef CellThread_hpp
#define CellThread_hpp

#include "CellSemaphore.hpp"
class CellThread
{
private:
    typedef std::function<void(CellThread*)> EventCall;
public:
    CellThread();
    //线程启动
    void start(EventCall onCreate = nullptr,
               EventCall onRun = nullptr,
               EventCall onDestory = nullptr);
    //线程关闭
    void close();
    //在工作函数中退出不需要使用信号量阻塞等待
    void exit();
    //线程是否在运行状态
    bool isRun();
protected:
    //线程工作函数
    void onWork();
private:
    EventCall _onCreate;
    EventCall _onRun;
    EventCall _onDestory;
    //不同线程改变数据需要加锁
    std::mutex _mutex;
    //信号量控制线程终止
    CellSemaphore _sem;
    //线程是否运行中
    bool _isRun;
};

#endif /* CellThread_hpp */
