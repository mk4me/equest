/********************************************************************
    created:  2014/01/02
    created:  2:1:2014   12:27
    filename: IMUCostumeListWidget.h
    author:   Mateusz Janiak
    
    purpose:  Widget zarz¹dzaj¹cy danymi kostiumów
*********************************************************************/
#ifndef HEADER_GUARD_IMU__IMUCOSTUMELISTWIDGET_H__
#define HEADER_GUARD_IMU__IMUCOSTUMELISTWIDGET_H__

#include <QtWidgets/QWidget>
#include <QtCore/QTimer>
//#include <objectwrapperlib/ObjectWrapper.h>
#include <threadingUtils/StreamData.h>
#include <imucostumelib/ProtocolSendBufferHelper.h>
#include <imucostumelib/ImuCostume.h>
#include <corelib/Variant.h>
#include <plugins/imuCostume/IIMUDataSource.h>
#include "CostumeParser.h"
#include <utils/Filesystem.h>

namespace Ui {
	class IMUCostumeListWidget;
}
class QTreeWidgetItem;

namespace IMU {
	class IMUCostumeDataSource;
}

class IMUCostumeWidget : public QWidget
{

	Q_OBJECT

private:

	struct CostumeStreams 
	{
		utils::shared_ptr<threadingUtils::IStreamT<imuCostume::ProtocolSendBufferHelper::Buffer>> rawStream;
		utils::shared_ptr<threadingUtils::ResetableStreamStatusObserver> rawStreamObserver;
		utils::shared_ptr<threadingUtils::IStreamT<imuCostume::Costume::Data>> costumeStream;
		utils::shared_ptr<threadingUtils::ResetableStreamStatusObserver> costumeStreamObserver;
	};

	struct RecordingDetails
	{
		utils::Filesystem::Path path;
		uint8_t counter;
		utils::shared_ptr<std::ofstream> motionOutput;
		kinematic::SkeletonPtr skeleton;
		acclaim::Skeleton acclaimSkeleton;
		acclaim::Skeleton::HelperMotionData helperMotionData;
		kinematic::SkeletonState::AcclaimActiveSkeletonMappingLocal activeMapping;
		kinematic::LinearizedSkeleton::GlobalMapping mapping;
	};

public:
	IMUCostumeWidget(IMU::IMUCostumeDataSource * ds, QWidget * parent = nullptr, const Qt::WindowFlags f = 0);
	virtual ~IMUCostumeWidget();

private slots:	

	void onConnectCostume();
	void onSensoresListContextMenu(const QPoint & position);
	void onCostumesListContextMenu(const QPoint & position);
	void onCostumeChange(QTreeWidgetItem * current, QTreeWidgetItem * previous);

	void onRecord(const bool record);
	void onLoadNewProfile();
	void onLoadProfile();

	void innerInitializeAndLoad(IMU::CostumeProfilePtr profile,
		IMU::IIMUDataSource::CostumeDescription & cd,
		const imuCostume::CostumeRawIO::CostumeAddress & id);

	void onUnload();
	void onSetSamplingRate();
	void onRefreshSensorsConfiguration();
	void onResetCostumeConnectionStatus();
	void onResetSensorConnectionStatus();

	void onLoadAll();
	void onUnloadAll();	
	void onSetSamplingRateAll();
	void onRefreshSensorsConfigurationAll();
	void onResetCostumeConnectionStatusAll();

    void onLoadDatFile();

	void testCommunication();

	void onRefresh();
	void refreshStatus();

	void watchRecordedData();

	void onOptions();

	void onOpen();

private:

	void tryLoadCostume(IMU::CostumeProfile profile);

	unsigned int getSamplingRate();
	unsigned int getSensorsConfigurationSamplesCount();

	void innerCostumeChange(const imuCostume::CostumeRawIO::CostumeAddress & costumeID);

	static std::string recordingDir(const imuCostume::CostumeRawIO::CostumeAddress & id,
		const std::chrono::system_clock::time_point & now);

	void saveMotionData();

private:
	utils::shared_ptr<std::ofstream> outputFile;
	IMU::IIMUDataSource::RecordingConfigurationPtr recordOutput;	
	QTimer recordTimer;
	QTimer statusRefreshTimer;
	IMU::IMUCostumeDataSource * ds;
	Ui::IMUCostumeListWidget * ui;
	std::size_t recordIndex;
	utils::Filesystem::Path recordingOutputDirectory;
	std::map<imuCostume::CostumeRawIO::CostumeAddress, RecordingDetails> recordingDetails;
	IMU::CostumeParser::CostumesMappings costumesMapping;
};

#endif	//	HEADER_GUARD_IMU__IMUCOSTUMELISTWIDGET_H__