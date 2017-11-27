/********************************************************************
	created:  2014/03/26
	created:  26:3:2014   12:39
	filename: AnnotationStatusFilter.h
	author:   Mateusz Janiak

	purpose:
	*********************************************************************/
#ifndef HEADER_GUARD_EQUEST__ANNOTATIONSTATUSFILTER_H__
#define HEADER_GUARD_EQUEST__ANNOTATIONSTATUSFILTER_H__

#include <plugins/equest/Export.h>
#include <plugins/hmdbCommunication/ShallowCopyFilter.h>
#include <set>

namespace equest
{
	class PLUGIN_EQUEST_EXPORT AnnotationStatusFilter : public hmdbCommunication::ShallowCopyFilter
	{
	public:
		//! Zbiór identyfikatorów
		typedef std::set<int> Identifiers;

		enum GeneralStatus
		{
			InEdition,
			InVerification,
			Verified
		};

	public:
		//! \param name Nazwa filtra
		//! \param accept Czy dane maj¹ byæ akceptowane czy odrzucane jesli znajdziemy identyfikator
		//! \param missingDefault Czy dane maj¹ byæ akceptowane czy odrzucane jesli nie znajdziemy identyfikator
		//! \param identifiers Identyfikatory na których bêdzie operowa³ filtr
		//! \param session Sesja zdalna
		AnnotationStatusFilter(const QString & name, const bool accept,
			const bool missingDefault, const GeneralStatus status, hmdbCommunication::IHMDBSession * session = nullptr);
		//! Destruktor wirtualny
		virtual ~AnnotationStatusFilter();
		//! \param shallowCopyContext Kontekst p³ytkiej kopii bazy danych
		//! \return Filtr danych
		virtual AnnotationStatusFilter * create(hmdbCommunication::IHMDBSession * session) const;
		//! Metoda wywo³ywana przed filtrowaniem
		virtual void beforeFiltering();
		//! \return Nazwa fitlra
		virtual QString name() const;
		//! Filtr dla motionów opisuj¹cych badania USG
		virtual bool filterMotion(const hmdbServices::MotionShallowCopy::Trial* motion) const;

	private:
		//! Metoda odœwie¿a identyfikatory
		void refreshIdentifiers();

		static void groupAnnotations(hmdbCommunication::IHMDBSession * session,
			AnnotationStatusFilter::Identifiers & inEdition,
			AnnotationStatusFilter::Identifiers & inVerification,
			AnnotationStatusFilter::Identifiers & verified);

	private:
		//! Nazwa filtra
		const QString name_;
		//! Identyfikatory
		mutable Identifiers identifiers;
		//! Czy akceptowaæ jeœli znajdziemy identyfikator
		const bool accept;
		//! Czy akceptowaæ jeœli nie znajdziemy identyfikator
		const bool missingDefault;
		//! Sesja
		hmdbCommunication::IHMDBSession * session;
		//! Czas ostatniej aktualizacji
		hmdbServices::DateTime lastUpdateTime;
		//! Status
		GeneralStatus status;
	};
}

#endif	//	HEADER_GUARD_EQUEST__ANNOTATIONSTATUSFILTER_H__
