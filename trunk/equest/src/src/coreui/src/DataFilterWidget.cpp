#include "CoreUiPCH.h"
#include <coreui/DataFilterWidget.h>
#include <coreui/FilterEntryWidget.h>
#include <QtGui/QCloseEvent>
#include <corelib/HierarchyItem.h>
#include "ui_filter.h"
//#include "FilterCommand.h"

using namespace PluginSubject;
using namespace coreUI;

const int PIXMAP_SIZE = 64;

DataFilterWidget::DataFilterWidget(const QString& name, const QIcon& icon) :
    active(false),
    filtersClosed(false),
    ui(new Ui::FilterWidget()),
    bundleFilter(new BundleFilter())
{
    ui->setupUi(this);
    setActive(false);
    ui->label->setText(name);
    ui->pictureLabel->setPixmap(icon.pixmap(QSize(PIXMAP_SIZE, PIXMAP_SIZE)));
    this->installEventFilter(this);
    ui->colorBox->installEventFilter(this);
}

bool DataFilterWidget::eventFilter(QObject *object, QEvent *event)
{
    if ( event->type() == QEvent::MouseButtonPress) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
        if (mouseEvent->button() == Qt::LeftButton) {
            onClick();
            return true;
        } else {
            return false;
        }
    }
    return false;
}

void DataFilterWidget::addFilter(FilterEntryWidget* entry) 
{
    UTILS_ASSERT(filtersClosed == false);
    entries.push_back(entry);
    connect(entry, SIGNAL(onFilterClicked(core::IFilterCommandPtr)), this, SLOT(onFilterEntryClicked(core::IFilterCommandPtr)));
    ui->verticalLayout->addWidget(entry);
}

void DataFilterWidget::addFilter( const QString& bigLabelText, core::IFilterCommandPtr command, const QIcon& icon)
{
    FilterEntryWidget* entry = new FilterEntryWidget(this, bigLabelText, command, icon);
    bundleFilter->addFilterCommand(command);
    addFilter(entry);
}

void DataFilterWidget::onClick()
{
    uncheckEntries();
    setActive(!getActive());
    Q_EMIT onFilterClicked(getActive() ? bundleFilter : core::IFilterCommandPtr());
}

void DataFilterWidget::mousePressEvent( QMouseEvent *e )
{
    if (e->button() == Qt::LeftButton) {
        onClick();
    } else {
        QWidget::mousePressEvent(e);
    }
}

void DataFilterWidget::setActive( bool val )
{
    setColor(val ? QColor(200, 200, 255) : Qt::white);
    active = val;
    Q_EMIT activityChanged(this); 
}

void DataFilterWidget::closeFilters()
{
    UTILS_ASSERT(filtersClosed == false && entries.empty() == false && entries.size() <= 3);
    filtersClosed = true;
    FilterEntryWidget* e = entries[0];
    QSpacerItem* item = new QSpacerItem(0, e->height() * (3 - entries.size()));
    ui->verticalLayout->addSpacerItem(item);
}

void DataFilterWidget::setColor( const QColor& color )
{
    QString style = QString("#colorBox { background: rgb(%1, %2, %3); }").arg(color.red()).arg(color.green()).arg(color.blue());
    ui->colorBox->setStyleSheet(style);
}

const FilterEntryWidget* DataFilterWidget::getEntry( int index ) const
{
    UTILS_ASSERT(index >= 0 && index < static_cast<int>(entries.size()));
    return entries[index];
}

void DataFilterWidget::uncheckEntries( FilterEntryWidget* toSkip)
{
    for (auto it = entries.begin(); it != entries.end(); ++it) {
        if (toSkip != *it) {
            (*it)->blockSignals(true);
            (*it)->setChecked(false);
            (*it)->blockSignals(false);
        }
    }
}

void DataFilterWidget::resetFilters()
{
    blockSignals(true);
    setActive(false);
    uncheckEntries();
    for (auto it = entries.begin(); it != entries.end(); ++it) {
        (*it)->getFilterCommand()->reset();
    }
    blockSignals(false);
}



const QPixmap& DataFilterWidget::getPixmap() const
{
    return *ui->pictureLabel->pixmap();
}

QString DataFilterWidget::getName() const
{
    return ui->label->text();
}

int DataFilterWidget::getNumEntries() const
{
    return entries.size();
}

core::IHierarchyItemPtr coreUI::BundleFilter::getFilteredTree( core::IHierarchyItemConstPtr root )
{
    core::HierarchyItemPtr results = utils::make_shared<core::HierarchyItem>(QObject::tr("Filtered"), "");
    for (auto it = commands.begin(); it != commands.end(); ++it) {
        results->appendChild((*it)->getFilteredTree(root));
    }
    return results;
}

QString coreUI::BundleFilter::getName() const
{
    return QObject::tr("Filter bundle");
}

QIcon coreUI::BundleFilter::getIcon() const
{
    return QIcon();
}

void coreUI::BundleFilter::addFilterCommand( core::IFilterCommandPtr command )
{
    commands.push_back(command);
}

void coreUI::DataFilterWidget::onFilterEntryClicked( core::IFilterCommandPtr filter )
{
    for (auto it = entries.begin(); it != entries.end(); ++it) {
        auto filterWidget = *it;
        if (filterWidget->getFilterCommand() == filter) {
            uncheckEntries(filterWidget);
            if (filterWidget->isChecked()) {
                Q_EMIT onFilterClicked(filter);
            } else {
                Q_EMIT onFilterClicked(core::IFilterCommandPtr());
            }

        }
    }
}
