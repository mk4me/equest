/********************************************************************
	created:	2012/12/20
	created:	20:12:2012   13:57
	filename: 	SimpleConnection.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_NEWVDF__SIMPLECONNECTION_H__
#define HEADER_GUARD_NEWVDF__SIMPLECONNECTION_H__

#include <plugins/newVdf/IVisualConnection.h>

namespace vdf {

class SimpleConnection : public QObject, public IVisualConnection
{
    Q_OBJECT;
public:
    SimpleConnection();
    virtual ~SimpleConnection() {}

    virtual QGraphicsItem * visualItem() const;

    virtual void setInputPin( IVisualInputPinPtr pin );
	virtual void setOutputPin( IVisualOutputPinPtr pin );

	virtual IVisualInputPinPtr getInputPin();
	virtual IVisualOutputPinPtr getOutputPin();

	virtual void setVisualStrategy(IVisualStrategyPtr strategy);

    virtual df::IConnection* getModelConnection() {  UTILS_ASSERT(_connection); return _connection.get(); }
    virtual const df::IConnection* getModelConnection() const { UTILS_ASSERT(_connection); return _connection.get(); }
    virtual void setModelConnection(df::IConnection* node) { _connection.reset(node); }

    virtual IVisualItem::Type getType() const { return IVisualItem::Connection; }
    virtual bool isType(IVisualItem::Type t) const { return t == IVisualItem::Connection; }
    
public slots:
    void update();
    virtual bool isSelected() const;
    virtual void setSelected( bool val );

private:
    IVisualInputPinPtr inputPin;
    IVisualOutputPinPtr outputPin;
    QGraphicsLineItem* item;
    utils::shared_ptr<df::IConnection> _connection;
};

}

#endif
