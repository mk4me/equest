/********************************************************************
	created:  2016/07/18
	filename: SkiData.h
	author:	  Mateusz Janiak
	
	purpose:  
*********************************************************************/
#ifndef __HEADER_GUARD_SKIDATA__SKIDATA_H__
#define __HEADER_GUARD_SKIDATA__SKIDATA_H__

#include <osg/Vec3>
#include <vector>
#include <objectwrapperlib/ObjectWrapper.h>
#include <utils/PtrPolicyStd.h>
#include <utils/ClonePolicies.h>

namespace skidata
{
	//! Dane IMU
	struct RawIMU
	{
		//! Akcelerometr [m/s^2]
		osg::Vec3 accelerometer;
		//! ¯yroskop [deg/s]
		osg::Vec3 gyroscope;
		//! Magnetometr [uT]
		osg::Vec3 magnetometer;
	};

	//! Przetworzone dane IMU - rozszerzone o orientacjê
	struct ProcessedIMU : public RawIMU
	{
		//! Orientacja jako 3 k¹ty Eulera [deg]
		osg::Vec3 orientation;
	};

	//! Wszystkie dane zbierane z IMU na ca³ym narciarzu
	struct IMUData
	{
		//! Dane lewej narty
		ProcessedIMU leftSki;
		//! Dane prawej narty
		ProcessedIMU rightSki;
		//! Dane klatki piersiowej
		ProcessedIMU chest;
	};

	//! Dane pozycji z GPS
	struct GPSPosition
	{
		//! D³ugoœæ georgraficzna [stopnie]
		float longnitude;
		//! Szerokoœæ georgraficzna [stopnie]
		float latitude;
		//! Wysokoœæ [m]
		float height;
		//! Kierunek (odchylenie od pó³nocy) [deg]
		float heading;
	};

	//! Dane prêdkoœci z GPS
	struct GPSSpeed
	{
		//! Prêdkoœæ 2D [m/s]
		float speed2D;
		//! Prêdkoœæ 3D [m/s]
		float speed3D;
	};

	//! Dane z GPS
	struct GPSData
	{
		//! Pozycja
		GPSPosition position;
		//! Prêdkoœæ
		GPSSpeed speed;
	};

	//! Dane œrodowiska
	struct EnvironmentData
	{
		//! Temperature [deg C]
		float temperature;
		//! Ciœnienie [hP]
		float preasure;
	};

	//! Ramka danych opisuj¹ca zjazd
	struct SkiDataFrame
	{
		//! Dane z IMU
		IMUData imu;
		//! Dane z GPS
		GPSData gps;
		//! Dane œrodowiska
		EnvironmentData environment;
	};

	//! Wektor danych z nart
	using SkiData = std::vector<SkiDataFrame>;
}

DEFINE_WRAPPER(skidata::SkiData, utils::PtrPolicyStd, utils::ClonePolicyCopyConstructor);

#endif  // __HEADER_GUARD_SKIDATA__SKIDATA_H__