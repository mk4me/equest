#include "NewChartPCH.h"
#include "StatsTable.h"
#include <dataaccessorlib/DescriptorFeature.h>
#include <dataaccessorlib/Statistics.h>
#include <dataaccessorlib/BoundedArgumentsFeature.h>
#include <dataaccessorlib/BoundedValuesFeature.h>

StatsTable::StatsTable( QWidget* parent /*= nullptr*/, Qt::WindowFlags f /*= 0*/ ) :
    QWidget(parent, f),
    rowHeight(13),
    treeItemDelegate(100, rowHeight)
{
    setupUi(this);
}

 int getTreeItemRows(const QTreeWidgetItem* item) 
 {
     int sum = 0;
     
     if (item->isExpanded()) {
         int count = item->childCount();
         for (int i = 0; i < count; ++i){
             sum += getTreeItemRows(item->child(i));
         }
     }

     ++sum;
     return sum;
 }

 //from
 //to
 //minValue
 //maxValue
 //minArgument
 //maxArgument
 //meanValue
 //stdDev
 //variance

 QTreeWidgetItem* StatsTable::addEntry(const QString& group, const QString& name, c3dlib::ScalarChannelReaderInterfaceConstPtr channel, const QColor& backgroundColor)
 {
     if (group.isEmpty()) {
         throw loglib::runtime_error("StatsTable: Passed empty group");
     }

     // zapelnienie elementu statystykami
     QTreeWidgetItem* item = new QTreeWidgetItem();
	 QString timeUnit = tr("Unknown time unit");
	 QString valueUnit = tr("Unknown time unit");

	 auto df = channel->feature<dataaccessor::DescriptorFeature>();

	 if (df != nullptr){
		 timeUnit = QString::fromStdString(df->argumentUnit());
		 valueUnit = QString::fromStdString(df->valueUnit());
	 }

	 auto stats = channel->template getOrCreateFeature<dataaccessor::StatisticsFeature>();
	 auto baf = channel->template getOrCreateFeature<dataaccessor::BoundedArgumentsFeature>();
	 auto bvf = channel->template getOrCreateFeature<dataaccessor::BoundedValuesFeature>();

     int i = -1;
     item->setText(++i, name);
	 item->setText(++i, QString("%1 - %2 (%3)").arg(baf->minArgument()).arg(baf->maxArgument()).arg(timeUnit));
	 item->setText(++i, QString("%1 (%2)").arg(bvf->minValue()).arg(valueUnit));
	 //TODO
     item->setText(++i, QString("%1 (%2)").arg(0).arg(timeUnit));
	 item->setText(++i, QString("%1 (%2)").arg(bvf->maxValue()).arg(valueUnit));
	 //TODO
     item->setText(++i, QString("%1 (%2)").arg(0).arg(timeUnit));
	 item->setText(++i, QString("%1 (%2)").arg(stats->values().mean).arg(valueUnit));
	 item->setText(++i, QString("%1 (%2)").arg(stats->values().standardDeviation).arg(valueUnit));
	 item->setText(++i, QString("%1 (%2^2)").arg(stats->values().variance).arg(valueUnit));

     for (int i = 1; i < item->columnCount(); ++i) {
        item->setBackgroundColor(i, backgroundColor);
     }
    
	 stats2TreeItems.insert(std::make_pair(channel, item));
     for (int i = 0; i < table->topLevelItemCount(); ++i) {
         QTreeWidgetItem* groupItem = table->topLevelItem(i);
         if (groupItem->text(0) == group) {
             groupItem->addChild(item);
             return item;  // <<------------------------ return !!
         }
     }

     // nie znaleziono odpowiedniej grupy, tworzenie nowej
     QTreeWidgetItem* groupItem = new QTreeWidgetItem();
     groupItem->setText(0, group);
     groupItem->addChild(item);
     table->addTopLevelItem(groupItem);
     groupItem->setExpanded(true);
     return item;
 }

 void StatsTable::setActive( const QString& group, const QString& name )
 {
     QTreeWidgetItem* item = tryGetEntry(group, name);
     if (item) {
         setActive(item);
     } else {
         throw loglib::runtime_error("Unable to find statistic item");
     }
 }

 void StatsTable::setActive( QTreeWidgetItem* treeItem )
 {
     QTreeWidgetItem* groupItem = treeItem->parent();
     if (groupItem) {
         int icount = groupItem->childCount(); 
         for (int i = 0; i < icount; ++i) {
             QTreeWidgetItem* child = groupItem->child(i);
             child->setHidden(child == treeItem);
         }
     }
 }

 QTreeWidgetItem* StatsTable::tryGetEntry( const QString& group, const QString& name )
 {
     int icount = table->topLevelItemCount();
     for (int i = 0; i < icount; ++i) {
         QTreeWidgetItem* groupItem = table->topLevelItem(i);
         if (groupItem->text(0) == group) {
             int jcount = groupItem->childCount();
             for (int j = 0; j < jcount; ++j) {
                 QTreeWidgetItem* item = groupItem->child(j);
                 if (item->text(0) == name) {
                    return item;  // <<------------------------ return !!
                 }
             }
         }
     }

     return nullptr;
 }

 void StatsTable::clear()
 {
     stats2TreeItems.clear();
     table->clear();
 }

 void StatsTable::recalculateHeight()
 {
     table->resizeColumnToContents(0);
     int height = 0;
     for (int i = 0; i < table->topLevelItemCount(); ++i) {
         height += getTreeItemRows(table->topLevelItem(i));
     }
     //table->setMaximumHeight(height * rowHeight + table->header()->height() + 8);
     label->height();
 }

QStringList StatsTable::getGroups() const
 {
     QStringList list;
     for(int i = 0; i < table->topLevelItemCount(); ++i) {
        list.push_back(table->topLevelItem(i)->text(0));
     }
     return list;
 }

StatsTable::range StatsTable::getEntries(c3dlib::ScalarChannelReaderInterfaceConstPtr channel)
{
	auto r = stats2TreeItems.equal_range(channel);
    return boost::make_iterator_range(r.first, r.second);
}
