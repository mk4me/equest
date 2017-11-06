/********************************************************************
	created:  2014/09/05	13:08:37
	filename: CANopenSensor.h
	author:	  Mateusz Janiak

	purpose:
	*********************************************************************/
#ifndef __HEADER_GUARD_IMU_COSTUME__CANOPENSENSOR_H__
#define __HEADER_GUARD_IMU_COSTUME__CANOPENSENSOR_H__

#include <imucostumelib/Export.h>
#include <imucostumelib/CANopenFrame.h>
#include <cstdint>
#include <type_traits>

namespace imuCostume
{
	//! Klasa opisuj¹ca generyczne urz¹dzenie CANopen
	class IMUCOSTUME_EXPORT CANopenSensor
	{
	public:

		enum SpeedLevel
		{
			SpeedLevel0,	//! Najwy¿sza prêdkoœæ komunikacji
			SpeedLevel1,
			SpeedLevel2,
			SpeedLevel3,
			SpeedLevel4,
			SpeedLevel5,
			SpeedLevel6,
			SpeedLevel7,
			SpeedLevel8,	//! Najni¿sza prêdkoœæ komunikacji
		};

		enum IDataSize
		{
			ISize1B = 0x4F,
			ISize2B = 0x4B,
			ISize4B = 0x43
		};

		enum ODataSize
		{
			OSize1B = 0x2F,
			OSize2B = 0x2B,
			OSize4B = 0x23
		};

		enum ErrorCode
		{
			ERROR_OK = 0x00,
			NO_TOOGLE_BIT = 0x05030000,
			OUT_OF_MEMORY = 0x05030005,
			UNKNOWN_COMMAND = 0x06040001,
			INDEX_NOT_FOUND = 0x06020000,
			READ_ONLY_OBJECT = 0x06010002,
			WRITE_ONLY_OBJECT = 0x06010001,
			PDO_MAPPING_FAILURE = 0x06040041,
			PDO_SIZE_OVERRUN = 0x06040042,
			PARAMETER_SIZE_MISMATCH = 0x06040043,
			INTERNAL_DATA_INCONSISTENCY = 0x06040047,
			FLASH_MEMORY_ACCESS_FAILURE = 0x06060000,
			SUB_INDEX_NOT_FOUND = 0x06090011,
			OUT_OF_RANGE_VALUE = 0x06090030,
			TOO_HIGH_VALUE = 0x06090031,
			TOO_LOW_VALUE = 0x06090032,
			GENERAL_ERROR = 0x08000000,
			SIGNATURE_MISMATCH = 0x08000020,
			TOO_SHORT_DATA = 0x06070013,
			TOO_LONG_DATA = 0x06070012
		};

		enum Dictionary
		{
			DEVICE_TYPE = 0x1000,
			ERROR_REGISTER = 0x1001,
			COB_ID_SYNC = 0x1005,
			COMMON_CYCLE_PERIOD = 0x1006,
			STORE_PARAMETERS = 0x1010,
			RESTORE_DEFAULT_PARAMETERS = 0x1011,
			COB_ID_EMCY = 0x1014,
			PRODUCED_HEARTBEAT_TIME = 0x1017,
			IDENTITY_OBJECT = 0x1018,
			SDO_SERVER = 0x1200,
			RPDO_PARAM_BASE = 0x1400,
			RPDO_MAPPING_BASE = 0x1600,
			TPDO_PARAM_BASE = 0x1800,
			TPDO_MAPPING_BASE = 0x1A00,
			NMT_STARTUP = 0x1F80
		};

		enum StoreParametersSub
		{
			STORE_PARAMETERS_NUMBER_OF_ENTRIES = 0,
			STORE_PARAMETERS_SAVE_ALL = 1,
		};

		enum RestoreParametersSub
		{
			RESTORE_DEFAULT_PARAMETERS_NUMBER_OF_ENTRIES = 0,
			RESTORE_DEFAULT_PARAMETERS_ALL = 1,
		};

		enum IdentityObjectParametersSub
		{
			IDENTITY_OBJECT_NUMBER_OF_ENTRIES = 0,
			IDENTITY_OBJECT_VENDOR_ID = 1,
			IDENTITY_OBJECT_PRODUCT_CODE = 2,
			IDENTITY_OBJECT_REVISION_NUMBER = 3,
			IDENTITY_OBJECT_SERIAL_NUMBER = 4,
		};

		enum SDOServerParametersSub
		{
			SDO_SERVER_NUMBER_OF_ENTRIES = 0,
			SDO_SERVER_COB_ID_CLINET_TO_SERVER = 1,
			SDO_SERVER_COB_ID_SERVER_TO_CLINET = 2,
		};

		enum RPDOParamParametersSub
		{
			RPDO_PARAM_NUMBER_OF_ENTRIES = 0,
			RPDO_PARAM_COB_ID = 1,
			RPDO_PARAM_TRANSMISSION_TYPE = 2,
			RPDO_PARAM_EVENT_TIMER = 5,
		};

		enum RPDOMappingParametersSub
		{
			RPDO_MAPPING_NUMBER_OF_ENTRIES = 0,
			RPDO_MAPPING_PDO_MAPPING_ENTRY_BASE = 1,	//TODO co znaczy YY?
		};

		enum TPDOParamParametersSub
		{
			TPDO_PARAM_NUMBER_OF_ENTRIES = 0,
			TPDO_PARAM_COB_ID = 1,
			TPDO_PARAM_TRANSMISSION_TYPE = 2,
			TPDO_PARAM_INHIBT_TIMER = 3,
			TPDO_PARAM_EVENT_TIMER = 5,
			TPDO_PARAM_SYNC_START_VALUE = 6
		};

		enum TPDOMappingParametersSub
		{
			TPDO_MAPPING_NUMBER_OF_ENTRIES = 0,
			TPDO_MAPPING_PDO_MAPPING_ENTRY_BASE = 1,	// TODO co znaczy YY?
		};

	public:

		//! \param messageID Identyfikator wiadomoœci NMT
		//! \param nodeID Identyfikator wêz³a (0 - broadcast)
		//! \return Sformatowana ramka CANopen
		static CANopenFrame formatNMTFrame(const uint8_t messageID,
			const uint8_t nodeID = 0);

		//! \param size Rozmiar danych do wysy³ki SDO w trybie krótkim		
		//! \return Odpowiadaj¹ca wartoœc wyliczenia opisuj¹ca parametr d³ugoœci danych
		static ODataSize dataSize(const std::size_t size);

		//! \param size Typ wyliczenia wielkœci danych do wysy³ki
		//! \return Faktyczna iloœæ [B] do wysy³ki
		static std::size_t dataBytes(const ODataSize size);

		//! \tparam T Typ który chcemy zapisaæ SDO w trybie krótkim
		template<typename T>
		//! \param nodeID Identyfikator wêz³a do którego maj¹ trafiæ dane
		//! \param dictID Identyfikator obiektu danych
		//! \param dictSubID Pod-identyfikator obiektu danych
		//! \param value Wartoœæ któr¹ wysy³amy
		//! \return Sformatowana ramka danych
		static CANopenFrame formatExpeditedSDOWriteFrame(const uint8_t nodeID,
			const uint16_t dictID, const uint8_t dictSubID, const T value)
		{
			static_assert(std::is_pod<T>::value, "Data is not POD");
			static_assert(sizeof(T) < 5, "Data size exceeds 4B limit");
			static_assert(sizeof(T) != 3, "Invalid data size");

			union{
				T v;
				uint32_t u;
			} data;

			data.v = value;

			return formatExpeditedSDOWriteFrame(nodeID, dictID, dictSubID, data.u, dataSize(sizeof(T)));
		}
		
		//! \param nodeID Identyfikator wêz³a do którego maj¹ trafiæ dane
		//! \param dictID Identyfikator obiektu danych
		//! \param dictSubID Pod-identyfikator obiektu danych
		//! \param value Wartoœæ któr¹ wysy³amy
		//! \param dataSize Faktyczny rozmiar do wysy³ki (mo¿na obc¹æ do wê¿szego lub rozszerzyæ)
		//! \return Sformatowana ramka danych
		static CANopenFrame formatExpeditedSDOWriteFrame(const uint8_t nodeID,
			const uint16_t dictID, const uint8_t dictSubID, const uint32_t value,
			const ODataSize dataSize);

		//! \param nodeID Identyfikator wêz³a z któego czytamy dane
		//! \param dictID Identyfikator obiektu danych
		//! \param dictSubID Pod-identyfikator obiektu danych
		//! \return Sformatowana ramka z komend¹ do odczytu
		static CANopenFrame formatExpeditedSDOReadFrame(const uint8_t nodeID,
			const uint16_t dictID, const uint8_t dictSubID);
	};
}

#endif	// __HEADER_GUARD_IMU_COSTUME__CANOPENSENSOR_H__