#ifndef WEEKDAY_H
#define WEEKDAY_H

// Standard C++ headers
// -> for Time
#include <iostream>
#include <iomanip>
#include <ctime>
#include <chrono>

// I am well aware that WeekDay only refers to 5 days in the week (in english). The proper naming would be DaysOfTheWeek, but I noticed that too late, so I just went with this.
// - Sunburst275

typedef std::chrono::system_clock::time_point TimePoint;

class WeekDay
{

public:
    WeekDay();

    //==[ Setter/Getter ]==========================================================
    inline void inhale(WeekDay temp)
    {
        this->enabled = temp.isEnabled();
        this->weekday = temp.weekday;
        this->lowerTimeLimit = temp.getLowerTimeLimit();
        this->upperTimeLimit = temp.getUpperTimeLimit();
    }

    inline bool isEnabled(){return enabled;}
    inline void setEnabled(bool in_enabled){enabled = in_enabled;}

    inline int getWeekday(){return this->weekday;}
    inline void setWeekDay(int weekday)
    {
        this->weekday = weekday;

        lowerTimeLimit.tm_wday = this->weekday;
        upperTimeLimit.tm_wday = this->weekday;
    }

    inline std::tm getLowerTimeLimit(){return lowerTimeLimit;}
    inline void setLowerTimeLimit(std::tm limits){this->lowerTimeLimit = limits;}
    void setLowerTimeLimit(int h, int min);

    inline std::tm getUpperTimeLimit(){return upperTimeLimit;}
    inline void setUpperTimeLimit(std::tm limits){this->upperTimeLimit = limits;}
    void setUpperTimeLimit(int h, int min);

    inline int getUpperTimeLimitMinutes(){return upperTimeLimit.tm_min;}
    inline int getUpperTimeLimitHours(){return upperTimeLimit.tm_hour;}

    inline int getLowerTimeLimitMinutes(){return lowerTimeLimit.tm_min;}
    inline int getLowerTimeLimitHours(){return lowerTimeLimit.tm_hour;}

    inline void resetDay()
    {
        enabled = false;
        lowerTimeLimit = {0, 0, 0, 0, 0, 0, 0, 0, 0};
        upperTimeLimit = {0, 0, 0, 0, 0, 0, 0, 0, 0};
    }

    //==[ Variables ]==============================================================
private:
    bool enabled;

    int weekday;

    std::tm lowerTimeLimit;
    std::tm upperTimeLimit;

    //==[ Functions ]==============================================================
public:
    inline void imbueWeekDay()
    {
        lowerTimeLimit.tm_wday = weekday;
        upperTimeLimit.tm_wday = weekday;
    }
};

#endif // WEEKDAY_H
