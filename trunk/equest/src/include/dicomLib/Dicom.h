/********************************************************************
	created:	2013/08/12
	created:	12:8:2013   22:54
	filename: 	Dicom.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_DICOM__DICOM_H__
#define HEADER_GUARD_DICOM__DICOM_H__

#include <dcmtk/config/osconfig.h>
#include <dcmtk/dcmdata/dcdatset.h>
#include <dcmtk/dcmimgle/dcmimage.h>
#include <objectwrapperlib/ObjectWrapper.h>
#include <QtGui/QPixmap>
//#include <plugins/dicomImporter/Export.h>
#include "utils/PtrPolicyStd.h"
#include "utils/ClonePolicies.h"
#include <objectwrapperlib/ObjectWrapper.h>

namespace dicomImporter {
    DEFINE_SMART_POINTERS(DicomImage);
    DEFINE_SMART_POINTERS(DcmDataset);
    QPixmap convertToPixmap(DicomImagePtr image);
}

DEFINE_WRAPPER(DicomImage, utils::PtrPolicyStd, utils::ClonePolicyForbidden);
DEFINE_WRAPPER(DcmDataset, utils::PtrPolicyStd, utils::ClonePolicyForbidden);
#endif
