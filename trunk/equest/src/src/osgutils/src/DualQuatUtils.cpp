#include <osgutils/DualQuatUtils.h>
#include <osgutils/QuatUtils.h>
#include <cmath>

using namespace osg;

const DualQuat DualQuatUtils::UNIT_DUALQUAT = DualQuat(0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0);
const double DualQuatUtils::DUALQUAT_EPSILON = QuatUtils::QUAT_EPSILON;

const DualQuat DualQuatUtils::normalize(const DualQuat & val)
{
	return DualQuat(osg::QuatUtils::normalize(val.rotation()), val.translation());
}

const DualQuat DualQuatUtils::exp(const DualQuat & val)
{
    Quat rotPart(val.rotation());
    Quat transPart(val.translation());

	rotPart = QuatUtils::exp(rotPart);
	return DualQuat(rotPart, transPart * rotPart);
}

const DualQuat DualQuatUtils::log(const DualQuat & val)
{
	Quat rotPart(val.rotation());
	return DualQuat(QuatUtils::log(rotPart), val.translation() * rotPart.inverse());
}

const DualQuat DualQuatUtils::pow(const DualQuat & base, const DualQuat::value_type power)
{
    return exp(log(base) * power);
}

const DualQuat DualQuatUtils::lerp(const DualQuat &q1, const DualQuat &q2, const DualQuat::value_type t)
{
    return q1 * (1.0 - t) + q2 * t;
}

const DualQuat DualQuatUtils::slerp(const DualQuat &q1, const DualQuat &q2, const DualQuat::value_type t)
{
    DualQuat ret;
    ret.slerp(t, q1, q2);
    return ret;
}

const DualQuat DualQuatUtils::slerpNoInvert(const DualQuat &from, const DualQuat &to, const DualQuat::value_type t) 
{
    static const double lowerRange = -1.0 + DUALQUAT_EPSILON;
	static const double upperRange = -lowerRange;

    double scale_from = 1.0 - t;
   
    // this is a dot product
    double cosomega = from.asVec8() * to.asVec8();

    if( lowerRange < cosomega && cosomega < upperRange)
    {
        double omega = std::acos(cosomega) ;  // 0 <= omega <= Pi (see man acos)
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
const DualQuat DualQuatUtils::squad(const DualQuat &q1,const DualQuat &q2,const DualQuat &a,const DualQuat &b, const DualQuat::value_type t)
{
	DualQuat c(slerpNoInvert(q1, q2, t));
	DualQuat d(slerpNoInvert(a, b, t));
	return slerpNoInvert(c, d, 2.0 * t * (1.0 - t));
}

const DualQuat DualQuatUtils::simpleSquadCall(const DualQuat &q1,const DualQuat &q2,const DualQuat &q3,const DualQuat &q4, const DualQuat::value_type t)
{
    return squad(q2, q3, spline(q1, q2, q3), spline(q2, q3, q4), t);
}

//! Shoemake-Bezier interpolation using De Castlejau algorithm
const DualQuat DualQuatUtils::bezier(const DualQuat &q1, const DualQuat &q2, const DualQuat &a, const DualQuat &b, const DualQuat::value_type t)
{
	// level 1
	DualQuat q11(slerpNoInvert(q1, a, t));
	DualQuat q12(slerpNoInvert(a, b, t));
	DualQuat q13(slerpNoInvert(b, q2, t));
	// level 2 and 3
	return slerpNoInvert(slerpNoInvert(q11, q12, t), slerpNoInvert(q12, q13, t), t);
}

const DualQuat DualQuatUtils::simpleBezierCall(const DualQuat &q1,const DualQuat &q2,const DualQuat &q3,const DualQuat &q4, const DualQuat::value_type t)
{
    return bezier(q2, q3, spline(q1, q2, q3), spline(q2, q3, q4), t);
}

//! Given 3 quaternions, qn-1,qn and qn+1, calculate a control point to be used in spline interpolation
const DualQuat DualQuatUtils::spline(const DualQuat &qnm1, const DualQuat &qn, const DualQuat &qnp1)
{
	//inverse should be done but only unit quaternions are expected so conjugate is more efficient!!
	DualQuat qni(qn.conjTrans());	
	return qn * exp((log(qni * qnm1)+log(qni * qnp1)) / -4.0);
}