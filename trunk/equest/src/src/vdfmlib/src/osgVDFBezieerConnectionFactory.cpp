#include "StdAfx.h"
#include <vdfmlib/osgVDFBezieerConnectionFactory.h>

namespace osgVDF {
    osgWidget::XYCoord osgVDFBezieerConnectionFactory::dirOut = osgWidget::XYCoord(1,0);
    osgWidget::XYCoord osgVDFBezieerConnectionFactory::dirIn = osgWidget::XYCoord(-1,0);
    unsigned int osgVDFBezieerConnectionFactory::interpCount = 100;
    float osgVDFBezieerConnectionFactory::coeffMul = 100;
    float osgVDFBezieerConnectionFactory::connectingWidth = 1.0;
    float osgVDFBezieerConnectionFactory::connectedWidth = osgVDFBezieerConnectionFactory::connectingWidth;
}
