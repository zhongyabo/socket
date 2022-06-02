//
//  CellLog.cpp
//  EasyTcpServer
//
//  Created by stone
//

#include "CellLog.hpp"

//CellLog::CellLog()
//{
//    _taskServer.Start();
//    _logFlie = nullptr;
//}
//
//CellLog::~CellLog()
//{
//    _taskServer.Close();
//    if(_logFlie)
//    {
//        info("CellLog fclose( _logFlie)\n");
//        fclose(_logFlie);
//        _logFlie = nullptr;
//    }
//}
//
//CellLog& CellLog::getInstance()
//{
//    static CellLog sLog;
//    return sLog;
//}
//
//void CellLog::info(const char *pStr)
//{
//    CellLog* pLog = &getInstance();
//    if(pLog->_logFlie)
//    {
//        fprintf(pLog->_logFlie, "%s",pStr);
//    }
//    printf("%s",pStr);
//    
//}
//
//template<typename ...Args>
//void CellLog::info(const char* pformat,Args ... args)
//{
//    printf(pformat,args...);
//}
//
//void CellLog::setLogPath(const char* logPath,const char* mode)
//{
//    if(_logFlie)
//    {
//        info("CellLog::setLogPath _logFlie != nullptr\n");
//        fclose(_logFlie);
//        _logFlie = nullptr;
//    }
//    
//    _logFlie = fopen(logPath, mode);
//    
//    if(_logFlie)
//    {
//        info("CellLog::setLogPath success,<%s,%s>\n",logPath,mode);
//    }
//    else
//    {
//        info("CellLog::setLogPath failed,<%s,%s>\n",logPath,mode);
//    }
//}
