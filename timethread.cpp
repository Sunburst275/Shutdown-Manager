#include "timethread.h"

TimeThread::TimeThread()
    :
      rawTime(std::chrono::system_clock::now()),
      ttime(std::chrono::system_clock::to_time_t(rawTime))
{

}

void TimeThread::run()
{
    while (isLooping)
    {
        rawTime = std::chrono::system_clock::now();
        ttime = std::chrono::system_clock::to_time_t(rawTime);
        now = localtime(&ttime);

        // Emit signal: time container
        emit sendTimeUpdate(now);

        this->msleep(REFRESH_RATE);
    }
}

void TimeThread::forceTimeUpdate()
{
    rawTime = std::chrono::system_clock::now();
    ttime = std::chrono::system_clock::to_time_t(rawTime);
    emit sendTimeUpdate(now);
}

