#include "timelimits.h"

TimeLimits::TimeLimits()
    :
      sunday(),
      monday(),
      tuesday(),
      wednesday(),
      thursday(),
      friday(),
      saturday()
{
    weekdays[WeekDays::Monday] = &monday;
    weekdays[WeekDays::Tuesday] = &tuesday;
    weekdays[WeekDays::Wednesday] = &wednesday;
    weekdays[WeekDays::Thursday] = &thursday;
    weekdays[WeekDays::Friday] = &friday;
    weekdays[WeekDays::Saturday] = &saturday;
    weekdays[WeekDays::Sunday] = &sunday;

    // Set tm_wday to weekdays
    sunday.setWeekDay(WeekDays::Sunday);
    monday.setWeekDay(WeekDays::Monday);
    tuesday.setWeekDay(WeekDays::Tuesday);
    wednesday.setWeekDay(WeekDays::Wednesday);
    thursday.setWeekDay(WeekDays::Thursday);
    friday.setWeekDay(WeekDays::Friday);
    saturday.setWeekDay(WeekDays::Saturday);

    beforeShutdownWarningEnabled = false;
    beforeShutdownWarningTime = 0;
}

void TimeLimits::inhale(TimeLimits toCopy)
{
    for(int currentDay = WeekDays::Sunday; currentDay <= WeekDays::Saturday; currentDay++)
    {
        this->weekdays[currentDay]->setEnabled(toCopy.weekdays[currentDay]->isEnabled());
        this->weekdays[currentDay]->setLowerTimeLimit(toCopy.weekdays[currentDay]->getLowerTimeLimit());
        this->weekdays[currentDay]->setUpperTimeLimit(toCopy.weekdays[currentDay]->getUpperTimeLimit());
    }

    this->lowerTimeLimitsEnabled = toCopy.lowerTimeLimitsEnabled;
    this->upperTimeLimitsEnabled = toCopy.upperTimeLimitsEnabled;

    this->beforeShutdownWarningTime = toCopy.beforeShutdownWarningTime;
    this->beforeShutdownWarningEnabled = toCopy.beforeShutdownWarningEnabled;
}

void TimeLimits::inhaleWeekDay(WeekDay toCopyFrom, int destinatedWeekDay)
{
    weekdays[destinatedWeekDay]->inhale(toCopyFrom);
}
