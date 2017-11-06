/********************************************************************
    created:  2011/10/28
    created:  28:10:2011   14:10
    filename: IPatient.h
    author:   Mateusz Janiak

    purpose:
*********************************************************************/
#ifndef HEADER_GUARD_HMDBCOMMUNICATION__IPATIENT_H__
#define HEADER_GUARD_HMDBCOMMUNICATION__IPATIENT_H__

#include <QtGui/QPixmap>
#include <string>
#include <map>
#include <plugins/subject/ISubject.h>
#include <utils/SmartPtr.h>

namespace hmdbCommunication
{

//! Obiekt schorzenia pacjenta
struct Disorder
{
	//! Nazwa schorzenia
    std::string name;
	//! Data diagnzy
    std::string diagnosisDate;
	//! Istotność schorzenia
    std::string focus;
	//! Komentarze
    std::string comments;
	//! Atrybuty
    std::map<std::string, std::string> attributes;
};

//! Dane antropometryczne
struct AntropometricData
{
	//! Typ przechowujący wartość i jednostkę (skalar + text)
    typedef std::pair<float, std::string> value_type;


	value_type bodyMass;
    value_type height;
    value_type interAsisDistance;
    value_type leftLegLength;
    value_type rightLegLenght;
    value_type leftKneeWidth;
    value_type rightKneeWidth;
    value_type leftAnkleWidth;
    value_type rightAnkleWidth;
    value_type leftCircuitThigh;
    value_type rightCircuitThight;
    value_type leftCircuitShank;
    value_type rightCircuitShank;
    value_type leftShoulderOffset;
    value_type rightShoulderOffset;
    value_type leftElbowWidth;
    value_type rightElbowWidth;
    value_type leftWristWidth;
    value_type rightWristWidth;
    value_type leftWristThickness;
    value_type rightWristThickness;
    value_type leftHandWidth;
    value_type rightHandWidth;
    value_type leftHandThickness;
    value_type rightHandThickness;
};

//! Interfejs obiektu pacjenta
class IPatient
{

public:
	//! Typ płci
	enum Gender {
		Male,
		Female,
		NotSpecifiedGender
	};

public:
	//! Wirtualny destruktor
    virtual ~IPatient() {};

	//! \return Identyfikator pacjenta - zgodny z identyfikatorem subjecta
    virtual PluginSubject::SubjectID getID() const = 0;

	//! \return Imię/Nazwa pacjenta
    virtual const std::string & getName() const = 0;
	//! \return Nazwisko pacjenta
    virtual const std::string & getSurname() const = 0;
	//! \return Data urodzenia pacjenta
    virtual const std::string & getBirthday() const = 0;
	//! \return Płeć pacjenta
    virtual Gender getGender() const = 0;
	//! \return Zdjęcie pacjenta
    virtual const utils::shared_ptr<const QPixmap> & getPhoto() const = 0;

	//! \return Ilość schorzeń
    virtual unsigned int disordersCount() const = 0;
	//! \param idx Indeks schorzenia
	//! \return Opis schorzenia
    virtual const Disorder & disorder(unsigned int idx) const = 0;
};

typedef utils::shared_ptr<IPatient> PatientPtr;
typedef utils::shared_ptr<const IPatient> PatientConstPtr;

}

DEFINE_WRAPPER(hmdbCommunication::IPatient, utils::PtrPolicyStd, utils::ClonePolicyNotImplemented);
DEFINE_WRAPPER(hmdbCommunication::AntropometricData, utils::PtrPolicyStd, utils::ClonePolicyCopyConstructor);

#endif //HEADER_GUARD_HMDBCOMMUNICATION__IPATIENT_H__
