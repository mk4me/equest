#include "TimeCounter.h"
#include <timelinelib/Model.h>
#include <OpenThreads/ScopedLock>
#include <utils/Debug.h>

TimeCounter::TimeCounter(QSlider * slider) : step(1), sleep(50000), paused(false), slider(slider), quitFlag(false)
{

}

TimeCounter::~TimeCounter()
{

}

void TimeCounter::pause()
{
    OpenThreads::ScopedLock<OpenThreads::Mutex> lock(stateMutex);

    if(paused == true){
        return;
    }

    paused = true;

    pauseTimer.lock();
}

void TimeCounter::resume()
{
    OpenThreads::ScopedLock<OpenThreads::Mutex> lock(stateMutex);

    if(paused == false){
        return;
    }

    paused = false;

    pauseTimer.unlock();
}

QSlider * TimeCounter::getSlider() const
{
    return slider;
}

void TimeCounter::setSlider(QSlider * slider)
{
    UTILS_ASSERT((slider != nullptr), "Nieprawidlowy slider dla timera");

    OpenThreads::ScopedLock<OpenThreads::Mutex> lock(sliderMutex);
    this->slider = slider;
}

void TimeCounter::quit()
{
    OpenThreads::ScopedLock<OpenThreads::Mutex> lock(quitMutex);
    quitFlag = true;
}

void TimeCounter::run()
{
    OpenThreads::ScopedLock<OpenThreads::Mutex> lock(sliderMutex);
    if(slider == nullptr){
        return;
    }

    slider->setValue(slider->minimum());
    if(slider->value() < slider->maximum()) {
        do{
            {
                OpenThreads::ScopedLock<OpenThreads::Mutex> lock(quitMutex);
                if(quitFlag == true){
                    return;
                }
            }
            
            microSleep(sleep);

            {
                OpenThreads::ScopedLock<OpenThreads::Mutex> lock(pauseTimer);
            }
            
            slider->setValue(std::min(std::max(slider->value() + step, slider->minimum()), slider->maximum()));
        }while(slider->value() < slider->maximum() && slider->value() > slider->minimum());
    }
}

int TimeCounter::getStep() const
{
    OpenThreads::ScopedLock<OpenThreads::Mutex> lock(const_cast<TimeCounter*>(this)->stateMutex);
    return step;
}

void TimeCounter::setStep(int step)
{
    UTILS_ASSERT((step != 0), "Zly krok dla timera");
    OpenThreads::ScopedLock<OpenThreads::Mutex> lock(stateMutex);
    this->step = step;
}

bool TimeCounter::isSuspended() const
{
    OpenThreads::ScopedLock<OpenThreads::Mutex> lock(const_cast<TimeCounter*>(this)->stateMutex);
    return paused;
}
