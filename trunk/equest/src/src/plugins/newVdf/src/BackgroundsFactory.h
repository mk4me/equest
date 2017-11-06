/********************************************************************
	created:	2013/02/15
	created:	15:2:2013   0:28
	filename: 	BackgroundsFactory.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_NEWVDF__BACKGROUNDSFACTORY_H__
#define HEADER_GUARD_NEWVDF__BACKGROUNDSFACTORY_H__

#include <plugins/newVdf/IBackgroundsFactory.h>

namespace vdf {

template<class Strategy>
class BackgroundsFactory : public IBackgroundsFactory
{
public:
	BackgroundsFactory(QString name, QString desc, QUuid id) :
		_name(name),
		_desc(desc),
		_id(id)
	{}
	virtual ~BackgroundsFactory() {}

public:
	virtual IBackgroundStrategyPtr createBackgroundStrategy() { return IBackgroundStrategyPtr(new Strategy); }
	virtual const QUuid id() const { return _id; }
	virtual const QString name() const { return _name; }
	virtual const QString description() const { return _desc; }

private:
	QString _name;
	QString _desc;
	QUuid _id;
};
}

#endif
