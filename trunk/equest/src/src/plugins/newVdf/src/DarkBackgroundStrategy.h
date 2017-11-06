/********************************************************************
	created:	2013/02/10
	created:	10:2:2013   18:42
	filename: 	DarkBackgroundStrategy.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_NEWVDF__DARKBACKGROUNDSTRATEGY_H__
#define HEADER_GUARD_NEWVDF__DARKBACKGROUNDSTRATEGY_H__

#include <plugins/newVdf/IBackgroundStrategy.h>

namespace vdf {

class DarkBackgroundStrategy : public IBackgroundStrategy
{
public:
	virtual ~DarkBackgroundStrategy() {}

	virtual void paint( QPainter*, const QRectF& );

};
DEFINE_SMART_POINTERS(DarkBackgroundStrategy);

}

#endif
