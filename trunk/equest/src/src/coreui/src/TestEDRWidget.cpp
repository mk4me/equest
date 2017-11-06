#include "TestEDRWidget.h"
#include <utils/Debug.h>

TestEDRWidget::TestEDRWidget(EDRTitleBar * titleBar, QWidget * parent /* = nullptr */)
    : QWidget(parent), titleBar(titleBar), currentSide(EDRTitleBar::Left), toDelete(nullptr)
{
    UTILS_ASSERT((titleBar != nullptr), "Zly toolbar!");
    setupUi(this);

    currentList = leftObjectsList;

    updateTitleBarHeight();
}

TestEDRWidget::~TestEDRWidget()
{

}

void TestEDRWidget::updateTitleBarHeight()
{
    titlebarHeight->setText(QString::number(titleBar->height()));
}

void TestEDRWidget::addAction()
{
    static int i = 0;

    QString nText = "Test Action nr: " + QString::number(i);
    QAction * newAction = new QAction(nText, nullptr);
    connect(newAction, SIGNAL(triggered(bool)), this, SLOT(showMessageBox()));

    titleBar->addObject(newAction, currentSide);
    currentList->addItem(new QListWidgetItem(nText));
    namedObjects[nText] = newAction;

    i++;

    updateTitleBarHeight();
}

void TestEDRWidget::addMenu()
{
    static int i = 0;

    QString nText = "Test Menu nr: " + QString::number(i);
    QMenu * newMenu = new QMenu(nText);
    
    for(int j = 0; j < 5; j++){
        QString nText = "Test Menu Action nr: " + QString::number(j);
        QAction * newAction = new QAction(nText, nullptr);
        connect(newAction, SIGNAL(triggered(bool)), this, SLOT(showMessageBox()));
        newMenu->addAction(newAction);
    } 

    titleBar->addObject(newMenu, currentSide);
    currentList->addItem(new QListWidgetItem(nText));
    namedObjects[nText] = newMenu;

    i++;

    updateTitleBarHeight();
}

void TestEDRWidget::addWidget()
{
    static int i = 0;

    QString nText = "Test Widget (Label) nr: " + QString::number(i);
    QLabel * newLabel = new QLabel(nText);

    titleBar->addObject(newLabel, currentSide);
    currentList->addItem(new QListWidgetItem(nText));
    namedObjects[nText] = newLabel;

    i++;

    updateTitleBarHeight();
}

void TestEDRWidget::showContextMenu(const QPoint & point)
{
    QListWidget * list = qobject_cast<QListWidget*>(sender());

    if(list == nullptr){
        return;
    }

    toDelete = list->itemAt(point);

    if(toDelete == nullptr){
        return;
    }
    
    QList<QAction *> actions;

    QAction * newAction = new QAction(toDelete->text(), nullptr);

    connect(newAction, SIGNAL(triggered(bool)), this, SLOT(deleteObject()));

    actions.append(newAction);

    if (actions.count() > 0){
        QMenu::exec(actions, list->mapToGlobal(point));
    }
}

void TestEDRWidget::deleteObject()
{
    if(toDelete == nullptr){
        return;
    }

    titleBar->removeObject(namedObjects[toDelete->text()]);
    delete currentList->takeItem(currentList->row(toDelete));
    toDelete = nullptr;
}

void TestEDRWidget::showMessageBox()
{
    static QMessageBox msgBox;
    msgBox.setWindowTitle("Test MessageBox called by " + qobject_cast<QAction*>(sender())->text() );
    msgBox.setText("Test is passed if in title You can see name of the control that You have pressed");
    msgBox.exec();
}

void TestEDRWidget::changeSide(bool checked)
{
    if(checked == true){
        currentSide = EDRTitleBar::Left;
        currentList = leftObjectsList;
    }else{
        currentSide = EDRTitleBar::Right;
        currentList = rightObjectsList;
    }
}

void TestEDRWidget::clearSide()
{
    titleBar->clearSide(currentSide);

    //clear sideObjectList and map!!
    for(int i = currentList->count() - 1; i > -1; i--){
        QListWidgetItem * item = currentList->takeItem(i);
        namedObjects.erase(item->text());
        delete item;
    }
}
