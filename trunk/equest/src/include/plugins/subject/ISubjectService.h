/********************************************************************
    created:  2011/10/28
    created:  28:10:2011   11:39
    filename: ISubjectService.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___ISUBJECTSERVICE_H__
#define HEADER_GUARD___ISUBJECTSERVICE_H__

#include <plugins/subject/Types.h>
#include <corelib/Variant.h>

namespace PluginSubject {

class ISubjectService
{
    friend class IDataFilter;

public:
    virtual ~ISubjectService() {}

    virtual core::VariantPtr createSubject() = 0;

    virtual core::VariantPtr createSession(const core::VariantConstPtr & subject) = 0;

    virtual core::VariantPtr createMotion(const core::VariantConstPtr & session) = 0;
};

}

#endif //   HEADER_GUARD___ISUBJECTSERVICE_H__

