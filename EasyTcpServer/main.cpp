#include "Alloctor.hpp"
#include "MyServer.hpp"
#include<thread>
int main(int argc,char* argv[])
{
	//char* strIp = argv[1];
	//uint16_t nPort = (uint16_t)atoi(argv[2]);
	//int nThread = atoi(argv[3]);
	//int nClient = atoi(argv[4]);
	
	CellLog::getInstance().setLogPath("serverLog", "w");
	MyServer server;
	server.InitSocket();
	server.Bind("182.16.70.186",9999);
	server.Listen(64);
	server.Start(4);

	while (true)
	{
		char cmdBuf[256] = {};
		scanf("%s", cmdBuf);
		if (0 == strcmp(cmdBuf, "exit"))
		{
			server.Close();
			CellLog::info("退出cmdThread\n");
			break;
		}
		else {
			CellLog::info("未定义命令\n");
		}
	}
	CellLog::info("已退出\n");
	return 0;
}
