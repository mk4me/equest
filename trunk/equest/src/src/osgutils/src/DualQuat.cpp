#include <osgutils/DualQuat.h>
#include <osgutils/DualQuatUtils.h>

using namespace osg;

void DualQuat::slerp(const value_type  t, const DualQuat& from, const DualQuat& to)
{
	static const double lowerRange = -1.0 + DualQuatUtils::DUALQUAT_EPSILON;
	static const double upperRange = -lowerRange;

	double scale_from = 1.0 - t;
	// this is a dot product
	double cosomega = from.asVec8() * to.asVec8();
    
    DualQuat quatTo(to);
    // this is a dot product
    
    cosomega = from.asVec8() * to.asVec8();
    
    if (cosomega < 0.0)
    { 
        cosomega = -cosomega; 
        quatTo = -to;
    }

    if(cosomega < upperRange)
    {
		double omega = std::acos(cosomega) ;  // 0 <= omega <= Pi (see man acos)
		//double sinomega = std::sin(omega) ;  // this sinomega should always be +ve so
		// could try sinomega=sqrt(1-cosomega*cosomega) to avoid a sin()?
		*this = ((from * std::sin(scale_from * omega)) + (to * std::sin(t * omega))) / std::sin(omega);
    }else{
		*this = (from * scale_from) + (to * t);
	}    
}