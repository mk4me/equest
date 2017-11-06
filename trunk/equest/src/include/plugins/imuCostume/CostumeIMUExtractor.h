/********************************************************************
	created:  2015/01/04	21:15:37
	filename: CostumeIMUExtractor.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_IMU__COSTUMEIMUEXTRACTOR_H__
#define __HEADER_GUARD_IMU__COSTUMEIMUEXTRACTOR_H__

#include <plugins/imuCostume/Export.h>
#include <imucostumelib/ImuCostume.h>
#include <plugins/imuCostume/IIMUDataSource.h>
#include <plugins/kinematic/Wrappers.h>

namespace IMU
{
	//! Klasa realizująca filtrację kompletnych danych IMU (pełne dane dla wszystkich czujników IMU)
	class IMU_EXPORT CostumeCompleteDataFilter
	{
	public:
		//! \param Other Obiekt przejmowany
		CostumeCompleteDataFilter(CostumeCompleteDataFilter && Other);
		//! \param Other Obiekt kopiowany
		CostumeCompleteDataFilter(const CostumeCompleteDataFilter & Other);
		//! \param sensorsIDs Identyfikatory sensorów IMU
		CostumeCompleteDataFilter(const imuCostume::Costume::SensorIDsSet & sensorsIDs);
		//! \param sensorsConfiguration Konfiguracja sensorów
		CostumeCompleteDataFilter(const imuCostume::Costume::SensorsConfiguration & sensorsConfiguration);
		//! Desturktor
		~CostumeCompleteDataFilter();
		//! \param dataToFilter Dane do filtracji
		//! \return Prawda jeżeli dane są kompletne
		bool operator()(const IMU::CostumeStream::value_type & dataToFilter) const;

	private:
		//! Zbiór identyfikatorów sensorów IMU
		imuCostume::Costume::SensorIDsSet sensorsIDs;
	};

	//! Klasa realizująca konwersję ze strumienia danych czujników do kompletnych danych IMU
	class IMU_EXPORT CostumeIMUExtractor
	{
	private:

		struct SensorDataWithStatus : public SensorData
		{
			//! Status danych
			int status;
		};

	public:
		//! \param Other Obiekt przejmowany
		CostumeIMUExtractor(CostumeIMUExtractor && Other);
		//! \param Other Obiekt kopiowany
		CostumeIMUExtractor(const CostumeIMUExtractor & Other);
		//! \param sensorsIDs Identyfikatory czujników IMU
		CostumeIMUExtractor(const imuCostume::Costume::SensorIDsSet & sensorsIDs);
		//! \param sensorsConfiguration Konfiguracja sensorów
		CostumeIMUExtractor(const imuCostume::Costume::SensorsConfiguration & sensorsConfiguration);
		//! Destruktor
		~CostumeIMUExtractor();
		//! \param dataToVerify Dane do veryfikacji pd kątem możliwości konwersji
		//! \return Prawda jeżeli dane można konwertować
		bool verify(const IMU::CostumeStream::value_type & dataToVerify) const;
		//! \param convertedData [out] Dane po konwersji
		void extract(const IMU::CostumeStream::value_type & dataToConvert, IMU::SensorsStreamData & convertedData) const;

	private:
		//! Obiekt realizujący weryfikację
		CostumeCompleteDataFilter completeDataFilter;
		//aktualne dane do wysyłki dalej w przetwarzanie
		mutable std::map<imuCostume::Costume::SensorID, SensorDataWithStatus> currentData;
	};

	//! Klasa procesora strumienia estymująca orientacje czujników na bazie pomiarów
	class IMU_EXPORT OrientationEstimator
	{
	public:
		//! \param orientationAlgorithms Algorytmy estymujące orientacje czujników
		OrientationEstimator(IMU::CostumeProfilePtr profile);
		//! Destruktor
		~OrientationEstimator();
		//! \param [out] data Dane do estymacji
		void operator()(IMU::SensorsStreamData & data) const;		

	private:
		//! Profil z algorytmami które estymują orientacje czujników
		IMU::CostumeProfilePtr profile;
		//! Mapa z ostatnimi czasami aktualizacji poszczególnych sensorów
		mutable std::map<imuCostume::Costume::SensorID, uint32_t> lastUpdateTime;
	};

	//! Klasa pozwalająca wypakowywać dane z wektorów
	class IMU_EXPORT ArrayExtractor
	{
	public:
		//! \param idx Index obiketu który chcemy wypakowywać z wektora
		ArrayExtractor(const std::size_t idx);
		//! \param Other Inny kopiowany extractor
		ArrayExtractor(const ArrayExtractor & Other);
		//! Destruktor
		~ArrayExtractor();

		//! \tparam AT Wektor do weryfikacji
		template<typename AT>
		//! \param a Wektor z którego chcemy wypakować dane
		//! \return Czy z wektora da się wypakować dane o zadanym indeksie
		bool verify(const AT & a) const
		{
			return true;
		}

		//! \tparam AT Wektor do weryfikacji
		//! \tparam Ret Typ który chcemy wypakować z wektora
		template<typename AT, typename Ret>
		//! \param Wektor z którego wypakowujemy dane
		//! \param ret [out] Obiekt docelowy dla wypakowanych danych
		void extract(const AT & a, Ret & ret) const
		{
			ret = a[idx];
		}

	private:
		//! Indeks spod którego wybieramy dane z wektora
		const std::size_t idx;
	};

	//! Klasa pozwalająca wypakowywać dane z wektorów
	class IMU_EXPORT CompoundArrayExtractor
	{
	public:
		//! \param idx Index obiketu który chcemy wypakowywać z wektora
		CompoundArrayExtractor(const std::size_t idx);
		//! \param Other Inny kopiowany extractor
		CompoundArrayExtractor(const CompoundArrayExtractor & Other);
		//! Destruktor
		~CompoundArrayExtractor();

		//! \tparam AT Wektor do weryfikacji
		template<typename AT>
		//! \param a Wektor z którego chcemy wypakować dane
		//! \return Czy z wektora da się wypakować dane o zadanym indeksie
		bool verify(const AT & a) const
		{
			return true;
		}

		//! \tparam AT Wektor do weryfikacji
		//! \tparam Ret Typ który chcemy wypakować z wektora
		template<typename AT, typename Ret>
		//! \param Wektor z którego wypakowujemy dane
		//! \param ret [out] Obiekt docelowy dla wypakowanych danych
		void extract(const AT & a, Ret & ret) const
		{
			ret.first = a.first;
			ret.second = a.second[idx];
		}

	private:
		//! Indeks spod którego wybieramy dane z wektora
		const std::size_t idx;
	};


	class IMU_EXPORT OrientationExtractor
	{
	public:
		OrientationExtractor(const unsigned int idx);
		OrientationExtractor(const OrientationExtractor & Other);
		~OrientationExtractor();

		bool verify(const IMU::MotionStream::value_type & a) const;

		void extract(const IMU::MotionStream::value_type & a, osg::Quat & ret) const;

	private:
		const std::size_t idx;
	};

	class IMU_EXPORT CANopenDataExtractor
	{
	public:

		CANopenDataExtractor();
		~CANopenDataExtractor();

		bool verify(const IMU::CANopenFramesStream::value_type & a);

		void extract(const IMU::CANopenFramesStream::value_type & a, IMU::CostumeStream::value_type & ret) const;
	};

	class ArrayStreamAdapter
	{
	public:

		template<typename Base, typename Dest = decltype(std::declval<Base>()[0])>
		static threadingUtils::StreamAdapterT<Base, Dest, ArrayExtractor> * create(typename threadingUtils::StreamAdapterT<Base, Dest, ArrayExtractor>::BaseStreamTypePtr baseStream, const std::size_t idx)
		{
			return new threadingUtils::StreamAdapterT<Base, Dest, ArrayExtractor>(baseStream, ArrayExtractor(idx));
		}
	};	

	class IMU_EXPORT ExtractCostumeMotion
	{
	public:
		ExtractCostumeMotion(ExtractCostumeMotion&& other);
		ExtractCostumeMotion(IMU::CostumeProfilePtr profile);

		~ExtractCostumeMotion();

		bool verify(const IMU::SensorsStreamData & input) const;

		void extract(const IMU::SensorsStreamData & input, IMU::MotionStream::value_type & output) const;

	private:
		IMU::CostumeProfilePtr profile;
		mutable kinematic::Skeleton skeleton;
		mutable imuCostume::CostumeCANopenIO::Timestamp previousTime;		
	};

	class IMU_EXPORT KinematicStreamExtractor
	{
	public:
		KinematicStreamExtractor(kinematic::Skeleton && skeleton);
		KinematicStreamExtractor(KinematicStreamExtractor&& other);
		~KinematicStreamExtractor();

		bool verify(const IMU::MotionStream::value_type & input) const;

		void extract(const IMU::MotionStream::value_type & input, IMU::SkeletonStateStream::value_type & output) const;

		const kinematic::Skeleton & skeleton() const;

	private:

		mutable kinematic::Skeleton skeleton_;
	};

	class IMU_EXPORT RawToCANopenExtractor
	{
	public:

		RawToCANopenExtractor();
		~RawToCANopenExtractor();

		bool verify(const IMU::RawDataStream::value_type & val) const;

		void extract(const IMU::RawDataStream::value_type & in, IMU::CANopenFramesStream::value_type & out) const;
	};

	TIMEMEMBER_EXTRACTOR(accelerometer);
	TIMEMEMBER_EXTRACTOR(magnetometer);
	TIMEMEMBER_EXTRACTOR(gyroscope);
	TIMEMEMBER_EXTRACTOR(orientation);

	class IMUExtractor
	{
	public:

		IMUExtractor(const imuCostume::Costume::SensorID id) : id(id) {}
		~IMUExtractor() {}

		bool verify(const IMU::SensorsStreamData & a)
		{
			return a.sensorsData.find(id) != a.sensorsData.end();
		}

		void extract(const IMU::SensorsStreamData & a, IMU::IMUStream::value_type & ret) const
		{
			ret.first = a.timestamp;
			ret.second = a.sensorsData.find(id)->second;
		}

	private:

		const imuCostume::Costume::SensorID id;
	};
}

#endif	// __HEADER_GUARD_IMU__COSTUMEIMUEXTRACTOR_H__
