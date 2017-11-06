/********************************************************************
	created:	2013/03/01
	created:	1:3:2013   8:55
	filename: 	INodeConfiguration.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_NEWVDF__INODECONFIGURATION_H__
#define HEADER_GUARD_NEWVDF__INODECONFIGURATION_H__

#include <QtCore/QString>
#include <corelib/IHierarchyItem.h>
#include <corelib/IDataHierarchyManager.h>

class QWidget;

namespace vdf {

    class INodeConfiguration
    {
    public:
	    virtual ~INodeConfiguration() {}
        virtual QWidget* getConfigurationWidget() = 0;
        //virtual void refreshConfiguration() = 0;
    };
    DEFINE_SMART_POINTERS(INodeConfiguration);

    class INodeValidation
    {
    public:
        virtual ~INodeValidation() {}
        virtual bool isNodeValid() = 0;
        virtual QString getErrorMessage() = 0;
    };
    DEFINE_SMART_POINTERS(INodeValidation);

    class INodeHierarchyObserver
    {
    public:
        virtual ~INodeHierarchyObserver() {}
        virtual void refresh(core::IDataHierarchyManager* dm, const core::IDataHierarchyManagerReader::ChangeList & changes) = 0;
    };
    DEFINE_SMART_POINTERS(INodeHierarchyObserver);
}

#endif
