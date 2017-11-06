/********************************************************************
	created:	2013/10/21
	created:	21:10:2013   8:37
	filename: 	IDicomInternalStruct.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_DICOM__IDICOMINTERNALSTRUCT_H__
#define HEADER_GUARD_DICOM__IDICOMINTERNALSTRUCT_H__

#include <utils/SmartPtr.h>
#include <utils/PtrPolicyBoost.h>
#include <utils/ClonePolicies.h>
#include <vector>
#include <boost/tuple/tuple.hpp>
#include <boost/range.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/nvp.hpp>


namespace dicom {

	namespace internalData {

		//! Reprezentacja obrazu USG
		struct Image {
			//! Œcie¿ka
			std::string imageFile;
			//! Œcie¿ka pliku z adnotacjami
			std::string adnotationsFile;
			//! Oryginalna nazwa pliku
			std::string originFilePath;
			//! Czy dane zawieraj¹ opis Power Doplera
			bool isPowerDoppler;

			//! Konstruktor domyslny
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
		DateTuple extractDate(std::string date);
		bool isDate(const std::string& date);

		//! Opis badania
		struct Serie {
			typedef std::vector<internalData::ImagePtr> Images;
			typedef boost::iterator_range<Images::iterator> ImagesRange;

			//! Identyfikator
			std::string serieId;
			//!
			std::string modality;
			//! Data
			std::string serieDate;
			//! Godzina
			std::string serieTime;
			//! Opis
			std::string serieDesc;
			//! Numer
			std::string serieNumber;
			//! Nazwa osoby przeprowadzaj¹cej badanie
			std::string physicianName;
			//! Obrazy powsta³e podczas badania
			Images images;

			//! Konstruktor domyslny
			Serie();

			//! \param other Kopiowane badanie
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

			//! Opis wizyty
		struct Study {
			//! Identyfikator
			std::string studyId;
			//! Opis
			std::string studyDesc;
			//! Data
			std::string studyDate;
			//! Godzina
			std::string studyTime;
			//! Nazwa przyjmuj¹cego
			std::string physicianName;
			//!
			std::string accesionNumber;
			//! Przeprowadzone badania
			std::vector<SeriePtr> series;
			//! Numer
			int studyNumber;

			//! Konstruktor domyœlny
			Study();
			//! \param other Kopiowana wizyta
			Study(const Study& other);
			//! \return Katalog docelowy
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
		

			//! Opis pacjenta
		struct Patient {
			//! Identyfikator
			std::string patientID;
			//! Nazwa
			std::string patientName;
			//! Data urodzenia
			std::string patientBirthdate;
			//! P³eæ pacjenta
			std::string patientSex;
			//! Wizyty pacjenta
			std::vector<StudyPtr> sessions;

			//! Konstruktor domyslny
			Patient();
			//! \param other Kopiowany pacjent
			Patient(const Patient& other);
			//! \param other Pacjent z którego kopiujemy metadane
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


class IDicomInternalStruct
{
public:
	virtual ~IDicomInternalStruct() {}

	//! \return Iloœæ pacjentów
	virtual int getNumPatients() const = 0;
	//! \return Iloœæ obrazów
	virtual int getNumImages() const = 0;
	//! \param no Numer Pacjenta
	//! \return Pacjent o zadanym numerze
	virtual internalData::PatientConstPtr getPatient(int no) const = 0;
};
typedef boost::shared_ptr<IDicomInternalStruct> IDicomInternalStructPtr;
typedef boost::shared_ptr<const IDicomInternalStruct> IDicomInternalStructConstPtr;
}

DEFINE_WRAPPER(dicom::IDicomInternalStruct, utils::PtrPolicyBoost, utils::ClonePolicyForbidden);
#endif
