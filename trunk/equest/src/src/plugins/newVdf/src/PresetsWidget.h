/********************************************************************
	created:	2013/09/30
	created:	30:9:2013   16:48
	filename: 	PresetsWidget.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_NEW_VDF__PRESETSWIDGET_H__
#define HEADER_GUARD_NEW_VDF__PRESETSWIDGET_H__


#include <QtWidgets/QWidget>
#include "SceneModel.h"
#include "TypesWindow.h"

class QListWidget;
class QPushButton;

namespace vdf {

//! Widget odpowiedzialny za wyœwietlanie zapisanych schematów Vdf
class PresetsWidget : public QWidget
{
    Q_OBJECT
public:
    PresetsWidget(SceneModelPtr scene, TypesModelPtr typesModel);
	virtual ~PresetsWidget() {}

private slots:
    void load();
    void save();

private:
    QListWidget* listWidget;
    QPushButton* loadButton;
    QPushButton* saveButton;
    SceneModelWeakPtr model;
	TypesModelWeakPtr typesModel;
};

}
#endif
