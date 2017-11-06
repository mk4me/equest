/********************************************************************
    created:  2011/11/02
    created:  2:11:2011   12:57
    filename: Types.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD_SUBJECT__TYPES_H__
#define HEADER_GUARD_SUBJECT__TYPES_H__

#include <utils/SmartPtr.h>
#include <vector>

namespace PluginSubject {

class ISubjectService;

typedef unsigned int SubjectID;

class ISubject;
typedef utils::shared_ptr<ISubject> SubjectPtr;
typedef utils::shared_ptr<const ISubject> SubjectConstPtr;

class ISession;
typedef utils::shared_ptr<ISession> SessionPtr;
typedef utils::shared_ptr<const ISession> SessionConstPtr;
typedef std::vector<SessionConstPtr> Sessions;

class IMotion;
typedef utils::shared_ptr<IMotion> MotionPtr;
typedef utils::shared_ptr<const IMotion> MotionConstPtr;
typedef std::vector<MotionConstPtr> Motions;

}

#endif //HEADER_GUARD_SUBJECT__TYPES_H__
