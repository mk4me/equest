#include "NewChartPCH.h"
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QPushButton>
#include "NewChartLegendItem.h"


NewChartLegendItem::NewChartLegendItem( const QwtLegendData & data, QWidget* parent /*= nullptr*/ ) :
QWidget(parent)
{
    QHBoxLayout* l = new QHBoxLayout();
    l->setContentsMargins(0, 0, 0, 0);
    check = new QCheckBox();
    check->setCheckable(true);
    check->setChecked(true);
    l->addWidget(check);

    button = new QPushButton();

    button->setStyleSheet(
        "QPushButton:!checked {                    "
        "    border-style: none;                   "
        "}                                         "
        " QPushButton:checked {                    "
        "     border-style: solid;                 "
        "     border-width: 1px;                   "
        "     border-radius: 4px;                  "
        "     border-color: rgb(91, 91, 91);       "
        "}                                         ");
    setData(data);
    button->setCheckable(true);
    l->addWidget(button);
    this->setLayout(l);

    connect(button, SIGNAL(clicked(bool)), this, SLOT(onClick(bool)));
    connect(check, SIGNAL(clicked(bool)), this, SLOT(onClick(bool)));
}

bool NewChartLegendItem::isItemVisible()
{
    return check->isChecked();
}

void NewChartLegendItem::setItemVisible(bool active)
{
    check->setChecked(active);
}

bool NewChartLegendItem::isItemActive() const
{
    return button->isChecked();
}

void NewChartLegendItem::setItemActive( bool checked )
{
    button->setChecked(checked);
    check->setEnabled(!checked);
}

void NewChartLegendItem::onClick( bool val )
{
    QPushButton* b = qobject_cast<QPushButton*>(sender());
    if (b) {
        emit buttonClicked(val);
        return;
    } 

    QCheckBox* c = qobject_cast<QCheckBox*>(sender());
    if (c) {
        emit checkClicked(val);
        return;
    }

    UTILS_ASSERT(false);
}

void NewChartLegendItem::setItemVisibleEnabled( bool enabled )
{
    check->setEnabled(enabled);
}

void NewChartLegendItem::setItemActiveEnabled( bool enabled )
{
    button->setEnabled(enabled);
}

void NewChartLegendItem::setTooltip( const QString& val )
{
    UTILS_ASSERT(button);
    button->setToolTip(val);
}

void NewChartLegendItem::setData( const QwtLegendData& data )
{
    button->setIcon(QIcon(data.icon().toPixmap()));
	QString title = data.title().text();
    button->setText(title);
}
