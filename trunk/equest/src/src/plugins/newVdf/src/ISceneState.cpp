#include "NewVdfPCH.h"
#include "ISceneState.h"
#include "SimpleItem.h"
#include <QtWidgets/QGraphicsScene>
#include <QtWidgets/QGraphicsView>
#include <QtWidgets/QHBoxLayout>
#include "SceneStateMachine.h"
#include "VdfScene.h"

using namespace vdf;

void changeCursor(SceneStateMachine* stateMachine, Qt::CursorShape cursor) 
{
	auto views = stateMachine->getScene()->views();
	for (auto it = views.begin(); it != views.end(); ++it) {
		(*it)->setCursor(cursor);
	}
}

