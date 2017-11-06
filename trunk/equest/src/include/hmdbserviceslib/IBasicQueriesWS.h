/********************************************************************
	created:  2012/02/28
	created:  28:2:2012   22:41
	filename: IBasicQueriesWS.h
	author:   Mateusz Janiak

	purpose:
	*********************************************************************/
#ifndef HEADER_GUARD__HMDBSERVICES_IBASICQUERIESWS_H__
#define HEADER_GUARD__HMDBSERVICES_IBASICQUERIESWS_H__

#include <string>
#include <utils/SmartPtr.h>
#include <hmdbserviceslib/DateTime.h>

namespace hmdbServices
{
	//! Interfejs dostarczający informacji o czasie ostatnich zmian danych po stronie bazy danych
	class IGeneralBasicQueriesWS
	{
	public:
		//! Wirtualny destruktor
		virtual ~IGeneralBasicQueriesWS() {}
		//! \return Czas ostatniej zmiany danych
		virtual DateTime dataModificationTime() const = 0;
		//! \return Czas ostatniej zmiany metadanych
		virtual DateTime metadataModificationTime() const = 0;
	};

	using GeneralBasicQueriesWSPtr = utils::shared_ptr<IGeneralBasicQueriesWS>;
	using GeneralBasicQueriesWSConstPtr = utils::shared_ptr<const IGeneralBasicQueriesWS>;

	//! Interfejs usługi związanej z danymi ruchu
	class IMotionBasicQueriesWS : public IGeneralBasicQueriesWS
	{
	public:
		//! Wirtualny destruktor
		virtual ~IMotionBasicQueriesWS() {}

		//! \param id Identyfikator użytkownika
		//! \return Opis XML dla danego użytkownika
		virtual std::string getPerformerByIdXML(const int id) const = 0;

		//! \param id Identyfikator sesji
		//! \return Opis XML dla danej sesji
		virtual std::string getSessionByIdXML(const int id) const = 0;

		//! \param id Identyfikator sesji
		//! \return Nazwa sesji
		virtual std::string getSessionLabel(const int id) const = 0;

		//! \param id Identyfikator próby pomiarowej
		//! \return Opis XML dla danej próby pomiarowej
		virtual std::string getTrialByIdXML(const int id) const = 0;

		//! \param id Identyfikator konfiguracji pomiarowej
		//! \return Opis XML dla danej konfiguracji pomiarowej
		virtual std::string getMeasurementConfigurationByIdXML(const int id) const = 0;

		//! \param id Identyfikator konfiguracji aktora
		//! \return Opis XML dla danej konfiguracji aktora
		virtual std::string getPerformerConfigurationByIdXML(const int id) const = 0;

		//! \return Opis XML dla aktorów
		virtual std::string listPerformersXML() const = 0;

		//! \return Opis XML dla aktorów wraz z ich atrybutami
		virtual std::string listPerformersWithAttributesXML() const = 0;

		//! \param labID Identyfikator laboratorium
		//! \return Opis XML dla aktorów wraz z ich atrybutami dla zadanego laboratorium
		virtual std::string listLabPerformersWithAttributesXML(const int labID) const = 0;

		//! \param labID Identyfikator sesji
		//! \return Opis XML dla aktorów wraz z ich atrybutami dla zadanej sesji
		virtual std::string listSessionPerformersWithAttributesXML(const int sessionID) const = 0;

		//! \param performerID Identyfikator aktora
		//! \return Opis XML dla sesji aktora
		virtual std::string listPerformerSessionsXML(const int performerID) const = 0;

		//! \param performerID Identyfikator aktora
		//! \return Opis XML dla sesji aktora wraz z ich atrybutami
		virtual std::string listPerformerSessionsWithAttributesXML(const int performerID) const = 0;

		//! \param labID Identyfikator laboratorium
		//! \return Opis XML dla sesji wykonanych w danym laboratorium wraz z ich atrybutami
		virtual std::string listLabSessionsWithAttributesXML(const int labID) const = 0;

		//! \param sessionGroupID Identyfikator grupy sesji
		//! \return Opis XML dla sesji w ramach danej grupy sesji wraz z ich atrybutami
		virtual std::string listGroupSessionsWithAttributesXML(const int sessionGroupID) const = 0;

		//! \param measurementConfID Identyfikator konfiguracji pomiarowej
		//! \return Opis XML dla sesji związanych z daną konfiguracją pomiarową wraz z ich atrybutami
		virtual std::string listMeasurementConfigSessionsWithAttributesXML(const int measurementConfID) const = 0;

		//! \param sessionID Identyfikator sesji
		//! \return Opis XML dla grup sesji do których należy dana sesja
		virtual std::string listSessionSessionGroups(const int sessionID) const = 0;

		//! \param sessionID Identyfikator sesji
		//! \return Opis XML dla prób pomiarowych związanych z tą sesją
		virtual std::string listSessionTrialsXML(const int sessionID) const = 0;

		//! \param sessionID Identyfikator sesji
		//! \return Opis XML dla prób pomiarowych związanych z tą sesją wraz z ich atrybutami
		virtual std::string listSessionTrialsWithAttributesXML(const int sessionID) const = 0;

		//! \return Opis XML dla konfiguracji pomiarowych wraz z ich atrybutami
		virtual std::string listMeasurementConfigurationsWithAttributesXML() const = 0;

		//! \param sessionID Identyfikator sesji
		//! \return Opis XML dla konfiguracji aktorów wraz z atrybutami dla zadanej sesji
		virtual std::string listSessionPerformerConfsWithAttributesXML(const int sessionID) const = 0;

		//! \param subjectID Identyfikator sesji
		//! \param subjectType Typ plików
		//! \return Opis XML dla plików zadanego aktora i typu plików
		virtual std::string listFilesXML(const int subjectID, const std::string & subjectType) const = 0;

		//! \param subjectID Identyfikator sesji
		//! \param subjectType Typ plików
		//! \return Opis XML dla plików wraz z atrybutami zadanego aktora i typu plików
		virtual std::string listFilesWithAttributesXML(const int subjectID, const std::string & subjectType) const = 0;

		//! \param subjectID Identyfikator sesji
		//! \param subjectType Typ plików
		//! \return Opis XML dla plików wraz z atrybutami zadanego aktora i typu plików
		virtual std::string listFileAttributeDataXML(const int subjectID, const std::string & subjectType) const = 0;

		//! \param subjectID Identyfikator sesji
		//! \param subjectType Typ plików
		//! \return Opis XML dla plików wraz z atrybutami zadanego aktora i typu plików
		virtual std::string listFileAttributeDataWithAttributesXML(const int subjectID, const std::string & subjectType) const = 0;

		//! \param attributeGroupName Nazwa grupy atrybutów
		//! \param entityKind Typ encji
		//! \return Opis XML dla atrybutów dla zadanej encji w zdefiniowanej grupie
		virtual std::string listAttributesDefined(const std::string & attributeGroupName,
			const std::string & entityKind) const = 0;

		//! \param entityKind Typ encji
		//! \return Opis XML dla grup atrybutów dla zadanej encji
		virtual std::string listAttributeGroupsDefined(const std::string & entityKind) const = 0;

		//! \return Opis XML dla grup sesji
		virtual std::string listSessionGroupsDefined() const = 0;

		//! \return Opis XML dla typów ruchów
		virtual std::string listMotionKindsDefined() const = 0;

		//! \param attributName Nazwa atrybutu
		//! \param entityKind Typ encji
		//! \return Opis XML dla typów ruchów
		virtual std::string listEnumValues(const std::string & attributeName,
			const std::string & entityKind) const = 0;

		//! \return Lista adnotacji użytkownika dla danych USG - użytkownik brany z połączenia
		virtual std::string listMyAnnotationsXML() const = 0;
		//! \return Lista wszystkich adnotacji w bazie dla danych USG
		virtual std::string listAnnotationsXML() const = 0;

		//! Metoda zwraca listę adnotacji oczekujących na weryfikację dla danych USG - użytkownik brany z połączenia
		virtual std::string listMyAwaitingAnnotationsXML() const = 0;

		//! Metoda zwraca listę adnotacji zatwierdzonych dla danych USG - użytkownik brany z połączenia
		virtual std::string listMyReviewedAnnotationsXML() const = 0;
	};

	using MotionBasicQueriesWSPtr = utils::shared_ptr<IMotionBasicQueriesWS>;
	using MotionBasicQueriesWSConstPtr = utils::shared_ptr<const IMotionBasicQueriesWS>;

	using IMedicalBasicQueriesWS = IGeneralBasicQueriesWS;

	using MedicalBasicQueriesWSPtr = utils::shared_ptr<IMedicalBasicQueriesWS>;
	using MedicalBasicQueriesWSConstPtr = utils::shared_ptr<const IMedicalBasicQueriesWS>;
}

#endif	//	HEADER_GUARD__HMDBSERVICES_IBASICQUERIESWS_H__
