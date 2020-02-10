#ifndef TIMETHREAD_H
#define TIMETHREAD_H

// Qt headers
#include "QThread"
#include "QDebug"

// Standard C++ headers
#include <chrono>

class TimeThread : public QThread
{
    Q_OBJECT

public:
    TimeThread();

//==[ Slots ]==================================================================

signals:
    void sendTimeUpdate(const std::tm* now);

//==[ Functions ]==============================================================
public:
    inline bool getLooping(){return isLooping;}
    inline void setLooping(bool isLooping){this->isLooping = isLooping;}

    void run();
    void forceTimeUpdate();

//==[ Variables ]==============================================================

private:
    static constexpr ulong REFRESH_RATE = 250; //mSec

    bool isLooping = false;

    std::chrono::system_clock::time_point rawTime;
    std::time_t ttime;
    std::tm *now;
};

#endif // TIMETHREAD_H
