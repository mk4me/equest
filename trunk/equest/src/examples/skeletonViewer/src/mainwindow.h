#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <osg/Node>
#include <osg/Group>
#include <osg/Geode>
#include <osg/Geometry>
#include <osg/ref_ptr>
#include <kinematiclib/SkeletalParsers.h>
#include "skeletalVisualizationScheme.h"
#include "skeletonViewerLogic.h"
#include <QtGui/QMainWindow>
#include <QtGUi/QTreeWidgetItem>
#include <QtGui/QMessageBox>
#include "QOsgWidgets.h"

namespace Ui {
    class MainWindow;
}


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public:
    void setLogic(SkeletonViewerLogicPtr val);  
    QTreeWidgetItem* getTreeRootItem() const { return treeRootItem; }
    osg::ref_ptr<osg::Group> getRootNode() const { return rootNode; }
    QTimer* getTimer() const { return timer; }
    QLabel* getTimeLabel() const;
    QLabel* getFrameLabel() const;
    QSlider* getSlider() const;

 signals:
     void timeout();

private slots:
    void on_loadSkeletonButton_clicked();
    void on_playButton_clicked();
    void on_pauseButton_clicked();
    void on_stopButton_clicked();
    void on_timeSlider_sliderMoved(int position);
    void on_update();

private:
    void initViewer();
    void MessageBox(const QString& message, QMessageBox::Icon icon = QMessageBox::NoIcon);

    void refreshTime();
    GeodePtr createFloor();

private:
    Ui::MainWindow *ui;
    osg::ref_ptr<osg::Group> rootNode;
    core::QOsgDefaultWidget* viewer;
    SkeletonViewerLogicPtr logic;
    QTreeWidgetItem* treeRootItem;
    QTimer* timer;
};

#endif // MAINWINDOW_H
