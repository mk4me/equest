/********************************************************************
	created:	2013/10/21
	created:	21:10:2013   8:38
	filename: 	DicomInternalStruct.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_DICOM__DICOMINTERNALSTRUCT_H__
#define HEADER_GUARD_DICOM__DICOMINTERNALSTRUCT_H__

#include <plugins/dicom/IDicomInternalStruct.h>

#include <boost/serialization/access.hpp>
#include <boost/serialization/split_member.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/shared_ptr.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/smart_ptr.hpp>

namespace dicom {


//! Wewnêtrzne drzewo danych
struct DicomInternalStruct : public IDicomInternalStruct
{
public:
	//! Destruktor wirtualny
	virtual ~DicomInternalStruct();

	//! \param patient Pacjent
    void addPatient(internalData::PatientPtr patient);
	//! \return Iloœæ pacjentów
    int getNumPatients() const;
	//! \return Iloœæ obrazów
    int getNumImages() const;
	//! \param no Numer Pacjenta
	//! \return Pacjent o zadanym numerze
    internalData::PatientConstPtr getPatient(int no) const;
	//! \param other Drzewo z którego kopiujemy metadane
    void cloneMeta(const DicomInternalStruct& other) {}
	//! \return Czy mamy jednego pacjenta z jednym badaniem
    bool isSingle() const;
	//! \return Œcie¿ka katalogu badania dla pojedynczego pacjenta
    std::string getSingleSessionOutputDir() const;

    template <typename Archive>
    void serialize(Archive& ar, const unsigned int version)
    {
        ar & boost::serialization::make_nvp("patients", patients);
    }
	//! \param sessionName Nazwa badania
	//! \return Badanie o zadanej nazwie
    internalData::StudyConstPtr getSession(const std::string& sessionName) const;
	//! \param sessionName Nazwa badania
	//! \return Badanie o zadanej nazwie lub nullptr
    internalData::StudyConstPtr tryGetSession(const std::string& sessionName) const;
	//! \param imageFilename Nazwa obrazu
	//! \return Obraz dla zadanej nazwy
    internalData::ImageConstPtr getImage(const std::string& imageFilename) const;
	//! \param imageFilename Nazwa obrazu
	//! \return Obraz dla zadanej nazwy
    internalData::ImageConstPtr tryGetImage(const std::string& imageFilename) const;

private:
	//! Pacjenci
    std::vector<internalData::PatientPtr> patients;
};
typedef boost::shared_ptr<DicomInternalStruct> DicomInternalStructPtr;
typedef boost::shared_ptr<const DicomInternalStruct> DicomInternalStructConstPtr;
}

DEFINE_WRAPPER_INHERITANCE(dicom::DicomInternalStruct, dicom::IDicomInternalStruct);


#endif
