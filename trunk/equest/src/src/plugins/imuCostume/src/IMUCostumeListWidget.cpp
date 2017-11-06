#include "PCH.h"
#include "IMUCostumeListWidget.h"
#include "ui_IMUCostumeListWidget.h"
#include "DataSource.h"
#include <QtWidgets/QMenu>
#include <coreui/CoreCursorChanger.h>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QFileDialog>
#include <utils/Filesystem.h>
#include "corelib/IVisualizerManager.h"
#include "corelib/PluginCommon.h"
#include <objectwrapperlib/ObjectWrapper.h>
#include <iomanip>
#include <ctime>
/*
#include "osg/PositionAttitudeTransform"
#include "osg/MatrixTransform"
#include "osg/Geode"
#include "osg/ShapeDrawable"
#include "osg/PolygonMode"
#include "osg/ref_ptr"
#include "osgDB/ReadFile"
#include "osg/LineWidth"
#include "osg/StateAttribute"
#include "osg/StateSet"
*/
#include <imucostumelib/CostumeRawIO.h>
#include <imucostumelib/ImuCostume.h>
#include <imucostumelib/CANopenSensor.h>
#include <imucostumelib/ImuSensor.h>
#include <boost/algorithm/hex.hpp>
#include <thread>
#include "IMUCostumeProfileEditionWizard.h"
#include <plugins/imuCostume/IIMUDataSource.h>
#include <QtWidgets/QProgressDialog>
#include <QtWidgets/QProgressBar>
#include "CostumeSkeletonMotionHelper.h"
#include <plugins/imuCostume/CostumeIMUExtractor.h>
#include "IMUCostumeProfileConfigurationWizard.h"
#include <QtWidgets/QInputDialog>
#include "RecordingWizard.h"
#include <fstream>
#include "CostumeParser.h"
#include <boost/uuid/uuid_io.hpp>
#include <boost/lexical_cast.hpp>
#include <acclaimformatslib/AsfParser.h>
#include <acclaimformatslib/AmcParser.h>
#include "ConnectCostumeInputDialog.h"

Q_DECLARE_METATYPE(imuCostume::CostumeRawIO::CostumeAddress);

typedef threadingUtils::StreamAdapterT<IMU::CostumeStream::value_type, IMU::SensorsStreamData, IMU::CostumeIMUExtractor> ExtractedCostumeStreamAdapter;

QIcon statusIcon(IMU::IIMUDataSource::ConnectionStatus connectionStatus)
{
	static const QIcon onlineIcon = QIcon(QString::fromUtf8(":/imuCostume/icons/dostepny.png"));
	static const QIcon offlineIcon = QIcon(QString::fromUtf8(":/imuCostume/icons/niedostepny.png"));
	static const QIcon connectionProblemsIcon = QIcon(QString::fromUtf8(":/imuCostume/icons/czesciowodostepny.png"));
	static const QIcon unknownIcon = QIcon(QString::fromUtf8(":/imuCostume/icons/nieznanydostepny.png"));


	switch (connectionStatus)
	{
	case IMU::IIMUDataSource::ONLINE:
		return onlineIcon;
	case IMU::IIMUDataSource::OFFLINE:
		return offlineIcon;
	case IMU::IIMUDataSource::CONNECTION_PROBLEMS:
		return connectionProblemsIcon;
	case IMU::IIMUDataSource::UNKNOWN:
		return unknownIcon;
	default:
		return unknownIcon;
	}
}

QString statusDescription(IMU::IIMUDataSource::ConnectionStatus connectionStatus)
{
	switch (connectionStatus)
	{
	case IMU::IIMUDataSource::ONLINE:
		return QObject::tr("online");
	case IMU::IIMUDataSource::OFFLINE:
		return QObject::tr("offline");
	case IMU::IIMUDataSource::CONNECTION_PROBLEMS:
		return QObject::tr("problematic");
	case IMU::IIMUDataSource::UNKNOWN:
		return QObject::tr("unknown");
	default:
		return QObject::tr("unknown");
	}
}

IMUCostumeWidget::IMUCostumeWidget(IMU::IMUCostumeDataSource * ds,
	QWidget * parent, const Qt::WindowFlags f)
	: QWidget(parent, f), ui(new Ui::IMUCostumeListWidget), ds(ds), recordIndex(0),
	recordingOutputDirectory(plugin::getPaths()->getPluginPath()/"recordings")
{
	ui->setupUi(this);
	ui->costumesTreeWidget->clear();
	ui->sensorsTree->clear();
	connect(&statusRefreshTimer, SIGNAL(timeout()), this, SLOT(refreshStatus()));
	connect(&recordTimer, SIGNAL(timeout()), this, SLOT(watchRecordedData()));

	if (utils::Filesystem::pathExists(recordingOutputDirectory) == false){
		try{
			utils::Filesystem::createDirectory(recordingOutputDirectory);
		}
		catch (...){
			PLUGIN_LOG_DEBUG("Failed to create recordings output directory: " << recordingOutputDirectory);
		}
	}
}

IMUCostumeWidget::~IMUCostumeWidget()
{
	delete ui;
}

void IMUCostumeWidget::innerCostumeChange(const imuCostume::CostumeRawIO::CostumeAddress & costumeID)
{
	ui->sensorsTree->clear();

	auto cd = ds->costumeDetails(costumeID);

	for (const auto & st : cd.descriptionDetails.sensorsConfiguration){
		for (const auto sid : st.second){
			auto sstatus = cd.statusDetails.sensorsStatus.find(sid)->second;
			auto sItem = new QTreeWidgetItem;
			sItem->setText(0, QObject::tr("Sensor %1").arg(sid));
			sItem->setData(0, Qt::UserRole, QVariant::fromValue(sid));
			sItem->setIcon(1, statusIcon(sstatus));
			sItem->setText(1, statusDescription(sstatus));
			ui->sensorsTree->addTopLevelItem(sItem);
		}
	}

	ui->recordPushButton->setEnabled(ds->costumeLoaded(costumeID));
}

void IMUCostumeWidget::onCostumeChange(QTreeWidgetItem * current, QTreeWidgetItem * previous)
{
	if (current != nullptr){
		auto id = current->data(0, Qt::UserRole).value<imuCostume::CostumeRawIO::CostumeAddress>();
		innerCostumeChange(id);
	}
}

void IMUCostumeWidget::onSensoresListContextMenu(const QPoint & position)
{
	auto cItem = ui->costumesTreeWidget->currentItem();
	auto sItem = ui->sensorsTree->currentItem();

	if (sItem != nullptr && cItem != nullptr){
		QMenu menu(this);

		auto resetStatus = menu.addAction(tr("Reset status"));
		connect(resetStatus, SIGNAL(triggered()), this, SLOT(onResetSensorConnectionStatus()));

		menu.exec(ui->sensorsTree->mapToGlobal(position));
	}
}

void IMUCostumeWidget::onConnectCostume()
{
	ConnectCostumeInputDialog dialog(this);

	auto ret = dialog.exec();

	if (ret == QDialog::Accepted){
		imuCostume::CostumeRawIO::CostumeAddress ca;
		ca.ip = dialog.ip().toStdString();
		ca.ip.erase(std::remove(ca.ip.begin(), ca.ip.end(), ' '), ca.ip.end());
		ca.port = dialog.port();

		auto cDetails = ds->costumesDetails();

		auto cIT = cDetails.find(ca);

		if (cIT != cDetails.end()){
			QMessageBox::warning(this, tr("Costume loaded"), tr("Requested costume already found"));
		}
		else{
			auto res = ds->addCostume(ca);
			if (res == true){
				auto ci = new QTreeWidgetItem;
				ci->setText(0, QString("%1:%2").arg(QString::fromStdString(ca.ip)).arg(ca.port));
				ci->setData(0, Qt::UserRole, QVariant::fromValue(ca));
				ci->setIcon(1, statusIcon(IMU::IIMUDataSource::ONLINE));
				ci->setText(1, statusDescription(IMU::IIMUDataSource::ONLINE));
				ui->costumesTreeWidget->addTopLevelItem(ci);
			}
			else{
				QMessageBox::critical(this, tr("Costume loading failed"), tr("Could not connect given costume - either costume is already connected or can not communicate with costume"));
			}
		}
	}
}

void IMUCostumeWidget::onCostumesListContextMenu(const QPoint & position)
{
	const auto cc = ds->costumesCout();

	QMenu menu(this);

	menu.addAction(ui->actionConnect_costume);

	if(cc > 0){	

		menu.addSeparator();

		auto lcc = ds->loadedCostumesCount();

		auto loadAll = menu.addAction(tr("Load all"));
		auto unloadAll = menu.addAction(tr("Unload all"));
		auto setSamplingRateAll = menu.addAction(tr("Set sampling rate all"));
		auto rsConfigAll = menu.addAction(tr("Refresh sensors configuration all"));
		auto resetStatusAll = menu.addAction(tr("Reset connection status all"));

		if (cc > ds->loadedCostumesCount()){
			connect(loadAll, SIGNAL(triggered()), this, SLOT(onLoadAll()));
			connect(setSamplingRateAll, SIGNAL(triggered()), this, SLOT(onSetSamplingRateAll()));
			connect(rsConfigAll, SIGNAL(triggered()), this, SLOT(onRefreshSensorsConfigurationAll()));
			connect(resetStatusAll, SIGNAL(triggered()), this, SLOT(onResetCostumeConnectionStatusAll()));
		}else{
			loadAll->setEnabled(false);
			setSamplingRateAll->setEnabled(false);
			rsConfigAll->setEnabled(false);
			resetStatusAll->setEnabled(false);			
		}

		if (lcc > 0){			
			connect(unloadAll, SIGNAL(triggered()), this, SLOT(onUnloadAll()));
		}else{
			unloadAll->setEnabled(false);
		}		

		if(ui->costumesTreeWidget->currentItem() != nullptr){

			auto profiles = ds->costumesProfiles();

			const auto id = ui->costumesTreeWidget->currentItem()->data(0, Qt::UserRole).value<imuCostume::CostumeRawIO::CostumeAddress>();

			menu.addSeparator();
					
			auto loadMenu = menu.addMenu(tr("Load"));

			auto newProfileLoad = loadMenu->addAction(tr("New profile"));
			
			if (profiles.empty() == false){

				loadMenu->addSeparator();

				for (const auto & profile : profiles)
				{
					auto a = loadMenu->addAction(QString::fromStdString(profile.first));					
					connect(a, SIGNAL(triggered()), this, SLOT(onLoadProfile()));
				}
			}

			auto unload = menu.addAction(tr("Unload"));
			auto setSamplingRate = menu.addAction(tr("Set sampling rate"));
			auto rsConfig = menu.addAction(tr("Refresh sensors configuration"));
			auto resetStatus = menu.addAction(tr("Reset connection status"));

			auto cl = ds->costumeLoaded(id);

			if (cl == false){
				connect(newProfileLoad, SIGNAL(triggered()), this, SLOT(onLoadNewProfile()));
				connect(setSamplingRate, SIGNAL(triggered()), this, SLOT(onSetSamplingRate()));
				connect(rsConfig, SIGNAL(triggered()), this, SLOT(onRefreshSensorsConfiguration()));
				connect(resetStatus, SIGNAL(triggered()), this, SLOT(onResetCostumeConnectionStatus()));
				unload->setEnabled(false);
			}
			else{
				connect(unload, SIGNAL(triggered()), this, SLOT(onUnload()));
				loadMenu->setEnabled(false);
				setSamplingRate->setEnabled(false);
				rsConfig->setEnabled(false);
				resetStatus->setEnabled(false);
			}		
		}
	}

	menu.exec(ui->costumesTreeWidget->mapToGlobal(position));
}

void IMUCostumeWidget::refreshStatus()
{
	auto cStatus = ds->costumesStatus();
	for (unsigned int i = 0; i < ui->costumesTreeWidget->topLevelItemCount(); ++i)
	{
		auto item = ui->costumesTreeWidget->topLevelItem(i);
		auto id = item->data(0, Qt::UserRole).value<imuCostume::CostumeRawIO::CostumeAddress>();
		auto it = cStatus.find(id);

		auto iS = IMU::IIMUDataSource::UNKNOWN;

		if (it != cStatus.end()){
			iS = it->second.status;

			if (item == ui->costumesTreeWidget->currentItem()){

				for (unsigned int j = 0; j < ui->sensorsTree->topLevelItemCount(); ++j)
				{
					auto sItem = ui->sensorsTree->topLevelItem(j);
					auto id = sItem->data(0, Qt::UserRole).value<imuCostume::Costume::SensorID>();
					auto iSS = IMU::IIMUDataSource::UNKNOWN;
					auto sit = it->second.sensorsStatus.find(id);
					if (sit != it->second.sensorsStatus.end())
					{
						iSS = sit->second;
					}

					sItem->setIcon(1, statusIcon(iSS));
					sItem->setText(1, statusDescription(iSS));
				}
			}
		}

		item->setIcon(1, statusIcon(iS));
		item->setText(1, statusDescription(iS));
	}
}

void IMUCostumeWidget::onRefresh()
{
	bool failed = false;
	QString message;
	try{
		coreUI::CoreCursorChanger cc;
		ds->refreshCostumes();
		ui->costumesTreeWidget->clear();
		ui->sensorsTree->clear();

		auto cDetails = ds->costumesDetails();

		for (const auto & cd : cDetails){
			auto ci = new QTreeWidgetItem;
			ci->setText(0, QString("%1:%2").arg(QString::fromStdString(cd.second.descriptionDetails.rawCostume->ip())).arg(cd.second.descriptionDetails.rawCostume->port()));
			ci->setData(0, Qt::UserRole, QVariant::fromValue(imuCostume::CostumeRawIO::CostumeAddress({ cd.second.descriptionDetails.rawCostume->ip(), cd.second.descriptionDetails.rawCostume->port() })));
			ci->setIcon(1, statusIcon(cd.second.statusDetails.status));
			ci->setText(1, statusDescription(cd.second.statusDetails.status));
			ui->costumesTreeWidget->addTopLevelItem(ci);
		}

		if (cDetails.empty() == true){
			statusRefreshTimer.stop();
		}
		else if (statusRefreshTimer.isActive() == false){
			statusRefreshTimer.start(1000 / 25);
		}
	}
	catch (std::exception & e)
	{
		failed = true;
		message = tr("Error while refreshing costumes list: ") + QString::fromStdString(e.what());
	}
	catch (...){
		failed = true;
		message = tr("Unknown error while refreshing costumes list - probably network failure");
	}

	if (failed == true)
	{
		QMessageBox::critical(this, tr("Refresh error"), message);
	}
}

void IMUCostumeWidget::onLoadNewProfile()
{
	const auto id = ui->costumesTreeWidget->currentItem()->data(0, Qt::UserRole).value<imuCostume::CostumeRawIO::CostumeAddress>();

	auto cd = ds->costumeDescription(id);
	const auto & sc = cd.sensorsConfiguration;
	imuCostume::Costume::SensorIDsSet sIDs;

	auto cit = sc.find(imuCostume::Costume::IMU);

	bool emptyConf = false;

	if (cit == sc.end()){
		emptyConf = true;
	}
	else if (cit->second.empty() == true){
		emptyConf = true;
	}

	if (emptyConf == true){
		QMessageBox::warning(this, tr("Empty costume configuration"), tr("Trying to load costume with no sensors in configuration. Refresh sensors configuration and try again."));
		return;
	}

	IMUCostumeProfileEditionWizard ew(cit->second,
		ds->orientationEstimationAlgorithms(),
		ds->calibrationAlgorithms(),
		ds->motionEstimationAlgorithms(),
		ds->skeletonModels(),
		tr("New profile"), this);

	auto res = ew.exec();

	if (res != QDialog::Accepted){
		return;
	}

	auto profile = utils::make_shared<IMU::CostumeProfile>(ew.costumeProfile());

	res = QMessageBox::question(this, tr("Save new profile"), tr("Would You like to save new profile?"));

	if (res == QMessageBox::Yes){
		const auto profiles = ds->costumesProfiles();
		bool ok = true;
		QString pName;
		while (ok == true){
			pName = QInputDialog::getText(this, tr("Profile name"), tr("Input new profile name"), QLineEdit::Normal, "", &ok);
			if (ok == false){

			}
			else if (pName.isEmpty() == true || profiles.find(pName.toStdString()) != profiles.end()){
				QMessageBox::information(this, tr("Profile name"), tr("Profile name must be unique and non empty - please try again"));
			}
			else{
				ok = false;
				profile->name = pName.toStdString();
				ds->registerCostumeProfile(profile);
				QMessageBox::information(this, tr("Profile registered"), tr("Profile registration complete"));
			}
		}
	}

	innerInitializeAndLoad(profile, cd, id);
}

void IMUCostumeWidget::onLoadProfile()
{
	QAction * action = qobject_cast<QAction*>(sender());
	if (action == nullptr){
		return;
	}

	const auto id = ui->costumesTreeWidget->currentItem()->data(0, Qt::UserRole).value<imuCostume::CostumeRawIO::CostumeAddress>();

	auto cd = ds->costumeDescription(id);
	const auto & sc = cd.sensorsConfiguration;
	imuCostume::Costume::SensorIDsSet sIDs;

	auto cit = sc.find(imuCostume::Costume::IMU);

	bool emptyConf = false;

	if (cit == sc.end()){
		emptyConf = true;
	}
	else if (cit->second.empty() == true){
		emptyConf = true;
	}

	if (emptyConf == true){
		QMessageBox::warning(this, tr("Empty costume configuration"), tr("Trying to load costume with no sensors in configuration. Refresh sensors configuration and try again."));
		return;
	}

	auto profiles = ds->costumesProfiles();
	
	auto it = profiles.find(action->text().toStdString());

	if (it == profiles.end()){
		QMessageBox::warning(this, tr("Non-existing profile"), tr("Selected profile is already unavailable, please try other profile"));
		return;
	}

	IMU::CostumeProfilePtr profile(it->second->clone());

	auto ret = 0;

	if (profile->skeleton != nullptr){
		ret = QMessageBox::question(this, tr("Profile edition?"), tr("Would You like to edit selected profile or use it directly?"), tr("Edit"), tr("Use"));
	}

	//czy edytujemy
	if (ret == 0){
		//edycja

		IMUCostumeProfileEditionWizard ew(cit->second,
			ds->orientationEstimationAlgorithms(),
			ds->calibrationAlgorithms(),
			ds->motionEstimationAlgorithms(),
			ds->skeletonModels(),			
			profile, this);		

		auto res = ew.exec();

		if (res != QDialog::Accepted){
			return;
		}

		profile.reset(ew.costumeProfile().clone());

		res = QMessageBox::question(this, tr("Save new profile"), tr("Would You like to save new profile?"));

		if (res == QMessageBox::Yes){
			bool ok = true;
			QString pName;
			while (ok == true){
				pName = QInputDialog::getText(this, tr("Profile name"), tr("Input new profile name"), QLineEdit::Normal, "", &ok);
				if (ok == false){

				}
				else if (pName.isEmpty() == true || profiles.find(pName.toStdString()) != profiles.end()){
					QMessageBox::information(this, tr("Profile name"), tr("Profile name must be unique and non empty - please try again"));
				}
				else{
					ok = false;
					profile->name = pName.toStdString();
					ds->registerCostumeProfile(profile);
					QMessageBox::information(this, tr("Profile registered"), tr("Profile registration complete"));
				}
			}			
		}
	}

	innerInitializeAndLoad(profile, cd, id);	
}

void IMUCostumeWidget::innerInitializeAndLoad(IMU::CostumeProfilePtr profile,
	IMU::IIMUDataSource::CostumeDescription &cd,
	const imuCostume::CostumeRawIO::CostumeAddress & id)
{
	PLUGIN_LOG_DEBUG("Profile creation done");

	//konfigurujemy algorytmy
	auto cw = IMU::IMUCostumeProfileConfigurationWizard::create(*profile);
	if (cw != nullptr){
		auto res = cw->exec();
		if (res != QDialog::Accepted){
			return;
		}

		PLUGIN_LOG_DEBUG("Profile configuration done");
	}

	//local mapping
	const auto localMapping = kinematic::LinearizedSkeleton::createNonLeafMapping(*(profile->skeleton));

	//iniclalizujemy algorytm kalibracji
	IMU::IMUCostumeCalibrationAlgorithm::SensorsDescriptions sa;

	for (const auto & sd : profile->sensorsDescriptions)
	{
		IMU::IMUCostumeCalibrationAlgorithm::SensorDescription localsd;
		localsd.jointName = sd.second.jointName;
		localsd.jointIdx = localMapping.data().right.find(localsd.jointName)->get_left();
		localsd.adjustment.offset = sd.second.adjustment.offset;
		localsd.adjustment.preMulRotation = sd.second.adjustment.preMulRotation;
		localsd.adjustment.postMulRotation = sd.second.adjustment.postMulRotation;

		sa.insert(IMU::IMUCostumeCalibrationAlgorithm::SensorsDescriptions::value_type(sd.first, localsd));
	}

	try{

		profile->calibrationAlgorithm->initialize(profile->skeleton, sa);

	}
	catch (const std::exception & e){
		QMessageBox::critical(this, tr("Calibration initialization failure"), tr("Could not initialize calibration algorithm. Reason: %1").arg(e.what()));
		return;
	}
	catch (...){
		QMessageBox::critical(this, tr("Calibration initialization failure"), tr("Could not initialize calibration algorithm. Unknown reason"));
		return;
	}

	PLUGIN_LOG_DEBUG("Calibration initialized");

	auto canOpenStream = utils::make_shared<threadingUtils::StreamAdapterT<IMU::RawDataStream::value_type, IMU::CANopenFramesStream::value_type, IMU::RawToCANopenExtractor>>(cd.rawDataStream);

	PLUGIN_LOG_DEBUG("canOpenStream created");

	//ilość próbek na rozgrzewkę algorytmów estymacji orientacji czujników
	unsigned int initializeFramesCount = 50;

	for (const auto & sa : profile->sensorsDescriptions)
	{
		initializeFramesCount = std::max(initializeFramesCount, sa.second.orientationEstimationAlgorithm->approximateEstimationDelay());
	}

	PLUGIN_LOG_DEBUG("Minimum samples for estimation algorithms done");

	auto costumeStream = utils::make_shared<threadingUtils::StreamAdapterT<IMU::CANopenFramesStream::value_type, IMU::CostumeStream::value_type, IMU::CANopenDataExtractor>>(canOpenStream);

	PLUGIN_LOG_DEBUG("costumeStream created");

	//mamy wszystko, jesteśmy po konfiguracji wszystkiego
	//można dalej konfigurować kostium - inicjalizować filtry, kalibrować i ładować całość do DataManager
	auto extractorAdapter = utils::make_shared<ExtractedCostumeStreamAdapter>(costumeStream, IMU::CostumeIMUExtractor(cd.sensorsConfiguration));

	PLUGIN_LOG_DEBUG("extractorAdapter created");

	auto calibSteps = std::min(profile->calibrationAlgorithm->maxCalibrationSteps(), 6000u);

	if (calibSteps <= 0){
		calibSteps = 6000;
	}

	{
		// inicjalizacja algorytmów estymacji orientacji czujników + kalibracja
		CostumeSkeletonMotionHelper csmh(extractorAdapter,
			profile, initializeFramesCount + calibSteps,
			initializeFramesCount, this);

		auto res = csmh.exec();

		//czy anulowano
		if (res == QDialog::Rejected){
			return;
		}

		//czy kalibracja ok?
		if (csmh.isComplete() == false){
			QMessageBox::information(this, tr("Costume initialization failed"), tr("Failed to calibrate costume with given configuration. Change configuration and try again or simply retry."));
			return;
		}
	}

	// pobieramy efekt kalibracji
	auto calibrationAdjustments = profile->calibrationAlgorithm->sensorsAdjustemnts();

	for (const auto & ca : calibrationAdjustments)
	{
		auto it = sa.find(ca.first);
		auto pit = profile->sensorsDescriptions.find(ca.first);
		pit->second.adjustment.offset = it->second.adjustment.offset = ca.second.offset;
		pit->second.adjustment.preMulRotation = it->second.adjustment.preMulRotation = ca.second.preMulRotation;
		pit->second.adjustment.postMulRotation = it->second.adjustment.postMulRotation = ca.second.postMulRotation;
	}

	PLUGIN_LOG_DEBUG("Costume initialization done");

	coreUI::CoreCursorChanger cc;

	try{
		//inicjalizacja algorytmu estymacji szkieletu
		profile->motionEstimationAlgorithm->initialize(profile->skeleton, sa);
		//ładowanie
		ds->loadCalibratedCostume(id, profile);
		ui->recordPushButton->setEnabled(true);
		PLUGIN_LOG_DEBUG("Costume loaded");
	}
	catch (...){
		QMessageBox::critical(this, tr("Failed to load calibrated costume"), tr("Internal error"));
	}
}

void IMUCostumeWidget::onUnload()
{
	coreUI::CoreCursorChanger cc;
	const auto id = ui->costumesTreeWidget->currentItem()->data(0, Qt::UserRole).value<imuCostume::CostumeRawIO::CostumeAddress>();
	ds->unloadCostume(id);
	ui->recordPushButton->setEnabled(ds->loadedCostumesCount() > 0);
	recordingDetails.erase(id);
}

void IMUCostumeWidget::onLoadAll()
{
	//ds->loadAllCostumes();
}

void IMUCostumeWidget::onUnloadAll()
{
	coreUI::CoreCursorChanger cc;
	ui->recordPushButton->setEnabled(false);
	for (unsigned int i = 0; i < ui->costumesTreeWidget->topLevelItemCount(); ++i){
		const auto id = ui->costumesTreeWidget->currentItem()->data(i, Qt::UserRole).value<imuCostume::CostumeRawIO::CostumeAddress>();
		ds->unloadCostume(id);
		recordingDetails.erase(id);
	}	
}

void IMUCostumeWidget::onLoadDatFile()
{
    //ds->testMethod();
    //return;
    QString file = QFileDialog::getOpenFileName(this, tr("Open Image"), QString(), tr("Imu file (*.dat)"));
    utils::Filesystem::Path p(file.toStdString());
    if (utils::Filesystem::isRegularFile(p)) {
        //ds->loadDatFile(p);
    }
    
}

void IMUCostumeWidget::testCommunication()
{
	try{

		auto costumeList = imuCostume::CostumeRawIO::listAvailableCostumes();

		if (costumeList.empty() == false){
			auto costumeAddress = costumeList.front();

			std::unique_ptr<imuCostume::CostumeRawIO> rawCostume(new imuCostume::CostumeRawIO(costumeAddress.ip));
		}
	}
	catch (std::exception & e){
		
	}
	catch (...){
		
	}
}

unsigned int IMUCostumeWidget::getSamplingRate()
{
	return QInputDialog::getInt(this, tr("Data sampling rate"), tr("Enter sampling rate in milliseconds [ms]: "), 20, 10, 2550, 1);
}

void IMUCostumeWidget::onSetSamplingRate()
{
	coreUI::CoreCursorChanger cc;
	auto id = ui->costumesTreeWidget->currentItem()->data(0, Qt::UserRole).value<imuCostume::CostumeRawIO::CostumeAddress>();
	try{
		auto desc = ds->costumeDescription(id);
		auto delay = getSamplingRate();
		desc.rawCostume->setSamplingDelay(delay);
	}
	catch (...){

	}
}

void IMUCostumeWidget::onSetSamplingRateAll()
{
	auto delay = getSamplingRate();
	coreUI::CoreCursorChanger cc;

	for (unsigned int i = 0; i < ui->costumesTreeWidget->topLevelItemCount(); ++i)
	{
		auto id = ui->costumesTreeWidget->topLevelItem(i)->data(0, Qt::UserRole).value<imuCostume::CostumeRawIO::CostumeAddress>();
		try{
			auto desc = ds->costumeDescription(id);
			desc.rawCostume->setSamplingDelay(delay);
		}
		catch (...){

		}
	}
}

unsigned int IMUCostumeWidget::getSensorsConfigurationSamplesCount()
{
	return QInputDialog::getInt(this, tr("Refresh sensors configuration"), tr("Enter number of samples used for detecting sensors configuration: "), 50, 1, 200, 1);
}

void IMUCostumeWidget::onRefreshSensorsConfiguration()
{
	auto id = ui->costumesTreeWidget->currentItem()->data(0, Qt::UserRole).value<imuCostume::CostumeRawIO::CostumeAddress>();
	auto sc = getSensorsConfigurationSamplesCount();
	coreUI::CoreCursorChanger cc;
	try{
		bool res = ds->refreshCostumeSensorsConfiguration(id, sc);
		if (res == true){
			innerCostumeChange(id);
		}
	}
	catch (...){

	}
}

void IMUCostumeWidget::onRefreshSensorsConfigurationAll()
{
	imuCostume::CostumeRawIO::CostumeAddress currentID;
	auto ci = ui->costumesTreeWidget->currentItem();
	if (ci != nullptr){
		if (ci->data(0, Qt::UserRole).canConvert<imuCostume::CostumeRawIO::CostumeAddress>() == true){
			currentID = ci->data(0, Qt::UserRole).value<imuCostume::CostumeRawIO::CostumeAddress>();
		}
	}

	auto sc = getSensorsConfigurationSamplesCount();
	coreUI::CoreCursorChanger cc;
	for (unsigned int i = 0; i < ui->costumesTreeWidget->topLevelItemCount(); ++i)
	{
		auto id = ui->costumesTreeWidget->topLevelItem(i)->data(0, Qt::UserRole).value<imuCostume::CostumeRawIO::CostumeAddress>();
		try{
			bool res = ds->refreshCostumeSensorsConfiguration(id, sc);
			if (res == true && id == currentID){
				innerCostumeChange(id);
			}
		}
		catch (...){

		}
	}
}

void IMUCostumeWidget::onResetCostumeConnectionStatus()
{
	auto id = ui->costumesTreeWidget->currentItem()->data(0, Qt::UserRole).value<imuCostume::CostumeRawIO::CostumeAddress>();	
	coreUI::CoreCursorChanger cc;
	try{
		ds->resetCostumeStatus(id);		
	}
	catch (...){

	}

	refreshStatus();
}

void IMUCostumeWidget::onResetSensorConnectionStatus()
{
	auto ci = ui->sensorsTree->currentItem();
	auto costumeID = ui->costumesTreeWidget->currentItem()->data(0, Qt::UserRole).value<imuCostume::CostumeRawIO::CostumeAddress>();
	auto sensorID = ci->data(0, Qt::UserRole).value<imuCostume::Costume::SensorID>();
	coreUI::CoreCursorChanger cc;
	try{
		ds->resetSensorStatus(costumeID, sensorID);
		ci->setIcon(1, statusIcon(IMU::IIMUDataSource::UNKNOWN));
		ci->setText(1, statusDescription(IMU::IIMUDataSource::UNKNOWN));
	}
	catch (...){

	}	
}

void IMUCostumeWidget::onResetCostumeConnectionStatusAll()
{
	coreUI::CoreCursorChanger cc;
	for (unsigned int i = 0; i < ui->costumesTreeWidget->topLevelItemCount(); ++i)
	{
		auto id = ui->costumesTreeWidget->topLevelItem(i)->data(0, Qt::UserRole).value<imuCostume::CostumeRawIO::CostumeAddress>();
		try{
			ds->resetCostumeStatus(id);			
		}
		catch (...){

		}
	}

	refreshStatus();
}

void IMUCostumeWidget::saveMotionData()
{
	IMU::IIMUDataSource::CostumesRecordingDataBuffer::ListT data;
	recordOutput->costumesDataBuffer.data(data);

	IMU::CostumeParser::save(*outputFile, data, costumesMapping, recordIndex);

	auto id = recordIndex;

	//serializujemy najpierw do pamięci - powinno iść dużo szybciej
	std::map<imuCostume::CostumeRawIO::CostumeAddress, std::stringstream> streams;

	//PLUGIN_LOG_DEBUG("Data frame size: " << data.size());

	//iteruje po ramkach
	for (const auto & d : data)
	{
		//PLUGIN_LOG_DEBUG("d costumes size: " << d.size());
		//iteruje po kostiumach w ramce
		for (const auto & dd : d){			
			auto it = recordingDetails.find(dd.first);
			auto & output = streams[dd.first];

			kinematic::SkeletonState::NonRigidCompleteStateLocal state;			

			PLUGIN_LOG_DEBUG("dd skeleton data size: " << d.size());

			for (const auto & sd : dd.second.skeletonData)
			{
				state.data().push_back(sd.localState);
			}
			
			kinematic::SkeletonState::applyState(*(it->second.skeleton), state);
			auto localRigidState = kinematic::SkeletonState::localRigidState(*(it->second.skeleton));

			//PLUGIN_LOG_DEBUG("localRigidState size: " << localRigidState.data().orientations.size());
			//PLUGIN_LOG_DEBUG("acclaim skeleton bones size: " << it->second.acclaimSkeleton.bones.size());
			//PLUGIN_LOG_DEBUG("active mapping size: " << it->second.activeMapping.data().size());
			//PLUGIN_LOG_DEBUG("helper motion data size: " << it->second.helperMotionData.size());

			acclaim::MotionData::FrameData fd;
			fd.bonesData = kinematic::SkeletonState::convert(it->second.acclaimSkeleton, localRigidState, it->second.activeMapping, it->second.helperMotionData);
			//PLUGIN_LOG_DEBUG("acclaim frame data size: " << fd.bonesData.size());
			fd.id = id;

			//korekcja ujemnych zer
			for (auto & bd : fd.bonesData)
			{
				for (auto & cv : bd.channelValues)
				{
					cv = utils::correctNegativeZero(cv);
				}
			}
			acclaim::AmcParser::serialize(fd, output);			
		}

		++id;
	}

	//teraz zrzucam dane z pamięci do plików
	for (const auto & sData : streams)
	{
		auto it = recordingDetails.find(sData.first);
		(*(it->second.motionOutput)) << sData.second.rdbuf();
	}

	//flushuje wszystkie pliki
	for (auto & rd : recordingDetails)
	{
		rd.second.motionOutput->flush();
	}

	recordIndex += data.size();
	outputFile->flush();
}

void IMUCostumeWidget::watchRecordedData()
{
	if (recordOutput->costumesDataBuffer.size() > 0.5 * recordOutput->costumesDataBuffer.maxBufferSize())
	{
		saveMotionData();
	}
}

static void serialize(std::ostream & stream, const osg::Vec3 & vec)
{
	stream << vec.x() << " " << vec.y() << " " << vec.z();
}

static void serialize(std::ostream & stream, const osg::Quat & orient)
{
	stream << orient.x() << " " << orient.y() << " " << orient.z() << " " << orient.w();
}

void serializeRecordedCostumeConfiguration(std::ostream & stream,
	const imuCostume::CostumeRawIO::CostumeAddress & costumeID,
	const imuCostume::Costume::SensorIDsSet & sensorsToRecord,
	const IMU::IIMUDataSource::CostumeDescription & cd)
{
	auto profile = cd.profile;
	stream << "Costume " << costumeID.ip << " " << costumeID.port << std::endl;
	stream << "DataRate " << std::to_string(cd.rawCostume->samplingDelay()) << std::endl;
	stream << "SkeletalModel " << profile->skeleton->root()->value().name() << " ";
	serialize(stream, profile->skeleton->root()->value().localPosition());
	stream << " ";
	serialize(stream, profile->skeleton->root()->value().localOrientation());
	stream << " " << profile->skeleton->root()->size() - 1 << std::endl;

	auto visitor = [&stream](kinematic::Skeleton::JointConstPtr joint)
	{
		stream << joint->parent()->value().name() << " " << joint->value().name() << " ";
		serialize(stream, joint->value().localPosition());
		stream << " ";
		serialize(stream, joint->value().localOrientation());
		stream << std::endl;
	};

	for (const auto & c : profile->skeleton->root()->children())
	{
		treeContainer::VisitPolicies::Node::PreOrder::visit(c, visitor);
	}

	stream << "MappingDetails " << sensorsToRecord.size() << std::endl;

	for (const auto & sID : sensorsToRecord)
	{
		auto it = profile->sensorsDescriptions.find(sID);
		stream << std::to_string(sID) << " " << it->second.jointName << " ";
		serialize(stream, it->second.adjustment.offset);
		stream << " ";
		serialize(stream, it->second.adjustment.preMulRotation);
		stream << " ";
		serialize(stream, it->second.adjustment.postMulRotation);
		stream << " " << boost::lexical_cast<std::string>(it->second.orientationEstimationAlgorithm->ID()) << std::endl;
	}

	stream << "CalibrationAlgorithm " << boost::lexical_cast<std::string>(profile->calibrationAlgorithm->ID()) << std::endl;
	stream << "MotionAlgorithm " << boost::lexical_cast<std::string>(profile->motionEstimationAlgorithm->ID()) << std::endl;
}

void IMUCostumeWidget::onRecord(const bool record)
{
	if (record == true){		

		RecordingWizard::CostumesToRecord costumes;
		IMU::CostumeParser::CostumesMappings cm;

		for (unsigned int i = 0; i < ui->costumesTreeWidget->topLevelItemCount(); ++i)
		{
			auto costumeID = ui->costumesTreeWidget->topLevelItem(i)->data(0, Qt::UserRole).value<imuCostume::CostumeRawIO::CostumeAddress>();
			auto cd = ds->costumeDescription(costumeID);
			costumes.insert({ costumeID, cd.sensorsConfiguration.find(imuCostume::Costume::IMU)->second });
			cm.insert({ costumeID, IMU::CostumeParser::createMapping(cd.profile->sensorsDescriptions, kinematic::LinearizedSkeleton::createCompleteMapping(*(cd.profile->skeleton))) });

			auto & costumeSensors = costumes.find(costumeID)->second;
			auto & profileSensors = cm.find(costumeID)->second;

			auto it = costumeSensors.begin();

			while (it != costumeSensors.end()){

				auto fIT = cd.profile->sensorsDescriptions.find(*it);

				if (fIT == cd.profile->sensorsDescriptions.end()){
					it = costumeSensors.erase(it);
				}
				else if(fIT->second.jointIdx == -1 || fIT->second.jointName.empty() == true ||
				fIT->second.orientationEstimationAlgorithm == nullptr){
					it = costumeSensors.erase(it);
					profileSensors.erase(*it);
				}
				else{
					++it;
				}
			}
		}

		costumesMapping = cm;

		//RecordingWizard w(costumes, this);

		//auto ret = w.exec();

		//if (ret == QWizard::Accepted){

			coreUI::CoreCursorChanger cc;

			const auto now = std::chrono::system_clock::now();

			//faktyczne kostiumy do nagrań
			//costumes = w.costumes();
			//dla każdego kostiumu
			for (const auto & c : costumes)
			{
				//czy mam już nazwe katalogu dla kostiumu i aktualnych nagrań?
				auto it = recordingDetails.find(c.first);
				if (it == recordingDetails.end()){
					auto cd = ds->costumeDescription(c.first);
					RecordingDetails rd;
					rd.counter = 0;
					rd.skeleton = utils::make_shared<kinematic::Skeleton>(*(cd.profile->skeleton));
					rd.path = recordingOutputDirectory / recordingDir(c.first, now);					
					rd.mapping = kinematic::LinearizedSkeleton::createCompleteMapping(*(cd.profile->skeleton));
					rd.acclaimSkeleton.name = c.first.ip;
					const auto angleType = kinematicUtils::Rad;
					rd.acclaimSkeleton.units.setAngleType(angleType);
					kinematic::Skeleton::convert(*(rd.skeleton), rd.acclaimSkeleton, rd.acclaimSkeleton.axisOrder, angleType);
					rd.acclaimSkeleton.bones[rd.acclaimSkeleton.root].axisOrder = rd.acclaimSkeleton.axisOrder;
					rd.acclaimSkeleton.bones[rd.acclaimSkeleton.root].dofs = acclaim::Bone::defaultRootDofs(angleType);
					const auto localMapping = kinematic::LinearizedSkeleton::createNonLeafMapping(*(rd.skeleton));
					//PLUGIN_LOG_DEBUG("cd.profile->activeJoints size: " << cd.profile->activeJoints.size());
					for (const auto id : cd.profile->activeJoints)
					{
						auto lIT = localMapping.data().left.find(id);
						if (lIT != localMapping.data().left.end()){
							const auto name = lIT->get_right();
							//PLUGIN_LOG_DEBUG("found joint ID: " << id << " name " << name);
							auto it = std::find_if(rd.acclaimSkeleton.bones.begin(), rd.acclaimSkeleton.bones.end(),
								[&name, &localMapping](const acclaim::Skeleton::Bones::value_type & boneData)
							{
								return name == boneData.second.name;
							});

							if (it != rd.acclaimSkeleton.bones.end()){
								//PLUGIN_LOG_DEBUG("found bone");
								if (it->first != rd.acclaimSkeleton.root){
									//PLUGIN_LOG_DEBUG("setup bone");
									it->second.dofs = acclaim::Bone::defaultRotationDofs(angleType);
								}
							}
							else{
								//PLUGIN_LOG_DEBUG("NOT found bone");
							}
						}
						else{
							//PLUGIN_LOG_DEBUG("could not find joint ID: " << id);
						}
					}

					rd.helperMotionData = acclaim::Skeleton::helperMotionData(rd.acclaimSkeleton);
					{
						const auto skeletonMapping = kinematic::LinearizedSkeleton::createNonLeafMapping(*(rd.skeleton));
						//PLUGIN_LOG_DEBUG("skeleton size: " << rd.skeleton->root()->size());
						//PLUGIN_LOG_DEBUG("skeletonMapping size: " << skeletonMapping.data().size());
						for (const auto & bd : rd.acclaimSkeleton.bones)
						{
							PLUGIN_LOG_DEBUG("bone " << bd.second.name << " active " << (bd.second.isActive() ? "true" : "false"));
							if (bd.second.isActive() == true){
								auto it = skeletonMapping.data().right.find(bd.second.name);
								if (it == skeletonMapping.data().right.end())
								{
									//PLUGIN_LOG_DEBUG("bone " << bd.second.name << " not found in mapping");
								}
							}
						}
					}

					rd.activeMapping = kinematic::SkeletonState::createAcclaimActiveMappingLocal(*(rd.skeleton), rd.acclaimSkeleton.bones);
					//PLUGIN_LOG_DEBUG("rd.activeMapping size: " << rd.activeMapping.data().size());
					//PLUGIN_LOG_DEBUG("rd.acclaimSkeleton.bones size: " << rd.acclaimSkeleton.bones.size());

					it = recordingDetails.insert({ c.first, rd }).first;
				}

				//sprawdź czy katalog istnieje - jak nie to utwórz
				if (utils::Filesystem::pathExists(it->second.path) == false){
					utils::Filesystem::createDirectory(it->second.path);
				}

				auto files = utils::Filesystem::listFiles(it->second.path);

				//sprawdź czy konfiguracja istnieje - jak nie to utwórz
				{
					bool cfgExists = false;
					
					if (files.empty() == false){
						cfgExists = (files.end() != std::find_if(files.begin(), files.end(), [](const utils::Filesystem::Path & path)
						{
							return path.extension().string() == ".ccfg";
						}));
					}

					if (cfgExists == false){
						std::ofstream outputCfgFile((it->second.path / "costumeConfiguration.ccfg").string());
						if (outputCfgFile.is_open() == true){
							auto cd = ds->costumeDescription(c.first);
							serializeRecordedCostumeConfiguration(outputCfgFile, c.first, c.second, cd);
						}
					}
				}

				//sprawdź czy szkielet istnieje - jak nie to utwórz
				{
					bool skeletonExists = false;

					if (files.empty() == false){
						skeletonExists = (files.end() != std::find_if(files.begin(), files.end(), [](const utils::Filesystem::Path & path)
						{
							return path.extension().string() == ".asf";
						}));
					}

					if (skeletonExists == false){
						std::ofstream outputSkeletonFile((it->second.path / ("skeleton.asf")).string());
						if (outputSkeletonFile.is_open() == true){
							acclaim::AsfParser::serialize(outputSkeletonFile, it->second.acclaimSkeleton);
						}
					}
				}

				//nowy plik AMC z danymi ruchu do zapisu
				auto counter = ++it->second.counter;
				auto op = (it->second.path / ("recording_" + std::to_string(counter)+".amc")).string();
				it->second.motionOutput = utils::make_shared<std::ofstream>(op.c_str());
				acclaim::AmcParser::initSerialize(*(it->second.motionOutput), kinematicUtils::Deg);

			
				//outputFile = utils::make_shared<std::ofstream>(w.outputPath().toStdString().c_str());

				outputFile = utils::make_shared<std::ofstream>((it->second.path / ("recording_" + std::to_string(counter) + ".imudata")).string().c_str());

				//if (outputFile->is_open() == true){					

					for (const auto & c : costumes)
					{
						auto cd = ds->costumeDescription(c.first);
						serializeRecordedCostumeConfiguration(*outputFile, c.first, c.second, cd);
					}

					(*outputFile) << "Data" << std::endl;

					recordOutput = utils::make_shared<IMU::IIMUDataSource::RecordingConfiguration>();
					recordOutput->costumesDataBuffer.setMaxBufferSize(10000);
					//recordOutput->costumesToRecord = w.costumes();
					recordOutput->costumesToRecord = costumes;

					recordTimer.start(500);
					recordIndex = 0;
					ds->startRecording(recordOutput);
				//}
				//else{

				//	//TODO - info że się nie udało jednak pliku stworzyć do zapisu
				//	ui->recordPushButton->blockSignals(true);
				//	ui->recordPushButton->setChecked(!record);
				//	ui->recordPushButton->blockSignals(false);
				//}
			}
			/*}
			else{
			ui->recordPushButton->blockSignals(true);
			ui->recordPushButton->setChecked(!record);
			ui->recordPushButton->blockSignals(false);
			}*/

			ui->recordPushButton->setText(tr("Recording..."));
	}
	else{
		coreUI::CoreCursorChanger cc;
		recordTimer.stop();
		ds->stopRecording(recordOutput);

		//kończymy zapis danych

		saveMotionData();

		outputFile->close();
		outputFile.reset();
		recordOutput.reset();

		for (auto & rd : recordingDetails)
		{
			rd.second.motionOutput->close();
		}

		ui->recordPushButton->setText(tr("Record"));
	}
}

void IMUCostumeWidget::onOptions()
{
	auto dir = QFileDialog::getExistingDirectory(this, tr("Recording output directory"), QString::fromStdString(recordingOutputDirectory.string()));

	if (dir.isEmpty() == false){
		recordingOutputDirectory = dir.toStdString();
	}
}

void IMUCostumeWidget::onOpen()
{
	auto dir = QFileDialog::getExistingDirectory(this, tr("Recorded data"), QString::fromStdString(recordingOutputDirectory.string()));

	if (dir.isEmpty() == false){
		coreUI::CoreCursorChanger cursorChanger;
		//TODO
		//weryfikacja czy mamy asf amc
		auto path = utils::Filesystem::Path(dir.toStdString());
		auto files = utils::Filesystem::listFiles(path);

		utils::Filesystem::Path asfFile;
		utils::Filesystem::PathsList amcFiles;
		utils::Filesystem::Path cfgFile;

		for (const auto & file : files)
		{
			if (asfFile.empty() == true && file.extension() == ".asf"){
				asfFile = file;
			}

			if (file.extension() == ".amc"){
				amcFiles.push_back(file);
			}

			if (cfgFile.empty() == true && file.extension() == ".ccfg"){
				cfgFile = file;
			}
		}

		if (amcFiles.empty() == true || asfFile.empty() == true){
			//INFO że nie mamu kompletnych danych
			return;
		}

		for (const auto & amcFile : amcFiles)
		{
			ds->loadRecordedData(asfFile, amcFile, cfgFile);
		}
	}
}

std::string IMUCostumeWidget::recordingDir(const imuCostume::CostumeRawIO::CostumeAddress & id,
	const std::chrono::system_clock::time_point & now)
{	
	const auto now_t = std::chrono::system_clock::to_time_t(now);
	std::stringstream ss;

#if defined(_WINDOWS)
	ss << std::put_time(std::localtime(&now_t), "%Y%m%d_%H%M%S");
#else
	// put_time nie jest zaimplementowany w gcc 4.7
	auto tm = *std::localtime(&now_t);
	ss << std::setfill('0');
	// tm_year to lata od 1900
	ss << (1900 + tm.tm_year);
	// tm_mon jest z przedzialu 0,11
	ss << std::setw(2) <<(tm.tm_mon + 1);
	ss << std::setw(2) << tm.tm_mday << "_";
	ss << std::setw(2) << tm.tm_hour;
	ss << std::setw(2) << tm.tm_min;
	ss << std::setw(2) << tm.tm_sec << "\n";
#endif
	ss << "_" << id.ip << "_" << id.port;

	return ss.str();
}
