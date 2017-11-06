#include "CoreUiPCH.h"
#include <coreui/CoreQuaternionEditorWidget.h>
#include "ui_CoreQuaternionEditorWidget.h"
#include <limits>

using namespace coreUI;

CoreQuaternionEditorWidget::CoreQuaternionEditorWidget(QWidget * parent)
	: QWidget(parent), ui(new Ui::CoreQuaternionEditorWidget),
	value_(0,0,0,1)
{
	ui->setupUi(this);
	setRange(std::numeric_limits<double>::lowest(), std::numeric_limits<double>::max());	
}

CoreQuaternionEditorWidget::~CoreQuaternionEditorWidget()
{
	delete ui;
}

void CoreQuaternionEditorWidget::setRange(const double minVal, const double maxVal)
{
	ui->quternionXDoubleSpinBox->setRange(minVal, maxVal);
	ui->quternionYDoubleSpinBox->setRange(minVal, maxVal);
	ui->quternionZDoubleSpinBox->setRange(minVal, maxVal);
	ui->quternionWDoubleSpinBox->setRange(minVal, maxVal);
}

void CoreQuaternionEditorWidget::setValidator(const CoreQuaternionValidator * validator)
{
	validator_ = const_cast<CoreQuaternionValidator*>(validator);
}

const CoreQuaternionValidator * CoreQuaternionEditorWidget::validator() const
{
	return validator_;
}

osg::Quat CoreQuaternionEditorWidget::value() const
{
	return value_;
}

bool validateValue(osg::Quat & value, const CoreQuaternionValidator * validator)
{
	osg::Quat lv(value);

	bool ret = true;

	if (validator != nullptr){
		if (validator->validate(lv) == false){
			if (validator->fixup(lv) == false){
				ret = false;
			}
			else{
				ret = validator->validate(lv);
			}

			if (ret == true){
				value = lv;
			}
		}
	}

	return ret;
}

void CoreQuaternionEditorWidget::setValue(const osg::Quat & value)
{
	if (value_ != value){

		osg::Quat v(value);

		if (validateValue(v, validator_) == true){
			if (v != value_){
				updateValue(value);
			}
		}
	}
}

void CoreQuaternionEditorWidget::updateValue(const osg::Quat & value)
{
	ui->quternionXDoubleSpinBox->blockSignals(true);
	ui->quternionYDoubleSpinBox->blockSignals(true);
	ui->quternionZDoubleSpinBox->blockSignals(true);
	ui->quternionWDoubleSpinBox->blockSignals(true);

	ui->quternionXDoubleSpinBox->setValue(value.x());
	ui->quternionYDoubleSpinBox->setValue(value.y());
	ui->quternionZDoubleSpinBox->setValue(value.z());
	ui->quternionWDoubleSpinBox->setValue(value.w());

	ui->quternionXDoubleSpinBox->blockSignals(false);
	ui->quternionYDoubleSpinBox->blockSignals(false);
	ui->quternionZDoubleSpinBox->blockSignals(false);
	ui->quternionWDoubleSpinBox->blockSignals(false);

	value_ = value;

	emit valueChanged(value_);
}

void CoreQuaternionEditorWidget::updateValue()
{
	osg::Quat v(ui->quternionXDoubleSpinBox->value(),
		ui->quternionYDoubleSpinBox->value(),
		ui->quternionZDoubleSpinBox->value(),
		ui->quternionWDoubleSpinBox->value());

	osg::Quat lv(v);

	if (validateValue(lv, validator_) == true){
		if ((lv == v) && (v != value_)){
			updateValue(v);
		}
	}
}