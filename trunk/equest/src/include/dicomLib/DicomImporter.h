/********************************************************************
	created:	2013/10/21
	created:	21:10:2013   8:36
	filename: 	DicomImporter.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_DICOMIMPORTER__DICOMIMPORTER_H__
#define HEADER_GUARD_DICOMIMPORTER__DICOMIMPORTER_H__

#include "DicomInternalStruct.h"
#include <utils/Filesystem.h>
#include <boost/tuple/tuple.hpp>
#include <QtGui/QPixmap>
#include <boost/function.hpp>

class DcmDirectoryRecord;

namespace dicomImporter {
class DicomImporter
{
public:
    typedef boost::function<void (const std::string&)> refresher;

public:
	DicomImporter(int studyFristId = 1, bool createSessionDir = false);

	virtual ~DicomImporter() {}
    DicomInternalStructPtr import( const utils::Filesystem::Path& from );
    void convertImages(DicomInternalStructPtr inter, const utils::Filesystem::Path& from, const utils::Filesystem::Path& to );
    std::vector<DicomInternalStructPtr> split(DicomInternalStructPtr);

    int getStudyCurrentIndex() const { return studyCurrentIndex; }
    void setStudyCurrentIndex(int val) { studyCurrentIndex = val; }

    void setCallBack(refresher r);

	static bool testPowerDoppler(const QPixmap &pixmap);
	DicomInternalStructPtr importRaw(const utils::Filesystem::Path& from);
	utils::Filesystem::Path findDicomRootDir(const utils::Filesystem::Path& from);

private:
    void handlePatientRecord( DcmDirectoryRecord * patientRecord, internalData::PatientPtr patient, std::string basePath );
    void handleStudyRecord( DcmDirectoryRecord * studyRecord, internalData::StudyPtr study,  std::string basePath );
    void handleSeriesRecord( DcmDirectoryRecord * seriesRecord, internalData::SeriePtr serie, std::string basePath );
    void handleFileRecord( DcmDirectoryRecord * fileRecord, internalData::ImagePtr image, std::string basePath );
    void convertImage( internalData::ImagePtr inter, const utils::Filesystem::Path& from, const utils::Filesystem::Path& to, const std::string& filenameBase );


    std::string getSessionDir() const;
private:
    int studyCurrentIndex;
    refresher refresh;
	bool createSessionDir;
};
DEFINE_SMART_POINTERS(DicomImporter);

class DicomSaver
{
public:
    virtual ~DicomSaver() {}
    void save( const utils::Filesystem::Path& to, DicomInternalStructPtr inter, bool createSessionFolder = true );
};
DEFINE_SMART_POINTERS(DicomSaver);

class DicomLoader
{
public:
    virtual ~DicomLoader() {}
    DicomInternalStructPtr load( const utils::Filesystem::Path& from);
};
DEFINE_SMART_POINTERS(DicomLoader);
}
#endif
