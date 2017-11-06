/********************************************************************
	created:	2013/10/21
	created:	21:10:2013   8:37
	filename: 	IDicomInternalStruct.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_DICOMIMPORTER__IDICOMINTERNALSTRUCT_H__
#define HEADER_GUARD_DICOMIMPORTER__IDICOMINTERNALSTRUCT_H__

#include "utils/SmartPtr.h"
namespace dicomImporter {
class IDicomInternalStruct
{
public:
	virtual ~IDicomInternalStruct() {}
};
DEFINE_SMART_POINTERS(IDicomInternalStruct);
}

#endif
