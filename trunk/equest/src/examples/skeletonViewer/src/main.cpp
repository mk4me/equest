#include "PCH.h"
#include <QtGui/QApplication>
#include <kinematiclib/KinematicModel.h>
#include "uniqueCollection.h"
#include "skeletalVisualizationScheme.h"
#include "skeletonViewerLogic.h"
#include "mainwindow.h"


using namespace kinematic;
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    SkeletonViewerLogicPtr logic(new SkeletonViewerLogic);
    
    SkeletalVisualizationSchemePtr visualization = SkeletalVisualizationScheme::create();
    logic->setVisualization(visualization);
    MainWindow w;
    w.setLogic(logic);
    w.show();

    return a.exec();
}
