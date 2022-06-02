#ifndef _CellTimestamp_hpp_
#define _CellTimestamp_hpp_

#include<chrono>
using namespace std::chrono;

class CellTime
{
public:
    //获取当前时间戳(毫秒)
    static time_t getNowTimeInMicroSec()
    {
        high_resolution_clock::now();
        return duration_cast<milliseconds>(high_resolution_clock::now().time_since_epoch()).count();
    }
};
class CellTimestamp
{
public:
    CellTimestamp();
    ~CellTimestamp();
    void update();
    /*
     *获取当前秒
    */
    double getElapsedSecond();
    /**
     *获取毫秒
    */
    double getElapsedTimeInMilliSec();
    /**
     *获取微妙
    */
    long long getElapsedTimeInMicroSec();
protected:
    time_point<high_resolution_clock> _begin;
};

#endif // !_CELLTimestamp_hpp_
