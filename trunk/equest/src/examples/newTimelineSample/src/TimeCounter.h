/********************************************************************
    created:  2011/04/06
    created:  6:4:2011   10:10
    filename: TimeCounter.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD_TIMELINE__TIMECOUNTER_H__
#define HEADER_GUARD_TIMELINE__TIMECOUNTER_H__

#include <OpenThreads/Mutex>
#include <OpenThreads/Thread>
#include <QtWidgets/QSlider>

class TimeCounter : public OpenThreads::Thread
{
public:
    TimeCounter(QSlider * slider = 0);

    ~TimeCounter();

    void pause();

    void resume();

    virtual void run();

    int getStep() const;

    void setStep(int step);

    bool isSuspended() const;

    QSlider * getSlider() const;

    void setSlider(QSlider * slider);

    void quit();

private:
    int step;

    unsigned int sleep; 

    volatile bool paused;

    OpenThreads::Mutex pauseTimer;
    OpenThreads::Mutex stateMutex;
    OpenThreads::Mutex sliderMutex;
    OpenThreads::Mutex quitMutex;

    QSlider * slider;
    volatile bool quitFlag;
};

#endif  //HEADER_GUARD_TIMELINE__TIMECOUNTER_H__
