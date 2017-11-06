#include "StdAfx.h"
#include <vdfmlib/osgVDFPin.h>

namespace osgVDF{

osgVDFPin::osgVDFPin(const std::string& name)
    : osgVDFBasePin(name)
{
    setMinimumSize(13.0f, 12.0f);
    setSize(13.0f, 12.0f);
}


osgVDFPin::osgVDFPin (const osgVDFPin& pin, const osg::CopyOp& co)
    : osgVDFBasePin(pin, co)
{

}


osgVDFPin::~osgVDFPin(void)
{

}

void osgVDFPin::graphSetStatus(osgVDFBasePin::VisualStatus pinVisualStatus)
{
    std::string status;
    std::string pinType;

    switch(this->getModelPin()->getType()){
    case dflm::Pin::IN:
        pinType = "in";
        break;

    case dflm::Pin::OUT:
        pinType = "out";
        break;
    }

    switch(pinVisualStatus){
    case osgVDFBasePin::ACTIVE:
        status = "active";
        break;

    case osgVDFBasePin::CONNECTED:
        status = "connected";
        break;

    case osgVDFBasePin::INCOMPATIBLE:
        status = "incompatible";
        break;

    case osgVDFBasePin::INCOMPLETE:
        status = "incomplete";
        break;

    case osgVDFBasePin::OK:
        status = "ok";
        break;

    case osgVDFBasePin::REQUIRED:
        status = "required";
        break;
    }

    this->setStyle("vdf.pin." + pinType + "." + status);

    osgWidget::WindowManager* wm = this->getWindowManager();
    if(wm != 0){
        osgWidget::StyleManager* sm = wm->getStyleManager();
        if(sm != 0){
            sm->applyStyles(this);
        }
    }
}

}
