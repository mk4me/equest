#include "CoreUiPCH.h"
#include <coreui/FilterEntryWidget.h>
#include "ui_filterEntry.h"

FilterEntryWidget::FilterEntryWidget( QWidget* parent, const QString& bigLabelText, 
            core::IFilterCommandPtr filterCommand, const QIcon& icon /*= nullptr*/ ) : 
        QWidget(parent),
        filterCommand(filterCommand),
        ui(new Ui::FilterEntry()),
        configurator(nullptr)
{
    ui->setupUi(this);
    ui->pushButton->setText(bigLabelText);
    ui->pushButton->setAttribute(Qt::WA_NoSystemBackground, true);
    ui->pushButton->setIcon(icon);
    
    connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(onButton()));
}

FilterEntryWidget::~FilterEntryWidget()
{
    if (configurator) {
        delete configurator;
    }
}

void FilterEntryWidget::onButton()
{
    if (!configurator) {
        configurator = filterCommand->getConfigurationWidget();
    }
    //setChecked(!isChecked());
    Q_EMIT onFilterClicked(filterCommand);
}

//QTreeWidgetItem* FilterEntryWidget::createTreeEntry(const core::ConstObjectsList& sessions)
//{
//    QTreeWidgetItem* item = filterCommand->createTreeBranch(getName(), sessions);
//    item->setExpanded(true);
//    return item;
//}

QString FilterEntryWidget::getName() const
{
    return ui->pushButton->text();
}

bool FilterEntryWidget::isChecked() const
{
    return ui->pushButton->isChecked();
}

void FilterEntryWidget::setChecked( bool val )
{
    ui->pushButton->setChecked(val);
}

