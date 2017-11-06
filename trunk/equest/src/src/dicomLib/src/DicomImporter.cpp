
#include "DicomImporterPCH.h"
#include "QtCore/QDateTime"

#include "dicomLib/DicomImporter.h"
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
//#include "dicomLib/DicomImporterSource.h"
#include <utils/Filesystem.h>
//#include <corelib/HierarchyItem.h>
//#include <corelib/HierarchyDataItem.h>
//#include "DicomImporterSourceWidget.h"
#include "dcmtk/dcmdata/dcdicdir.h"
#include "dcmtk/dcmdata/dcdirrec.h"
#include "dcmtk/dcmdata/dcdeftag.h"
#include <dcmtk/ofstd/ofbmanip.h>
#include <dcmtk/dcmimage/diregist.h>
//#include <corelib/HierarchyHelper.h>
#include <dicomLib/Dicom.h>
//#include <plugins/dicomImporter/ILayeredImage.h>

#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>
#include <boost/format.hpp>
#include "loglib/Exceptions.h"



using namespace dicomImporter;
QPixmap dicomImporter::convertToPixmap(DicomImagePtr image)
{
	QPixmap result;
	// kod z forum : http://forum.dcmtk.org/viewtopic.php?t=120
	// rozszerzony o obs³ugê kolorów
	if ((image != NULL) && (image->getStatus() == EIS_Normal)) {
		/* get image extension */
		const int width = (int)(image->getWidth());
		const int height = (int)(image->getHeight());
		//const int depth = (int)(image->getDepth());
		char header[32];
		/* create PGM header */
		sprintf(header, "P%i\n%i %i\n255\n", image->isMonochrome() ? 5 : 6, width, height);
		const int offset = strlen(header);
		const unsigned int length = image->getOutputDataSize(8) + offset;//width * height + offset;
		/* create output buffer for DicomImage class */
		Uint8 *buffer = new Uint8[length];
		if (buffer != NULL) {
			/* copy PGM header to buffer */
			OFBitmanipTemplate<Uint8>::copyMem((const Uint8 *)header, buffer, offset);
			if (image->getOutputData((void *)(buffer + offset), length)) {
				result.loadFromData((const unsigned char *)buffer, length, image->isMonochrome() ? "PGM" : "PMM", Qt::AvoidDither);
			}
			/* delete temporary pixel buffer */
			delete[] buffer;
		}
	}

	return result;


	// ponizszy kod przeksztalcil 16 bitowe monochromatyczne obrazy

	//if ((image != NULL) && (image->getStatus() == EIS_Normal)) {
	//    const int width = (int)(image->getWidth());
	//    const int height = (int)(image->getHeight());
	//    const int depth = (int)(image->getDepth());

	//	QImage qi(width, height, QImage::Format_RGB32);
	//	const unsigned int length = image->getOutputDataSize(16);
	//	double maxv = 0, minv = 65535;
	//	//image->getMinMaxValues(minv, maxv);
	//    /* create output buffer for DicomImage class */
	//    Uint16 *buffer = new Uint16[length];
	//    if (buffer != NULL)
	//    {
	//        if (image->getOutputData((void *)(buffer), length,16))
	//        {
	//			for (int y = 0; y < height; ++y) {
	//				for (int x = 0; x < width; ++x) {
	//					Uint16 p = buffer[(y*width + x)];
	//					if (p < minv) {
	//						minv = p;
	//					}
	//					if (p > maxv) {
	//						maxv = p;
	//					}
	//				}
	//			}

	//			for (int y = 0; y < height; ++y) {
	//				for (int x = 0; x < width; ++x) {
	//					Uint16 p = buffer[(y*width + x)];
	//					auto val = 255.0 * (p-minv)/(maxv-minv);
	//					qi.setPixel(x, y, qRgb(val, val, val));
	//				}
	//			}
	//        }
	//        /* delete temporary pixel buffer */
	//        delete[] buffer;
	//    }

	//	return QPixmap::fromImage(qi);
	//}
	//return QPixmap();

}


void DicomImporter::handleFileRecord( DcmDirectoryRecord * fileRecord, internalData::ImagePtr image, std::string basePath )
{
    OFString tmpString;
    if(fileRecord->findAndGetOFStringArray(DCM_DirectoryRecordType,tmpString).good() && (tmpString == "IMAGE" || tmpString == "ENCAP DOC"))
    {
        if(fileRecord->findAndGetOFStringArray(DCM_ReferencedFileID,tmpString).good())
        {
            OFString uidImagen,instanceNumber;
            fileRecord->findAndGetOFString(DCM_ReferencedSOPInstanceUIDInFile, uidImagen);
            fileRecord->findAndGetOFString(DCM_InstanceNumber, instanceNumber);

            utils::Filesystem::Path currentPath(basePath);
            
			std::string suffix(tmpString.c_str()), toFind("\\"), toReplace("/");
			for (std::string::size_type i = 0; (i = suffix.find(toFind, i)) != std::string::npos;) {
				suffix.replace(i, toFind.length(), toReplace);
				i += toReplace.length();
			}

			currentPath /= suffix;
            if (utils::Filesystem::pathExists(currentPath)) {
                image->originFilePath = suffix;
            } 
        }
    }
}

void DicomImporter::handleSeriesRecord( DcmDirectoryRecord * seriesRecord, internalData::SeriePtr serie, std::string basePath )
{
    OFString serieId, modality, serieDate, serieTime, serieDesc, serieNumber, physicianName;
    seriesRecord->findAndGetOFString(DCM_SeriesInstanceUID, serieId);
    seriesRecord->findAndGetOFString(DCM_Modality, modality);
    seriesRecord->findAndGetOFString(DCM_SeriesDate, serieDate);
    seriesRecord->findAndGetOFString(DCM_SeriesTime, serieTime);
    seriesRecord->findAndGetOFString(DCM_SeriesDescription, serieDesc);
    seriesRecord->findAndGetOFString(DCM_SeriesNumber, serieNumber);
    seriesRecord->findAndGetOFString(DCM_ReferringPhysicianName, physicianName);
    
    serie->serieId = serieId.c_str();
    serie->modality = modality.c_str();
    serie->serieDate = serieDate.c_str();
    serie->serieTime = serieTime.c_str();
    serie->serieDesc = serieDesc.c_str();
    serie->serieNumber = serieNumber.c_str();
    serie->physicianName = physicianName.c_str();

    DcmDirectoryRecord * fileRecord = nullptr;
    bool powerDoppler = false;
    while((fileRecord = seriesRecord->nextSub(fileRecord)) != nullptr) {
        auto image = boost::make_shared<internalData::Image>();
        handleFileRecord(fileRecord, image, basePath);
        image->isPowerDoppler = powerDoppler;
        powerDoppler = !powerDoppler;
        serie->images.push_back(image);
    }	
}

void DicomImporter::handleStudyRecord( DcmDirectoryRecord * studyRecord, internalData::StudyPtr study, std::string basePath )
{
    OFString studyId, studyDesc, studyDate, studyTime, physicianName, accesionNumber;
    studyRecord->findAndGetOFString(DCM_StudyInstanceUID, studyId);
    studyRecord->findAndGetOFString(DCM_StudyDescription, studyDesc);
    studyRecord->findAndGetOFString(DCM_StudyDate, studyDate);
    studyRecord->findAndGetOFString(DCM_StudyTime, studyTime);
    studyRecord->findAndGetOFString(DCM_ReferringPhysicianName, physicianName);
    studyRecord->findAndGetOFString(DCM_AccessionNumber, accesionNumber);

    study->studyNumber = studyCurrentIndex++;
    study->studyId = studyId.c_str();
    study->studyDesc = studyDesc.c_str();
    study->studyDate = studyDate.c_str();
    study->studyTime = studyTime.c_str();
    study->physicianName = physicianName.c_str();
    study->accesionNumber = accesionNumber.c_str();

    DcmDirectoryRecord* seriesRecord = nullptr;    
    while (((seriesRecord = studyRecord->nextSub(seriesRecord)) != nullptr)) {
        auto serie = boost::make_shared<internalData::Serie>();
        handleSeriesRecord(seriesRecord, serie, basePath);
        study->series.push_back(serie);
    }
}

void DicomImporter::handlePatientRecord( DcmDirectoryRecord * patientRecord, internalData::PatientPtr patient, std::string basePath )
{
    OFString patientID, patientName, patientBirthdate, patientSex;
    patientRecord->findAndGetOFString(DCM_PatientID,patientID);
    patientRecord->findAndGetOFString(DCM_PatientName,patientName);
    patientRecord->findAndGetOFString(DCM_PatientBirthDate, patientBirthdate);
    patientRecord->findAndGetOFString(DCM_PatientSex, patientSex);
    
    patient->patientID = patientID.c_str();
    patient->patientName = patientName.c_str();
    patient->patientBirthdate = patientBirthdate.c_str();
    patient->patientSex = patientSex.c_str();

    DcmDirectoryRecord* studyRecord = nullptr;
    while ((studyRecord = patientRecord->nextSub(studyRecord)) != nullptr)  {
        auto study = boost::make_shared<internalData::Study>();
        handleStudyRecord(studyRecord, study, basePath);
        patient->sessions.push_back(study);
    }

}

dicomImporter::DicomInternalStructPtr DicomImporter::import( const utils::Filesystem::Path& from )
{
    DicomInternalStructPtr internalStruct = boost::make_shared<DicomInternalStruct>();
	
    utils::Filesystem::Path dirfile = from / "DICOMDIR";
    if (utils::Filesystem::pathExists(dirfile)) {
        DcmDicomDir dicomdir(dirfile.string().c_str());
        DcmDirectoryRecord* root = &(dicomdir.getRootRecord());

		std::string basePath = from.string();

        if (root) {
            DcmDirectoryRecord *patientRecord = nullptr;
            while ((patientRecord = root->nextSub(patientRecord)) != nullptr) {
                internalData::PatientPtr patient = boost::make_shared<internalData::Patient>();
                handlePatientRecord(patientRecord, patient, basePath);
                internalStruct->addPatient(patient);
            }
        } 

    } else {
        throw loglib::runtime_error("file not found");
    }

    return internalStruct;
}

void dicomImporter::DicomImporter::convertImages( DicomInternalStructPtr inter, const utils::Filesystem::Path& from, const utils::Filesystem::Path& to )
{
    for (auto itPatient = inter->patients.begin(); itPatient != inter->patients.end(); ++itPatient) {
        for (auto itSession = (*itPatient)->sessions.begin(); itSession != (*itPatient)->sessions.end(); ++itSession) {
            std::string sessionDir = (*itSession)->getOutputDirectory();
			if (createSessionDir) {
				utils::Filesystem::createDirectory(to / sessionDir);
			} else {
				utils::Filesystem::createDirectory(to);
			}
            int trialImageNo = 1;
            for (auto itSerie = (*itSession)->series.begin(); itSerie != (*itSession)->series.end(); ++itSerie) {
                internalData::SeriePtr serie = *itSerie;
                int trialCount = serie->images.size();
                for (int i = 0; i < trialCount; ++i) {
                    internalData::DateTuple dt = internalData::extractDate((*itSession)->studyDate);
                    if (inter->isSingle()) {
                        boost::format fmt("%s-%s-%s-S%04d-T%04d");
                        fmt % dt.get<0>() % dt.get<1>() % dt.get<2>() % (*itSession)->studyNumber % trialImageNo++;
                        convertImage((*itSerie)->images[i], from, createSessionDir ? to/sessionDir : to, fmt.str());
                    } else {
                        boost::format fmt("%s/%s-%s-%s-S%04d-T%04d");
                        fmt % sessionDir % dt.get<0>() % dt.get<1>() % dt.get<2>() % (*itSession)->studyNumber % trialImageNo++;
                        convertImage((*itSerie)->images[i], from, to, fmt.str());
                    }
                }
            }
        }
    }
}

void dicomImporter::DicomImporter::convertImage( internalData::ImagePtr inter, const utils::Filesystem::Path& from, const utils::Filesystem::Path& to, const std::string& filenameBase )
{
    utils::Filesystem::Path origin = from / inter->originFilePath;
	
    if (utils::Filesystem::pathExists(origin)) {
  //      DicomParser parser;
  //      parser.parse(origin.string());
		//auto object = core::Variant::create<DicomImage>();
  //      parser.getObject(*object, 0);
  //      //utils::ObjectWrapperPtr wrapper = object->get();
		DicomImagePtr di = utils::make_shared<DicomImage>(origin.string().c_str());//dfile, xfer, opt_compatibilityMode, opt_frame - 1, opt_frameCount);
		if (di && di->getStatus() == EIS_Normal) {
			di->processNextFrames();
			auto count = di->getFrameCount();
			QPixmap pixmap = convertToPixmap(di);
			if (pixmap.isNull()) {
				di->writeBMP("temp.bmp");
				pixmap = QPixmap("temp.bmp");
			}

			utils::Filesystem::Path outfile = to / (filenameBase + ".png");
    
			pixmap.save(outfile.string().c_str());

			inter->imageFile = filenameBase + ".png";
			inter->adnotationsFile = filenameBase + ".xml";
			inter->isPowerDoppler = testPowerDoppler(pixmap);
			refresh(filenameBase);
		}
    }
}



std::vector<DicomInternalStructPtr> dicomImporter::DicomImporter::split( DicomInternalStructPtr root )
{
    std::vector<DicomInternalStructPtr> results;
    int count = root->getNumPatients();
    for (int i = 0; i < count; ++i) {
        internalData::PatientConstPtr patient = root->getPatient(i);
        for (auto it = patient->sessions.begin(); it != patient->sessions.end(); ++it) {
            internalData::PatientPtr patientClone = boost::make_shared<internalData::Patient>();
            patientClone->cloneMeta(*patient);
            patientClone->sessions.push_back(boost::make_shared<internalData::Study>(**it));

            DicomInternalStructPtr inter = boost::make_shared<DicomInternalStruct>();
            inter->cloneMeta(*root);
            inter->addPatient(patientClone);
            results.push_back(inter);
        }

    }

    return results;
}


static void dummy(const std::string&) {}

dicomImporter::DicomImporter::DicomImporter( int studyFristId /*= 1*/ , bool createSessionDir) :
    studyCurrentIndex(studyFristId),
	createSessionDir(createSessionDir)
{
    refresh = dummy;
}

void dicomImporter::DicomImporter::setCallBack( refresher r )
{
    refresh = r;
}

bool dicomImporter::DicomImporter::testPowerDoppler( const QPixmap &pixmap )
{
	// badamy napisy po prawej stronie obrazu. Te z power dopplerem maja ich wiecej.
    int x1 = 855, y1 = 130, x2 = 950, y2 = 400, grey = 140;
    if (!pixmap.isNull() && pixmap.width() >= x2 && pixmap.height() >= y2) {
        QImage cut = (pixmap.copy(x1, y1, x2 - x1, y2 - y1)).toImage();
        
        int count = 0;
        for (int i = cut.width() - 1; i >= 0; --i) {
            for (int j = cut.height() - 1; j >= 0; --j) {
                QColor c(cut.pixel(i , j));
                if (c.red() == grey && c.green() == grey && c.blue() == grey) {
                    count ++;
                }
                //count += c.red();
                //count += c.green();
                //count += c.blue();
            }
        }

        //if (count > 2000000) {
        if (count > 2500) {
            return true;
        }
    }

    return false;
}

dicomImporter::DicomInternalStructPtr dicomImporter::DicomImporter::importRaw(const utils::Filesystem::Path& from)
{
	internalData::PatientPtr patient = boost::make_shared<internalData::Patient>();
	QDateTime current = QDateTime::currentDateTime();
	std::string date = current.toString(QString("yyyyMMdd")).toStdString();
	std::string time = current.toString(QString("hhmmss")).toStdString();
	auto study = boost::make_shared<internalData::Study>();
	study->studyDate = date;
	study->studyTime = time;
	study->studyNumber = getStudyCurrentIndex();
	auto serie = boost::make_shared<internalData::Serie>();
	serie->serieDate = date;
	serie->serieTime = time;
	auto files = utils::Filesystem::listFiles(from, true, ".dcm");
	for (auto& path : files) {
		auto image = boost::make_shared<internalData::Image>();
		image->originFilePath = path.filename().string();
		serie->images.push_back(image);
	}
	

	study->series.push_back(serie);
	patient->sessions.push_back(study);
	DicomInternalStructPtr internalStruct = boost::make_shared<DicomInternalStruct>();
	internalStruct->addPatient(patient);
	return internalStruct;
}

utils::Filesystem::Path dicomImporter::DicomImporter::findDicomRootDir(const utils::Filesystem::Path& from)
{
	if (utils::Filesystem::pathExists(from / "DICOMDIR")) {
		return from;
	}

	auto subs = utils::Filesystem::listSubdirectories(from);
	if (subs.size() == 1) {
		return findDicomRootDir(*subs.begin());
	}

	throw loglib::runtime_error("Directory does not contain dicom structure");
}

void DicomSaver::save( const utils::Filesystem::Path& to, DicomInternalStructPtr inter, bool createSessionFolder )
{
    utils::Filesystem::Path filename;
    if (inter->isSingle()) {
        std::string sessionName = inter->getSingleSessionOutputDir();
        filename = createSessionFolder ?  to / sessionName / (sessionName + ".xml") : to / (sessionName + ".xml");
    } else {
        filename = to / "main.xml";
    }
    std::ofstream ofs(filename.c_str());
    if(ofs.good()) {
        boost::archive::xml_oarchive oa(ofs);
        oa << BOOST_SERIALIZATION_NVP(inter);
        ofs.close();
    } else {
        throw loglib::runtime_error("Unable to create file");
    }
}

dicomImporter::DicomInternalStructPtr dicomImporter::DicomLoader::load( const utils::Filesystem::Path& from )
{
    DicomInternalStructPtr inter = boost::make_shared<DicomInternalStruct>();
    std::ifstream ifs(from.c_str());
    if (ifs.good()) {
        boost::archive::xml_iarchive xmlIn(ifs);
        xmlIn >> BOOST_SERIALIZATION_NVP(inter);
    }

    return inter;
}
