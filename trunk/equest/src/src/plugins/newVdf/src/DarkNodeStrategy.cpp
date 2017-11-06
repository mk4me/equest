#include "DarkNodeStrategy.h"
#include <plugins/newVdf/IVisualNode.h>
#include <plugins/newVdf/IVisualSourceNode.h>
#include <plugins/newVdf/IVisualSinkNode.h>

using namespace vdf;

void DarkNodeStrategy::setNode( IVisualNodeWeakPtr item )
{
	node = item;
    lowerFrame->setAlignment(Qt::AlignCenter);
}

DarkNodeStrategy::DarkNodeStrategy() : 
	labelHeight(24),
    invalid(false),
    selected(false)
{
	label = new QLabel();
	label->setStyleSheet(
		"QLabel{									  "
		"border: 2px solid rgb(64, 64, 64);		      "
		"background-color: QLinearGradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0  rgb(0, 0, 0), stop: 1  rgb(77, 77, 77));"
		"border-radius: 4px;						  "
		"border-bottom-right-radius: 0px; 			  "
		"border-bottom-left-radius: 0px;			  "
		"padding: 0px;								  "
		"color: rgb(250, 250, 250);					  "
		"}											  "
		);

	lowerFrame = new QLabel();
	label->setBuddy(lowerFrame);
	lowerFrame->setStyleSheet(
		"QFrame{								"
		"border: 2px solid rgb(64, 64, 64);		"
		"background-color: rgb(104, 104, 104);	"
		"border-radius: 4px;					"
		"border-top-right-radius: 0px; 			"
		"border-top-left-radius: 0px;			"
		"border-bottom-right-radius: 0px;		"
		"border-top-width:0px;				    "
		"padding: 0px;							"
		"}										"
		"QFrame:hover{							"
		"background-color: rgb(129, 129, 129);	"
		"padding: 0px;							"
		"}										"
		);
	container = new QFrame();
	// HACK!
	setContainerStyle(INodeStrategy::Normal);

	QVBoxLayout* layout = new QVBoxLayout();

	layout->addWidget(label);
	layout->addWidget(lowerFrame);
	layout->setContentsMargins(0, 0, 0, 0);
	layout->setMargin(0);
	layout->setSpacing(0);
	container->setLayout(layout);
}

const QRectF& DarkNodeStrategy::getRect()
{
	int maximum = 0;
	// TODO: optymalniej
	auto src = utils::dynamic_pointer_cast<IVisualSourceNode>(node.lock());
	auto snk = utils::dynamic_pointer_cast<IVisualSinkNode>(node.lock());
	maximum = src ? src->getNumOutputPins() : 0;
	maximum = (std::max)(maximum, snk ? snk->getNumInputPins() : 0);
	rect.setRect(0, 0, 100, (std::max)(10 + maximum * 20, 40) + labelHeight );
	label->setMaximumHeight(labelHeight);
	label->setMinimumHeight(labelHeight);
	container->setMinimumSize(rect.width(), rect.height());
	container->setMaximumSize(rect.width(), rect.height());
	return rect;
}

QPointF DarkNodeStrategy::getPinPosition( int no, bool input )
{
	return QPointF(input ? -7 : 91, labelHeight + 5 + no * 20);
}

QWidget* DarkNodeStrategy::getWidget()
{
	return container;
}

void DarkNodeStrategy::update()
{
    auto ptr = node.lock();
    if (ptr) {
	    label->setText(ptr->getName());
        QIcon icon = ptr->getIcon();
        if (!icon.isNull()) {
            lowerFrame->setPixmap(icon.pixmap(32, 32));
        }
    }
}

void DarkNodeStrategy::setState( State state )
{
	setContainerStyle(state);
}

void DarkNodeStrategy::setContainerStyle(INodeStrategy::State state)
{
    // TODO wproadzic flagi do obslugi stanow wezla
	QString s;
	switch(state) {
    case INodeStrategy::Valid:
        invalid = false;
        break;
	case INodeStrategy::Normal:
        selected = false;
		//s = "border: 0px;";
		break;
	case INodeStrategy::Selected:
        selected = true;
		//s = "border: 2px solid rgb(128, 213, 220);";
		break;
    case INodeStrategy::Invalid:
        //s = "border: 2px solid rgb(255, 213, 220);";
        invalid = true;
        break;
	default:
		UTILS_ASSERT(false);
	}
    if (invalid && selected) {
        s = "border: 2px solid rgb(255, 110, 0);";
    } else if (invalid) {
        s = "border: 2px solid rgb(200, 0, 0);";
    } else if (selected) {
        s = "border: 2px solid rgb(128, 213, 220);";
    } else {
        s = "border: 0px;";
    }

	container->setStyleSheet(QString(
		"QFrame{								"
		"background-color: rgb(104, 104, 104);	"
		"%1										"
		"border-radius: 4px;					"
		"border-bottom-right-radius: 0px;		"
		"}										"
		).arg(s)
		);
}
