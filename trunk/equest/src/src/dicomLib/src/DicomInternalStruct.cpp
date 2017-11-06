#include "DicomImporterPCH.h"
#include "dicomLib/DicomInternalStruct.h"
#include <boost/format.hpp>
#include <boost/make_shared.hpp>

using namespace dicomImporter;


internalData::PatientConstPtr dicomImporter::DicomInternalStruct::getPatient( int no ) const
{
    return patients[no];
}

int dicomImporter::DicomInternalStruct::getNumPatients() const
{
    return patients.size();
}

void dicomImporter::DicomInternalStruct::addPatient( internalData::PatientPtr patient )
{
    patients.push_back(patient);
}

dicomImporter::DicomInternalStruct::~DicomInternalStruct()
{

}

bool dicomImporter::DicomInternalStruct::isSingle() const
{
    if (patients.size() == 1 && patients[0]->sessions.size() == 1) {
        return true;
    }

    return false;
}

std::string dicomImporter::DicomInternalStruct::getSingleSessionOutputDir() const
{
    if (!isSingle()) {
        UTILS_ASSERT(false);
        throw loglib::runtime_error("Unable to get single output directory, multiple sessions are present");
    }

    return patients[0]->sessions[0]->getOutputDirectory();
}

int dicomImporter::DicomInternalStruct::getNumImages() const
{
    int num = 0;
    for (auto itPatient = patients.begin(); itPatient != patients.end(); ++itPatient) {
        for (auto itSession = (*itPatient)->sessions.begin(); itSession != (*itPatient)->sessions.end(); ++itSession) {
            for (auto itSerie = (*itSession)->series.begin(); itSerie != (*itSession)->series.end(); ++itSerie) {
                num += (*itSerie)->images.size();
            }
        }
    }
    return num;
}

dicomImporter::internalData::Patient::Patient( const Patient& other )
{
    cloneMeta(other);
    sessions.reserve(other.sessions.size());
    for (auto it = other.sessions.begin(); it != other.sessions.end(); ++it) {
        sessions.push_back(boost::make_shared<Study>(**it));
    }
}

dicomImporter::internalData::Patient::Patient()
{

}

void dicomImporter::internalData::Patient::cloneMeta( const Patient& other )
{
    patientID = other.patientID;
    patientName = other.patientName;
    patientBirthdate = other.patientBirthdate;
    patientSex = other.patientSex;
}

dicomImporter::internalData::Study::Study( const Study& other ) :
studyId(other.studyId),
    studyDesc(other.studyDesc),
    studyDate(other.studyDate),
    studyTime(other.studyTime),
    physicianName(other.physicianName),
    accesionNumber(other.accesionNumber),
    studyNumber(other.studyNumber)
{
    series.reserve(other.series.size());
    for (auto it = other.series.begin(); it != other.series.end(); ++it) {
        series.push_back(boost::make_shared<Serie>(**it));
    }
}

dicomImporter::internalData::Study::Study() :
    studyNumber(-1)
{

}

std::string dicomImporter::internalData::Study::getOutputDirectory()
{
    DateTuple sdate = extractDate(studyDate);
    boost::format fmt("%s-%s-%s-S%04d");
    fmt % sdate.get<0>() % sdate.get<1>() % sdate.get<2>() % studyNumber;
    return fmt.str();
}

dicomImporter::internalData::Serie::Serie( const Serie& other ) :
serieId(other.serieId),
    modality(other.modality),
    serieDate(other.serieDate),
    serieTime(other.serieTime),
    serieDesc(other.serieDesc),
    serieNumber(other.serieNumber),
    physicianName(other.physicianName)
{
    images.reserve(other.images.size());
    for (auto it = other.images.begin(); it != other.images.end(); ++it) {
        images.push_back(boost::make_shared<Image>(**it));
    }
}

dicomImporter::internalData::Serie::Serie()
{

}


dicomImporter::internalData::DateTuple dicomImporter::internalData::extractDate( std::string date )
{
    if (!isDate(date)) {
        date = "XXXXXXXX";
    }
    return boost::make_tuple(date.substr(0,4), date.substr(4,2), date.substr(6,2));
}

bool dicomImporter::internalData::isDate( const std::string& date )
{
    return (date.size() == 8) && (std::all_of(date.begin(), date.end(), ::isdigit));
}