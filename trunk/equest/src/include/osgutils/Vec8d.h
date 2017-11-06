#ifndef OSG_VEC8D
#define OSG_VEC8D 1

#include <osg/Vec3d>
#include <osg/Vec4d>
#include <algorithm>

namespace osg {

/** General purpose double octanion.
  * No support yet added for double * Vec8d - is it necessary?
  * Need to define a non-member non-friend operator*  etc.
  * Vec8d * double is okay
*/
	
class Vec8d
{
public:

    /** Data type of vector components.*/
    typedef double value_type;

    /** Number of vector components. */
    enum { num_components = 8 };
        
private:

    /** Vec member variable. */
	value_type _v[num_components];

public:

    // Methods are defined here so that they are implicitly inlined

    /** Constructor that sets all components of the vector to zero */
	Vec8d()
	{		
		std::fill(_v, _v + 8, 0.0);
	}
        
    Vec8d(value_type x, value_type y, value_type z, value_type w, value_type xe, value_type ye, value_type ze, value_type we)
    {
        set(x, y, z, w, xe, ye, ze, we);
    }

    Vec8d(const osg::Vec3d& v3,value_type w, const osg::Vec3d& v3e,value_type we)
    {
        set(v3, w, v3e, we);
    }

	Vec8d(const osg::Vec4d& v4,const osg::Vec4d& v4e)
	{
		set(v4, v4e);
	}
            
    inline const bool operator == (const Vec8d& v) const { return _v[0]==v._v[0] && _v[1]==v._v[1] && _v[2]==v._v[2] && _v[3]==v._v[3]; _v[4]==v._v[4] && _v[5]==v._v[5] && _v[6]==v._v[6] && _v[7]==v._v[7];}

    inline const bool operator != (const Vec8d& v) const { return _v[0]!=v._v[0] || _v[1]!=v._v[1] || _v[2]!=v._v[2] || _v[3]!=v._v[3]; _v[4]!=v._v[4] || _v[5]!=v._v[5] || _v[6]!=v._v[6] || _v[7]!=v._v[7];}

    inline const bool operator <  (const Vec8d& v) const
    {
        if (_v[0]<v._v[0]) return true;
        else if (_v[0]>v._v[0]) return false;
        else if (_v[1]<v._v[1]) return true;
        else if (_v[1]>v._v[1]) return false;
        else if (_v[2]<v._v[2]) return true;
        else if (_v[3]>v._v[3]) return false;
        else if (_v[4]>v._v[4]) return false;
        else if (_v[5]>v._v[5]) return false;
        else if (_v[6]>v._v[6]) return false;
        else return (_v[7]>v._v[7]);
    }

    inline value_type* ptr() { return _v; }
    inline const value_type* ptr() const { return _v; }

    inline void set(value_type x, value_type y, value_type z, value_type w, value_type xe, value_type ye, value_type ze, value_type we)
    {
        _v[0]=x; _v[1]=y; _v[2]=z; _v[3]=w; _v[4]=xe; _v[5]=ye; _v[6]=ze; _v[7]=we;
    }

	inline void set(const osg::Vec3d& v3,value_type w, const osg::Vec3d& v3e,value_type we)
	{
		_v[0]=v3[0];
		_v[1]=v3[1];
		_v[2]=v3[2];
		_v[3]=w;
		_v[4]=v3e[0];
		_v[5]=v3e[1];
		_v[6]=v3e[2];
		_v[7]=we;
	}

	inline void set(const osg::Vec4d& v4, const osg::Vec4d& v4e)
	{
		setLowerHalf(v4);
		setUpperHalf(v4e);
	}

	inline void setLowerHalf(const osg::Vec4d & lowerHalf)
	{
		_v[0]=lowerHalf[0];
		_v[1]=lowerHalf[1];
		_v[2]=lowerHalf[2];
		_v[3]=lowerHalf[3];
	}

	inline void setUpperHalf(const osg::Vec4d & upperHalf)
	{
		_v[4]=upperHalf[0];
		_v[5]=upperHalf[1];
		_v[6]=upperHalf[2];
		_v[7]=upperHalf[3];
	}

    inline value_type& operator [] (int i) { return _v[i]; }
    inline const value_type  operator [] (int i) const { return _v[i]; }

	inline value_type& x() { return _v[0]; }
	inline value_type& y() { return _v[1]; }
	inline value_type& z() { return _v[2]; }
	inline value_type& w() { return _v[3]; }
	inline value_type& xe() { return _v[4]; }
	inline value_type& ye() { return _v[5]; }
	inline value_type& ze() { return _v[6]; }
	inline value_type& we() { return _v[7]; }

	inline const value_type x() const { return _v[0]; }
	inline const value_type y() const { return _v[1]; }
	inline const value_type z() const { return _v[2]; }
	inline const value_type w() const { return _v[3]; }
	inline const value_type xe() const { return _v[4]; }
	inline const value_type ye() const { return _v[5]; }
	inline const value_type ze() const { return _v[6]; }
	inline const value_type we() const { return _v[7]; }

    /** Returns true if all components have values that are not NaN. */
    inline const bool valid() const { return !isNaN(); }
    /** Returns true if at least one component has value NaN. */
    inline const bool isNaN() const { return osg::isNaN(_v[0]) || osg::isNaN(_v[1]) || osg::isNaN(_v[2]) || osg::isNaN(_v[3]) ||
        osg::isNaN(_v[4]) || osg::isNaN(_v[5]) || osg::isNaN(_v[6]) || osg::isNaN(_v[7]); }

    /** Dot product. */
    inline const value_type operator * (const Vec8d& rhs) const
    {
        return _v[0]*rhs._v[0]+
                _v[1]*rhs._v[1]+
                _v[2]*rhs._v[2]+
                _v[3]*rhs._v[3]+
                _v[4]*rhs._v[4]+
                _v[5]*rhs._v[5]+
                _v[6]*rhs._v[6]+
                _v[7]*rhs._v[7];
    }

    /** Multiply by scalar. */
    inline const Vec8d operator * (value_type rhs) const
    {
        return Vec8d(_v[0]*rhs, _v[1]*rhs, _v[2]*rhs, _v[3]*rhs, _v[4]*rhs, _v[5]*rhs, _v[6]*rhs, _v[7]*rhs);
    }

    /** Unary multiply by scalar. */
    inline Vec8d& operator *= (value_type rhs)
    {
        _v[0]*=rhs;
        _v[1]*=rhs;
        _v[2]*=rhs;
        _v[3]*=rhs;
        _v[4]*=rhs;
        _v[5]*=rhs;
        _v[6]*=rhs;
        _v[7]*=rhs;
        return *this;
    }

    /** Divide by scalar. */
    inline const Vec8d operator / (value_type rhs) const
    {
		value_type inv = 1.0 / rhs;
        return *this * rhs;
    }

    /** Unary divide by scalar. */
    inline Vec8d& operator /= (value_type rhs)
    {
		value_type inv = 1.0 / rhs;
        *this *= rhs;            
        return *this;
    }

    /** Binary vector add. */
    inline const Vec8d operator + (const Vec8d& rhs) const
    {
        return Vec8d(_v[0]+rhs._v[0], _v[1]+rhs._v[1],
                        _v[2]+rhs._v[2], _v[3]+rhs._v[3],
                        _v[4]+rhs._v[4], _v[5]+rhs._v[5],
                        _v[6]+rhs._v[6], _v[7]+rhs._v[7]);
    }

    /** Unary vector add. Slightly more efficient because no temporary
        * intermediate object.
    */
    inline Vec8d& operator += (const Vec8d& rhs)
    {
        _v[0] += rhs._v[0];
        _v[1] += rhs._v[1];
        _v[2] += rhs._v[2];
        _v[3] += rhs._v[3];
        _v[4] += rhs._v[4];
        _v[5] += rhs._v[5];
        _v[6] += rhs._v[6];
        _v[7] += rhs._v[7];
        return *this;
    }

    /** Binary vector subtract. */
    inline const Vec8d operator - (const Vec8d& rhs) const
    {
        return Vec8d(_v[0]-rhs._v[0], _v[1]-rhs._v[1],
                        _v[2]-rhs._v[2], _v[3]-rhs._v[3],
                        _v[4]-rhs._v[4], _v[5]-rhs._v[5],
                        _v[6]-rhs._v[6], _v[7]-rhs._v[7]);
    }

    /** Unary vector subtract. */
    inline Vec8d& operator -= (const Vec8d& rhs)
    {
        _v[0]-=rhs._v[0];
        _v[1]-=rhs._v[1];
        _v[2]-=rhs._v[2];
        _v[3]-=rhs._v[3];
        _v[4]-=rhs._v[4];
        _v[5]-=rhs._v[5];
        _v[6]-=rhs._v[6];
        _v[7]-=rhs._v[7];
        return *this;
    }

    /** Negation operator. Returns the negative of the Vec8d. */
    inline const Vec8d operator - () const
    {
        return *this * -1;
    }

    /** Length of the vector = sqrt( vec . vec ) */
    inline const value_type length() const
    {
        return std::sqrt(length2());
    }

    /** Length squared of the vector = vec . vec */
    inline const value_type length2() const
    {
        return *this * *this;
    }

    /** Normalize the vector so that it has length unity.
        * Returns the previous length of the vector.
    */
    inline const value_type normalize()
    {
        value_type norm2 = length2();
        if (norm2>0.0f)
        {
			const auto norm = std::sqrt(norm2);
            value_type inv = 1.0f/norm2;
            _v[0] *= inv;
            _v[1] *= inv;
            _v[2] *= inv;
            _v[3] *= inv;
            _v[4] *= inv;
            _v[5] *= inv;
            _v[6] *= inv;
            _v[7] *= inv;
			return norm;
        }

		return 0.0;
        
    }

    inline const osg::Vec4d lowerHalf() const
    {
        return osg::Vec4d(_v[0], _v[1], _v[2], _v[3]);
    }

    inline const osg::Vec4d upperHalf() const
    {
        return osg::Vec4d(_v[4], _v[5], _v[6], _v[7]);
    }

};    // end of class Vec8d

/** multiply by vector components. */
inline const Vec8d componentMultiply(const Vec8d& lhs, const Vec8d& rhs)
{
    return Vec8d(lhs[0]*rhs[0], lhs[1]*rhs[1], lhs[2]*rhs[2], lhs[3]*rhs[3], lhs[4]*rhs[4], lhs[5]*rhs[5], lhs[6]*rhs[6], lhs[7]*rhs[7]);
}

/** divide rhs components by rhs vector components. */
inline const Vec8d componentDivide(const Vec8d& lhs, const Vec8d& rhs)
{
    return Vec8d(lhs[0]/rhs[0], lhs[1]/rhs[1], lhs[2]/rhs[2], lhs[3]/rhs[3], lhs[4]/rhs[4], lhs[5]/rhs[5], lhs[6]/rhs[6], lhs[7]/rhs[7]);
}

}	//end of namespace osg

#endif //OSG_VEC8D