#ifndef _EasyTcpServer_hpp_
#define _EasyTcpServer_hpp_
#include<stdio.h>
#include<vector>
#include<thread>
#include<mutex>
#include<atomic>

#include"MessageHeader.hpp"
#include"CellTimestamp.hpp"
#include "CellServer.hpp"
#include "InetEvent.hpp"
#include "Configuration.hpp"
#include "CellNetWork.hpp"
class EasyTcpServer : public INetEvent
{
private:
	CellThread _thread;
	//消息处理对象，内部会创建线程
	std::vector<CellServer*> _cellServers;
	//每秒消息计时
	CellTimestamp _tTime;
	
	SOCKET _sock;
protected:
	//recv函数计数
	std::atomic_int _recvCount;
	//收到消息的计数
	std::atomic_int _msgCount;
	//客户端计数
	std::atomic_int _clientCount;
	
private:
	//处理网络消息
	void OnRun(CellThread* pThread);
	//计算并输出每秒网络消息
	void time4msg();
	
public:
	EasyTcpServer();
	virtual ~EasyTcpServer();
	//初始化socket
	SOCKET InitSocket();

	//绑定IP和端口号
	int Bind(const char* ip, unsigned short port);

	//监听端口号
	int Listen(int n);

	//接收客户端链接
	SOCKET Accept();

	//将新客户端分配给客户数量最小的cellserver
	void addClientToCellServer(ClientSocket* pClient);

	void Start(int nCellServer);
	//关闭Socket
	void Close();

	//发送指定socket数据
	int SendData(SOCKET cSock, DataHeader* header);

	//只会在一个线程中触发    线程安全
	virtual void OnNetJoin(ClientSocket*& pClient) override;
	//线程不安全
	virtual void OnNetLeave(ClientSocket*& pClient) override;
	//线程不安全
	virtual void OnNetMsg(CellServer* pCellServer,ClientSocket*& pClient, DataHeader* header) override;
	
	virtual void OnNetRecv(ClientSocket*& pClient) override;
};
#endif // !_EasyTcpServer_hpp_
