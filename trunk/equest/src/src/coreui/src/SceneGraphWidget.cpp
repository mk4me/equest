#include "CorePCH.h"
#include "SceneGraphWidget.h"
#include <core/OsgSceneDump.h>
#include <core/StringTools.h>

////////////////////////////////////////////////////////////////////////////////

Q_DECLARE_METATYPE(Visualizer*);

SceneGraphWidget::SceneGraphWidget(void):
Ui::SceneGraphWidget(), 
QWidget()
{
    setupUi(this); 
}


void SceneGraphWidget::refreshButtonClicked()
{
    QComboBox* comboVisualizer = this->comboVisualizer;
    if ( comboVisualizer->count() ) {
        setCurrentVisualizer(comboVisualizer->currentIndex());
    }
}

void SceneGraphWidget::setSceneGraph(osg::Node* root)
{
    clearTreeWidget();
    if ( root ) {
        treeSceneGraph->setEnabled(true);
        core::OsgSceneDumpQtTree populator(treeSceneGraph);
        populator.setTraversalMode( osg::NodeVisitor::TRAVERSE_ALL_CHILDREN );
        root->accept(populator);
    } else {
        treeSceneGraph->setEnabled(false);
        new QTreeWidgetItem(treeSceneGraph, QStringList()<<""<<"Scene is empty"<<"");
    }
}

void SceneGraphWidget::clearTreeWidget()
{
    treeSceneGraph->clear();
}

void SceneGraphWidget::setCurrentVisualizer( int index )
{
    if ( index >= 0 ) {
        Visualizer* visualizer = comboVisualizer->itemData(index).value<Visualizer*>();
        QWidget* widget = visualizer->getWidget();
        auto view = dynamic_cast<osgViewer::View*>(widget);
        if ( view ) {
            setSceneGraph(view->getSceneData());
        } else {
            setSceneGraph(nullptr);
        }
    } else {
        setSceneGraph(nullptr);
    }
}

void SceneGraphWidget::addVisualizer( Visualizer* visualizer )
{
    // ustalenie parametrów...
    QString name = core::toQString(visualizer->getName());
    QVariant userData = qVariantFromValue(visualizer);

    // dodanie do komba
    comboVisualizer->addItem(name, userData);
}

void SceneGraphWidget::removeVisualizer( Visualizer* visualizer )
{
    for ( int i = 0; i < comboVisualizer->count(); ++i ) {
        Visualizer* weak = comboVisualizer->itemData(i).value<Visualizer*>();
        if ( weak == visualizer ) {
            comboVisualizer->removeItem(i);
            return;
        }
    }
    UTILS_FAIL("Nie powinien tutaj wejśc.");
}
