/********************************************************************
    created:  2011/12/15
    created:  15:12:2011   22:18
    filename: DualQuat.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef OSG_DUALQUAT
#define OSG_DUALQUAT 1

#include <osgutils/Vec8d.h>
#include <osgutils/QuatUtils.h>
#include <osg/Quat>
#include <osg/Matrixd>
#include <utility>

// Based on: http://www.euclideanspace.com/maths/algebra/realNormedAlgebra/other/dualQuaternion/index.htm

namespace osg {

	class DualQuatUtils;

/** A dual quaternion class. It can be used to represent an orientation and position in 3D space.*/
class DualQuat
{
	private:

		Vec8d  _v;    // a 8-vector

    public:

        typedef double value_type;
        typedef std::pair<value_type, value_type> module_type;

	public:

		//zero rotation and translation dual quaternion or position at center and no orientation
        inline DualQuat()
		{
			_v.w() = 1.0;
		}

		inline DualQuat( const Quat & rotation, const Vec3d & translation)
		{
			setRotation(rotation);
			setTranslation(translation);
		}

		inline DualQuat( const Quat & rotation, const Quat & translation)
		{
			setRotation(rotation);
			setTranslation(translation);
		}

        inline DualQuat( const Quat & rotation)
        {
            setRotation(rotation);
        }

        inline DualQuat( value_type x, value_type y, value_type z, value_type w, value_type xe, value_type ye, value_type ze, value_type we )
        {
            _v.set(x, y, z, w, xe, ye, ze, we);
        }

        inline DualQuat( const Vec8d& v )
        {
            _v = v;
        }

        inline DualQuat(const Vec3d& translation)
        {
			_v.w() = 1.0;
            setTranslation(translation);
        }

        inline DualQuat& operator = (const DualQuat& v) { _v=v._v; return *this; }

        inline const bool operator == (const DualQuat& v) const { return _v == v._v; }

        inline const bool operator != (const DualQuat& v) const { return _v != v._v; }

		//TODO
		//In quaternion space and dual quaternion space there is no order operation
        inline const bool operator < (const DualQuat& v) const
        {
            return _v < v._v;
        }

        /* ----------------------------------
           Methods to access data members
        ---------------------------------- */

        inline const Vec8d & asVec8() const
        {
            return _v;
        }

        inline void set(value_type x, value_type y, value_type z, value_type w, value_type xe, value_type ye, value_type ze, value_type we)
        {
            _v.set(x, y, z, w, xe, ye, ze, we);
        }
        
        inline void set(const Vec8d& v)
        {
            _v = v;
        }

        inline void setRotation(const Quat & rotation)
        {
            _v.setLowerHalf(rotation.asVec4());
        }

		inline void clearRotation()
		{
			_v[0] = _v[1] = _v[2] = 0.0;
			_v[3] = 1.0;
		}

        inline void setTranslation(const Vec3d & translation)
        {
			_v.setUpperHalf(Vec4d(translation, 0.0));
        }

		inline void setTranslation(const Quat & translation)
		{
			_v.setUpperHalf(translation.asVec4());
		}

		inline void clearTranslation()
		{
			_v[4] = _v[5] = _v[6] = _v[7] = 0.0;
		}

		inline void setOrientation(const Vec3d & orientation)
		{
			_v[0] = orientation[0];
			_v[1] = orientation[1];
			_v[2] = orientation[2];
			_v[3] = 1.0;
		}

		inline void setPosition(const Vec3d & position)
		{
			setTranslation(position);
		}

		inline void setTranslation(const Matrixd& matrix)
		{
			Quat rotation;
			Quat so;
			Vec3d translation;
			Vec3d scale;
			matrix.decompose(translation, rotation, scale, so);
			setTranslation(translation);
		}

		inline void setRotation(const Matrixd& matrix)
		{
			Quat rotation;
			Quat so;
			Vec3d translation;
			Vec3d scale;		
			matrix.decompose(translation, rotation, scale, so);
			setRotation(rotation);
		}

		inline void setRotationThenTranslation(const Matrixd& matrix)
		{
			Quat rotation;
			Quat so;
			Vec3d translation;
			Vec3d scale;
			matrix.decompose(translation, rotation, scale, so);
			makeRotationThenTranslation(rotation, translation);
		}

		inline void setTranslationThenRotation(const Matrixd& matrix)
		{
			Quat rotation;
			Quat so;
			Vec3d translation;
			Vec3d scale;
			matrix.decompose(translation, rotation, scale, so);
			makeTranslationThenRotation(rotation, translation);
		}

		inline void getTranslation(Matrixd& matrix) const
		{
			matrix.setTrans(translation().asVec3());
		}

		inline void getRotation(Matrixd& matrix) const
		{
			matrix.makeRotate(rotation());
		}

		inline void getTranslationThenRotation(Matrixd& matrix) const
		{
			Matrixd hM;
			hM.makeRotate(rotation());
			matrix.makeTranslate(translation().asVec3());

			matrix *= hM;
		}

		inline void getRotationThenTranslation(Matrixd& matrix) const
		{
			Matrixd hM;
			hM.makeTranslate(translation().asVec3());
			matrix.makeRotate(rotation());

			matrix *= hM;
		}

        inline value_type & operator [] (int i) { return _v[i]; }
        inline const value_type   operator [] (int i) const { return _v[i]; }

        inline value_type & x() { return _v.x(); }
        inline value_type & y() { return _v.y(); }
        inline value_type & z() { return _v.z(); }
        inline value_type & w() { return _v.w(); }
        inline value_type & xe() { return _v.xe(); }
        inline value_type & ye() { return _v.ye(); }
        inline value_type & ze() { return _v.ze(); }
        inline value_type & we() { return _v.we(); }

		inline const value_type x() const { _v.x(); }
		inline const value_type y() const { _v.y(); }
		inline const value_type z() const { _v.z(); }
		inline const value_type w() const { _v.w(); }
		inline const value_type xe() const { _v.xe(); }
		inline const value_type ye() const { _v.ye(); }
		inline const value_type ze() const { _v.ze(); }
		inline const value_type we() const { _v.we(); }

        /** return true if the DualQuat represents a zero rotation and zero translation, and therefore can be ignored in computations.*/
        const bool zeroTransformation() const { return (zeroRotation() && zeroTranslation()) ; } 

        /** return true if the Quat represents a zero rotation, and therefore can be ignored in computations.*/
        const bool zeroRotation() const { return _v[0]==0.0 && _v[1]==0.0 && _v[2]==0.0 && _v[3]==1.0; } 

        /** return true if the Quat represents a zero rotation, and therefore can be ignored in computations.*/
        const bool zeroTranslation() const { return _v[4]==0.0 && _v[5]==0.0 && _v[6]==0.0 && _v[7]==0.0; } 

        const Quat rotation() const
        {
            return Quat(_v.lowerHalf());
        }

        const Quat translation() const
        {
            return Quat(_v.upperHalf());
        }

		const Vec3d orientation() const
		{
			return rotation().asVec3();
		}

		const Vec3d position() const
		{
			return translation().asVec3();
		}

        /** return dual module of dual quaternion */
        const module_type module() const
        {
            value_type rotLength = rotation().length();
            return module_type(rotLength, rotation().asVec4() * translation().asVec4() / rotLength);
        }

         /* ------------------------------------------------------------- 
                   BASIC ARITHMETIC METHODS            
        Implemented in terms of Vec8d.  Some Vec8 operators, e.g.
        operator* are not appropriate for quaternions (as
        mathematical objects) so they are implemented differently.
        Also define methods for conjugate and the multiplicative inverse.            
        ------------------------------------------------------------- */
        /// Multiply by scalar 
        inline const DualQuat operator * (value_type rhs) const
        {
            return DualQuat(_v*rhs);
        }

        /// Unary multiply by scalar 
        inline DualQuat& operator *= (value_type rhs)
        {
            _v*=rhs;
            return *this;        // enable nesting
        }

        /// Binary multiply 
        inline const DualQuat operator*(const DualQuat& rhs) const
        {
			DualQuat ret;
            Quat a(rotation());
            Quat b(translation());
            Quat c(rhs.rotation());
            Quat d(rhs.translation());

            ret.setRotation(a*c);
			ret.setTranslation(a*d + b*c);

            return ret;
        }

        /// Unary multiply 
        inline DualQuat& operator*=(const DualQuat& rhs)
        {
            Quat a(rotation());
            Quat b(translation());
            Quat c(rhs.rotation());
            Quat d(rhs.translation());

            setRotation(a*c);
            setTranslation(a*d + b*c);

            return (*this);            // enable nesting
        }

        /// Divide by scalar 
        inline const DualQuat operator / (value_type rhs) const
        {
            return DualQuat(_v/rhs);
        }

        /// Unary divide by scalar 
        inline DualQuat& operator /= (value_type rhs)
        {
            _v/=rhs;
            return *this;
        }

        /// Binary divide 
        inline const DualQuat operator/(const DualQuat& denom) const
        {
            return ( (*this) * denom.inverse() );
        }

        /// Unary divide 
        inline DualQuat& operator/=(const DualQuat& denom)
        {
            (*this) = (*this) * denom.inverse();
            return (*this);            // enable nesting
        }

        /// Binary addition 
        inline const DualQuat operator + (const DualQuat& rhs) const
        {
            return DualQuat(_v + rhs._v);
        }

        /// Unary addition
        inline DualQuat& operator += (const DualQuat& rhs)
        {
            _v += rhs._v;
            return *this;            // enable nesting
        }

        /// Binary subtraction 
        inline const DualQuat operator - (const DualQuat& rhs) const
        {
            return DualQuat(_v-rhs._v);
        }

        /// Unary subtraction 
        inline DualQuat& operator -= (const DualQuat& rhs)
        {
            _v -= rhs._v;
            return *this;            // enable nesting
        }

        /** Negation operator - returns the negative of the quaternion.
        Basically just calls operator - () on the Vec4 */
        inline const DualQuat operator - () const
        {
            return DualQuat (-_v);
        }

        /// Length of the quaternion = sqrt( vec . vec )
        inline const value_type length() const
        {
            return _v.length();
        }

        /// Length of the quaternion = vec . vec
        inline const value_type length2() const
        {
            return _v.length2();
        }

        /// Conjugate for translation
        inline const DualQuat conjTrans () const
        { 
             return DualQuat( rotation().conj(), -translation().conj() );
        }

        /// Conjugate for reversing multiplicands
        inline const DualQuat conjReverse () const
        { 
            return DualQuat( rotation().conj(), translation().conj() );
        }

        /// Conjugate
        inline const DualQuat conjOther () const
        { 
            return DualQuat( rotation(), -translation() );
        }

        /// Multiplicative inverse method: q^(-1) = q^*/(q.q^*)
        inline const DualQuat inverse () const
        {
			DualQuat ret;
            Quat rot(rotation());
            Quat trans(translation());
            Quat rotInv(rot.inverse());

			ret.setRotation(rotInv * rot.length2());
			ret.setTranslation(trans.inverse() * rot.length2() - rotInv * (rot.asVec4() * trans.asVec4()));
            return ret;
         }

		/* -------------------------------------------------------- 
               METHODS RELATED TO ROTATIONS AND TRANSLATIONS
        Set a dual quaternion which will perform a rotation and translation.

        Define Spherical Linear interpolation method also
        -------------------------------------------------------- */
        //ROTATION
		void makeRotate(const Quat & rot)
		{
			setRotation(rot);
			clearTranslation();
		}

		//TRANSLATION
		/** Make a translation DualQuat which will translate us with vector 'translation'. */
		void makeTranslation(const Vec3d& translation)
		{
			clearRotation();
			setPosition(translation * 0.5);
		}

		//MIX
		void makeTranslationThenRotation(const Quat & rot, const Vec3d& translation)
		{
			*this = DualQuat(translation * 0.5) * DualQuat(rot);
		}
		
		void makeRotationThenTranslation(const Quat & rot, const Vec3d& translation)
		{
			*this = DualQuat(rot) * DualQuat(translation * 0.5);
		}

		//ROTATION ABOUT A POINT
		void makeRotationAboutPoint(const Quat & rot, const Vec3d & point)
		{
			setRotation(rot);

			Quat p(point.x(), point.y(), point.z(), 1.0);

			setTranslation((rot * p - p * rot) * 0.5);
		}

        /** Spherical Linear Interpolation.
        As t goes from 0 to 1, the DualQuat object goes from "from" to "to". */
        void slerp(const value_type  t, const DualQuat& from, const DualQuat& to);
               
        /** Rotate and translate a vector by this dual quaternion.*/
        Vec3d operator* (const Vec3d& v) const
        {
            DualQuat vQ(QuatUtils::UNIT_QUAT, v);

            vQ = conjTrans() * vQ * (*this);
            return vQ.position();
        }
    
};    // end of class prototype

}	//end of namespace osg

#endif //OSG_DUALQUAT

