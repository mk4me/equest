#include "CoreUiPCH.h"
#include <coreui/CoreTextEditWidget.h>
#include <coreui/CoreFlexiToolBarSection.h>
#include <coreui/CoreFlexiToolBar.h>
#include <QtWidgets/QToolButton>
#include <QtWidgets//QAction>
#include <coreui/ReportsTab.h>
#include <coreui/IAppUsageContextManager.h>

using namespace coreUI;

ReportsTab::ReportsTab( const QIcon& icon, const QString& label ) :
    AbstractTab(icon, label),
    widget(new coreUI::CoreTextEditWidget())
{
}


QWidget* ReportsTab::getMainWidget()
{
    return widget;
}

void ReportsTab::registerConxtext( coreUI::IAppUsageContextManager* manager, coreUI::IAppUsageContextPtr parent, QTabWidget * flexiTabWidget )
{
    context = ReportsTabContextPtr(new ReportsTabContext(flexiTabWidget));
    manager->addContext(context, parent);
    manager->addWidgetToContext(context, widget);
}

void coreUI::ReportsTab::enable()
{
    setEnabled(true);
    setActive(true);
}

ReportsTabContext::ReportsTabContext( QTabWidget * flexiTabWidget ) :
flexiTabWidget(flexiTabWidget),
    editSection(nullptr),
    groupID(-1),
    textSection(nullptr),
    fileSection(nullptr)
{

}


void ReportsTabContext::activateContext( QWidget * contextWidget )
{
    //nie wpsieramy kontekstu bez widgeta i nie ma sensu nic z kontekstem robic skoro jest ju¿ zaladowany
    if(contextWidget == nullptr || groupID != -1){
        return;
    }

    groupID = flexiTabWidget->addTab(widget, QObject::tr("Raports Tab"));
    widget->setVisible(true);
    flexiTabWidget->setCurrentIndex(groupID);
}

void ReportsTabContext::deactivateContext( QWidget * nextContextWidget, bool refresh )
{
    if(nextContextWidget == getCurrentContextWidget()){
        return;
    }

    if(groupID != -1){
        widget->setVisible(false);
        flexiTabWidget->removeTab(groupID);
        groupID = -1;
    }
}

void ReportsTabContext::onRegisterContextWidget( QWidget * contextWidget )
{
    auto textEdit = qobject_cast<coreUI::CoreTextEditWidget*>(contextWidget);

    auto twoLine = createTwoLineWidget();
    editSection = twoLine.get<0>();
    placeObjects(textEdit->getEditActions(), twoLine.get<1>(), twoLine.get<2>());

    twoLine = createTwoLineWidget();
    fileSection = twoLine.get<0>();
    placeObjects(textEdit->getFileActions(), twoLine.get<1>(), twoLine.get<2>());

    twoLine = createTwoLineWidget();
    textSection = twoLine.get<0>();
    placeObjects(textEdit->getTextActions(), twoLine.get<1>(), twoLine.get<2>(), true);

    widget = new coreUI::CoreFlexiToolBar(flexiTabWidget);

    auto section = new coreUI::CoreFlexiToolBarSection(tr("File"));
    section->setInnerWidget(fileSection);

    widget->addSection(section);

    section = new coreUI::CoreFlexiToolBarSection(tr("Edit"));
    section->setInnerWidget(editSection);

    widget->addSection(section);

    section = new coreUI::CoreFlexiToolBarSection(tr("Text"));
    section->setInnerWidget(textSection);

    widget->addSection(section);
    widget->setVisible(false);
}

void ReportsTabContext::onUnregisterContextWidget( QWidget * contextWidget )
{

}

void ReportsTabContext::placeObjects( const QList<QObject*> &editList, QLayout* lowerLayout, QLayout* upperLayout, bool actionsOnTop)
{
    int count = editList.size();
    for (int i = 0; i < count; ++i) {
        QAction* a = qobject_cast<QAction*>(editList[i]);
        if (a) {
            QToolButton* b = new QToolButton();
            b->setStyleSheet("background-color: rgba(0, 0, 0, 0%);");
            b->setBackgroundRole(QPalette::Window);
            b->setDefaultAction(a);
            b->setToolTip(a->text().remove("&"));
            b->setFixedSize(20,20);
            b->setIconSize(QSize(20, 20));
            if (actionsOnTop || i < count / 2) {
                upperLayout->addWidget(b);
            } else {
                lowerLayout->addWidget(b);
            }
        } else {
            QWidget* w = qobject_cast<QWidget*>(editList[i]);
            if (actionsOnTop || i >= count / 2) {
                lowerLayout->addWidget(w);
            } else {
                upperLayout->addWidget(w);
            }
        }

    }
}

boost::tuple<QWidget*, QLayout*, QLayout*> ReportsTabContext::createTwoLineWidget()
{
    QWidget* flexiSection = new QWidget();
    QVBoxLayout* l = new QVBoxLayout();
    l->setContentsMargins(0, 0, 0, 0);
    l->setSpacing(0);
    QWidget* upper = new QWidget();
    QLayout* upperLayout = new QHBoxLayout();
    upper->setLayout(upperLayout);
    QWidget* lower = new QWidget();
    QLayout* lowerLayout = new QHBoxLayout();
    lower->setLayout(lowerLayout);

    upperLayout->setContentsMargins(3, 3, 3, 3);
    lowerLayout->setContentsMargins(3, 3, 3, 3);
    l->addWidget(upper);
    l->addWidget(lower);
    flexiSection->setLayout(l);

    return boost::make_tuple(flexiSection, lowerLayout, upperLayout);
}