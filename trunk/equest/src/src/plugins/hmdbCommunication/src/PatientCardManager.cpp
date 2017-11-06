#include "CommunicationPCH.h"
#include "PatientCardManager.h"
#include <utils/Debug.h>
#include <loglib/Exceptions.h>

PatientCardManager::PatientCardManager() :currentPatientCard_(-1)
{

}

PatientCardManager::~PatientCardManager()
{

}

int PatientCardManager::registerPatientCard(hmdbCommunication::IPatientCard * patientCard)
{
	UTILS_ASSERT(patientCard != nullptr);

	for(auto it = patientCards.begin(); it != patientCards.end(); ++it){
		if((*it).patientCard.get() == patientCard){
			throw loglib::runtime_error("Patient card already registered");
		}
	}

	std::string name(patientCard->name());

	if(name.empty() == true){
		throw loglib::runtime_error("Unnamed patient card");
	}

	QWidget * widget = patientCard->widget();

	if(widget == nullptr){
		throw loglib::runtime_error("Patient card without widget");
	}

	int ret = patientCards.size();

	PatientCardData pData = {name, widget, ret, PatientCardPtr(patientCard)};
	patientCards.push_back(pData);

	return ret;
}

int PatientCardManager::currentPatientCardIndex() const
{
	return currentPatientCard_;
}

hmdbCommunication::IPatientCard * PatientCardManager::currentPatientCard() const
{
	if(currentPatientCard_ == -1){
		return nullptr;
	}

	return patientCards[currentPatientCard_].patientCard.get();
}

void PatientCardManager::setPatientCard(int idx)
{
	if(currentPatientCard_ != -1){
		patientCards[currentPatientCard_].widget->setVisible(false);
	}

	UTILS_ASSERT(idx > -1 && idx < static_cast<int>(patientCards.size()));
	currentPatientCard_ = idx;

	if(currentPatientCard_ != -1){
		patientCards[currentPatientCard_].widget->setVisible(true);
	}
}

QWidget * PatientCardManager::patientCardWidget(int idx) const
{
	UTILS_ASSERT(idx > -1 && idx < static_cast<int>(patientCards.size()));
	return patientCards[idx].widget;
}

const std::string & PatientCardManager::patientCardName(int idx) const
{
	UTILS_ASSERT(idx > -1 && idx < static_cast<int>(patientCards.size()));
	return patientCards[idx].name;
}

int PatientCardManager::patientCardsCount() const
{
	return patientCards.size();
}
