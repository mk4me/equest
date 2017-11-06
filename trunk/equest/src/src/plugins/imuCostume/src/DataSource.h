/********************************************************************
	created:  2013/11/15
	created:  15:11:2013   13:54
	filename: DataSource.h
	author:   Mateusz Janiak

	purpose:  èrÛd≥o danych obs≥ugujπce kostiumy z IMU
	*********************************************************************/
#ifndef HEADER_GUARD_IMU_COSTUME__DATASOURCE_H__
#define HEADER_GUARD_IMU_COSTUME__DATASOURCE_H__

#include <corelib/ISource.h>
#include <osg/Vec3>
#include <mutex>
#include <list>
#include <imucostumelib/ImuCostume.h>
#include <imucostumelib/CostumeCANopenIO.h>
#include <imucostumelib/ProtocolSendBufferHelper.h>
#include <corelib/HierarchyItem.h>
#include <corelib/IThreadPool.h>
#include <corelib/HierarchyDataItem.h>
#include <plugins/imuCostume/CostumeIMUExtractor.h>
#include <plugins/hmdbCommunication/IHMDBShallowCopyContext.h>
#include <utils/SamplesStatus.h>
#include "CostumeParser.h"

typedef threadingUtils::StreamAdapterT<IMU::CostumeStream::value_type, IMU::SensorsStreamData, IMU::CostumeIMUExtractor> ExtractedCostumeStreamAdapter;

namespace IMU
{
	DEFINE_SMART_POINTERS(ExtractedCostumeStreamAdapter);

	class IMUCostumeDataSource : public plugin::ISource, public IIMUDataSource
	{
		UNIQUE_ID("{441BB894-1019-4382-97EE-F18A511A49CB}");
		CLASS_DESCRIPTION("IMUCostume", "IMU Costume Data Source");

	private:

		struct SensorData
		{
			utils::shared_ptr<utils::SamplesStatus> samplesStatus;
			core::VariantsList domainData;
		};

		typedef std::map<imuCostume::Costume::SensorID, SensorData> SensorsData;

		struct CostumeData : public CostumeDescription
		{
			utils::shared_ptr<utils::SamplesStatus> samplesStatus;
			SensorsData sensorsData;
			CANopenFramesStreamPtr CANopenStream;
			CostumeStreamPtr costumeStream;
			ExtractedCostumeStreamAdapterPtr completeImuStream;

			CostumeSkeletonMotionPtr skeletonMotion;

			core::HierarchyItemPtr hierarchyRootItem;
			core::VariantsList domainData;		
		};

		typedef std::map<CostumeID, CostumeData> CostumesData;

	public:
		//! Konstruktor
		IMUCostumeDataSource();
		//! Destruktor wirtualny
		virtual ~IMUCostumeDataSource();

		//! Inicjalizacja ürÛd≥a. NastÍpuje juø po wczytaniu pluginÛw i skonstruowaniu
		//! (nie zainicjalizowaniu!) wszystkich ürÛde≥.
		virtual void init(core::IDataManager * memoryDM,
			core::IStreamDataManager * streamDM,
			core::IFileDataManager * fileDM,
			core::IDataHierarchyManager * hierarchyDM);

		//! Zobacz podobnπ metode dla IService
		//! PÛüna inicjalizacja ürÛd≥a, nastÍpuje po wczytaniu i inicjalizacji wszystkich innych ürÛde≥
		virtual bool lateInit();

		//! Metoda powinna w bezpieczny sposÛb zwalniac zasoby, majπc na uwadze øe niekoniecznie wszystkie us≥ugi i zasoby pobrane z zewnπtrz sπ jeszcze dostÍpne.
		//! Ta metoda w szczegolnoscis powinna zamknac wszystkie watki, ktÛre uruchomi≥ serwis, moøe tez zwalniac pamieÊ przydzielonπ dynamicznie
		//! Generalnie to taki bezpieczny destruktor uniezaleøniajπcy dana us≥ugÍ od pozosta≥ych us≥ug i przygotowujπcy ja na usuniÍcie
		virtual void finalize();

		//! Metoda aktualizujπca pochodzi z wπtku UI! Powinny tu byÊ realizowane lekkie operacje odúwieøania widgetÛw!!
		//! Jako parametr dostajemy przyrost czasu jaki mina≥ od poprzedniego wywo≥ania
		virtual void update(double deltaTime);

		//! èrÛd≥o nie musi mieÊ wizualnej reprezentacji.
		//! \return Widget tworzony przez ürÛd≥o bπdü NULL.
		virtual QWidget* getWidget();
		//! \return Widget kontrolujπcy zachowanie us≥ugi/us≥ug zaleønych.
		virtual QWidget* getControlWidget();
		//! \return Widget dostarczajπcy opcji zwiπzanych z us≥ugπ/us≥ugami zaleønymi.
		virtual QWidget* getSettingsWidget();
		//! \param offeredTypes Typy oferowane przez to ürÛd≥o
		virtual void getOfferedTypes(utils::TypeInfoList & offeredTypes) const;
		
		virtual bool addCostume(const CostumeID & id) override;

		virtual const bool refreshCostumes() override;

		virtual const unsigned int costumesCout() const override;

		virtual const bool costumesEmpty() const override;

		virtual bool refreshCostumeSensorsConfiguration(const CostumeID & id, const uint8_t samplesCount) override;
		
		virtual void resetCostumeStatus(const CostumeID & id) override;

		virtual void resetSensorStatus(const CostumeID & costumeID,
			const imuCostume::Costume::SensorID sensorID) override;

		virtual CostumeStatus costumeStatus(const CostumeID & id) const override;

		virtual CostumeDetails costumeDetails(const CostumeID & id) const override;

		virtual CostumesDescriptions costumesDescriptions() const override;

		virtual CostumesStatus costumesStatus() const override;

		virtual CostumesDetails costumesDetails() const override;

		virtual CostumeDescription costumeDescription(const CostumeID & id) const override;

		virtual void loadRawCostume(const CostumeID & id) override;

		virtual void loadCalibratedCostume(const CostumeID & id,
			CostumeProfilePtr profile) override;

		virtual void unloadCostume(const CostumeID & id);

		virtual unsigned int loadedCostumesCount() const override;

		virtual bool costumeLoaded(const CostumeID & id) const override;

		virtual core::ConstVariantsList costumeData(const CostumeID & id) const override;

		virtual void startRecording(RecordingConfigurationPtr recording) override;
		virtual void stopRecording(RecordingConfigurationPtr recording) override;

		virtual void registerOrientationEstimationAlgorithm(IIMUOrientationEstimationAlgorithmPtr algorithm) override;
		virtual void registerCostumeCalibrationAlgorithm(IMUCostumeCalibrationAlgorithmPtr algorithm) override;
		virtual void registerMotionEstimationAlgorithm(IMUCostumeMotionEstimationAlgorithmPtr algorithm) override;
		virtual void registerSkeletonModel(SkeletonPtr skeleton) override;
		virtual void registerCostumeProfile(CostumeProfilePtr profile) override;
		
		virtual OrientationEstimationAlgorithms orientationEstimationAlgorithms() const override;
		virtual CostumeCalibrationAlgorithms calibrationAlgorithms() const override;
		virtual CostumeMotionEstimationAlgorithms motionEstimationAlgorithms() const override;
		virtual SkeletonModels skeletonModels() const override;
		virtual CostumesProfiles costumesProfiles() const override;

		hmdbCommunication::IHMDBShallowCopyContextPtr selectUploadContext() const;

		void uploadSession(const utils::Filesystem::Path & configuration,
			const utils::Filesystem::FilesList & recordings);

		virtual void loadRecordedData(const utils::Filesystem::Path & asfFile,
			const utils::Filesystem::Path & amcFile,
			const utils::Filesystem::Path & configFile) override;

	private:

		static CostumeStatus innerCreateCostumeStatus(const IMUCostumeDataSource::CostumeData & cData);

		core::HierarchyItemPtr fillRawCostumeData(CostumeData & cData);

		static void configureCostume(CostumeDescription & cd);
		static bool innerRefreshCostumeSensorsConfiguration(CostumeData & data, const uint8_t MaxSamplesCount);

		void innerLoadCostume(const unsigned int idx);
		void innerUnloadCostume(const unsigned int idx);		

		/*static core::HierarchyItemPtr createRootItem();
		static core::HierarchyItemPtr createStreamItem();
		static core::HierarchyItemPtr createRecordItem();
		static core::HierarchyItemPtr createIMUsItem();*/

		void generateCostumeItem(const unsigned int idx,
			const core::VariantsList & data,
			core::HierarchyItemPtr parent);

		/*void tryCreateRootItem();

		void tryCreateStreamItem();
		void tryCreateRecordedItem();*/

		void resfreshCostumesData();

		void tryRecord(std::set<imuCostume::CostumeRawIO::CostumeAddress> &costumes);

		void updateSensorsStatus(const imuCostume::CostumeCANopenIO::Frame &frame, const uint16_t length, const CostumeData &cd);


		static void unpackSensorsStream(SensorsStreamPtr stream,
			SensorsData & sensorsData,
			core::IHierarchyItemPtr root,
			core::VariantsList & domainData,
			CostumeProfileConstPtr profile);

	private:

		//! Czy koÒczymy dzia≥anie wπtku odúwieøajπcego dane
		volatile bool finish;
		//! Wπtek odúwieøajπcy dane i status kostiumÛw
		core::Thread refreshThread;
		//! Obiekt synchronizujπcy
		mutable std::recursive_mutex synch;		
		//! Manager danych w pamiÍci
		core::IDataManager * memoryDM;
		//! Manager danych w pamiÍci
		core::IDataHierarchyManager * hierarchyDM;
		//! Manager danych w plikach
		core::IFileDataManager * fileDM;
		//! Korzen drzewa dla analiz
		//core::HierarchyItemPtr root;
		//! Dane strumieniowe
		//core::HierarchyItemPtr streamItems;
		//! Dane nagrane
		core::HierarchyItemPtr recordedItems;

		CostumesData costumesData;
		std::set<RecordingConfigurationPtr> recordings;

		OrientationEstimationAlgorithms orientationEstimationAlgorithms_;
		CostumeCalibrationAlgorithms calibrationAlgorithms_;
		CostumeMotionEstimationAlgorithms motionEstimationAlgorithms_;
		SkeletonModels skeletonModels_;
		CostumesProfiles costumesProfiles_;
	};
}

#endif	//	HEADER_GUARD_IMU_COSTUME__DATASOURCE_H__
