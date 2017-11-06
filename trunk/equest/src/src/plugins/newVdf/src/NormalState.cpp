#include "NewVdfPCH.h"
#include "NormalState.h"
#include <QtWidgets/QInputDialog>
#include <coreui/MoveCommand.h>
#include <coreui/MultiCommand.h>
#include "SimpleItem.h"
#include "SceneStateMachine.h"
#include "VdfScene.h"
#include <QtWidgets/QGraphicsSceneEvent>

using namespace vdf;

void NormalState::selectionChanged(const QList<QGraphicsItem*>& list)
{
	auto nodes = stateMachine->getSceneModel()->getVisualItems<IVisualNodePtr>();
	for (auto it = nodes.begin(); it != nodes.end(); ++it) {
		IVisualNodePtr node = *it;
		node->setSelection(node->visualItem()->isSelected());
	}
    if (list.size() == 1) {
		IVisualItemPtr item = stateMachine->getSceneModel()->tryGetVisualItem(list.first());
        if (item && item->isType(IVisualItem::Pin)) {
            IVisualPinPtr pin = utils::dynamic_pointer_cast<IVisualPin>(item);
            ConnectStatePtr connectState = stateMachine->getConnectState();
            connectState->setFirstPin(pin);
            stateMachine->setState(stateMachine->getConnectState());
        }
    }
}

bool NormalState::mousePressEvent( QGraphicsSceneMouseEvent* e )
{
	if (e->button() == Qt::LeftButton) {
		if (!stateMachine->getScene()->items(e->scenePos()).isEmpty()) {
			positions.clear();
			auto nodes = stateMachine->getSceneModel()->getVisualItems<IVisualNodePtr>();
			for (auto it = nodes.begin(); it != nodes.end(); ++it) {
				positions[*it] = (*it)->visualItem()->scenePos();
			}
		} else {
			stateMachine->setState(stateMachine->getGroupState());
		}
	}

	// celowo zwracany jest false, nawet gdy obs³u¿yliœmy event (domyœlna obs³uga jest w tym przypadku porz¹dana)
	return false;
}

bool NormalState::mouseReleaseEvent( QGraphicsSceneMouseEvent* e )
{
	if (e->button() == Qt::LeftButton) {
		std::vector<utils::ICommandPtr> commands;
		for (auto it = positions.begin(); it != positions.end(); ++it) {
			QGraphicsItem* item = it->first->visualItem();
			QPointF oldP = it->second;
			QPointF newP = item->pos();
			if (newP != oldP) {
				commands.push_back(utils::ICommandPtr(new coreUI::MoveCommand(item, newP, oldP)));
			}
		}
		if (!commands.empty()) {
			stateMachine->getCommandStack()->addCommand(utils::ICommandPtr(new coreUI::MultiCommand(commands)));
		}
	}
	return false;
}

bool NormalState::keyReleaseEvent( QKeyEvent *event )
{
	if (event->key() == Qt::Key_Delete) {
		auto selected = stateMachine->getScene()->selectedItems();
		auto command = utils::ICommandPtr(new RemoveSelectedCommand(stateMachine->getSceneModel(), selected));
		stateMachine->getCommandStack()->addCommand(command);
		return true;
	} else if (event->key() == Qt::Key_F2) {
		auto selected = stateMachine->getScene()->selectedItems();
		if (selected.size() == 1) {
			auto item = stateMachine->getSceneModel()->tryGetVisualItem(*selected.begin());
			auto node = utils::dynamic_pointer_cast<IVisualNode>(item);
			if (node) {
				bool ok;
				QString text = QInputDialog::getText(nullptr, QObject::tr("Change node name"),
					QObject::tr("New name:"), QLineEdit::Normal,
					node->getName(), &ok);
				if (ok && !text.isEmpty()) {
					node->setName(text);
				}
			}
		}
	}

	return false;
}


void vdf::NormalState::end()
{
    positions.clear();
}
