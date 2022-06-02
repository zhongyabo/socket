//
//  Configuration.hpp
//  EasyTcpServer
//
//  Created by stone
//

#ifndef Configuration_hpp
#define Configuration_hpp

#ifdef _WIN32
    #define FD_SETSIZE      2506
    #define WIN32_LEAN_AND_MEAN
    #define _WINSOCK_DEPRECATED_NO_WARNINGS
    #include<windows.h>
    #include<WinSock2.h>
    #pragma comment(lib,"ws2_32.lib")
#else
    #include<unistd.h> //uni std
    #include<arpa/inet.h>
    #include<string.h>
    #include <signal.h>
    #include "CellLog.hpp"
    #define SOCKET int
    #define INVALID_SOCKET  (SOCKET)(~0)
    #define SOCKET_ERROR            (-1)
#endif
//缓冲区最小单元大小
#ifndef RECV_BUFF_SZIE
#define RECV_BUFF_SZIE 10240*4
#endif // !RECV_BUFF_SZIE

#ifndef SEND_BUFF_SZIE
#define SEND_BUFF_SZIE 102400
#endif // !SEND_BUFF_SZIE

#endif /* Configuration_hpp */
