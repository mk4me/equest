#ifndef _OSG_QUAT_UTILS_
#define _OSG_QUAT_UTILS_

#include <osg/Quat>
#include <type_traits>
#include <utils/Utils.h>

namespace osg {

	class QuatUtils {

	public:

		static const Quat UNIT_QUAT;
		static const Quat::value_type QUAT_EPSILON;

		static const Quat exp(const Quat & val);

		static const Quat log(const Quat & val);

		static const Quat pow(const Quat & base, const Quat::value_type power);

		static const Quat normalize(const Quat & val);
		static const Quat normalizeScalarPart(const Quat & val);
		static const Quat normalizeVectorPart(const Quat & val);

		static const Quat lerp(const Quat &q1, const Quat &q2, const Quat::value_type t);

		static const Quat slerp(const Quat &q1, const Quat &q2, const Quat::value_type t);

		static const Quat slerpNoInvert(const Quat &from, const Quat &to, const Quat::value_type t);

		//! spherical cubic interpolation
		static const Quat squad(const Quat &q1, const Quat &q2, const Quat &a, const Quat &b, const Quat::value_type t);

		static const Quat controlPoint(const Quat &prev, const Quat &mid, const Quat &next);

		static const Quat simpleSquadCall(const Quat &q1, const Quat &q2, const Quat &q3, const Quat &q4, const Quat::value_type t);

		//! Shoemake-Bezier interpolation using De Castlejau algorithm
		static const Quat bezier(const Quat &q1, const Quat &q2, const Quat &a, const Quat &b, const Quat::value_type t);

		static const Quat simpleBezierCall(const Quat &q1, const Quat &q2, const Quat &q3, const Quat &q4, const Quat::value_type t);

		//! Given 3 quaternions, qn-1,qn and qn+1, calculate a control point to be used in spline interpolation
		static const Quat spline(const Quat &qnm1, const Quat &qn, const Quat &qnp1);
	};

}

#endif