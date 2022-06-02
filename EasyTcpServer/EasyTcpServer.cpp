#include "EasyTcpServer.hpp"
EasyTcpServer::EasyTcpServer()
{
	_sock = INVALID_SOCKET;
	_recvCount=0;
	_clientCount=0;
	_msgCount=0;
}
EasyTcpServer::~EasyTcpServer()
{
	Close();
}

SOCKET EasyTcpServer::InitSocket()
{
	CellNetWork::init();
	if (INVALID_SOCKET != _sock)
	{
		CellLog::info("<socket=%d>关闭旧链接\n", (int)_sock);
		Close();
	}
	_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == _sock)
	{
		CellLog::info("初始化socket失败...\n");
	}
	else {
		CellLog::info("<socket=%d>初始化socket成功..\n", (int)_sock);
	}
	return _sock;
}

int EasyTcpServer::Bind(const char* ip, unsigned short port)
{
	//if (INVALID_SOCKET == _sock)
	//{
	//    InitSocket();
	//}
	// 2 bind 绑定用于接受客户端连接的网络端口
	sockaddr_in _sin = {};
	_sin.sin_family = AF_INET;
	_sin.sin_port = htons(port);//host to net unsigned short

#ifdef _WIN32
	if (ip){
		_sin.sin_addr.S_un.S_addr = inet_addr(ip);
	}
	else {
		_sin.sin_addr.S_un.S_addr = INADDR_ANY;
	}
#else
	if (ip) {
		_sin.sin_addr.s_addr = inet_addr(ip);
	}
	else {
		_sin.sin_addr.s_addr = INADDR_ANY;
	}
#endif
	int ret = bind(_sock, (sockaddr*)&_sin, sizeof(_sin));
	if (SOCKET_ERROR == ret)
	{
		CellLog::info("错误,绑定网络端口<%d>失败...\n", port);
	}
	else {
		CellLog::info("绑定网络端口<%d>成功...\n", port);
	}
	return ret;
}

int EasyTcpServer::Listen(int n)
{
	// 3 listen 监听网络端口
	int ret = listen(_sock, n);
	if (SOCKET_ERROR == ret)
	{
		CellLog::info("socket=<%d>错误,监听网络端口失败...\n",_sock);
	}
	else {
		CellLog::info("socket=<%d>监听网络端口成功...\n", _sock);
	}
	return ret;
}

SOCKET EasyTcpServer::Accept()
{
	// 4 accept 等待接受客户端连接
	sockaddr_in clientAddr = {};
	int nAddrLen = sizeof(sockaddr_in);
	SOCKET cSock = INVALID_SOCKET;
#ifdef _WIN32
	cSock = accept(_sock, (sockaddr*)&clientAddr, &nAddrLen);
#else
	cSock = accept(_sock, (sockaddr*)&clientAddr, (socklen_t *)&nAddrLen);
#endif
	if (INVALID_SOCKET == cSock)
	{
		CellLog::info("socket=<%d>错误,接受到无效客户端SOCKET...\n", (int)_sock);
	}
	else
	{
		//std::shared_ptr<ClientSocket> c(new ClientSocket(cSock));
		//addClientToCellServer(std::make_shared<ClientSocket>(cSock));
		//addClientToCellServer(c);
		addClientToCellServer(new ClientSocket(cSock));
		//获取客户端ip inet_ntoa(clientAddr.sin_addr));
	}
	return cSock;
}

void EasyTcpServer::addClientToCellServer(ClientSocket* pClient)
{
	//_clients.push_back(pClient);
	
	//查找客户数量最少的CellServer消息处理对象
	auto pMinServer = _cellServers[0];
	for(auto pCellServer : _cellServers)
	{
		if (pMinServer->getClientCount() > pCellServer->getClientCount())
		{
			pMinServer = pCellServer;
		}
	}
	pMinServer->addClient(pClient);
}

void EasyTcpServer::Start(int nCellServer)
{
	for (int n = 0; n < nCellServer; n++)
	{
		auto ser =new CellServer(n+1);
		_cellServers.push_back(ser);
		//注册网络事件接受对象
		ser->setEventObj(this);
		//启动消息处理线程
		ser->Start();
	}
	_thread.start(
				  //onCreate
				  nullptr,
				  //onRun
				  [this](CellThread* pThread){
					  OnRun(&_thread);
				  }
				  );
}

void EasyTcpServer::Close()
{
	CellLog::info("EasyTcpServer::Close() start...\n");
	_thread.close();
	if (_sock != INVALID_SOCKET)
	{
		for(auto s : _cellServers)
		{
			delete s;
		}
		_cellServers.clear();
#ifdef _WIN32
		// 8 关闭套节字closesocket
		closesocket(_sock);
#else
		// 8 关闭套节字closesocket
		close(_sock);
#endif
		_sock = INVALID_SOCKET;
	}
	CellLog::info("EasyTcpServer::Close() end...\n");
}

//处理网络消息
void EasyTcpServer::OnRun(CellThread* pThread)
{
	while (pThread->isRun())
	{
		time4msg();
		//伯克利套接字 BSD socket
		fd_set fdRead;//描述符（socket） 集合
		//清理集合
		FD_ZERO(&fdRead);
		//将描述符（socket）加入集合
		FD_SET(_sock, &fdRead);
		///nfds 是一个整数值 是指fd_set集合中所有描述符(socket)的范围，而不是数量
		///既是所有文件描述符最大值+1 在Windows中这个参数可以写0
		timeval t = { 0,1};
		int ret = select(_sock + 1, &fdRead, 0, 0, &t);
		if (ret < 0)
		{
			CellLog::info("EasyTcpServer.onRun select exit.\n");
			pThread->exit();
			break;
		}
		//判断描述符（socket）是否在集合中
		if (FD_ISSET(_sock, &fdRead))
		{
			FD_CLR(_sock, &fdRead);
			Accept();
		}
	}
}

void EasyTcpServer::time4msg()
{
	auto t1 = _tTime.getElapsedSecond();
	if (t1 >= 1.0)
	{
		CellLog::info("thread<%lu>,time<%lf>,socket<%d>,clients<%d>,recv<%d>,msg<%d>\n", _cellServers.size(), t1, _sock,(int)_clientCount, (int)(_recvCount/ t1),(int)(_msgCount/ t1));
		_recvCount=0;
		_msgCount=0;
		_tTime.update();
	}
}

void EasyTcpServer::OnNetJoin(ClientSocket*& pClient)
{
	_clientCount++;
}

void EasyTcpServer::OnNetLeave(ClientSocket*& pClient)
{
	_clientCount--;
}

void EasyTcpServer::OnNetMsg(CellServer* pCellServer,ClientSocket*& pClient, DataHeader* header)
{
	_msgCount++;
}

void EasyTcpServer::OnNetRecv(ClientSocket*& pClient)
{
	_recvCount++;
}
