/********************************************************************
	created:  2014/09/03	07:40:53
	filename: ImuSensor.h
	author:	  Mateusz Janiak

	purpose:
	*********************************************************************/
#ifndef __HEADER_GUARD_IMU_COSTUME__IMUSENSOR_H__
#define __HEADER_GUARD_IMU_COSTUME__IMUSENSOR_H__

#include <imucostumelib/Export.h>

namespace imuCostume
{
	class CANopenSensor;

	class IMUCOSTUME_EXPORT IMUSensor
	{
	public:

		enum SDOFeatures
		{
			CONFIGURATION = 0x2000,
			RAW_ACCELEROMETER = 0x2001,
			RAW_MAGNETOMETER = 0x2002,
			RAW_GYROSCOPE = 0x2003,
			CALIBRATED_ACCELEROMETER = 0x2004,
			CALIBRATED_MAGNETOMETER = 0x2005,
			CALIBRATED_GYROSCOPE = 0x2006,
			QUATERNION = 0x2007,
			EULER_ANGLES = 0x2008,
			RANDOM_DATA_GENERATOR = 0x2009,
			GPS_FEEDBACK = 0x200A,
			MOTION_GYRO = 0x200B,
			INTERNAL_TIMESTAMP = 0x200C
		};

		enum ConfigurationParametersSub
		{
			CONFIGURATION_NUMBER_OF_ENTRIES = 0,
			CONFIGURATION_CALIBRATED_DATA_ENABLED = 1,
			CONFIGURATION_ORIENTATION_CALCULATION_ENABLED = 2,
			CONFIGURATION_ACC_SCALE_FACTOR = 3,
			CONFIGURATION_MAG_SCALE_FACTOR = 4,
			CONFIGURATION_GYRO_SCALE_FACTOR = 5,
			CONFIGURATION_QUATERNION_SCALE_FACTOR = 6,
			CONFIGURATION_YPR_SCALE_FACTOR = 7,
			CONFIGURATION_IRON_CAL_ENABLE = 8,
			CONFIGURATION_SOFT_IRON_CAL_ENABLE = 15,
			CONFIGURATION_WKAD_FACTOR = 16,
		};

		enum RawAccelerometerParametersSub
		{
			RAW_ACCELEROMETER_NUMBER_OF_ENTRIES = 0,
			RAW_ACCELEROMETER_RAW_X = 1,
			RAW_ACCELEROMETER_RAW_Y = 2,
			RAW_ACCELEROMETER_RAW_Z = 3
		};

		enum RawMagnetometerParametersSub
		{
			RAW_MAGNETOMETER_NUMBER_OF_ENTRIES = 0,
			RAW_MAGNETOMETER_RAW_X = 1,
			RAW_MAGNETOMETER_RAW_Y = 2,
			RAW_MAGNETOMETER_RAW_Z = 3,
			RAW_MAGNETOMETER_RAW_TEMP = 4
		};

		enum RawGyroscopeParametersSub
		{
			RAW_GYROSCOPE_NUMBER_OF_ENTRIES = 0,
			RAW_GYROSCOPE_RAW_X = 1,
			RAW_GYROSCOPE_RAW_Y = 2,
			RAW_GYROSCOPE_RAW_Z = 3,
			RAW_GYROSCOPE_RAW_TEMP = 4
		};

		enum CalibratedAccelerometerParametersSub
		{
			CALIBRATED_ACCELEROMETER_NUMBER_OF_ENTRIES = 0,
			CALIBRATED_ACCELEROMETER_X = 1,
			CALIBRATED_ACCELEROMETER_Y = 2,
			CALIBRATED_ACCELEROMETER_Z = 3
		};

		enum CalibratedMagnetometerParametersSub
		{
			CALIBRATED_MAGNETOMETER_NUMBER_OF_ENTRIES = 0,
			CALIBRATED_MAGNETOMETER_X = 1,
			CALIBRATED_MAGNETOMETER_Y = 2,
			CALIBRATED_MAGNETOMETER_Z = 3
		};

		enum CalibratedGyroscopeParametersSub
		{
			CALIBRATED_GYROSCOPE_NUMBER_OF_ENTRIES = 0,
			CALIBRATED_GYROSCOPE_X = 1,
			CALIBRATED_GYROSCOPE_Y = 2,
			CALIBRATED_GYROSCOPE_Z = 3
		};

		enum QuaternionParametersSub
		{
			QUATERNION_NUMBER_OF_ENTRIES = 0,
			QUATERNION_R = 1,
			QUATERNION_I = 2,
			QUATERNION_J = 3,
			QUATERNION_K = 4
		};

		enum EulerAnglesParametersSub
		{
			EULER_ANGLES_NUMBER_OF_ENTRIES = 0,
			EULER_ANGLES_YAW = 1,
			EULER_ANGLES_PITCH = 2,
			EULER_ANGLES_ROLL = 3
		};

		enum RandomDataGeneratorParametersSub
		{
			RANDOM_DATA_GENERATOR_NUMBER_OF_ENTRIES = 0,
			RANDOM_DATA_GENERATOR_1B = 1,
			RANDOM_DATA_GENERATOR_2B = 2,
			RANDOM_DATA_GENERATOR_4B = 3
		};

		enum GPSFeedbackParametersSub
		{
			GPS_FEEDBACK_NUMBER_OF_ENTRIES = 0,
			GPS_FEEDBACK_VEL_N = 1,
			GPS_FEEDBACK_VEL_E = 2,
			GPS_FEEDBACK_VEL_D = 3
		};

	public:

		//! \param sensor Sensor
		IMUSensor(CANopenSensor * sensor);
		//! Destruktor
		virtual ~IMUSensor();
		//! \return Sensor
		CANopenSensor * sensor() const;

	private:
		//! Sensor
		CANopenSensor * sensor_;
	};
}

#endif	// __HEADER_GUARD_IMU_COSTUME__IMUSENSOR_H__