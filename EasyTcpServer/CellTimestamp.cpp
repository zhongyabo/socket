#include "CellTimestamp.hpp"

CellTimestamp::CellTimestamp()
{
    update();
}

CellTimestamp::~CellTimestamp()
{
}

void CellTimestamp::update()
{
    _begin = high_resolution_clock::now();
}

double CellTimestamp::getElapsedSecond()
{
    return  getElapsedTimeInMicroSec() * 0.000001;
}

double CellTimestamp::getElapsedTimeInMilliSec()
{
    return this->getElapsedTimeInMicroSec() * 0.001;
}

long long CellTimestamp::getElapsedTimeInMicroSec()
{
    return duration_cast<microseconds>(high_resolution_clock::now() - _begin).count();
}
