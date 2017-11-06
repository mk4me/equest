/********************************************************************
	created:  2012/02/28
	created:  28:2:2012   22:39
	filename: IBasicUpdatesWS.h
	author:   Mateusz Janiak

	purpose:
	*********************************************************************/
#ifndef HEADER_GUARD__HMDBSERVICES_IBASICUPDATESWS_H__
#define HEADER_GUARD__HMDBSERVICES_IBASICUPDATESWS_H__

#include <set>
#include <string>
#include <utils/SmartPtr.h>

namespace hmdbServices
{
	//! Interfejs usługi pozwalającej edytować dane w bazie ruchu
	class IMotionBasicUpdatesWS
	{
	public:
		//! Wirtualny destruktor
		virtual ~IMotionBasicUpdatesWS() {}

		//! \param name Imię aktora
		//! \param surname Nazwisko aktora
		//! \return Identyfikator nowego aktora
		virtual int createPerformer(const std::string & name,
			const std::string & surname) = 0;

		//! \param labID Identyfikator laboratorium w którym przeprowadzono sesję
		//! \param motionKindName Nazwa typu ruchu nagrywanego podczas sesji
		//! \param sessionDate Data sesji
		//! \param sessionDescription Opis sesji
		//! \param sessionGroupIDs Identyfikatory grup sesji do tkóej nowa sesja ma należeć
		//! \return Identyfikator nowej sesji
		virtual int createSession(const int labID, const std::string & motionKindName,
			const std::string  & sessionDate, const std::string & sessionDescription,
			const std::set<int> & sessionGroupIDs) = 0;

		//! \param sessionID Identyfikator sesji w ramach której przeprowadzony był trial (próba pomiarowa)
		//! \param trialDescription Opis próby pomiarowej
		//! \return Identyfokator nowej próby pomiarowej
		virtual int createTrial(const int sessionID, const std::string & trialDescription) = 0;

		//! \param mcName Nazwa konfiguracji pomiarowej
		//! \param mcKind Typ konfiguracji pomiarowej
		//! \param mcDescription Opis konfiguracji pomiarowej
		//! \return Identyfokator nowej konfiguracji pomiarowej
		virtual int createMeasurementConfiguration(const std::string & mcName,
			const std::string & mcKind, const std::string & mcDescription) = 0;

		//! \param sessionID Identyfikator przypisywanej sesji
		//! \param sessionsGroupID Identyfikator grupy sesji
		//! \return Wartość logiczna informująca o powodzeniu operacji (TODO - wyjaśnić)
		virtual int assignSessionToGroup(const int sessionID, const int sessionsGroupID) = 0;

		//! \param performerID Identyfikator przypisywanego aktora
		//! \param sessionID Identyfikator sesji
		//! \return Identyfikator powstałej konfiguracji aktora
		virtual int assignPerformerToSession(const int performerID, const int sessionID) = 0;

		//! \param performerID Identyfikator przypisywanego aktora
		//! \param measurementID Identyfikator pomiaru
		//! \return Rezultat operacji - powodzenie/niepowodzenie przypisania
		virtual bool assignPerformerToMeasurement(const int performerID, const int measurementID) = 0;

		//! \param performerID Identyfikator aktora
		//! \param attributeName Nazwa atrybutu
		//! \param attributeValue Wartośc atrybutu
		//! \param update Czy aktualizować wartość jeśli istnieje
		virtual void setPerformerAttribute(const int performerID, const std::string & attributeName,
			const std::string & attributeValue, const bool update) = 0;

		//! \param sessionID Identyfikator sesji
		//! \param attributeName Nazwa atrybutu
		//! \param attributeValue Wartośc atrybutu
		//! \param update Czy aktualizować wartość jeśli istnieje
		virtual void setSessionAttribute(const int sessionID, const std::string & attributeName,
			const std::string & attributeValue, const bool update) = 0;

		//! \param trialID Identyfikator próby pomiarowej
		//! \param attributeName Nazwa atrybutu
		//! \param attributeValue Wartośc atrybutu
		//! \param update Czy aktualizować wartość jeśli istnieje
		virtual void setTrialAttribute(const int trialID, const std::string & attributeName,
			const std::string & attributeValue, const bool update) = 0;

		//! \param measurementConfID Identyfikator konfiguracji pomiarowej
		//! \param attributeName Nazwa atrybutu
		//! \param attributeValue Wartośc atrybutu
		//! \param update Czy aktualizować wartość jeśli istnieje
		virtual void setMeasurementConfAttribute(const int measurementConfID, const std::string & attributeName,
			const std::string & attributeValue, const bool update) = 0;

		//! \param performerConfID Identyfikator konfiguracji aktora
		//! \param attributeName Nazwa atrybutu
		//! \param attributeValue Wartośc atrybutu
		//! \param update Czy aktualizować wartość jeśli istnieje
		virtual void setPerformerConfAttribute(const int performerConfID, const std::string & attributeName,
			const std::string & attributeValue, const bool update) = 0;

		//! \param fileID Identyfikator pliku
		//! \param attributeName Nazwa atrybutu
		//! \param attributeValue Wartośc atrybutu
		//! \param update Czy aktualizować wartość jeśli istnieje
		virtual void setFileAttribute(const int fileID, const std::string & attributeName,
			const std::string & attributeValue, const bool update) = 0;

		//! \param resourceID Identyfikator zasobu
		//! \param entity Typ encji
		//! \param attributeName Nazwa atrybutu
		//! \param fileID Identyfikator pliku
		//! \param update Czy aktualizować wartość jeśli istnieje
		virtual void setFileTypedAttributeValue(const int resourceID, const std::string & entity,
			const std::string & attributeName, const int fileID, const bool update) = 0;

		//! \param resourceID Identyfikator zasobu
		//! \param attributeName Nazwa atrybutu
		//! \param entity Typ encji
		virtual void clearAttributeValue(const int resourceID, const std::string & attributeName,
			const std::string & entity) = 0;

		//! \param trialID Identyfikator triala dla ktorego przypisujemy status adnotacji
		//! \param status Status jaki przypisujemy
		//! \param comment Komentarz jaki przypisujemy dla statusu adnotacji
		virtual void setMyAnnotationStatus(const int trialID, const int status,
			const std::string & comment) = 0;

		//! \param trialID Identyfikator triala dla ktorego przypisujemy status adnotacji
		//! \param status Status jaki przypisujemy
		//! \param note Notatka jaka przypisujemy dla statusu adnotacji
		virtual void setAnnotationReview(const int trialID, const int userID,
			const int status,
			const std::string & note) = 0;
	};

	using MotionBasicUpdatesWSPtr = utils::shared_ptr<IMotionBasicUpdatesWS>;
	using MotionBasicUpdatesWSConstPtr = utils::shared_ptr<const IMotionBasicUpdatesWS>;

	//! Interfejs usługi pozwalającej edytować dane w bazie medycznej
	class IMedicalBasicUpdatesWS
	{
	public:
		//! Destrutkor wirtualny
		virtual ~IMedicalBasicUpdatesWS() {}
	};

	using MedicalBasicUpdatesWSPtr = utils::shared_ptr<IMedicalBasicUpdatesWS>;
	using MedicalBasicUpdatesWSConstPtr = utils::shared_ptr<const IMedicalBasicUpdatesWS>;
}

#endif	//	HEADER_GUARD__HMDBSERVICES_IBASICUPDATESWS_H__
