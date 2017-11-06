/********************************************************************
	created:  2014/12/03	12:27:42
	filename: IIMUOrientationEstimationAlgorithm.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_IMU__IIMUORIENTATIONESTIMATIONALGORITHM_H__
#define __HEADER_GUARD_IMU__IIMUORIENTATIONESTIMATIONALGORITHM_H__

#include <string>
#include <osg/Quat>
#include <osg/Vec3d>
#include <corelib/IIdentifiable.h>

class QWidget;

namespace IMU
{
	//! Generic quaternion-based orientation filter - generates orientation as a quaternion using IMU sensor fusion
	class IIMUOrientationEstimationAlgorithm : public plugin::IIdentifiable
	{
	public:
		//! Make it polymorphic
		virtual ~IIMUOrientationEstimationAlgorithm() {}

		//! \return Nowy algorytm estymacji
		virtual IIMUOrientationEstimationAlgorithm * create() const = 0;
		
		//! \return Internal filter name
		virtual std::string name() const = 0;

		//! Resets filter internal state
		virtual void reset() = 0;

		//! \return Number (n) of frames that are probably unstable after filter create() / reset() - call estimate() at least (n) times
		virtual unsigned int approximateEstimationDelay() const { return 0; }

		//! \param orientation Orientacja z której mo¿e zacz¹æ algorytm
		virtual void initialize(const osg::Quat & orientation) {}

		//! Calculates orientation from sensor fusion		
		//! \param accelerometer accelerometer vector from IMU
		//! \param gyroscope gyroscope vector from IMU
		//! \param magnetometer magnetometer vector from IMU
		//! \param inDeltaT time between acquisitions in seconds [s] from IMU sensor
		//! \param refOrient Orientacja referencyjna, wyliczna np. na samym czujniku,
		//! nie zawsze musi byæ poprawna i nie zawsze musi byæ dostêpna (brak = kwaternion jednostkowy dla zerowego obrotu)
		//! \return Returns estimated orientation.		
		virtual osg::Quat estimate(const osg::Vec3d& accelerometer,
			const osg::Vec3d& gyroscope, const osg::Vec3d& magnetometer,
			const double inDeltaT, const osg::Quat & refOrient = osg::Quat(0,0,0,1)) = 0;

		//! \return Widget konfiguracyjny
		virtual QWidget* configurationWidget() { return nullptr; }
	};

	DEFINE_SMART_POINTERS(IIMUOrientationEstimationAlgorithm);
}

#endif	// __HEADER_GUARD_IMU__IIMUORIENTATIONESTIMATIONALGORITHM_H__