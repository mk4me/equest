/********************************************************************
	created:	2011/07/17
	created:	17:7:2011   11:28
	filename: 	QuaternionManipulators.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_KINEMATIC__QUATERNIONMANIPULATORS_H__
#define HEADER_GUARD_KINEMATIC__QUATERNIONMANIPULATORS_H__

#include <osg/Quat>
#include <stdexcept>
#include <utils/Debug.h>

namespace kinematic {

    //! Klasa bazowa dla manipulatorów używanych w kanałach zawierających kwaterniony.
    //! implementuje standardowe metody manipulatorów
	class QuaternionManipulator
	{
	public:
		//! powielony operator z osg, można polemizować nad jego sensem
		//! \param q1 pierwszy porównywany kwaternion
		//! \param q2 drugi porównywany kwaternion
		static inline bool isLower(const osg::Quat& q1, const osg::Quat& q2)
		{
			return q1 < q2;
		}
		//! normalizacja zabroniona
		static osg::Quat normalize(const osg::Quat& q, const osg::Quat& min, const osg::Quat& max)
		{
			UTILS_ASSERT("Unable to normalize quaternion");
			throw std::runtime_error("Unable to normalize quaternion");
		}
		//! metoda abstrakcyjna, zapewnia interpolację między kwaternionami
		//! \param ret zwracany, wyinterpolowany kwaternion
		//! \param q1 dolny zakres
		//! \param q2 górny zakres
		//! \param t współczynnik interpolacji
		virtual void interpolate(osg::Quat& ret, const osg::Quat& q1, const osg::Quat& q2, float t) const = 0;
	};

    //! Manipulator dostarczający interpolacji sferycznej kwaternionów
	class QuaternionManipulatorSlerp : public QuaternionManipulator
	{
	public:
        //! interpolacja sferyczna między kwaternionami
        //! \param ret zwracany, wyinterpolowany kwaternion
        //! \param q1 dolny zakres
        //! \param q2 górny zakres
        //! \param t współczynnik interpolacji
		virtual void interpolate(osg::Quat& ret, const osg::Quat& q1, const osg::Quat& q2, float t ) const
		{
            ret.slerp(t, q1, q2);
		}
	};

    //! Manipulator dostarczający interpolacji liniowej kwaternionów
	class QuaternionManipulatorLerp : public QuaternionManipulator
	{
	public:
        //! interpolacja liniowa między kwaternionami
        //! \param ret zwracany, wyinterpolowany kwaternion
        //! \param q1 dolny zakres
        //! \param q2 górny zakres
        //! \param t współczynnik interpolacji
		virtual void interpolate(osg::Quat& ret, const osg::Quat& q1, const osg::Quat& q2, float t ) const
		{
			ret._v[0] = q1._v[0] * (1.0f - t)  + q2._v[0] * t;
			ret._v[1] = q1._v[1] * (1.0f - t)  + q2._v[1] * t;
			ret._v[2] = q1._v[2] * (1.0f - t)  + q2._v[2] * t;
			ret._v[3] = q1._v[3] * (1.0f - t)  + q2._v[3] * t;
		}
	};
}

#endif
