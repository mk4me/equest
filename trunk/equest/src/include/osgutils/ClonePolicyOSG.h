#ifndef HEADER_GUARD_OSGUTILS__CLONEPOLICIES_H__
#define HEADER_GUARD_OSGUTILS__CLONEPOLICIES_H__

#include <exception>
#include <utils/Debug.h>
#include <osg/Object>
#include <osg/CopyOp>

////////////////////////////////////////////////////////////////////////////////
namespace utils {
////////////////////////////////////////////////////////////////////////////////

    //! Klonowanie odbywa się za pomoca metody clone()
    struct ClonePolicyOsgCloneMethod
    {
        template <typename T>
		static T* clone(const T* s) {
            auto copy = osg::clone(s, osg::CopyOp::DEEP_COPY_ALL);
            T* result = dynamic_cast<T*>(copy);
            UTILS_ASSERT(result);
            return result;
        }
    };

////////////////////////////////////////////////////////////////////////////////
} // namespace utils
////////////////////////////////////////////////////////////////////////////////

#endif    // HEADER_GUARD_UTILS__CLONEPOLICIES_H__
