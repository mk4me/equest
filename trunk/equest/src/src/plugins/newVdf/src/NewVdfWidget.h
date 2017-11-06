/********************************************************************
	created:	2012/12/13
	created:	13:12:2012   12:10
	filename: 	NewVdfWidget.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_NEWVDF__NEWVDFWIDGET_H__
#define HEADER_GUARD_NEWVDF__NEWVDFWIDGET_H__

#include <QtWidgets/QWidget>
#include <QtWidgets/QGraphicsView>
#include <QtGui/QWheelEvent>

#include "SceneModel.h"
#include <utils/CommandStack.h>
#include "Command.h"
#include <corelib/HierarchyItem.h>

class QGraphicsScene;
class QMouseEvent;

namespace coreUI { 
    class WheelGraphicsView; 
    class HierarchyTreeModel;
}


namespace vdf {

//class SimplePinItem;
class SceneStateMachine;
class VdfScene;  
//class VdfView;


class NewVdfWidget : public QWidget
{
    Q_OBJECT;

public:
    NewVdfWidget(utils::ICommandStackPtr stack, SceneModelPtr sceneModel, TypesModelPtr typesModel, coreUI::HierarchyTreeModel* treeModel);
    virtual ~NewVdfWidget();

public:
    SceneModelPtr getSceneModel() const { return sceneModel; }
	VdfScene* getScene() const { return scene; }  
	coreUI::WheelGraphicsView* getView() const { return view; }
	utils::ICommandStackPtr getCommandStack() const { return commandStack; }

public slots:
	void undo();
	void redo();
	void merge();
    void demerge();
    void saveScene();
	void deleteSelected();
	void clearScene();
    void runDF();

private slots:
    void onSinkGenerate(core::IHierarchyItemPtr item);

signals:
    void singleNodeSelected(IVisualNodePtr node);

private:
    VdfScene* scene;  
    coreUI::WheelGraphicsView* view;
    SceneModelPtr sceneModel;
    bool connectionMode;
	utils::ICommandStackPtr commandStack;
	utils::shared_ptr<SceneStateMachine> stateMachine;
    core::HierarchyItemPtr root;
    coreUI::HierarchyTreeModel* treeModel;
};

}
#endif
