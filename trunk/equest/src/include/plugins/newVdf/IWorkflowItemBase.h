/********************************************************************
	created:	2011/05/12
	created:	12:5:2011   9:10
	filename: 	IWorkflowItemBase.h
	
	purpose:	
*********************************************************************/


#ifndef HEADER_GUARD_CORE__IWORKFLOWITEMBASE_H__
#define HEADER_GUARD_CORE__IWORKFLOWITEMBASE_H__

#include <QtGui/QIcon>
#include <utils/Debug.h>
#include <utils/SmartPtr.h>
#include <corelib/BaseDataTypes.h>

namespace df {
	class INode;
}

class QDialog;

namespace vdf {

    
//! Interfejs podstawowych elementów logicznych aplikacji mających operować na danych bądź dostarczać danych
class IWorkflowItemBase
{
public:
    virtual ~IWorkflowItemBase() {}

public:
    //! \return Nazwa elementu logicznego
    virtual const std::string getName() const = 0;
    virtual df::INode* getPrototype() const = 0;
	virtual df::INode* create() const = 0;
	virtual core::UniqueID getID() const = 0;
	
    virtual QIcon getIcon() const = 0;
    virtual void setIcon(const QIcon& val) = 0;
};

DEFINE_SMART_POINTERS(IWorkflowItemBase);
}
#endif
