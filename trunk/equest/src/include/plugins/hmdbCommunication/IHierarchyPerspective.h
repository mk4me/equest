/********************************************************************
	created:	2013/12/19
	created:	19:12:2013   18:39
	filename: 	IHierarchyPerspective.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_NEW_COMMUNICATION__IHIERARCHYPERSPECTIVE_H__
#define HEADER_GUARD_NEW_COMMUNICATION__IHIERARCHYPERSPECTIVE_H__

#include <corelib/IHierarchyItem.h>
#include <plugins/subject/ISubject.h>

namespace hmdbCommunication
{
    class IHierarchyPerspective
    {
    public:
	    virtual ~IHierarchyPerspective() {}
        virtual core::IHierarchyItemPtr getPerspective(PluginSubject::SubjectPtr subject) = 0;
    };
    
    DEFINE_SMART_POINTERS(IHierarchyPerspective);
}

#endif
