//
//  CellNetWork.hpp
//  EasyTcpServer
//
//  Created by stone
//

#ifndef CellNetWork_hpp
#define CellNetWork_hpp
#include "Configuration.hpp"
class CellNetWork
{
private:
    CellNetWork()
    {
#ifdef _WIN32
    //启动Windows socket 2.x环境
    WORD ver = MAKEWORD(2, 2);
    WSADATA dat;
    WSAStartup(ver, &dat);
#endif
    
#ifndef _WIN32
    signal(SIGPIPE, SIG_IGN);
#endif
    }
    
public:
    
    ~CellNetWork()
    {
#ifdef _WIN32
        //清除Windows socket环境
        WSACleanup();
#endif
    }
    
    static void init()
    {
        static CellNetWork obj;
    }
};

#endif /* CellNetWork_hpp */
