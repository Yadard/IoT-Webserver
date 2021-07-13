#ifndef SCHEDULER_HPP
#define SCHEDULER_HPP
#include<Arduino.h>

template<typename Function>
class Scheduler{
    Function m_Work;
    uint32_t m_interval;
    uint64_t m_start;
public:
    Function f_work;

public:
    Scheduler(Function Work, uint32_t interval) : m_Work(Work), m_interval(interval) {startCount();}
    Scheduler(uint32_t interval) : m_interval(interval) {startCount();}

    bool isTimeToWork();

    bool work();

    void setWork(Function);

    uint32_t getInterval();

    void startCount();

    uint32_t operator+(uint32_t);

    uint32_t operator-(uint32_t);

    void operator+=(uint32_t);

    void operator-=(uint32_t);

    void operator=(uint32_t);

    bool operator==(uint32_t);
};

//! If you now a prittier way to separate template classes/functions in .hpp and .cpp. Please let me know
#include "Scheduler.cpp"
#define SCHEDULER_CPP
#endif