#include <osgutils/QuatUtils.h>
#include <cmath>
#include <algorithm>
#include <assert.h>
#include <osg/Vec3>

namespace osg
{
	const double QuatUtils::QUAT_EPSILON = 0.000001;
	const Quat QuatUtils::UNIT_QUAT = Quat(0.0, 0.0, 0.0, 1.0);

	const Quat QuatUtils::exp(const Quat & val)
	{
		assert(val.w() == 0.0);

		Vec3d vec(val.asVec3());
		Vec3d::value_type l = vec.length();
		const double sinL = std::sin(l);

		if (std::fabs(sinL) > QUAT_EPSILON){
			vec *= sinL / l;
		}
		else {
			l = vec[0] = vec[1] = vec[2] = 0.0;
		}

		return Quat(vec.x(), vec.y(), vec.z(), std::cos(l));
	}

	const Quat QuatUtils::log(const Quat & val)
	{
		const double angle = std::acos(std::min(std::max(val.w(), -1.0), 1.0));
		const double sinAlpha = std::sin(angle);
		Vec3d vec(0.0, 0.0, 0.0);

		if (std::fabs(sinAlpha) > QUAT_EPSILON){
			vec = val.asVec3() * angle / sinAlpha;
		}

		return Quat(vec.x(), vec.y(), vec.z(), 0.0);
	}

	const Quat QuatUtils::pow(const Quat & base, const Quat::value_type power)
	{
		return exp(log(base) * power);
	}

	const Quat QuatUtils::normalize(const Quat & val)
	{
		const double l = val.length2();
		if (l < QUAT_EPSILON){
			return UNIT_QUAT;
		}

		return val / std::sqrt(l);
	}

	const Quat QuatUtils::normalizeScalarPart(const Quat & val)
	{
		auto l = val.asVec3().length2();
		if (l < 1.0){
			auto q(val);
			q.w() = std::sqrt(1.0 - l);
			return q;
		}

		return UNIT_QUAT;
	}

	const Quat QuatUtils::normalizeVectorPart(const Quat & val)
	{
		if (val.w() < 1.0){
			auto rest = std::sqrt(1.0 - std::pow(val.w(), 2.0));
			auto v3 = val.asVec3();
			auto l = v3.length();
			v3 *= (rest / l);
			return Quat(v3.x(), v3.y(), v3.z(), val.w());
		}

		return UNIT_QUAT;
	}

	const Quat QuatUtils::lerp(const Quat &q1, const Quat &q2, const Quat::value_type t)
	{
		return q1 * (1.0 - t) + q2 * t;
	}

	const Quat QuatUtils::slerp(const Quat &q1, const Quat &q2, const Quat::value_type t)
	{
		Quat ret;
		ret.slerp(t, q1, q2);
		return ret;
	}

	const Quat QuatUtils::slerpNoInvert(const Quat &from, const Quat &to, const Quat::value_type t)
	{
		static const double lowerRange = -1.0 + QUAT_EPSILON;
		static const double upperRange = -lowerRange;

		const double scale_from = 1.0 - t;
		// this is a dot product
		const double cosomega = from.asVec4() * to.asVec4();

		if (lowerRange < cosomega && cosomega < upperRange)
		{
			const double omega = std::acos(cosomega);  // 0 <= omega <= Pi (see man acos)
			//double sinomega = std::sin(omega) ;  // this sinomega should always be +ve so
			// could try sinomega=sqrt(1-cosomega*cosomega) to avoid a sin()?
			return ((from * std::sin(scale_from * omega)) + (to * std::sin(t * omega))) / std::sin(omega);
		}

		/* --------------------------------------------------
			If quaternions are close to each other we can skip else clause
			The ends of the vectors are very close
			we can use simple linear interpolation - no need
			to worry about the "spherical" interpolation
			-------------------------------------------------- */

		return (from * scale_from) + (to * t);
	}

	//! spherical cubic interpolation
	const Quat QuatUtils::squad(const Quat &q1, const Quat &q2, const Quat &a, const Quat &b, const Quat::value_type t)
	{
		const Quat c(slerpNoInvert(q1, q2, t));
		const Quat d(slerpNoInvert(a, b, t));
		return slerpNoInvert(c, d, 2.0 * t * (1.0 - t));
	}

	const Quat QuatUtils::controlPoint(const Quat &prev, const Quat &mid, const Quat &next)
	{
		return mid * exp(-(log(mid.inverse() * next) + log(mid.inverse() * prev)) / 4);
	}

	const Quat QuatUtils::simpleSquadCall(const Quat &q1, const Quat &q2, const Quat &q3, const Quat &q4, const Quat::value_type t)
	{
		return squad(q2, q3, spline(q1, q2, q3), spline(q2, q3, q4), t);
	}

	//! Shoemake-Bezier interpolation using De Castlejau algorithm
	const Quat QuatUtils::bezier(const Quat &q1, const Quat &q2, const Quat &a, const Quat &b, const Quat::value_type t)
	{
		// level 1
		const Quat q11(slerpNoInvert(q1, a, t));
		const Quat q12(slerpNoInvert(a, b, t));
		const Quat q13(slerpNoInvert(b, q2, t));
		// level 2 and 3
		return slerpNoInvert(slerpNoInvert(q11, q12, t), slerpNoInvert(q12, q13, t), t);
	}

	const Quat QuatUtils::simpleBezierCall(const Quat &q1, const Quat &q2, const Quat &q3, const Quat &q4, const Quat::value_type t)
	{
		return bezier(q2, q3, spline(q1, q2, q3), spline(q2, q3, q4), t);
	}

	//! Given 3 quaternions, qn-1,qn and qn+1, calculate a control point to be used in spline interpolation
	const Quat QuatUtils::spline(const Quat &qnm1, const Quat &qn, const Quat &qnp1)
	{
		//inverse should be done but only unit quaternions are expected so conjugate is more efficient!!
		const Quat qni(qn.conj());
		return qn * exp((log(qni * qnm1) + log(qni * qnp1)) / -4.0);
	}

}