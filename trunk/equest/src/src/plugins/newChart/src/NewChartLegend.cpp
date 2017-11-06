#include "NewChartPCH.h"
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QPushButton>
#include "NewChartLegend.h"
#include "NewChartLegendItem.h"

QWidget * NewChartLegend::createWidget( const QwtLegendData & data ) const
{
    NewChartLegendItem* item = new NewChartLegendItem(data);
    connect( item, SIGNAL( buttonClicked( bool ) ), this,  SLOT( itemChecked( bool ) ) );
    connect( item, SIGNAL( checkClicked(bool)), this, SLOT(onCheck(bool)));
    return item;
}

void NewChartLegend::onCheck(bool checked)
{
    QWidget* w = qobject_cast<QWidget*>(sender());
    UTILS_ASSERT(w);
    if (w) {
        QVariant info = itemInfo(w);
        emit checkboxChanged(info, checked);
    }
}

//void NewChartLegend::updateLegend( const QVariant &v, const QList<QwtLegendData> & d)
//{
//    QwtLegend::updateLegend(v, d);
//}

void NewChartLegend::updateWidget( QWidget* w, const QwtLegendData& data )
{
    NewChartLegendItem* item = qobject_cast<NewChartLegendItem*>(w);
    UTILS_ASSERT(item);
    if (item) {
        item->setData(data);
    }
}

