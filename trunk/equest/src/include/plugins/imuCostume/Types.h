/********************************************************************
	created:  2014/12/21	13:24:57
	filename: Types.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_IMU__TYPES_H__
#define __HEADER_GUARD_IMU__TYPES_H__

#include <imucostumelib/ImuCostume.h>
#include <boost/bimap.hpp>
#include <boost/bimap/multiset_of.hpp>
#include <boost/bimap/set_of.hpp>
#include <osg/Quat>
#include <osg/Vec3d>
#include <map>

namespace IMU
{
	//! Mapa identyfikatorów sensorów do nazw jointów z którymi są skojarzone, jeden sensor, jeden joint, ale joint moze miec więcej sensorów!!
	typedef boost::bimap<boost::bimaps::set_of<imuCostume::Costume::SensorID>, boost::bimaps::multiset_of<std::string>> SensorsMapping;

	//! Struktura opisująca orientacje sensora
	struct SensorData
	{
		//! Orientacja
		osg::Quat orientation;
		//! Akcelerometr
		osg::Vec3d accelerometer;
		//! Żyroskop
		osg::Vec3d gyroscope;
		//! Magnetometr
		osg::Vec3d magnetometer;
	};

	//! Agregacja danych z sensorów
	typedef std::map<imuCostume::Costume::SensorID, SensorData> SensorsData;

	//! Struktura opisująca dane strumienia z danymi czujników
	struct SensorsStreamData
	{
		//! Stempel czasu danych
		imuCostume::CostumeCANopenIO::Timestamp timestamp;
		//! Dane sensorów
		SensorsData sensorsData;
	};

	//! Struktura opisująca joint
	struct JointData
	{
		//! Orientacja lokalna
		osg::Quat localOrientation;
		//! Orientacja lokalna
		osg::Quat globalOrientation;
		//! Lokalna pozycja
		osg::Vec3d localPosition;
		//! Lokalna pozycja
		osg::Vec3d globalPosition;
	};

	//! Klasa usuwająca czas z danych
	class TimeRemoverExtractor
	{
	public:
		//! \tparam Src Typ źródłowy
		template<typename Src>
		inline static bool verify(const Src & timeData)
		{
			return true;
		}
		//! \tparam Src Typ źródłowy
		template<typename Src, typename Dest = typename Src::second_type>
		inline static void extract(const Src & timeData, Dest & data)
		{
			data = timeData.second;
		}
	};

}

//! Makro realizujące nazwę ekstraktora pola strumienia czasowego
#define TIMEMEMBER_EXTRACTOR_NAME(memberName) \
	TimeMemberExtractor##memberName

//! Makro definiujące ekstraktor pola strumienia czasowego
#define TIMEMEMBER_EXTRACTOR(memberName) \
class TIMEMEMBER_EXTRACTOR_NAME(memberName){\
public:\
	template<typename SrcType>\
	inline static bool verify(const SrcType &) { return true; }\
	template<typename SrcType, typename DestType = typename SrcType::second_type>\
	inline static void extract(const SrcType & src, DestType & dest){\
		dest.first = src.first;\
		dest.second = src.second.memberName;\
	}\
};

//! Makro realizujące adapter pola strumienia czasowego
#define TIMEMEMBER_ADAPTER_NAME(baseType, memberName) \
	TimeMemberAdapter##baseType##memberName

//! Makro definiujące adapter pola strumienia czasowego
#define TIMEMEMBER_ADAPTER_EXT(baseType, destType, memberName) \
typedef StreamAdapterT<baseType, destType, TIMEMEMBER_EXTRACTOR_NAME(memberName)> TIMEMEMBER_ADAPTER_NAME(baseType,memberName);

//! Makro definiujące adapter pola strumienia czasowego
#define TIMEMEMBER_ADAPTER(baseType, memberName) \
	TIMEMEMBER_ADAPTER_EXT(baseType, decltype(std::declval<baseType>().second.memberName), memberName);

#endif	// __HEADER_GUARD_IMU__TYPES_H__
