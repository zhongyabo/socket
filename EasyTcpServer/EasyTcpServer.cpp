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
		CellLog::info("<socket=%d>�رվ�����\n", (int)_sock);
		Close();
	}
	_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == _sock)
	{
		CellLog::info("��ʼ��socketʧ��...\n");
	}
	else {
		CellLog::info("<socket=%d>��ʼ��socket�ɹ�..\n", (int)_sock);
	}
	return _sock;
}

int EasyTcpServer::Bind(const char* ip, unsigned short port)
{
	//if (INVALID_SOCKET == _sock)
	//{
	//    InitSocket();
	//}
	// 2 bind �����ڽ��ܿͻ������ӵ�����˿�
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
		CellLog::info("����,������˿�<%d>ʧ��...\n", port);
	}
	else {
		CellLog::info("������˿�<%d>�ɹ�...\n", port);
	}
	return ret;
}

int EasyTcpServer::Listen(int n)
{
	// 3 listen ��������˿�
	int ret = listen(_sock, n);
	if (SOCKET_ERROR == ret)
	{
		CellLog::info("socket=<%d>����,��������˿�ʧ��...\n",_sock);
	}
	else {
		CellLog::info("socket=<%d>��������˿ڳɹ�...\n", _sock);
	}
	return ret;
}

SOCKET EasyTcpServer::Accept()
{
	// 4 accept �ȴ����ܿͻ�������
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
		CellLog::info("socket=<%d>����,���ܵ���Ч�ͻ���SOCKET...\n", (int)_sock);
	}
	else
	{
		//std::shared_ptr<ClientSocket> c(new ClientSocket(cSock));
		//addClientToCellServer(std::make_shared<ClientSocket>(cSock));
		//addClientToCellServer(c);
		addClientToCellServer(new ClientSocket(cSock));
		//��ȡ�ͻ���ip inet_ntoa(clientAddr.sin_addr));
	}
	return cSock;
}

void EasyTcpServer::addClientToCellServer(ClientSocket* pClient)
{
	//_clients.push_back(pClient);
	
	//���ҿͻ��������ٵ�CellServer��Ϣ�������
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
		//ע�������¼����ܶ���
		ser->setEventObj(this);
		//������Ϣ�����߳�
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
		// 8 �ر��׽���closesocket
		closesocket(_sock);
#else
		// 8 �ر��׽���closesocket
		close(_sock);
#endif
		_sock = INVALID_SOCKET;
	}
	CellLog::info("EasyTcpServer::Close() end...\n");
}

//����������Ϣ
void EasyTcpServer::OnRun(CellThread* pThread)
{
	while (pThread->isRun())
	{
		time4msg();
		//�������׽��� BSD socket
		fd_set fdRead;//��������socket�� ����
		//������
		FD_ZERO(&fdRead);
		//����������socket�����뼯��
		FD_SET(_sock, &fdRead);
		///nfds ��һ������ֵ ��ָfd_set����������������(socket)�ķ�Χ������������
		///���������ļ����������ֵ+1 ��Windows�������������д0
		timeval t = { 0,1};
		int ret = select(_sock + 1, &fdRead, 0, 0, &t);
		if (ret < 0)
		{
			CellLog::info("EasyTcpServer.onRun select exit.\n");
			pThread->exit();
			break;
		}
		//�ж���������socket���Ƿ��ڼ�����
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
