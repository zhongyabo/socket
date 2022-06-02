//
//  CellSemaphore.hpp
//  EasyTcpServer
//
//  Created by stone
//

#ifndef CellSemaphore_hpp
#define CellSemaphore_hpp

#include <thread>
#include <chrono>
#include <functional>
#include <condition_variable>
class CellSemaphore
{
public:
    CellSemaphore();
    ~CellSemaphore();
    //阻塞当前线程
    void wait();
    //唤醒当前线程
    void wakeup();
private:
    std::mutex _mutex;
    //阻塞等待的条件变量
    std::condition_variable _cv;
    //等待计数
    int _waitValue;
    //唤醒计数
    int _wakeupValue;
};
#endif /* CellSemaphore_hpp */
