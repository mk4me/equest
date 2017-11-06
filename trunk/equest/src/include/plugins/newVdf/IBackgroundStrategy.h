/********************************************************************
	created:	2013/02/10
	created:	10:2:2013   20:18
	filename: 	IBackgroundStrategy.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_NEWVDF__IBACKGROUNDSTRATEGY_H__
#define HEADER_GUARD_NEWVDF__IBACKGROUNDSTRATEGY_H__

#include <utils/SmartPtr.h>
#include <QtGui/QPainter>

namespace vdf {

class IBackgroundStrategy
{
public:
	virtual ~IBackgroundStrategy() {}
	virtual void paint(QPainter*, const QRectF&) = 0;
};
DEFINE_SMART_POINTERS(IBackgroundStrategy);

}
#endif
