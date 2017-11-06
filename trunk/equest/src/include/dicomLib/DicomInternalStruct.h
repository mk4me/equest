/********************************************************************
	created:	2013/10/21
	created:	21:10:2013   8:38
	filename: 	DicomInternalStruct.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_DICOMIMPORTER__DICOMINTERNALSTRUCT_H__
#define HEADER_GUARD_DICOMIMPORTER__DICOMINTERNALSTRUCT_H__

#include <dicomLib/IDicomInternalStruct.h>

#include <boost/serialization/access.hpp>
#include <boost/serialization/split_member.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/shared_ptr.hpp>
#include <boost/tuple/tuple.hpp>

namespace dicomImporter {

namespace internalData {

    struct Image {
        std::string imageFile;
        std::string adnotationsFile;
        std::string originFilePath;
        bool isPowerDoppler;

        Image() : isPowerDoppler(false) {}

        template <typename Archive>
        void serialize(Archive& ar, const unsigned int version)
        {
            ar & boost::serialization::make_nvp("imageFile", imageFile);
            ar & boost::serialization::make_nvp("adnotationsFile", adnotationsFile);
            ar & boost::serialization::make_nvp("originFilePath", originFilePath);
            ar & boost::serialization::make_nvp("isPowerDoppler", isPowerDoppler);
        }
    };
	typedef boost::shared_ptr<Image> ImagePtr;
	typedef boost::shared_ptr<const Image> ImageConstPtr;


    typedef boost::tuple<std::string, std::string, std::string> DateTuple;
    DateTuple extractDate( std::string date );
    bool isDate(const std::string& date);

    struct Serie {
        std::string serieId;
        std::string modality;
        std::string serieDate;
        std::string serieTime;
        std::string serieDesc;
        std::string serieNumber;
        std::string physicianName;
        std::vector<ImagePtr> images;

        Serie();

        Serie(const Serie& other);

        template <typename Archive>
        void serialize(Archive& ar, const unsigned int version)
        {
            ar & boost::serialization::make_nvp("serieId", serieId);
            ar & boost::serialization::make_nvp("modality", modality);
            ar & boost::serialization::make_nvp("serieDate", serieDate);
            ar & boost::serialization::make_nvp("serieTime", serieTime);
            ar & boost::serialization::make_nvp("serieDesc", serieDesc);
            ar & boost::serialization::make_nvp("serieNumber", serieNumber);
            ar & boost::serialization::make_nvp("physicianName", physicianName);
            ar & boost::serialization::make_nvp("images", images);
        }
     };
	typedef boost::shared_ptr<Serie> SeriePtr;
	typedef boost::shared_ptr<const Serie> SerieConstPtr;

     struct Study {
         std::string studyId;
         std::string studyDesc;
         std::string studyDate;
         std::string studyTime;
         std::string physicianName;
         std::string accesionNumber;
         std::vector<SeriePtr> series;
         int studyNumber;

         Study();
         Study(const Study& other);

         std::string getOutputDirectory();

         template <typename Archive>
         void serialize(Archive& ar, const unsigned int version)
         {
            ar & boost::serialization::make_nvp("studyId", studyId);
            ar & boost::serialization::make_nvp("studyDesc", studyDesc);
            ar & boost::serialization::make_nvp("studyDate", studyDate);
            ar & boost::serialization::make_nvp("studyTime", studyTime);
            ar & boost::serialization::make_nvp("physicianName", physicianName);
            ar & boost::serialization::make_nvp("accesionNumber", accesionNumber);
            ar & boost::serialization::make_nvp("series", series); 
            ar & boost::serialization::make_nvp("studyNumber", studyNumber);
         }
     };
	 typedef boost::shared_ptr<Study> StudyPtr;
	 typedef boost::shared_ptr<const Study> StudyConstPtr;

     struct Patient {
        std::string patientID;
        std::string patientName;
        std::string patientBirthdate;
        std::string patientSex;
        std::vector<StudyPtr> sessions;

        Patient();
        Patient(const Patient& other);
        void cloneMeta(const Patient& other);

        template <typename Archive>
        void serialize(Archive& ar, const unsigned int version)
        {
            ar & boost::serialization::make_nvp("patientID", patientID);
            ar & boost::serialization::make_nvp("patientName", patientName);
            ar & boost::serialization::make_nvp("patientBirthdate", patientBirthdate);
            ar & boost::serialization::make_nvp("patientSex", patientSex);
            ar & boost::serialization::make_nvp("sessions", sessions);
        }
     };
	 typedef boost::shared_ptr<Patient> PatientPtr;
	 typedef boost::shared_ptr<const Patient> PatientConstPtr;
}

struct DicomInternalStruct : public IDicomInternalStruct
{
	virtual ~DicomInternalStruct();

    void addPatient(internalData::PatientPtr patient);
    int getNumPatients() const;
    int getNumImages() const;
    internalData::PatientConstPtr getPatient(int no) const;
    void cloneMeta(const DicomInternalStruct& other) {}

    bool isSingle() const;
    std::string getSingleSessionOutputDir() const;

    template <typename Archive>
    void serialize(Archive& ar, const unsigned int version)
    {
        ar & boost::serialization::make_nvp("patients", patients);
    }

    std::vector<internalData::PatientPtr> patients;
};
typedef boost::shared_ptr<DicomInternalStruct> DicomInternalStructPtr;
typedef boost::shared_ptr<const DicomInternalStruct> DicomInternalStructConstPtr;
}
#endif
