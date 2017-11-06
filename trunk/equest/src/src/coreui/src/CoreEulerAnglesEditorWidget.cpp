#include "CoreUiPCH.h"
#include <coreui/CoreEulerAnglesEditorWidget.h>
#include "ui_CoreEulerAnglesEditorWidget.h"
#include <QtGui/QStandardItemModel>
#include <kinematicUtils/RotationConverter.h>

double convertAngle(const double val, const bool toRadians)
{
	if (toRadians == true){
		return osg::DegreesToRadians(val);
	}
	else{
		return osg::RadiansToDegrees(val);
	}
}

using namespace coreUI;

CoreEulerAnglesEditorWidget::CoreEulerAnglesEditorWidget(QWidget * parent)
	: QWidget(parent), ui(new Ui::CoreEulerAnglesEditorWidget),
	orientation(0, 0, 0, 1), anglesRadiansRepresentation(false),
	anglesConvention(kinematicUtils::AxisOrder::XYZ),
	values(0,0,0)
{
	ui->setupUi(this);

	int index = 0;
	//| Tait–Bryan angles | Cardan angles | nautical angles | Yaw, Pitch, Roll | Heading, Elevation, Bank"
	ui->eulerAnglesConventionComboBox->blockSignals(true);
	ui->eulerAnglesConventionComboBox->addItem(tr("Intrinsic rotations"));
	ui->eulerAnglesConventionComboBox->setItemData(index, tr("Tait–Bryan angles | Cardan angles | nautical angles | Yaw, Pitch, Roll | Heading, Elevation, Bank"), Qt::ToolTipRole);

	const QStandardItemModel* model = qobject_cast<const QStandardItemModel*>(ui->eulerAnglesConventionComboBox->model());
	QStandardItem* item = model->item(index);

	item->setFlags(item->flags() & ~(Qt::ItemIsSelectable | Qt::ItemIsEnabled));
	// visually disable by greying out - works only if combobox has been painted already and palette returns the wanted color
	item->setData(ui->eulerAnglesConventionComboBox->palette().color(QPalette::Disabled, QPalette::Text));

	ui->eulerAnglesConventionComboBox->addItem(QString::fromStdString(kinematicUtils::AxisOrder::toString(kinematicUtils::AxisOrder::XYZ)), (int)kinematicUtils::AxisOrder::XYZ);
	ui->eulerAnglesConventionComboBox->addItem(QString::fromStdString(kinematicUtils::AxisOrder::toString(kinematicUtils::AxisOrder::YZX)), (int)kinematicUtils::AxisOrder::YZX);
	ui->eulerAnglesConventionComboBox->addItem(QString::fromStdString(kinematicUtils::AxisOrder::toString(kinematicUtils::AxisOrder::ZXY)), (int)kinematicUtils::AxisOrder::ZXY);
	ui->eulerAnglesConventionComboBox->addItem(QString::fromStdString(kinematicUtils::AxisOrder::toString(kinematicUtils::AxisOrder::XZY)), (int)kinematicUtils::AxisOrder::XZY);
	ui->eulerAnglesConventionComboBox->addItem(QString::fromStdString(kinematicUtils::AxisOrder::toString(kinematicUtils::AxisOrder::ZYX)), (int)kinematicUtils::AxisOrder::ZYX);
	ui->eulerAnglesConventionComboBox->addItem(QString::fromStdString(kinematicUtils::AxisOrder::toString(kinematicUtils::AxisOrder::YXZ)), (int)kinematicUtils::AxisOrder::YXZ);

	ui->eulerAnglesConventionComboBox->addItem(tr("Extrinsic rotations)"));

	index = ui->eulerAnglesConventionComboBox->count() - 1;

	ui->eulerAnglesConventionComboBox->setItemData(index, tr("Classical Euler angles"), Qt::ToolTipRole);

	item = model->item(index);

	item->setFlags(item->flags() & ~(Qt::ItemIsSelectable | Qt::ItemIsEnabled));
	// visually disable by greying out - works only if combobox has been painted already and palette returns the wanted color
	item->setData(ui->eulerAnglesConventionComboBox->palette().color(QPalette::Disabled, QPalette::Text));

	ui->eulerAnglesConventionComboBox->addItem(QString::fromStdString(kinematicUtils::AxisOrder::toString(kinematicUtils::AxisOrder::ZXZ)), (int)kinematicUtils::AxisOrder::ZXZ);
	ui->eulerAnglesConventionComboBox->addItem(QString::fromStdString(kinematicUtils::AxisOrder::toString(kinematicUtils::AxisOrder::XYX)), (int)kinematicUtils::AxisOrder::XYX);
	ui->eulerAnglesConventionComboBox->addItem(QString::fromStdString(kinematicUtils::AxisOrder::toString(kinematicUtils::AxisOrder::YZY)), (int)kinematicUtils::AxisOrder::YZY);
	ui->eulerAnglesConventionComboBox->addItem(QString::fromStdString(kinematicUtils::AxisOrder::toString(kinematicUtils::AxisOrder::ZYZ)), (int)kinematicUtils::AxisOrder::ZYZ);
	ui->eulerAnglesConventionComboBox->addItem(QString::fromStdString(kinematicUtils::AxisOrder::toString(kinematicUtils::AxisOrder::XZX)), (int)kinematicUtils::AxisOrder::XZX);
	ui->eulerAnglesConventionComboBox->addItem(QString::fromStdString(kinematicUtils::AxisOrder::toString(kinematicUtils::AxisOrder::YXY)), (int)kinematicUtils::AxisOrder::YXY);
	ui->eulerAnglesConventionComboBox->blockSignals(false);

	ui->eulerAnglesConventionComboBox->setCurrentIndex(1);
}

CoreEulerAnglesEditorWidget::~CoreEulerAnglesEditorWidget()
{
	delete ui;
}

osg::Quat CoreEulerAnglesEditorWidget::value() const
{
	return orientation;
}

void CoreEulerAnglesEditorWidget::setValue(const osg::Quat & orientation)
{
	auto l = orientation.length2();
	if (std::abs(1.0 - l) <= 0.0001){

		auto rotAngles = kinematicUtils::convert(orientation, anglesConvention);

		if (anglesRadiansRepresentation == false){
			rotAngles = kinematicUtils::toDegrees(rotAngles);
		}

		ui->euler1DoubleSpinBox->blockSignals(true);
		ui->euler2DoubleSpinBox->blockSignals(true);
		ui->euler3DoubleSpinBox->blockSignals(true);

		ui->euler1DoubleSpinBox->setValue(rotAngles.x());
		ui->euler2DoubleSpinBox->setValue(rotAngles.y());
		ui->euler3DoubleSpinBox->setValue(rotAngles.z());

		ui->euler1DoubleSpinBox->blockSignals(false);
		ui->euler2DoubleSpinBox->blockSignals(false);
		ui->euler3DoubleSpinBox->blockSignals(false);

		this->orientation = orientation;		
	}
	else{
		throw std::runtime_error("Invalid quaternion format - expected unit quaternion");
	}
}

void CoreEulerAnglesEditorWidget::setAngleRepresentation(const bool radians)
{
	QString suffix;

	if (radians == true){
		ui->radiansRadioButton->setChecked(true);
		suffix = QChar(0260);
	}
	else{
		ui->degreesRadioButton->setChecked(true);
	}

	ui->euler1DoubleSpinBox->setSuffix(suffix);
	ui->euler2DoubleSpinBox->setSuffix(suffix);
	ui->euler3DoubleSpinBox->setSuffix(suffix);
}

void CoreEulerAnglesEditorWidget::setConvention(const int axisOrder)
{
	auto idx = ui->eulerAnglesConventionComboBox->findData(axisOrder);
	ui->eulerAnglesConventionComboBox->setCurrentIndex(idx);
}

void CoreEulerAnglesEditorWidget::updateValue(const osg::Vec3d & rotAngles)
{
	auto angles = rotAngles;

	if (anglesRadiansRepresentation == false){
		angles = kinematicUtils::toRadians(angles);
	}

	orientation = kinematicUtils::convert(angles, anglesConvention);

	emit valueChanged(orientation);
}

void CoreEulerAnglesEditorWidget::updateValue()
{
	values = osg::Vec3d(ui->euler1DoubleSpinBox->value(),
		ui->euler2DoubleSpinBox->value(),
		ui->euler3DoubleSpinBox->value());

	updateValue(values);
}

void updateSpinBox(QDoubleSpinBox * spinBox, const bool radians, const QString & suffix, const double val)
{
	spinBox->setRange(convertAngle(spinBox->minimum(), radians), convertAngle(spinBox->maximum(), radians));
	spinBox->setDecimals(radians == true ? spinBox->decimals() * 2 : spinBox->decimals() / 2);
	spinBox->setSingleStep(convertAngle(spinBox->singleStep(), radians));
	spinBox->setValue(val);
	spinBox->setSuffix(suffix);
}

void CoreEulerAnglesEditorWidget::updateAngleRepresentation(const bool radians)
{
	QString suffix;

	if (radians == false){
		suffix = QChar(0260);
	}

	ui->euler1DoubleSpinBox->blockSignals(true);
	ui->euler2DoubleSpinBox->blockSignals(true);
	ui->euler3DoubleSpinBox->blockSignals(true);

	if (radians == true){
		values = kinematicUtils::toRadians(values);
	}
	else{
		values = kinematicUtils::toDegrees(values);
	}

	updateSpinBox(ui->euler1DoubleSpinBox, radians, suffix, values[0]);
	updateSpinBox(ui->euler2DoubleSpinBox, radians, suffix, values[1]);
	updateSpinBox(ui->euler3DoubleSpinBox, radians, suffix, values[2]);

	ui->euler1DoubleSpinBox->blockSignals(false);
	ui->euler2DoubleSpinBox->blockSignals(false);
	ui->euler3DoubleSpinBox->blockSignals(false);

	anglesRadiansRepresentation = radians;
}

void CoreEulerAnglesEditorWidget::changeAngleRepresentation(const int idx)
{
	auto button = ui->angleRepresentationButtonGroup->button(idx);

	bool radians = true;

	if (button != ui->radiansRadioButton){
		radians = false;
	}
	
	updateAngleRepresentation(radians);
}

void CoreEulerAnglesEditorWidget::updateConvention(const kinematicUtils::AxisOrder::Type anglesConvention)
{
	auto name = kinematicUtils::AxisOrder::toString(anglesConvention);
	this->anglesConvention = anglesConvention;

	ui->euler1Label->setText(QChar(name[0]));
	ui->euler2Label->setText(QChar(name[1]));
	ui->euler3Label->setText(QChar(name[2]));

	updateValue(values);
}

void CoreEulerAnglesEditorWidget::changeConvention(const int idx)
{
	auto var = ui->eulerAnglesConventionComboBox->itemData(idx);
	int value = var.toInt();
	updateConvention((kinematicUtils::AxisOrder::Type)value);
}