/********************************************************************
    created:	2012/03/25
    created:	25:3:2012   20:35
    filename: 	C3DCollections.h
    author:		Wojciech Kniec

    purpose:
*********************************************************************/

#ifndef HEADER_GUARD_C3DPLUGIN__C3DCOLLECTIONS_H__
#define HEADER_GUARD_C3DPLUGIN__C3DCOLLECTIONS_H__

#include <plugins/c3d/Export.h>
#include <c3dlib/C3DCollectionTypes.h>
#include <objectwrapperlib/ObjectWrapper.h>

DEFINE_WRAPPER(c3dlib::EMGCollection, utils::PtrPolicyStd, utils::ClonePolicyCopyConstructor);
DEFINE_WRAPPER(c3dlib::C3DEventsCollection, utils::PtrPolicyStd, utils::ClonePolicyCopyConstructor);
DEFINE_WRAPPER(c3dlib::VectorChannelCollection, utils::PtrPolicyStd, utils::ClonePolicyCopyConstructor);
DEFINE_WRAPPER(c3dlib::MovieDelays, utils::PtrPolicyStd, utils::ClonePolicyCopyConstructor);
DEFINE_WRAPPER_INHERITANCE(c3dlib::ForceCollection,  c3dlib::VectorChannelCollection);
DEFINE_WRAPPER_INHERITANCE(c3dlib::AngleCollection,  c3dlib::VectorChannelCollection);
DEFINE_WRAPPER_INHERITANCE(c3dlib::PowerCollection,  c3dlib::VectorChannelCollection);
DEFINE_WRAPPER_INHERITANCE(c3dlib::MomentCollection, c3dlib::VectorChannelCollection);
DEFINE_WRAPPER(c3dlib::MarkerCollection, utils::PtrPolicyStd, utils::ClonePolicyForbidden);
DEFINE_WRAPPER(c3dlib::GRFCollection, utils::PtrPolicyStd, utils::ClonePolicyForbidden);

#endif
