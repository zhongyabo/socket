//
//  CellLog.hpp
//  EasyTcpServer
//
//  Created by stone
//

#ifndef CellLog_hpp
#define CellLog_hpp

//#include <stdio.h>
#include <iostream>
#include "CellTask.hpp"
#include <ctime>
class CellLog
{
#ifdef _DEBUG
    #ifndef CellLog_Debug
        #define CellLog_Debug(...) CEllLog::Debug(__VA_ARGS__)
    #endif
#else
    #ifndef CellLog_Debug
        #define CellLog_Debug(...)
    #endif
#endif
    
#define CellLog_Info(...) CEllLog::info(__VA_ARGS__)
#define CellLog_Warning(...) CEllLog::Warning(__VA_ARGS__)
#define CellLog_Error(...) CEllLog::Error(__VA_ARGS__)
private:
    CellLog()
    {
        _taskServer.Start();
        _logFlie = nullptr;
    }

public:
    ~CellLog()
    {
        _taskServer.Close();
        if(_logFlie)
        {
            info("CellLog fclose( _logFlie)\n");
            fclose(_logFlie);
            _logFlie = nullptr;
        }
    }

    static CellLog& getInstance()
    {
        static CellLog sLog;
        return sLog;
    }

    static void Error(const char *pStr)
    {
        Error("%s",pStr);
    }
    
    template<typename ...Args>
    static void Error(const char* pformat,Args ... args)
    {
        Echo("Error",pformat,args...);
    }
    
    static void Debug(const char *pStr)
    {
        Debug("%s",pStr);
    }
    
    template<typename ...Args>
    static void Debug(const char* pformat,Args ... args)
    {
        Echo("Debug",pformat,args...);
    }
    
    static void Warning(const char *pStr)
    {
        Warning("%s",pStr);
    }
    
    template<typename ...Args>
    static void Warning(const char* pformat,Args ... args)
    {
        Echo("Warning",pformat,args...);
    }
    
    static void info(const char *pStr)
    {
        info("%s",pStr);
    }
    
    template<typename ...Args>
    static void info(const char* pformat,Args ... args)
    {
        Echo("Info",pformat,args...);
    }

    template<typename ...Args>
    static void Echo(const char* type,const char* pformat,Args ... args)
    {
        CellLog* pLog = &getInstance();
        pLog->_taskServer.addTask([=]()
        {
            if(pLog->_logFlie)
            {
                auto t = std::chrono::system_clock::now();
                auto tNow = std::chrono::system_clock::to_time_t(t);
                std::tm* now = std::localtime(&tNow);
                fprintf(pLog->_logFlie, "%s",type);
                fprintf(pLog->_logFlie, "[%d-%d-%d %d:%d:%d]",now->tm_year+1900,now->tm_mon+1,now->tm_mday,now->tm_hour,now->tm_min,now->tm_sec);
                
                fprintf(pLog->_logFlie,pformat,args...);
                fflush(pLog->_logFlie);
            }
            printf(pformat,args...);
        });
    }

    void setLogPath(const char* logName,const char* mode)
    {
        if(_logFlie)
        {
            info("CellLog::setLogPath _logFlie != nullptr\n");
            fclose(_logFlie);
            _logFlie = nullptr;
        }
        
        static char logPath[256]={};
        auto t = std::chrono::system_clock::now();
        auto tNow = std::chrono::system_clock::to_time_t(t);
        std::tm* now = std::localtime(&tNow);
        sprintf(logPath, "%s[%d-%d-%d_%d-%d-%d].txt",logName,now->tm_year+1900,now->tm_mon+1,now->tm_mday,now->tm_hour,now->tm_min,now->tm_sec);
        _logFlie = fopen(logPath, mode);
        
        if(_logFlie)
        {
            info("CellLog::setLogPath success,<%s,%s>\n",logPath,mode);
        }
        else
        {
            info("CellLog::setLogPath failed,<%s,%s>\n",logPath,mode);
        }
    }

private:
    FILE* _logFlie;
    CellTaskServer _taskServer;
};

#endif /* CellLog_hpp */
