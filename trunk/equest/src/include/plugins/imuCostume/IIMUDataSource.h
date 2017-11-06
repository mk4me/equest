/********************************************************************
	created:  2014/12/03	12:35:25
	filename: IIMUDataSource.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_IMU__IIMUDATASOURCE_H__
#define __HEADER_GUARD_IMU__IIMUDATASOURCE_H__

#include <corelib/IIdentifiable.h>
#include <corelib/Variant.h>
#include <plugins/imuCostume/Streams.h>
#include <plugins/imuCostume/IIMUOrientationEstimationAlgorithm.h>
#include <plugins/imuCostume/IMUCostumeMotionEstimationAlgorithm.h>
#include <kinematiclib/SkeletonState.h>
#include <plugins/imuCostume/CostumeProfile.h>
#include <utils/Filesystem.h>

namespace IMU
{
	//! Interfejs Ÿród³a danych obs³uguj¹cego kostiumy
	class IIMUDataSource
	{
	public:

		//! Typ statusu po³¹czenia z kostiumem i czujników
		enum ConnectionStatus
		{
			ONLINE,				//! Po³¹czenie OK
			OFFLINE,			//! Brak po³¹czenia
			CONNECTION_PROBLEMS,//! Problemy z po³¹czeniem - gubimy pakiety
			UNKNOWN				//! Nieznany stan - jeszcze siê nie próbowlaiœmy pod³anczaæ
		};	

		//! Typ opisuj¹cy identyfikator kostiumy
		typedef imuCostume::CostumeRawIO::CostumeAddress CostumeID;

		//! Zapisywane dane sensora
		struct RecordedSensorData : SensorData
		{
			//! Estymowana orientacja
			osg::Quat estimatedOrientation;
		};

		struct RecordedJointData
		{
			kinematic::SkeletonState::NonRigidJointState localState;
			kinematic::SkeletonState::NonRigidJointState globalState;
		};

		//! Zapisywane dane kostiumu
		struct RecordedCostumeData
		{
			//! Czas
			imuCostume::CostumeCANopenIO::Timestamp timestamp;
			//! Dane sensorów
			std::map<imuCostume::Costume::SensorID, RecordedSensorData> sensorsData;
			//! Dane szkieletu
			std::vector<RecordedJointData> skeletonData;
		};

		//! Mapa pakietów wg kostiumów
		typedef std::map<CostumeID, RecordedCostumeData> CostumesDataFrame;
		//! Strumieñ danych pakietów kostiumów
		typedef threadingUtils::StreamBufferT<CostumesDataFrame> CostumesRecordingDataBuffer;

		typedef std::map<imuCostume::CostumeRawIO::CostumeAddress, imuCostume::Costume::SensorIDsSet> CostumesToRecord;

		//! Struktura opisuj¹ca zapis danych z kostiumów
		struct RecordingConfiguration
		{
			//! Strumieñ buforów z danymi kostiumów
			CostumesRecordingDataBuffer costumesDataBuffer;
			//! Zbiór nagrywanych kostiumów
			CostumesToRecord costumesToRecord;
		};

		DEFINE_SMART_POINTERS(RecordingConfiguration);

		//! Struktura opisuj¹ca status po³¹czenia kostiumu i czujników
		struct CostumeStatus
		{
			//! Status po³¹czenie z kostiumem
			volatile ConnectionStatus status;
			//! Status poszczególnych czujników
			std::map<imuCostume::Costume::SensorID, ConnectionStatus> sensorsStatus;
		};

		//! Mapa statusów kostiumów i ich sensorów
		typedef std::map<CostumeID, CostumeStatus> CostumesStatus;

		//! Struktura opisuj¹ca kostium
		struct CostumeDescription
		{
			//! Konfiguracja czujników
			imuCostume::Costume::SensorsConfiguration sensorsConfiguration;
			//! Obiekt obs³uguj¹cy komunikacjê z kostirumem w trybie raw (wyslij/odbierz surowe pakiety danych)
			utils::shared_ptr<imuCostume::CostumeRawIO> rawCostume;
			//! Strumieñ surowych danych
			RawDataStreamPtr rawDataStream;
			//! Profil kosiumu
			CostumeProfilePtr profile;
		};

		//! Mapa statusów kostiumów i ich sensorów
		typedef std::map<CostumeID, CostumeDescription> CostumesDescriptions;

		//! Struktura opisuj¹ca szczegó³y kostiumu
		struct CostumeDetails
		{
			CostumeStatus statusDetails;
			CostumeDescription descriptionDetails;
		};

		//! Mapa pe³nej informajci o kostiumach
		typedef std::map<CostumeID, CostumeDetails> CostumesDetails;

		//! Mapa identyfikatorów sensorów do algorytmów estymujacych ich otientacjê
		typedef std::map<imuCostume::Costume::SensorID, IIMUOrientationEstimationAlgorithmConstPtr> OrientationEstimationPrototypeAlgorithmsMapping;

		//! Mapa identyfikatorów sensorów do algorytmów estymujacych ich otientacjê
		typedef std::map<imuCostume::Costume::SensorID, IIMUOrientationEstimationAlgorithmPtr> OrientationEstimationAlgorithmsMapping;
		
		//! Mapa algorytmów estymacji orientacji czujników
		typedef std::map<core::UniqueID, IIMUOrientationEstimationAlgorithmConstPtr> OrientationEstimationAlgorithms;
		//! Mapa algorytmów kalibracyjnych kostium
		typedef std::map<core::UniqueID, IMUCostumeCalibrationAlgorithmConstPtr> CostumeCalibrationAlgorithms;
		//! Mapa algorytmów estymuj¹cych ruchu
		typedef std::map<core::UniqueID, IMUCostumeMotionEstimationAlgorithmConstPtr> CostumeMotionEstimationAlgorithms;
		//! Lista modeli (szkieletów, hierarchii)
		typedef std::map<core::UniqueID, SkeletonConstPtr> SkeletonModels;
		//! Mapa profili kostiumów
		typedef std::map<std::string, CostumeProfileConstPtr> CostumesProfiles;

	public:
		//! Desturktor wirtualny
		virtual ~IIMUDataSource() {}

		//! \param id Indeks kostiumu
		//! \return Czy dany kostium jest za³adowany
		virtual bool addCostume(const CostumeID & id) = 0;
		//! Metoda odœwie¿a dostepne kostiumu
		//! \return Prawda jeœli uda³o siê odœwie¿yæ kostiumy
		virtual const bool refreshCostumes() = 0;
		//! \return Iloœæ dostepnych kostiumów
		virtual const unsigned int costumesCout() const = 0;
		//! \return Iloœæ dostepnych kostiumów
		virtual const bool costumesEmpty() const = 0;
		//! \param id Indeks kostiumu
		//! \return Czy dany kostium jest za³adowany
		virtual bool costumeLoaded(const CostumeID & id) const = 0;
		//! \return Iloœæ za³adowanych kostiumóe
		virtual unsigned int loadedCostumesCount() const = 0;
		//! \param id Identyfikator kosstiumu
		//! \param samplesCount iloœæ prób pozyskania danych do wyciagniecia konfiguracji kostiumu
		//! \return Czy uda³o siê chocia¿ z jednej próbki wyci¹gnaæ konfiguracjê
		virtual bool refreshCostumeSensorsConfiguration(const CostumeID & id, const uint8_t samplesCount) = 0;
		//! \param id Indeks kostiumu
		//! \return Opis kostiumu
		virtual CostumeDescription costumeDescription(const CostumeID & id) const = 0;

		//! \param id Indeks kostiumu
		virtual void resetCostumeStatus(const CostumeID & id) = 0;
		//! \param costumeID Indeks kostiumu
		//! \param sensorID Indeks kostiumu
		virtual void resetSensorStatus(const CostumeID & costumeID,
			const imuCostume::Costume::SensorID sensorID) = 0;
		//! \param id Indeks kostiumu
		//! \return Opis kostiumu
		virtual CostumeStatus costumeStatus(const CostumeID & id) const = 0;
		//! \param id Indeks kostiumu
		//! \return Opis kostiumu
		virtual CostumeDetails costumeDetails(const CostumeID & id) const = 0;
		//! \return Opis kostiumu
		virtual CostumesDescriptions costumesDescriptions() const = 0;
		//! \return Opis kostiumu
		virtual CostumesStatus costumesStatus() const = 0;
		//! \return Opis kostiumu
		virtual CostumesDetails costumesDetails() const = 0;
		//! Metoda ³aduj¹ca kostium do DM - tylko surowe dane + konwersja
		//! \param id Indeks kostiumu
		virtual void loadRawCostume(const CostumeID & id) = 0;
		//! Metoda ³aduj¹ca kostium do DM - surowe dane + konwersja + estymacja ruchu po kalibracji
		//! \param id Indeks kostiumu
		//! \param profileInstance Profil kostiumu
		virtual void loadCalibratedCostume(const CostumeID & id,
			CostumeProfilePtr profile) = 0;

		//! Metoda wy³¹dowywujê dane z DM dla kostiumu
		//! \param id Indeks kostiumu
		virtual void unloadCostume(const CostumeID & id) = 0;
		//! \param id Indeks kostiumu
		//! \return Dane domenowe kostiumu z DM
		virtual core::ConstVariantsList costumeData(const CostumeID & id) const = 0;

		//! \param algorithm Prototyp algorytmu estymacji orientacji czujnika
		virtual void registerOrientationEstimationAlgorithm(IIMUOrientationEstimationAlgorithmPtr algorithm) = 0;
		//! \param algorithm Prototyp algorytmu kalibracji kostiumu
		virtual void registerCostumeCalibrationAlgorithm(IMUCostumeCalibrationAlgorithmPtr algorithm) = 0;
		//! \param algorithm Prototyp algorytmu poprawy estymacji ruchu kostiumu dla ca³ego szkieletu
		virtual void registerMotionEstimationAlgorithm(IMUCostumeMotionEstimationAlgorithmPtr algorithm) = 0;
		//! \param skeleton Model szkieletu który mo¿na estymowaæ
		virtual void registerSkeletonModel(SkeletonPtr skeleton) = 0;
		//! \param profile Profil kostiumu
		virtual void registerCostumeProfile(CostumeProfilePtr profile) = 0;

		virtual void startRecording(RecordingConfigurationPtr recording) = 0;
		virtual void stopRecording(RecordingConfigurationPtr recording) = 0;

		//! \return Lista prototypów algorytmów estymacji orientacji czujnika
		virtual OrientationEstimationAlgorithms orientationEstimationAlgorithms() const = 0;
		//! \return Lista prototypów algorytmów kalibracji kostiumu
		virtual CostumeCalibrationAlgorithms calibrationAlgorithms() const = 0;
		//! \return Lista prototypów algorytmów poprawy estymacji ruchu kostiumu dla ca³ego szkieletu
		virtual CostumeMotionEstimationAlgorithms motionEstimationAlgorithms() const = 0;
		//! \return Lista prototypów algorytmów poprawy estymacji ruchu kostiumu dla ca³ego szkieletu
		virtual SkeletonModels skeletonModels() const = 0;
		//! \return Profile kostiumu
		virtual CostumesProfiles costumesProfiles() const = 0;

		virtual void loadRecordedData(const utils::Filesystem::Path & asfFile,
			const utils::Filesystem::Path & amcFile,
			const utils::Filesystem::Path & configFile) = 0;
	};
}

#endif	// __HEADER_GUARD_IMU__IIMUDATASOURCE_H__