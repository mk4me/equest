/********************************************************************
    created:  2011/05/18
    created:  18:5:2011   12:25
    filename: osgVDFPin.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD_VDF__OSGVDFPIN_H__
#define HEADER_GUARD_VDF__OSGVDFPIN_H__

#include <vdfmlib/osgVDFBasePin.h>
#include <osgDB/ReadFile>
#include <osgui/macroHacks.h>

////////////////////////////////////////////////////////////////////////////////
namespace osgVDF{
////////////////////////////////////////////////////////////////////////////////

//! Klasa wizualnie preprezentująca pin. Przechowuje referencję do pinu który reprezentuje oraz do wizualnego modelu w kontekście którego występuje

class osgVDFPin : public osgVDFBasePin
{
public:
	META_Widget(osgWidget::Widget, osgVDFPin);

	osgVDFPin (const std::string& name = "");
    osgVDFPin (const osgVDFPin& pin, const osg::CopyOp& co);

	~osgVDFPin(void);

protected:
	virtual void graphSetStatus(VisualStatus pinVisualStatus);
};

}

#endif  //  HEADER_GUARD_VDF__OSGVDFPIN_H__
