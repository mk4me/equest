#ifndef TIMELINE_H
#define TIMELINE_H

#include <QtGui/QWidget>
#include <osg/AnimationPath>
#include <osgViewer/Scene>

#include "ui_TimeLine.h"

class TimeLine: public QWidget, Ui::TimeLine
{
    Q_OBJECT
public:
    TimeLine(void);
    ~TimeLine(void);

    void AnalizeNode(osg::Node* node); 
    void Clear(); 
    void TogglePlay(); 
    void SetPause(bool pause); 
    void AnimationTimeTest(); 
    void Update(double dt); 
    double GetCurrentAnimationTime(osg::AnimationPathCallback* apc); 
    void SetScene(osgViewer::Scene* scene); 
    void TimeLineSliderActionTriggered(int act); 
    void OnAnimationStop();

public slots: 
  virtual void PlayClicked2();
    virtual void PlayClicked();
    virtual void PauseClicked();
    virtual void StopClicked();

    virtual void Echo();
    virtual void TimeLineSliderMoved(int);
    virtual void TimeLineSliderPressed();
    virtual void TimeLineSliderReleased();

private: 
    std::vector<osg::AnimationPathCallback*> _animPathCallbacks; 
    bool _play; 
    float _animationTime; 

};

#endif
