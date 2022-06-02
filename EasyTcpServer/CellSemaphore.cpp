//
//  CellSemaphore.cpp
//  EasyTcpServer
//
//  Created by stone
//

#include "CellSemaphore.hpp"
CellSemaphore::CellSemaphore()
{
    _waitValue = 0;
    _wakeupValue = 0;
}
CellSemaphore::~CellSemaphore()
{
    
}
void CellSemaphore::wait()
{
    std::unique_lock<std::mutex> lock(_mutex);
    if(--_waitValue<0)
    {
        _cv.wait(lock,[this]()->bool{
            return _wakeupValue>0;
        });
        --_wakeupValue;
    }
}

void CellSemaphore::wakeup()
{
    std::lock_guard<std::mutex> lock(_mutex);
    if(++_waitValue<=0)
    {
        ++_wakeupValue;
        _cv.notify_one();
    }
}
