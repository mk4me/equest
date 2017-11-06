/********************************************************************
	created:  2014/12/11	08:51:59
	filename: RotationConverter.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_KINEMATICUTILS__ROTATIONCONVERTER_H__
#define __HEADER_GUARD_KINEMATICUTILS__ROTATIONCONVERTER_H__

#include <kinematicUtils/Types.h>
#include <osg/Vec3d>
#include <osg/Quat>
#include <utils/Debug.h>

namespace kinematicUtils
{
	//! Wyliczenie jednostek opisjących kąty
	enum AngleUnitType
	{
		Deg,	//! Stopnie
		Rad		//! Radiany
	};


	const double _2PI = 2 * osg::PI;

	template<AngleUnitType T>
	struct AngleNormalizedMax{};
	template<> struct AngleNormalizedMax < Deg > { static double value() { return 360.0; } };
	template<> struct AngleNormalizedMax < Rad > { static double value() { return _2PI; } };


	double convertAngleUnit(const double value, const AngleUnitType destType);

	template<int DestinationAngleUnitType>
	inline double convertAngleUnit(const double value)
	{
		UTILS_ASSERT(false, "Nalezy użyć jednej z dozwolonych specjalizacji wynikających z wyliczenia dla AngleUnitType");
		return 0.0;
	}

	
	template<AngleUnitType T>
	inline double normalizeAngle(const double value)
	{
		auto maxValue = AngleNormalizedMax<T>::value();
		if (value < 0.0) {
			return normalizeAngle<T>(value + maxValue);
		}
		if (value > maxValue) {
			return normalizeAngle<T>(value - maxValue);
		}
		return value;
	}

	template<AngleUnitType T>
	inline bool compareAngle(double a, double b, double epsilon = 0.00001)
	{
		a = normalizeAngle<T>(a);
		b = normalizeAngle<T>(b);

		auto diff = abs(a - b);
		if (diff < epsilon || diff > (AngleNormalizedMax<T>::value() - epsilon)) {
			return true;
		} 
		return false;
	}

	bool isSameDirection(const osg::Vec3d& a, const osg::Vec3d& b, osg::Vec3d::value_type epsilon = 0.00001);

	using EulerConverter = osg::Quat(*)(const osg::Vec3d &);

	EulerConverter eulerConverter(const AxisOrder::Type axisOrder);

	osg::Quat convertXYX(const osg::Vec3d & rotation);
	osg::Quat convertXYZ(const osg::Vec3d & rotation);
	osg::Quat convertXZX(const osg::Vec3d & rotation);
	osg::Quat convertXZY(const osg::Vec3d & rotation);
	osg::Quat convertYXY(const osg::Vec3d & rotation);
	osg::Quat convertYXZ(const osg::Vec3d & rotation);
	osg::Quat convertYZX(const osg::Vec3d & rotation);
	osg::Quat convertYZY(const osg::Vec3d & rotation);
	osg::Quat convertZXY(const osg::Vec3d & rotation);
	osg::Quat convertZXZ(const osg::Vec3d & rotation);
	osg::Quat convertZYX(const osg::Vec3d & rotation);
	osg::Quat convertZYZ(const osg::Vec3d & rotation);
	osg::Quat convert(const osg::Vec3d & rotation, const AxisOrder::Type axisOrder);

	template<int AxisOrder>
	inline osg::Quat convert(const osg::Vec3d & rotation)
	{
		UTILS_ASSERT(false, "Nalezy użyć jednej z dozwolonych specjalizacji wynikających z wyliczenia dla AxisOrder");
		return osg::Quat(0, 0, 0, 1);
	}

	osg::Vec3d orderedAngles(const osg::Vec3d & angles, const AxisOrder::Type axisOrder);

	using QuaternionConverter = osg::Vec3d(*)(const osg::Quat &);

	QuaternionConverter quaternionConverter(const AxisOrder::Type axisOrder);
	
	osg::Vec3d convertXYX(const osg::Quat & rotation);
	osg::Vec3d convertXYZ(const osg::Quat & rotation);
	osg::Vec3d convertXZX(const osg::Quat & rotation);
	osg::Vec3d convertXZY(const osg::Quat & rotation);
	osg::Vec3d convertYXY(const osg::Quat & rotation);
	osg::Vec3d convertYXZ(const osg::Quat & rotation);
	osg::Vec3d convertYZX(const osg::Quat & rotation);
	osg::Vec3d convertYZY(const osg::Quat & rotation);
	osg::Vec3d convertZXY(const osg::Quat & rotation);
	osg::Vec3d convertZXZ(const osg::Quat & rotation);
	osg::Vec3d convertZYX(const osg::Quat & rotation);
	osg::Vec3d convertZYZ(const osg::Quat & rotation);
	osg::Vec3d convert(const osg::Quat & rotation, const AxisOrder::Type axisOrder);


	template<int AxisOrder>
	inline osg::Vec3d convert(const osg::Quat & rotation)
	{
		UTILS_ASSERT(false, "Nalezy użyć jednej z dozwolonych specjalizacji wynikających z wyliczenia dla AxisOrder");
		return osg::Vec3d(0, 0, 0);
	}

	osg::Vec3d toRadians(const osg::Vec3d & rotation);
	osg::Vec3d toDegrees(const osg::Vec3d & rotation);

//-------------------------------------------------------

	template<>
	inline double convertAngleUnit<AngleUnitType::Deg>(const double value)
	{		
		return osg::RadiansToDegrees(value);
	}

	template<>
	inline double convertAngleUnit<AngleUnitType::Rad >(const double value)
	{
		return osg::DegreesToRadians(value);
	}




//-------------------------------------------------------

	template<>
	inline osg::Quat convert<AxisOrder::XYX>(const osg::Vec3d & rotation)
	{
		return convertXYX(rotation);
	}

	template<>
	inline osg::Quat convert<AxisOrder::XYZ>(const osg::Vec3d & rotation)
	{
		return convertXYZ(rotation);
	}

	template<>
	inline osg::Quat convert<AxisOrder::XZX>(const osg::Vec3d & rotation)
	{
		return convertXZX(rotation);
	}

	template<>
	inline osg::Quat convert<AxisOrder::XZY>(const osg::Vec3d & rotation)
	{
		return convertXZY(rotation);
	}

	template<>
	inline osg::Quat convert<AxisOrder::YXY>(const osg::Vec3d & rotation)
	{
		return convertYXY(rotation);
	}

	template<>
	inline osg::Quat convert<AxisOrder::YXZ>(const osg::Vec3d & rotation)
	{
		return convertYXZ(rotation);
	}

	template<>
	inline osg::Quat convert<AxisOrder::YZX>(const osg::Vec3d & rotation)
	{
		return convertYZX(rotation);
	}

	template<>
	inline osg::Quat convert<AxisOrder::YZY>(const osg::Vec3d & rotation)
	{
		return convertYZY(rotation);
	}

	template<>
	inline osg::Quat convert<AxisOrder::ZXY>(const osg::Vec3d & rotation)
	{
		return convertZXY(rotation);
	}

	template<>
	inline osg::Quat convert<AxisOrder::ZXZ>(const osg::Vec3d & rotation)
	{
		return convertZXZ(rotation);
	}

	template<>
	inline osg::Quat convert<AxisOrder::ZYX>(const osg::Vec3d & rotation)
	{
		return convertZYX(rotation);
	}

	template<>
	inline osg::Quat convert<AxisOrder::ZYZ>(const osg::Vec3d & rotation)
	{
		return convertZYZ(rotation);
	}

//------------------------------------------------------------------

	template<>
	inline osg::Vec3d convert<AxisOrder::XYX>(const osg::Quat & rotation)
	{
		return convertXYX(rotation);
	}

	template<>
	inline osg::Vec3d convert<AxisOrder::XYZ>(const osg::Quat & rotation)
	{
		return convertXYZ(rotation);
	}

	template<>
	inline osg::Vec3d convert<AxisOrder::XZX>(const osg::Quat & rotation)
	{
		return convertXZX(rotation);
	}

	template<>
	inline osg::Vec3d convert<AxisOrder::XZY>(const osg::Quat & rotation)
	{
		return convertXZY(rotation);
	}

	template<>
	inline osg::Vec3d convert<AxisOrder::YXY>(const osg::Quat & rotation)
	{
		return convertYXY(rotation);
	}

	template<>
	inline osg::Vec3d convert<AxisOrder::YXZ>(const osg::Quat & rotation)
	{
		return convertYXZ(rotation);
	}

	template<>
	inline osg::Vec3d convert<AxisOrder::YZX>(const osg::Quat & rotation)
	{
		return convertYZX(rotation);
	}

	template<>
	inline osg::Vec3d convert<AxisOrder::YZY>(const osg::Quat & rotation)
	{
		return convertYZY(rotation);
	}

	template<>
	inline osg::Vec3d convert<AxisOrder::ZXY>(const osg::Quat & rotation)
	{
		return convertZXY(rotation);
	}

	template<>
	inline osg::Vec3d convert<AxisOrder::ZXZ>(const osg::Quat & rotation)
	{
		return convertZXZ(rotation);
	}

	template<>
	inline osg::Vec3d convert<AxisOrder::ZYX>(const osg::Quat & rotation)
	{
		return convertZYX(rotation);
	}

	template<>
	inline osg::Vec3d convert<AxisOrder::ZYZ>(const osg::Quat & rotation)
	{
		return convertZYZ(rotation);
	}
}

#endif	// __HEADER_GUARD_KINEMATICUTILS__ROTATIONCONVERTER_H__
