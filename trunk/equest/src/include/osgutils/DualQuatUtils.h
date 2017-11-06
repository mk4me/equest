#ifndef _OSG_DUALQUAT_UTILS
#define _OSG_DUALQUAT_UTILS 1

#include <osg/Quat>
#include <osgutils/DualQuat.h>

namespace osg {

class DualQuatUtils {

public:
	//! Epsilon dla operacji na kwaternionach
	static const double DUALQUAT_EPSILON;
	//! Jednostkowy kwaternion dualny
	static const DualQuat UNIT_DUALQUAT;

	static const DualQuat normalize(const DualQuat & val);

	static const DualQuat exp(const DualQuat & val);

	static const DualQuat log(const DualQuat & val);

	static const DualQuat pow(const DualQuat & base, const DualQuat::value_type power);

	static const DualQuat lerp(const DualQuat &q1, const DualQuat &q2, const DualQuat::value_type t);

	static const DualQuat slerp(const DualQuat &q1, const DualQuat &q2, const DualQuat::value_type t);

	static const DualQuat slerpNoInvert(const DualQuat &from, const DualQuat &to, const DualQuat::value_type t);

	static const DualQuat squad(const DualQuat &q1,const DualQuat &q2,const DualQuat &a,const DualQuat &b, const DualQuat::value_type t);

	static const DualQuat simpleSquadCall(const DualQuat &q1,const DualQuat &q2,const DualQuat &q3,const DualQuat &q4, const DualQuat::value_type t);

	static const DualQuat bezier(const DualQuat &q1, const DualQuat &q2, const DualQuat &a, const DualQuat &b, const DualQuat::value_type t);

	static const DualQuat simpleBezierCall(const DualQuat &q1,const DualQuat &q2,const DualQuat &q3,const DualQuat &q4, const DualQuat::value_type t);

	static const DualQuat spline(const DualQuat &qnm1, const DualQuat &qn, const DualQuat &qnp1);
};

}

#endif //   _OSG_DUALQUAT_UTILS