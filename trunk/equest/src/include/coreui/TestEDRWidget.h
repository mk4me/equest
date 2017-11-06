/********************************************************************
    created:  2011/04/29
    created:  29:4:2011   9:33
    filename: TestEDRWidget.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD_CORE__TESTEDRWIDGET_H__
#define HEADER_GUARD_CORE__TESTEDRWIDGET_H__

#include "ui_TestEDRWidget.h"
#include "EDRTitleBar.h"

class TestEDRWidget : public QWidget, private Ui::TestEDRWidget
{
    Q_OBJECT

public:
    TestEDRWidget(EDRTitleBar * titleBar, QWidget * parent = nullptr);
    ~TestEDRWidget();

    public slots:

        void addAction();
        void addMenu();
        void addWidget();
        void showContextMenu(const QPoint & point);
        void changeSide(bool checked);
        void clearSide();

        void deleteObject();

        void showMessageBox();

private:
    void updateTitleBarHeight();

private:
    typedef std::map<QString, QObject*> NAMED_OBJECTS;

private:
    NAMED_OBJECTS namedObjects;
    EDRTitleBar::SideType currentSide;
    QListWidget * currentList;
    EDRTitleBar * titleBar;
    QListWidgetItem * toDelete;


};


#endif  //  HEADER_GUARD_CORE__TESTEDRWIDGET_H__
