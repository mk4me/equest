#include "SensorsConfigurationWizardPage.h"
#include "ui_SensorsConfigurationWizardPage.h"
#include <QtWidgets/QMenu>
#include <QtWidgets/QInputDialog>
#include <QtWidgets/QMessageBox>
#include <set>
#include <string>
#include "SensorIDDelegate.h"
#include "SkeletonJointsDelegate.h"
#include "Vector3DDelegate.h"
#include "OrientationDelegate.h"
#include "OrientationEstimationAlgorithmDelegate.h"
#include <coreui/CoreVectorInputDelegate.h>
#include <kinematiclib/LinearizedSkeleton.h>
#include "ModelConfigurationWizardPage.h"

Q_DECLARE_METATYPE(IMU::IIMUOrientationEstimationAlgorithmConstPtr);
Q_DECLARE_METATYPE(kinematic::Skeleton::JointConstPtr);

Q_DECLARE_METATYPE(osg::Vec3d);
Q_DECLARE_METATYPE(osg::Quat);

Q_DECLARE_METATYPE(IMU::SkeletonConstPtr);

static const int UseIDX = 0;
static const int SensorIDX = 0;
static const int JointIDX = 1;
static const int AlgorithmIDX = 2;
static const int PreMulRotationIDX = 3;
static const int PostMulRotationIDX = 4;
static const int OffsetIDX = 5;

SensorsConfigurationWizardPage::SensorsConfigurationWizardPage(
	const imuCostume::Costume::SensorIDsSet & costumeSensors,
	const IMU::IIMUDataSource::OrientationEstimationAlgorithms & orientAlgorithms,
	ModelConfigurationWizardPage * modelPage, QWidget *parent)
	: QWizardPage(parent), modelPage(modelPage), ui(new Ui::SensorsConfigurationWizardPage),
	checkStates(costumeSensors.size(), Qt::Unchecked)
{
	ui->setupUi(this);
	connect(ui->sensorsConfigurationTableWidget, SIGNAL(cellChanged(int, int)), this, SIGNAL(completeChanged()));

	sensorDelegate = new SensorIDDelegate(ui->sensorsConfigurationTableWidget);
	sensorDelegate->setSensors(costumeSensors);

	skeletonJointDelegate = new SkeletonJointsDelegate(ui->sensorsConfigurationTableWidget);

	vector3DDelegate = new Vector3DDelegate(ui->sensorsConfigurationTableWidget);

	orientationDelegate = new OrientationDelegate(ui->sensorsConfigurationTableWidget);

	orientationAlgorithmDelegate = new OrientationEstimationAlgorithmDelegate(ui->sensorsConfigurationTableWidget);
	orientationAlgorithmDelegate->setOrientationEstimationAlgorithms(orientAlgorithms);

	ui->sensorsConfigurationTableWidget->setItemDelegateForColumn(SensorIDX, sensorDelegate);
	ui->sensorsConfigurationTableWidget->setItemDelegateForColumn(JointIDX, skeletonJointDelegate);
	ui->sensorsConfigurationTableWidget->setItemDelegateForColumn(AlgorithmIDX, orientationAlgorithmDelegate);
	ui->sensorsConfigurationTableWidget->setItemDelegateForColumn(PreMulRotationIDX, orientationDelegate);
	ui->sensorsConfigurationTableWidget->setItemDelegateForColumn(PostMulRotationIDX, orientationDelegate);
	ui->sensorsConfigurationTableWidget->setItemDelegateForColumn(OffsetIDX, vector3DDelegate);

	ui->sensorsConfigurationTableWidget->setRowCount(costumeSensors.size());
	
	ui->sensorsConfigurationTableWidget->blockSignals(true);
	ui->sensorsConfigurationTableWidget->model()->blockSignals(true);

	unsigned int row = 0;	

	//idziemy po wszystkich sensorach
	for (const auto & id : costumeSensors)
	{
		//SENSOR column
		{
			auto sensorItem = new QTableWidgetItem(QString("%1").arg(id));
			sensorItem->setFlags(sensorItem->flags() | Qt::ItemFlag::ItemIsUserCheckable | Qt::ItemFlag::ItemIsEnabled);
			sensorItem->setCheckState(Qt::Unchecked);
			ui->sensorsConfigurationTableWidget->setItem(row, SensorIDX, sensorItem);
			ui->sensorsConfigurationTableWidget->model()->setData(ui->sensorsConfigurationTableWidget->model()->index(row, SensorIDX), QVariant(id));
		}

		//JOINT column
		{
			ui->sensorsConfigurationTableWidget->setItem(row, JointIDX, new QTableWidgetItem(SkeletonJointsDelegate::defaultText()));
			ui->sensorsConfigurationTableWidget->model()->setData(ui->sensorsConfigurationTableWidget->model()->index(row, JointIDX), static_cast<unsigned long long>(SkeletonJointsDelegate::defaultValue()), Qt::UserRole);
		}

		//ALGORITHM column
		{
			ui->sensorsConfigurationTableWidget->setItem(row, AlgorithmIDX, new QTableWidgetItem(OrientationEstimationAlgorithmDelegate::defaultText()));
			ui->sensorsConfigurationTableWidget->model()->setData(ui->sensorsConfigurationTableWidget->model()->index(row, AlgorithmIDX), QVariant::fromValue(OrientationEstimationAlgorithmDelegate::defaultValue()), Qt::UserRole);
		}

		//PRE MUL ROTATION column
		{
			ui->sensorsConfigurationTableWidget->setItem(row, PreMulRotationIDX, new QTableWidgetItem(coreUI::CoreVectorEditor::quaternionToString(osg::Quat(0, 0, 0, 1)._v)));
			ui->sensorsConfigurationTableWidget->model()->setData(ui->sensorsConfigurationTableWidget->model()->index(row, PreMulRotationIDX), QVariant::fromValue(osg::Quat(0, 0, 0, 1)), Qt::UserRole);
		}

		//POST MUL ROTATION column
		{
			ui->sensorsConfigurationTableWidget->setItem(row, PostMulRotationIDX, new QTableWidgetItem(coreUI::CoreVectorEditor::quaternionToString(osg::Quat(0, 0, 0, 1)._v)));
			ui->sensorsConfigurationTableWidget->model()->setData(ui->sensorsConfigurationTableWidget->model()->index(row, PostMulRotationIDX), QVariant::fromValue(osg::Quat(0, 0, 0, 1)), Qt::UserRole);
		}

		//OFFSET column
		{
			ui->sensorsConfigurationTableWidget->setItem(row, OffsetIDX, new QTableWidgetItem(coreUI::CoreVectorEditor::vectorToString(osg::Vec3d(0, 0, 0), 3, coreUI::CoreVectorEditor::vectorPattern(3))));
			ui->sensorsConfigurationTableWidget->model()->setData(ui->sensorsConfigurationTableWidget->model()->index(row, OffsetIDX), QVariant::fromValue(osg::Vec3d(0, 0, 0)), Qt::UserRole);
		}

		++row;
	}

	ui->sensorsConfigurationTableWidget->blockSignals(false);
	ui->sensorsConfigurationTableWidget->model()->blockSignals(false);
}

SensorsConfigurationWizardPage::SensorsConfigurationWizardPage(
	const imuCostume::Costume::SensorIDsSet & costumeSensors,
	const IMU::IIMUDataSource::OrientationEstimationAlgorithms & orientAlgorithms,
	const IMU::CostumeProfile::SensorsDescriptions & sensorsDescriptions,
	ModelConfigurationWizardPage * modelPage, QWidget *parent)
	: SensorsConfigurationWizardPage(costumeSensors, orientAlgorithms, modelPage, parent)
{
	int row = 0;

	ui->sensorsConfigurationTableWidget->blockSignals(true);
	ui->sensorsConfigurationTableWidget->model()->blockSignals(true);

	for (const auto & id : costumeSensors)
	{
		auto it = sensorsDescriptions.find(id);
		if (it != sensorsDescriptions.end()){			

			QString jointName;
			osg::Vec3d offset(0, 0, 0);
			osg::Quat preMulRotation(0, 0, 0, 1);
			osg::Quat postMulRotation(0, 0, 0, 1);
			IMU::IIMUOrientationEstimationAlgorithmConstPtr algorithm;


			//mam wartość dla obu wpisów
			auto use = true;			
			auto sID = id;
			auto sensorText = QString("%1").arg(id);
			jointName = QString::fromStdString(it->second.jointName);
			offset = it->second.adjustment.offset;
			preMulRotation = it->second.adjustment.preMulRotation;
			postMulRotation = it->second.adjustment.postMulRotation;
			algorithm = it->second.orientationEstimationAlgorithm;

			//JOINT column
			{
				if (jointName.isEmpty() == true || it->second.jointIdx == -1){
					ui->sensorsConfigurationTableWidget->setItem(row, JointIDX, new QTableWidgetItem(SkeletonJointsDelegate::defaultText()));
					ui->sensorsConfigurationTableWidget->model()->setData(ui->sensorsConfigurationTableWidget->model()->index(row, JointIDX), -1, Qt::UserRole);
				}else{
					ui->sensorsConfigurationTableWidget->setItem(row, JointIDX, new QTableWidgetItem(jointName));
					ui->sensorsConfigurationTableWidget->model()->setData(ui->sensorsConfigurationTableWidget->model()->index(row, JointIDX), static_cast<unsigned long long>(it->second.jointIdx), Qt::UserRole);
				}
			}

			//ALGORITHM column
			{
				if (algorithm == nullptr){
					ui->sensorsConfigurationTableWidget->setItem(row, AlgorithmIDX, new QTableWidgetItem(OrientationEstimationAlgorithmDelegate::defaultText()));
					ui->sensorsConfigurationTableWidget->model()->setData(ui->sensorsConfigurationTableWidget->model()->index(row, AlgorithmIDX), QVariant::fromValue(OrientationEstimationAlgorithmDelegate::defaultValue()), Qt::UserRole);
				}
				else{
					ui->sensorsConfigurationTableWidget->setItem(row, AlgorithmIDX, new QTableWidgetItem(QString::fromStdString(algorithm->name())));
					ui->sensorsConfigurationTableWidget->model()->setData(ui->sensorsConfigurationTableWidget->model()->index(row, AlgorithmIDX), QVariant::fromValue(algorithm), Qt::UserRole);
				}
			}

			//PRE MUL ROTATION column
			{
				ui->sensorsConfigurationTableWidget->setItem(row, PreMulRotationIDX, new QTableWidgetItem(coreUI::CoreVectorEditor::quaternionToString(preMulRotation._v)));
				ui->sensorsConfigurationTableWidget->model()->setData(ui->sensorsConfigurationTableWidget->model()->index(row, PreMulRotationIDX), QVariant::fromValue(preMulRotation), Qt::UserRole);
			}

			//POST MUL ROTATION column
			{
				ui->sensorsConfigurationTableWidget->setItem(row, PostMulRotationIDX, new QTableWidgetItem(coreUI::CoreVectorEditor::quaternionToString(postMulRotation._v)));
				ui->sensorsConfigurationTableWidget->model()->setData(ui->sensorsConfigurationTableWidget->model()->index(row, PostMulRotationIDX), QVariant::fromValue(postMulRotation), Qt::UserRole);
			}

			//OFFSET column
			{
				ui->sensorsConfigurationTableWidget->setItem(row, OffsetIDX, new QTableWidgetItem(coreUI::CoreVectorEditor::vectorToString(offset, 3, coreUI::CoreVectorEditor::vectorPattern(3))));
				ui->sensorsConfigurationTableWidget->model()->setData(ui->sensorsConfigurationTableWidget->model()->index(row, OffsetIDX), QVariant::fromValue(offset), Qt::UserRole);
			}

			//SENSOR column
			{
				auto sensorItem = new QTableWidgetItem(sensorText);
				sensorItem->setFlags(sensorItem->flags() | Qt::ItemFlag::ItemIsUserCheckable | Qt::ItemFlag::ItemIsEnabled);
				checkStates[row] = (use == true ? Qt::Checked : Qt::Unchecked);
				sensorItem->setCheckState(checkStates[row]);
				ui->sensorsConfigurationTableWidget->setItem(row, SensorIDX, sensorItem);
				ui->sensorsConfigurationTableWidget->model()->setData(ui->sensorsConfigurationTableWidget->model()->index(row, SensorIDX), QVariant(id));
			}
		}

		++row;
	}

	ui->sensorsConfigurationTableWidget->blockSignals(false);
	ui->sensorsConfigurationTableWidget->model()->blockSignals(false);
}

SensorsConfigurationWizardPage::~SensorsConfigurationWizardPage()
{

}

void SensorsConfigurationWizardPage::initializePage()
{
	auto skeleton = modelPage->skeleton();
	
	order = kinematic::LinearizedSkeleton::createNonLeafOrder(*skeleton);
	const auto joints = treeContainer::Linearization::convert(order);
	skeletonJointDelegate->setJoints(joints);

	//przejść przez kolumne z jointami w mapowaniu i wyzerować te dla których nie ma odpowiednika w aktualnym szkielecie
	auto model = ui->sensorsConfigurationTableWidget->model();
	for (unsigned int i = 0; i < model->rowCount(); ++i)
	{
		auto index = model->index(i, JointIDX);
		auto jointName = model->data(index, Qt::DisplayRole).toString().toStdString();
		if (joints.right.find(jointName) == joints.right.end()){
			model->setData(index, SkeletonJointsDelegate::defaultText());
			model->setData(index, static_cast<unsigned long long>(SkeletonJointsDelegate::defaultValue()), Qt::UserRole);
		}
	}
}

bool SensorsConfigurationWizardPage::isComplete() const
{
	bool ret = false;
	for (const auto & val : checkStates)
	{
		if (val == Qt::Checked)
		{
			ret = true;
			break;
		}
	}

	return ret;
}

void SensorsConfigurationWizardPage::onSensorsConfigurationTableContextMenu(const QPoint & position)
{
	//wyznaczamy ile mamy zaznaczonych, niezaznaczonych
	unsigned int selected = 0;
	unsigned int unselected = 0;

	for (unsigned int row = 0; row < ui->sensorsConfigurationTableWidget->rowCount(); ++row)
	{
		if (ui->sensorsConfigurationTableWidget->item(row, UseIDX)->checkState() == Qt::Unchecked){
			++unselected;
		}
		else{
			++selected;
		}
	}

	QMenu menu(ui->sensorsConfigurationTableWidget);	
	menu.addAction(ui->actionSelect_all_sensors);
	ui->actionSelect_all_sensors->setEnabled(unselected > 0);

	menu.addAction(ui->actionDeselect_all_sensors);
	ui->actionDeselect_all_sensors->setEnabled(selected > 0);

	menu.addAction(ui->actionInverse_sensors_selection);	

	menu.addSeparator();	

	menu.addAction(ui->actionSelect_common_estimation_algorithm);	

	menu.exec(ui->sensorsConfigurationTableWidget->mapToGlobal(position));
}

bool SensorsConfigurationWizardPage::rowIsValid(int row) const
{
	bool ret = false;	

	auto sensorIDData = ui->sensorsConfigurationTableWidget->item(row, SensorIDX)->data(Qt::DisplayRole);
	auto jointIDData = ui->sensorsConfigurationTableWidget->item(row, JointIDX)->data(Qt::UserRole);
	auto algoData = ui->sensorsConfigurationTableWidget->item(row, AlgorithmIDX)->data(Qt::UserRole);
	
	if (sensorIDData.isValid() && sensorIDData.canConvert<imuCostume::Costume::SensorID>() == true &&
		jointIDData.isValid() && jointIDData.canConvert<kinematic::LinearizedSkeleton::NodeIDX>() &&
		algoData.isValid()){

		auto sid = sensorIDData.value<imuCostume::Costume::SensorID>();
		auto jointIDX = jointIDData.value<kinematic::LinearizedSkeleton::NodeIDX>();
		auto algo = algoData.value<IMU::IIMUOrientationEstimationAlgorithmConstPtr>();	

		if (sid > -1 && jointIDX != -1 && algo != nullptr){

			auto jointName = ui->sensorsConfigurationTableWidget->item(row, JointIDX)->data(Qt::DisplayRole).toString().toStdString();		

			if (jointIDX < order.size() && order[jointIDX] == jointName){
				ret = true;
			}
		}
	}

	return ret;
}

void SensorsConfigurationWizardPage::onSelectAllSensors()
{
	ui->sensorsConfigurationTableWidget->blockSignals(true);
	for (unsigned int row = 0; row < ui->sensorsConfigurationTableWidget->rowCount(); ++row)
	{	
		if (ui->sensorsConfigurationTableWidget->item(row, UseIDX)->checkState() == Qt::Unchecked){			
			auto checkState = Qt::PartiallyChecked;
			if (rowIsValid(row) == true){
				checkState = Qt::Checked;
			}			

			ui->sensorsConfigurationTableWidget->item(row, UseIDX)->setCheckState(checkState);
			checkStates[row] = checkState;
		}
	}
	ui->sensorsConfigurationTableWidget->blockSignals(false);

	emit completeChanged();
}

void SensorsConfigurationWizardPage::onDeselectAllSensors()
{
	ui->sensorsConfigurationTableWidget->blockSignals(true);
	for (unsigned int row = 0; row < ui->sensorsConfigurationTableWidget->rowCount(); ++row)
	{
		if (ui->sensorsConfigurationTableWidget->item(row, UseIDX)->checkState() != Qt::Unchecked){

			ui->sensorsConfigurationTableWidget->item(row, UseIDX)->setCheckState(Qt::Unchecked);
			checkStates[row] = Qt::Unchecked;
		}
	}
	ui->sensorsConfigurationTableWidget->blockSignals(false);

	emit completeChanged();
}

void SensorsConfigurationWizardPage::onInversSensorsSelection()
{
	ui->sensorsConfigurationTableWidget->blockSignals(true);
	for (unsigned int row = 0; row < ui->sensorsConfigurationTableWidget->rowCount(); ++row)
	{
		auto checkState = Qt::Unchecked;

		if (ui->sensorsConfigurationTableWidget->item(row, UseIDX)->checkState() == Qt::Unchecked){

			checkState = Qt::PartiallyChecked;

			if (rowIsValid(row) == true){
				checkState = Qt::Checked;
			}
		}

		ui->sensorsConfigurationTableWidget->item(row, UseIDX)->setCheckState(checkState);
		checkStates[row] = checkState;
	}
	ui->sensorsConfigurationTableWidget->blockSignals(false);

	emit completeChanged();
}

void SensorsConfigurationWizardPage::onSelectCommonEstimationAlgorithm()
{
	//najpierw sprawdzamy czy nie ma już tak że wszystkie algo są takie 
	IMU::IIMUOrientationEstimationAlgorithmConstPtr algo;
	bool same = true;

	auto data = ui->sensorsConfigurationTableWidget->item(0, AlgorithmIDX)->data(Qt::UserRole);

	if (data.isValid() == true){
		algo = data.value<IMU::IIMUOrientationEstimationAlgorithmConstPtr>();
	}

	for (unsigned int row = 1; row < ui->sensorsConfigurationTableWidget->rowCount(); ++row)
	{
		data = ui->sensorsConfigurationTableWidget->item(row, AlgorithmIDX)->data(Qt::UserRole);

		if (data.isValid() == true){
			if (algo != data.value<IMU::IIMUOrientationEstimationAlgorithmConstPtr>()){
				same = false;
				break;
			}
		}
		else if (algo != nullptr){
			same = false;
			break;
		}
	}

	QStringList values;
	values << OrientationEstimationAlgorithmDelegate::defaultText();

	int currentID = 0;

	auto algorithms = orientationAlgorithmDelegate->orientationEstimationAlgorithms();

	int i = 1;
	for (const auto & a : algorithms)
	{
		values << QString("%1 (%2)").arg(QString::fromStdString(a.second->name())).arg(QString::fromStdString(boost::lexical_cast<std::string>(a.first)));

		if (same == true && algo == a.second){
			currentID = i;
		}

		++i;
	}

	bool ok = true;

	auto item = QInputDialog::getItem(this, tr("Orientation estimation algorithm"), tr("Algorithm"), values, currentID, false, &ok);

	if (ok == true && item != values.first()){		
		auto it = std::find(values.begin(), values.end(), item);
		unsigned int idx = std::distance(values.begin(), it);
		--idx;

		auto algoIT = algorithms.begin();
		std::advance(algoIT, idx);

		auto nameData = QVariant::fromValue(QString::fromStdString(algoIT->second->name()));
		auto valueData = QVariant::fromValue(algoIT->second);

		for (unsigned int row = 0; row < ui->sensorsConfigurationTableWidget->rowCount(); ++row)
		{
			ui->sensorsConfigurationTableWidget->blockSignals(true);
			ui->sensorsConfigurationTableWidget->item(row, AlgorithmIDX)->setData(Qt::DisplayRole, nameData);
			ui->sensorsConfigurationTableWidget->blockSignals(false);
			ui->sensorsConfigurationTableWidget->item(row, AlgorithmIDX)->setData(Qt::UserRole, valueData);
		}		
	}
}

void SensorsConfigurationWizardPage::onOneToOneMappingChanged(int state)
{
	if (state == Qt::Checked){
		auto model = ui->sensorsConfigurationTableWidget->model();

		//szukamy duplikatów
		bool found = false;
		{

			std::set<std::string> jointsNames;
			for (unsigned int i = 0; i < model->rowCount(); ++i){
				auto jointName = model->data(model->index(i, JointIDX)).toString().toStdString();
				if (jointsNames.find(jointName) != jointsNames.end()){
					found = true;
					break;
				}
			}
		}

		if (found == true){
			auto ret = QMessageBox::question(this, tr("Duplicate joints found"), tr("You wish to use one-to-one sensor to joint mapping but some joints have more than one sensor attached. Would You like to reset duplicates?"));
			if (ret == QMessageBox::No){
				ui->oneToOneMappingCheckBox->blockSignals(true);
				ui->oneToOneMappingCheckBox->setChecked(Qt::Unchecked);
				ui->oneToOneMappingCheckBox->blockSignals(false);
				return;
			}
		}

		std::set<std::string> jointsNames;
		for (unsigned int i = 0; i < model->rowCount(); ++i){
			auto jointName = model->data(model->index(i, JointIDX)).toString().toStdString();
			if (jointsNames.find(jointName) != jointsNames.end()){
				//już mamy taki joint
				ui->sensorsConfigurationTableWidget->blockSignals(true);
				model->setData(model->index(i, JointIDX), SkeletonJointsDelegate::defaultText());
				ui->sensorsConfigurationTableWidget->blockSignals(false);
				model->setData(model->index(i, JointIDX), static_cast<unsigned long long>(SkeletonJointsDelegate::defaultValue()), Qt::UserRole);
			}
			else{
				//jointa nie było - zapamiętujemy
				jointsNames.insert(jointName);
			}
		}
	}
}

void SensorsConfigurationWizardPage::onSensorConfigurationCellChange(int row, int column)
{
	auto cs = ui->sensorsConfigurationTableWidget->item(row, UseIDX)->checkState();

	if (column == UseIDX){

		switch (cs)
		{
		case Qt::Unchecked:
			checkStates[row] = Qt::Unchecked;
			break;
		case Qt::Checked:

			if (checkStates[row] == Qt::PartiallyChecked){
				checkStates[row] = Qt::Unchecked;
			}
			else if (rowIsValid(row) == false){
				checkStates[row] = Qt::PartiallyChecked;
			}
			else {
				checkStates[row] = Qt::Checked;
			}			

			break;
		case Qt::PartiallyChecked:
			
			if (rowIsValid(row) == true){
				checkStates[row] = Qt::Checked;
			}
			else{
				checkStates[row] = Qt::PartiallyChecked;
			}

			break;		
		}
	}
	else if ((checkStates[row] == Qt::PartiallyChecked) && (rowIsValid(row) == true)){		
		checkStates[row] = Qt::Checked;		
	}

	ui->sensorsConfigurationTableWidget->blockSignals(true);
	ui->sensorsConfigurationTableWidget->item(row, UseIDX)->setCheckState(checkStates[row]);
	ui->sensorsConfigurationTableWidget->blockSignals(false);	
}

IMU::CostumeProfile::SensorsDescriptions SensorsConfigurationWizardPage::sensorsDescriptions() const
{
	IMU::CostumeProfile::SensorsDescriptions ret;

	for (unsigned int row = 0; row < ui->sensorsConfigurationTableWidget->rowCount(); ++row)
	{
		auto item = ui->sensorsConfigurationTableWidget->item(row, UseIDX);
		if (item->checkState() == Qt::Checked){
			IMU::CostumeProfile::SensorDescription sd;
			{
				item = ui->sensorsConfigurationTableWidget->item(row, JointIDX);
				sd.jointName = item->data(Qt::DisplayRole).toString().toStdString();
				sd.jointIdx = item->data(Qt::UserRole).value<decltype(sd.jointIdx)>();
			}

			{
				item = ui->sensorsConfigurationTableWidget->item(row, AlgorithmIDX);
				sd.orientationEstimationAlgorithm.reset(item->data(Qt::UserRole).value<IMU::IIMUOrientationEstimationAlgorithmConstPtr>()->create());
			}

			//PRE MUL ROTATION column
			{
				item = ui->sensorsConfigurationTableWidget->item(row, PreMulRotationIDX);
				sd.adjustment.preMulRotation = item->data(Qt::UserRole).value<osg::Quat>();
			}

			//POST MUL ROTATION column
			{
				item = ui->sensorsConfigurationTableWidget->item(row, PostMulRotationIDX);
				sd.adjustment.postMulRotation = item->data(Qt::UserRole).value<osg::Quat>();
			}

			//OFFSET column
			{
				item = ui->sensorsConfigurationTableWidget->item(row, OffsetIDX);
				sd.adjustment.offset = item->data(Qt::UserRole).value<osg::Vec3d>();
			}

			ret.insert(ret.end(),
			{
				ui->sensorsConfigurationTableWidget->item(row, SensorIDX)->data(Qt::DisplayRole).value<imuCostume::Costume::SensorID>(),
				sd
			});
		}
	}

	return ret;
}