/********************************************************************
    created:  2012/03/09
    created:  9:3:2012   11:41
    filename: PatientCardManager.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD_HMDBCOMMUNICATION__PATIENTCARDMANAGER_H__
#define HEADER_GUARD_HMDBCOMMUNICATION__PATIENTCARDMANAGER_H__

#include <plugins/hmdbCommunication/IPatientCardManager.h>
#include <utils/SmartPtr.h>
#include <vector>

class PatientCardManager : public hmdbCommunication::IPatientCardManager
{
private:

	typedef utils::shared_ptr<hmdbCommunication::IPatientCard> PatientCardPtr;

	struct PatientCardData
	{
		std::string name;
		QWidget * widget;
		int idx;
		PatientCardPtr patientCard;
	};

	typedef std::vector<PatientCardData> PatientCards;

public:

	PatientCardManager();

	virtual ~PatientCardManager();

	//! \param patientCard Schemat karty pacjenta do zarejestrowania, staje się własnością managera
	virtual int registerPatientCard(hmdbCommunication::IPatientCard * patientCard);
	//! \return Aktualna karty pacjenta
	virtual hmdbCommunication::IPatientCard * currentPatientCard() const;
	//! \return Index aktualnej karty pacjenta
	virtual int currentPatientCardIndex() const;
	//! \param idx Index karty pacjenta do ustawienia
	virtual void setPatientCard(int idx);
	//! \param idx Index karty pacjenta dla której chcemy uzyskać widget
	//! \return Widget danej karty pacjenta
	virtual QWidget * patientCardWidget(int idx) const;
	//! \param idx Index karty pacjenta której nazwę chcemy pobrać
	//! \return Nazwa karty pacjenta
	virtual const std::string & patientCardName(int idx) const;
	//! \return Ilośc zarejestrowanych kart pacjenta
	virtual int patientCardsCount() const;

private:

	int currentPatientCard_;

	PatientCards patientCards;

};

#endif	//	HEADER_GUARD_HMDBCOMMUNICATION__PATIENTCARDMANAGER_H__
