#ifndef TIMELIMITS_H
#define TIMELIMITS_H

// Own headers
#include "weekday.h"

//using namespace std::literals::chrono_literals;

class TimeLimits
{
public:
    TimeLimits();

    static constexpr int MAX_WEEKDAYS = 7;
    static constexpr int MIN_TIME_DIFF = 10; // In minutes
    enum WeekDays{Sunday, Monday, Tuesday, Wednesday, Thursday, Friday, Saturday};

//==[ Functions ]==============================================================
    void inhale(TimeLimits toCopyFrom);
    void inhaleWeekDay(WeekDay toCopyFrom, int destinatedWeekDay);


//==[ Variables ]==============================================================
    WeekDay sunday;
    WeekDay monday;
    WeekDay tuesday;
    WeekDay wednesday;
    WeekDay thursday;
    WeekDay friday;
    WeekDay saturday;

    WeekDay *weekdays[MAX_WEEKDAYS];

    bool lowerTimeLimitsEnabled;
    bool upperTimeLimitsEnabled;

    // Gehört eigentlich in "Settings"
    int beforeShutdownWarningTime;
    bool beforeShutdownWarningEnabled;

};



#endif // TIMELIMITS_H
