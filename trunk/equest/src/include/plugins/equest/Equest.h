/********************************************************************
	created:	2013/08/12
	created:	12:8:2013   22:54
	filename: 	Equest.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_EQUEST__EQUEST_H__
#define HEADER_GUARD_EQUEST__EQUEST_H__

#include <objectwrapperlib/ObjectWrapper.h>
#include <plugins/equest/Export.h>
#include "utils/PtrPolicyStd.h"
#include "utils/ClonePolicies.h"

namespace equest
{
	class EquestAnnotations;
}

DEFINE_WRAPPER(equest::EquestAnnotations, utils::PtrPolicyStd, utils::ClonePolicyForbidden);
#endif
