#include "NewVdfPCH.h"
#include <CanvasStyleEditorWidget.h>

using namespace vdf;

CanvasStyleEditorWidget::CanvasStyleEditorWidget(CanvasStyleEditorPtr editor, QWidget* parent, Qt::WindowFlags f) :
    editor(editor),
    QWidget(parent, f)
{
	setupUi(this);
    auto nodes = editor->getNodesRange();
    for (auto it = nodes.begin(); it != nodes.end(); ++it) {
        nodeStyleSelectComboBox->addItem((*it)->name());
    }
    auto pins = editor->getPinsRange();
    for (auto it = pins.begin(); it != pins.end(); ++it) {
        pinStyleSelectComboBox->addItem((*it)->name());
    }
    auto connection = editor->getConnectionsRange();
    for (auto it = connection.begin(); it != connection.end(); ++it) {
        connectionStyleSelectComboBox->addItem((*it)->name());
    }

	auto backgrounds = editor->getBackgroundsRange();
	for (auto it = backgrounds.begin(); it != backgrounds.end(); ++it) {
		canvasStyleComboBox->addItem((*it)->name());
	}

	nodeStyleSelectComboBox->setCurrentIndex(nodeStyleSelectComboBox->count() - 1);
	pinStyleSelectComboBox->setCurrentIndex(pinStyleSelectComboBox->count() - 1);
	connectionStyleSelectComboBox->setCurrentIndex(connectionStyleSelectComboBox->count() - 1);
	canvasStyleComboBox->setCurrentIndex(canvasStyleComboBox->count() - 1);
	
	connect(nodeStyleSelectComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(nodesFactoryChanged(int)));
	connect(pinStyleSelectComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(pinsFactoryChanged(int)));
	connect(connectionStyleSelectComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(connectionsFactoryChanged(int)));
	connect(canvasStyleComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(backgroundsFactoryChanged(int)));
	connect(applyCanvasButton, SIGNAL(clicked()), this, SLOT(backgroundApplyClicked()));
}

CanvasStyleEditorWidget::~CanvasStyleEditorWidget()
{

}


void CanvasStyleEditorWidget::resetToDefaults()
{

}

void CanvasStyleEditorWidget::previevChanged( bool active )
{

}

void CanvasStyleEditorWidget::selectionsStyleConfigure()
{

}

void CanvasStyleEditorWidget::connectionsFactoryConfigure()
{

}

void CanvasStyleEditorWidget::pinsFactoryConfigure()
{

}

void CanvasStyleEditorWidget::nodesFactoryConfigure()
{

}

void CanvasStyleEditorWidget::selectionsStyleMakeDefault()
{

}

void CanvasStyleEditorWidget::connectionsFactoryMakeDefault()
{

}

void CanvasStyleEditorWidget::pinsFactoryMakeDefault()
{

}

void CanvasStyleEditorWidget::nodesFactoryMakeDefault()
{

}

void CanvasStyleEditorWidget::nodesFactoryChanged( int idx )
{
	UTILS_ASSERT(idx >= 0 && idx < editor->getNumNodeFactories());
	auto factory = editor->getNodeFactory(idx);
	editor->setCurrentNodesFactory(factory);
}

void CanvasStyleEditorWidget::pinsFactoryChanged( int idx )
{
	UTILS_ASSERT(idx >= 0 && idx < editor->getNumPinFactories());
	auto factory = editor->getPinFactory(idx);
	editor->setCurrentPinsFactory(factory);
}

void CanvasStyleEditorWidget::connectionsFactoryChanged( int idx )
{
	UTILS_ASSERT(idx >= 0 && idx < editor->getNumConnectionFactories());
	auto factory = editor->getConnectionFactory(idx);
	editor->setCurrentConnectionsFactory(factory);
}

void CanvasStyleEditorWidget::backgroundsFactoryChanged( int idx )
{
	UTILS_ASSERT(idx >= 0 && idx < editor->getNumBackgroundFactories());
	auto background = editor->getBackgroundFactory(idx);
	editor->setCurrentBackgroundsFactory(background);
}

void CanvasStyleEditorWidget::cancelPushed()
{

}

void CanvasStyleEditorWidget::acceptPushed()
{

}

void CanvasStyleEditorWidget::backgroundApplyClicked()
{
	auto strategy = editor->getCurrentBackgroundsFactory()->createBackgroundStrategy();
	emit backgroundAccepted(strategy);
}
