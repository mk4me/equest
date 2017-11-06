/********************************************************************
	created:  2014/01/22
	created:  22:1:2014   9:34
	filename: Wrappers.h
	author:   Mateusz Janiak

	purpose:
	*********************************************************************/
#ifndef HEADER_GUARD_NEW_CHART__WRAPPERS_H__
#define HEADER_GUARD_NEW_CHART__WRAPPERS_H__

#include <utils/PtrPolicyStd.h>
#include <utils/ClonePolicies.h>
#include <objectwrapperlib/ObjectWrapper.h>
#include <threadingUtils/StreamData.h>
#include <plugins/c3d/C3DChannels.h>

typedef threadingUtils::IStreamT<std::pair<float, float>> ScalarStream;
DEFINE_SMART_POINTERS(ScalarStream);

//DEFINE_WRAPPER(c3dlib::ScalarChannelReaderInterface, utils::PtrPolicyStd, utils::ClonePolicyForbidden);
DEFINE_WRAPPER(ScalarStream, utils::PtrPolicyStd, utils::ClonePolicyForbidden);

#endif	//	HEADER_GUARD_NEW_CHART__WRAPPERS_H__
