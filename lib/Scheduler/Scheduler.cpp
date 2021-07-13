#include "Scheduler.hpp"
#ifndef SCHEDULER_CPP
template <typename Function>
bool Scheduler<Function>::isTimeToWork(){
    return esp_timer_get_time() - m_start >= m_interval;
}

template <typename Function>
bool Scheduler<Function>::work(){
    if(isTimeToWork()){
        m_start = esp_timer_get_time();
        f_work = m_Work;
        return true;
    }
    else
        return false;
}

template <typename Function>
void Scheduler<Function>::setWork(Function Work){
    m_Work = Work;
}

template <typename Function>
void Scheduler<Function>::startCount(){
    m_start = esp_timer_get_time();
}

template <typename Function>
uint32_t Scheduler<Function>::getInterval(){
    return m_interval;
}

template <typename Function>
uint32_t Scheduler<Function>::operator+(uint32_t other){
    return m_interval + other;
}

template <typename Function>
uint32_t Scheduler<Function>::operator-(uint32_t other){
    return m_interval - other;
}

template <typename Function>
void Scheduler<Function>::operator+=(uint32_t other){
    m_interval = m_interval + other;
}

template <typename Function>
void Scheduler<Function>::operator-=(uint32_t other){
    m_interval = m_interval - other;
}

template <typename Function>
void Scheduler<Function>::operator=(uint32_t other){
    m_interval = other;
}

template <typename Function>
bool Scheduler<Function>::operator==(uint32_t other){
    return m_interval == other;
}
#endif