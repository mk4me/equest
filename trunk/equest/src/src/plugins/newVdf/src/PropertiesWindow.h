/********************************************************************
	created:	2013/04/29
	created:	29:4:2013   13:15
	filename: 	PropertiesWindow.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_NEW_VDF__PROPERTIESWINDOW_H__
#define HEADER_GUARD_NEW_VDF__PROPERTIESWINDOW_H__

#include <QtWidgets/QWidget>
#include <utils/ObserverPattern.h>
#include <utils/CommandStack.h>
#include "Command.h"
#include "ui_PropertiesWindow.h"
#include "SceneModel.h"

namespace vdf {

class NewVdfWidget;


class PropertiesWindow : public QWidget, private Ui::PropertiesWindow
{
    Q_OBJECT;
public:
    explicit PropertiesWindow(utils::ICommandStackPtr stack, QWidget* parent = 0, Qt::WindowFlags f = 0);
	virtual ~PropertiesWindow() {}

public slots:
    void onNodeSelected(IVisualNodePtr node);
    
private:
    utils::ICommandStackPtr commmandStack;
};

}

#endif
