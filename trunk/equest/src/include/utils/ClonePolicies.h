#ifndef HEADER_GUARD_UTILS__CLONEPOLICIES_H__
#define HEADER_GUARD_UTILS__CLONEPOLICIES_H__

#include <exception>
#include <utils/Debug.h>
//Plik zawiera podstawowe zasady kopiowania obiektów domenowych

////////////////////////////////////////////////////////////////////////////////
namespace utils {
////////////////////////////////////////////////////////////////////////////////

    //! Klonowanie odbywa się za pomoca konstruktora kopiującego
    struct ClonePolicyCopyConstructor
    {
        template <typename T>
        static T* clone(const T* s) {
            return new T(*s);
        }		
    };

    //! Klonowanie odbywa się za pomoca metody clone()
    struct ClonePolicyVirtualCloneMethod
    {
        template <typename T>
		static T* clone(const T* s) {
            auto copy = s->clone();
            T* result = dynamic_cast<T*>(copy);
            UTILS_ASSERT(result);
            return result;
        }
    };

    //! Powinno podawać się ta zasade, jeśli klonowanie z jakiegos powodu jest zabronione
    struct ClonePolicyForbidden
    {
        template <typename T>
		static T* clone(const T* s) {
            //static_assert(false, "Cloning forbidden!");
            throw std::runtime_error("Cloning forbidden!");
        }
    };

    using ClonePolicyNotImplemented = ClonePolicyForbidden;

////////////////////////////////////////////////////////////////////////////////
} // namespace utils
////////////////////////////////////////////////////////////////////////////////

#endif    // HEADER_GUARD_UTILS__CLONEPOLICIES_H__
