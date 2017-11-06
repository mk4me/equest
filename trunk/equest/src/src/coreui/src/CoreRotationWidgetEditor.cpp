#include "CoreUiPCH.h"
#include <coreui/CoreRotationWidgetEditor.h>
#include <coreui/CoreEulerAnglesEditorWidget.h>
#include <coreui/CoreQuaternionEditorWidget.h>
#include <QtWidgets/QLabel>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QVBoxLayout>
#include <coreui/CoreUnitQuaternionValidator.h>

using namespace coreUI;

CoreRotationWidgetEditor::CoreRotationWidgetEditor(QWidget * parent)
	: QWidget(parent), eulerAngles(new CoreEulerAnglesEditorWidget(this)),
	quaternion(new CoreQuaternionEditorWidget(this)),
	anglesNamelabel(new QLabel(tr("Rotation angles"), this)),
	dataIsOrientation(false), validator(new CoreUnitQuaternionValidator(quaternion))
{
	connect(eulerAngles, SIGNAL(valueChanged(osg::Quat)), this, SLOT(onValueChanged(osg::Quat)));
	connect(quaternion, SIGNAL(valueChanged(osg::Quat)), this, SLOT(onValueChanged(osg::Quat)));

	quaternion->setRange(-1.0, 1.0);
	quaternion->setValidator(validator);

	auto h = new QHBoxLayout;

	auto v = new QVBoxLayout;
	auto l = new QLabel(this);
	l->setText(tr("Quaternion:"));
	v->addWidget(l);
	v->addWidget(quaternion);

	h->addLayout(v);

	v = new QVBoxLayout;
	v->addWidget(anglesNamelabel);
	v->addWidget(eulerAngles);

	h->addLayout(v);

	setLayout(h);
}

CoreRotationWidgetEditor::~CoreRotationWidgetEditor()
{

}

osg::Quat CoreRotationWidgetEditor::value() const
{
	return quaternion->value();
}

void CoreRotationWidgetEditor::setValue(const osg::Quat & orientation)
{
	if (validator->validate(orientation) == false){
		return;
	}

	quaternion->blockSignals(true);
	eulerAngles->blockSignals(true);

	quaternion->setValue(orientation);
	eulerAngles->setValue(orientation);

	quaternion->blockSignals(false);
	eulerAngles->blockSignals(false);

	emit valueChanged(orientation);
}

void CoreRotationWidgetEditor::setDataInterpretation(const bool orientation)
{
	if (orientation != dataIsOrientation){
		dataIsOrientation = orientation;
		if (orientation == true){
			anglesNamelabel->setText(tr("Orientation angles"));
		}
		else{
			anglesNamelabel->setText(tr("Rotation angles"));
		}
	}
}

void CoreRotationWidgetEditor::onValueChanged(const osg::Quat & orientation)
{
	auto s = sender();

	if (s == quaternion){
		s = eulerAngles;
		s->blockSignals(true);
		auto rs = static_cast<CoreEulerAnglesEditorWidget*>(s);
		rs->setValue(orientation);
		s->blockSignals(false);
	}
	else{
		s = quaternion;
		s->blockSignals(true);
		auto rs = static_cast<CoreQuaternionEditorWidget*>(s);
		rs->setValue(orientation);
		s->blockSignals(false);
	}

	emit valueChanged(orientation);
}