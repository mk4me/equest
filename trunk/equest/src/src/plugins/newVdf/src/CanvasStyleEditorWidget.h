/********************************************************************
    created:  2012/11/19
    created:  19:11:2012   10:04
    filename: CanvasStyleEditorWidget.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___CANVASSTYLEEDITORWIDGET_H__
#define HEADER_GUARD___CANVASSTYLEEDITORWIDGET_H__

#include <QtWidgets/QWidget>
#include "ui_canvasStyleEditorWidget.h"
#include "CanvasStyleEditor.h"

namespace vdf {

class INodesFactory;
class IPinsFactory;
class IConnectionsFactory;


class CanvasStyleEditorWidget : public QWidget, private Ui::CanvasStyleEditor
{
	Q_OBJECT

public:

	explicit CanvasStyleEditorWidget(CanvasStyleEditorPtr editor, QWidget* parent = 0, Qt::WindowFlags f = 0);
	virtual ~CanvasStyleEditorWidget();

signals:
	void styleChanged();
	void nodesStyleChanged();
	void pinsStyleChanged();
	void connectionsStyleChanged();
	void backgroundsFactoryChanged();
	void styleAccepted();
	void styleCanceled();
	void livePreviewChanged(bool active);
	void backgroundAccepted(IBackgroundStrategyPtr);

private slots:

	void acceptPushed();
	void cancelPushed();

	void nodesFactoryChanged(int idx);
	void pinsFactoryChanged(int idx);
	void connectionsFactoryChanged(int idx);
	void backgroundsFactoryChanged(int idx);

	void nodesFactoryMakeDefault();
	void pinsFactoryMakeDefault();
	void connectionsFactoryMakeDefault();
	void selectionsStyleMakeDefault();

	void nodesFactoryConfigure();
	void pinsFactoryConfigure();
	void connectionsFactoryConfigure();
	void selectionsStyleConfigure();

	void previevChanged(bool active);

	void resetToDefaults();
	void backgroundApplyClicked();
private:
    CanvasStyleEditorPtr editor;
};
}

#endif	//	HEADER_GUARD___CANVASSTYLEEDITORWIDGET_H__
