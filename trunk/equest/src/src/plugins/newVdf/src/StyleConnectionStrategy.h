/********************************************************************
	created:	2013/02/01
	created:	1:2:2013   13:31
	filename: 	StyleConnection.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_NEWVDF__STYLECONNECTION_H__
#define HEADER_GUARD_NEWVDF__STYLECONNECTION_H__

#include <plugins/newVdf/IVisualConnection.h>

class StyleItem;
class StyleConnection : public QObject, public IVisualConnection
{
    Q_OBJECT;
public:
    StyleConnection();
    virtual ~StyleConnection() {}

    virtual QGraphicsItem * visualItem() const;

    virtual void setBegin( const IVisualPin* pin );
    virtual void setEnd( const IVisualPin* pin );
	virtual void setVisualStrategy( IVisualStrategy* strategy );

public slots:
    void update();

private:
    const IVisualPin* begin;
    const IVisualPin* end;
    StyleItem* item;
};


#endif
