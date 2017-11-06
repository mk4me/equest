/********************************************************************
    created:  2011/08/01
    created:  1:8:2011   13:31
    filename: State.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD_TIMELINE__STATE_H__
#define HEADER_GUARD_TIMELINE__STATE_H__

//! Stan modelu i timeline'a.
struct State 
{
public:

    State() : time(0), length(0), normalizedTime(0),
		timeScaleFactor(1), isPlaying(false), offset(0) {}
    ~State() {}

    //! Czas.
    double time;
    //! Długość strumienia. 
    double length;
    //! Znormalizowany czas.
    double normalizedTime;
    //! Jaka jest skala czasowa?
    double timeScaleFactor;
    //! Czy timeline jest odtwarzany
    bool isPlaying;
    //! Offset timeline
    double offset;
};

#endif  //  HEADER_GUARD_TIMELINE__STATE_H__
