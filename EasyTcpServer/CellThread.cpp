//
//  CellThread.cpp
//  EasyTcpServer
//
//  Created by stone
//

#include "CellThread.hpp"

CellThread::CellThread()
{
	_isRun = false;
}
void CellThread::start(EventCall onCreate,EventCall onRun,EventCall onDestory)
{
	std::lock_guard<std::mutex> lock(_mutex);
	if(!_isRun)
	{
		_isRun = true;
		if(onCreate)
			_onCreate = onCreate;
		if(onRun)
			_onRun = onRun;
		if(onDestory)
			_onDestory = onDestory;
		
		std::thread t(std::mem_fn(&CellThread::onWork),this);
		t.detach();

	}
	
}

void CellThread::onWork()
{
	if(_onCreate)
		_onCreate(this);
	if(_onRun)
		_onRun(this);
	if(_onDestory)
		_onDestory(this);
	
	_sem.wakeup();
}

void CellThread::close()
{
	std::lock_guard<std::mutex> lock(_mutex);
	if(_isRun)
	{
		_isRun = false;
		_sem.wait();
	}
}

void CellThread::exit()
{
	std::lock_guard<std::mutex> lock(_mutex);
	if(_isRun)
	{
		_isRun = false;
	}
}

bool CellThread::isRun()
{
	return _isRun;
}
