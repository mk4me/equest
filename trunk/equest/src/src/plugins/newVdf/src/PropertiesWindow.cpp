//#include "newVdfPCH.h"
#include "PropertiesWindow.h"
#include <plugins/newVdf/INodeConfiguration.h>
#include <corelib/PluginCommon.h>

using namespace vdf;

PropertiesWindow::PropertiesWindow(utils::ICommandStackPtr stack, QWidget* parent, Qt::WindowFlags f) :
    QWidget(parent, f),
	commmandStack(stack)
{
    setupUi(this);
}

void PropertiesWindow::onNodeSelected( IVisualNodePtr node )
{
    vdf::INodeConfiguration* conf = nullptr;
    if (node) {
        auto modelNode = node->getModelNode();
        conf = dynamic_cast<INodeConfiguration*>(modelNode);
    }
    
    auto list = children();
    for (auto it = list.begin(); it != list.end(); ++it) {
        if ((*it)->isWidgetType()) {
            auto widget = qobject_cast<QWidget*>(*it);
            widget->setVisible(false);
        }
    }

    if (conf) {
        //conf->refreshConfiguration();
        auto widget = conf->getConfigurationWidget();
        
        if (widget->parent() == this) {
            widget->setVisible(true);
        } else {
            layout()->addWidget(widget);
        }
    } else {
        label->setVisible(true);
    }
    
}

