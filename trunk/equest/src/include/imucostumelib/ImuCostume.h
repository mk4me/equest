/********************************************************************
	created:  2014/01/09
	created:  9:1:2014   8:32
	filename: ImuCostume.h
	author:   Mateusz Janiak

	purpose:
	*********************************************************************/
#ifndef HEADER_GUARD_IMUCOSTUME__IMUCOSTUME_H__
#define HEADER_GUARD_IMUCOSTUME__IMUCOSTUME_H__

#include <imucostumelib/Export.h>
#include <list>
#include <set>
#include <map>
#include <osg/Vec3>
#include <osg/Quat>
#include <utils/SmartPtr.h>
#include <imucostumelib/CostumeCANopenIO.h>
#include <imucostumelib/ProtocolSendBufferHelper.h>

namespace imuCostume
{
	//! Klasa obs³uguj¹ca kostium
	class IMUCOSTUME_EXPORT Costume
	{
	public:

		//! Typ identyfikatora sensora
		using SensorID = uint8_t;

		//! Typ wyliczeniowy obs³ugiwanych czujników przez kostium
		enum SensorType
		{
			UNKNOWN = -1,	//! Nieznany typ
			IMU = 1,		//! Czujnik IMU
			INSOLE = 2		//! Czujnik INSOLE
		};

		//! Generyczny sensor - baza dla pozosta³ych
		class IMUCOSTUME_EXPORT GenericSensor
		{
		protected:
			//! \param id Identyfikator CANopen sensora
			//! \param type Typ sensora
			GenericSensor(const SensorID id, const int type);

		public:
			//! Destruktor wirtualny
			virtual ~GenericSensor();
			//! \return Identyfikator czujnika
			SensorID id() const;
			//! \return Typ czujnika
			int type() const;

		private:
			//! Identyfikator wêz³a
			const SensorID id_;
			//! Typ czujnika
			const int type_;
		};

		using SensorDataPtr = utils::shared_ptr<GenericSensor>;

		//! Klasa opisuj¹ca dane czujników IMU
		class IMUCOSTUME_EXPORT IMUSensor : public GenericSensor
		{
		public:

			//! Flagi statusu danych - czy poszczególne dane s¹ dostepne do odczytu w aktualnej ramce
			enum DataStatus
			{
				ACC_DATA = 0x01,	//! Akcelerometr
				GYRO_DATA = 0x02,	//! Gyroskop
				MAG_DATA = 0x04,	//! Magnetometer
				ORIENT_DATA = 0x08	//! Orientacja
			};

		public:

			//! \param id Identyfikator CANopen sensora
			//! \param type Typ sensora
			//! \param accelerometer
			//! \param magnetometer
			//! \param gyroscope
			//! \param orientation
			IMUSensor(const SensorID id, const int dataStatus,
				const osg::Vec3 & accelerometer, const osg::Vec3 & magnetometer,
				const osg::Vec3 & gyroscope, const osg::Quat & orientation);

			//! Destruktor wirtualny
			virtual ~IMUSensor();

			//! \return Dane akcelerometru 
			const osg::Vec3 & accelerometer() const;
			//! \return Dane magnetometru 
			const osg::Vec3 & magnetometer() const;
			//! \return Dane ¿yroskopu 
			const osg::Vec3 & gyroscope() const;
			//! \return Estymowana orientacja na czujniku
			const osg::Quat & orientation() const;
			//! \return Status danych
			int dataStatus() const;

		private:
			const int dataStatus_;
			const osg::Vec3 accelerometer_;
			const osg::Vec3 magnetometer_;
			const osg::Vec3 gyroscope_;
			const osg::Quat orientation_;
		};

		//! Klasa opisuj¹ca dane czujników INSOLE
		class IMUCOSTUME_EXPORT INSOLESensor : public GenericSensor
		{
		public:

			//! Struktura danych czujnika
			struct INSOLEData
			{
				//! Identyfikator punktu nacisku
				unsigned int id;
				//! Wektor si³y
				osg::Vec3 grf;
			};

			//! Lista punktów nacisku
			using INSOLESData = std::list<INSOLEData>;

		public:

			//! \param id Identyfikator sensora
			//! \param insolesData Dane punktów naciskowych
			INSOLESensor(const SensorID id, const INSOLESData & insolesData);

			//! Destruktor wirtualny
			virtual ~INSOLESensor();
			//! \return Dane punktów naciskowych
			const INSOLESData & insolesData() const;

		private:
			//! Dane punktów naciskowych
			const INSOLESData insolesData_;
		};		

		//! Zbiór identyfikatorów sensorów
		using SensorIDsSet = std::set<SensorID>;

		//! Konfiguracja sensorów kostiumu
		using SensorsConfiguration = std::map<SensorType, SensorIDsSet>;

		//! Struktura opisuj¹ca rozpakowane dane sensorów
		struct IMUCOSTUME_EXPORT Data
		{
			//! Znacznik czasu ramki
			CostumeCANopenIO::Timestamp timestamp;
			//! Dane sensorów
			std::list<SensorDataPtr> sensorsData;
		};

	public:

		//! \param data WskaŸnik bufora danych do analizy z surowymi ramkami
		//! \param length Rozmiar bufora z danymi [B]
		//! \return Konfiguracja sensorów kostiumu
		static SensorsConfiguration sensorsConfiguration(const void * data, const uint16_t length);
		//! \param data Surowe dane CANopen
		//! \return Dane sensorów
		static std::list<SensorDataPtr> extractSensorsData(const std::list<CANopenFrame> & data);
		//! \param data WskaŸnik bufora danych do analizy z surowymi ramkami
		//! \param length Rozmiar bufora z danymi [B]
		//! \return Dane sensorów
		static std::list<SensorDataPtr> extractSensorsData(const void * buffer, const uint16_t length);
		//! Metoda przygotowuje kostium do dzia³ania - wy³¹cza komunikaty heartbeat,
		//! przestawia TPDO w trym asynchroniczny (zrzut z danych niezale¿nie od dostêpnoœci pozosta³ych
		//! czujników + brak zrzucania wartoœci domyœlnych)
		//! \param sendBuffer Bufor danych do wysy³ki
		static void prepareCostumeConfiguration(ProtocolSendBufferHelper & sendBuffer);
		//! To samo co poprzednia metoda ale konfigurujemy ju¿ tylko specyficzne sensory a nie wszystkie
		static void prepareCostumeConfiguration(ProtocolSendBufferHelper & sendBuffer,
			const SensorsConfiguration & sensorsConfiguration);
		//! \param data Dane rozpakowane z protoko³u (w formie ramek CANopen)
		//! \return Rozpakowane dane sensorów
		static Data convert(const CostumeCANopenIO::Data & data);
	};
}

#endif	//	HEADER_GUARD_IMUCOSTUME__IMUCOSTUME_H__
