/********************************************************************
	created:	2013/10/02
	created:	2:10:2013   10:39
	filename: 	UniversalSink.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_NEWVDF__UNIVERSALSINK_H__
#define HEADER_GUARD_NEWVDF__UNIVERSALSINK_H__

#include <corelib/HierarchyHelper.h>
#include <corelib/HierarchyDataItem.h>
#include <dflib/IDFNode.h>
#include <dflib/Node.h>
#include <plugins/newVdf/INodeConfiguration.h>
#include <plugins/newVdf/UniversalInputPin.h>
#include <plugins/newVdf/Export.h>

class QLineEdit;

namespace vdf {

class PLUGIN_NEWVDF_EXPORT UniversalSink : public QObject, public df::SinkNode, public df::IDFSink, public vdf::INodeConfiguration
{
    Q_OBJECT;
public:
    UniversalSink();

    virtual void reset();
    virtual void consume();

    //void insertToTree( core::HierarchyHelperPtr channelHelper);//, utils::ObjectWrapperPtr wrp );

    virtual QWidget* getConfigurationWidget();
    // todo: ta metoda nie jest potrzebna, bo mamy sygnal
    // problem w tym ,ze sygnal nie dziala z jakiegos wzgledu
    //core::IHierarchyItemPtr getDataItem() const { return dataItem; }

private:
    void _UniversalSink();

signals:
    void itemConsumed(core::IHierarchyItemPtr dataItem);

private:
    UniversalInputPinBase * inPinA;
    QWidget* widget;
    QLineEdit* line;
    QString defaultName;
    int runNo;
    core::IHierarchyItemPtr dataItem;
};

}
#endif
