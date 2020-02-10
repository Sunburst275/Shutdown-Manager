#include "weekday.h"

WeekDay::WeekDay()
{
    enabled = false;
    lowerTimeLimit = {0, 0, 0, 0, 0, 0, 0, 0, 0};
    upperTimeLimit = {0, 0, 0, 0, 0, 0, 0, 0, 0};
}

void WeekDay::setLowerTimeLimit(int h, int min)
{
    std::tm tempTime;

    if(h >= 24){h = 23; min = 59;}
    if(h < 0){h = 0; min = 0;}

    if(min >= 60){h++; min -= 60;}
    if(min < 0){h--; min = 60 + min;}


    if(h >= 24){h = 23; min = 59;}
    if(h < 0){h = 0; min = 0;}

    if(min >= 60){h++; min -= 60;}
    if(min < 0){h--; min = 60 + min;}

    tempTime.tm_min = min;
    tempTime.tm_hour = h;
    tempTime.tm_wday = weekday;

    tempTime.tm_sec = 0;
    tempTime.tm_mday = 0;
    tempTime.tm_yday = 0;
    tempTime.tm_mon = 0;
    tempTime.tm_isdst = -1;

    lowerTimeLimit = tempTime;
}

void WeekDay::setUpperTimeLimit(int h, int min)
{
    std::tm tempTime;

    if(h >= 24){h = 23; min = 59;}
    if(h < 0){h = 0; min = 0;}

    if(min >= 60){h++; min -= 60;}
    if(min < 0){h--; min = 60 + min;}


    if(h >= 24){h = 23; min = 59;}
    if(h < 0){h = 0; min = 0;}

    if(min >= 60){h++; min -= 60;}
    if(min < 0){h--; min = 60 + min;}

    tempTime.tm_min = min;
    tempTime.tm_hour = h;
    tempTime.tm_wday = weekday;

    tempTime.tm_sec = 0;
    tempTime.tm_mday = 0;
    tempTime.tm_yday = 0;
    tempTime.tm_mon = 0;
    tempTime.tm_isdst = -1;

    upperTimeLimit = tempTime;
}
