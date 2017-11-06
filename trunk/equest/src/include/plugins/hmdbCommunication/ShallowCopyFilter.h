/********************************************************************
	created:  2012/02/04
	created:  4:2:2012   11:15
	filename: ShallowCopyFilter.h
	author:   Mateusz Janiak

	purpose:  Filtry danych
	*********************************************************************/
#ifndef HEADER_GUARD_HMDBCOMMUNICATION__SHALLOWCOPYFILTER_H__
#define HEADER_GUARD_HMDBCOMMUNICATION__SHALLOWCOPYFILTER_H__

#include <hmdbserviceslib/ShallowCopy.h>
#include <QtCore/QString>
#include <utils/SmartPtr.h>

namespace hmdbCommunication
{

	//! Forward declaration
	class IHMDBSession;

	//! Interfejs filtrów danych
	class ShallowCopyFilter
	{
	public:

		//! Wirtualny destruktor
		virtual ~ShallowCopyFilter() {}

		//! \param session Sesja
		//! \return Filtr danych
		virtual ShallowCopyFilter * create(IHMDBSession * session) const = 0;
		//! Odświeża wewnętrzne dane filtru (np po odświeżeniu danych w bazie)
		virtual void refresh() {}

		//! \return Nazwa filtra
		virtual QString name() const = 0;

		//! Metoda wywoływana przed filtrowaniem
		virtual void beforeFiltering() {}
		//! Metoda wywoływana po filtrowaniu
		virtual void afterFiltering() {}

		//! \param disorder Schorzenie do weryfikacji
		//! \return true jeżeli schorzenie pomyślnie przeszło weryfikację
		virtual bool filterDisorder(const hmdbServices::MedicalShallowCopy::Disorder* disorder) const
		{
			return true;
		}
		//! Pacjent uzależniony jest od weryfikacji subjecta!!
		//! \param patient Pacjent do weryfikacji
		//! \return true jeżeli pacjent pomyślnie przeszedł weryfikację
		virtual bool filterPatient(const hmdbServices::MedicalShallowCopy::Patient* patient) const
		{
			return true;
		}

		//! \param subject Subject do weryfikacji
		//! \return true jeżeli subject pomyślnie przeszedł weryfikację
		virtual bool filterSubject(const hmdbServices::MotionShallowCopy::Performer* subject) const
		{
			return true;
		}
		//! \param session Sesja do weryfikacji
		//! \return true jeżeli sesja pomyślnie przeszła weryfikację
		virtual bool filterSession(const hmdbServices::MotionShallowCopy::Session* session) const
		{
			return true;
		}
		//! \param motion Motion do weryfikacji
		//! \return true jeżeli motion pomyślnie przeszedł weryfikację
		virtual bool filterMotion(const hmdbServices::MotionShallowCopy::Trial* motion) const
		{
			return true;
		}
		//! \param file Plik do weryfikacji
		//! \return true jeżeli plik pomyślnie przeszedł weryfikację
		virtual bool filterFile(const hmdbServices::MotionShallowCopy::File* file) const
		{
			return true;
		}
	};


	DEFINE_SMART_POINTERS(ShallowCopyFilter);
}

#endif	//	HEADER_GUARD_HMDBCOMMUNICATION__SHALLOWCOPYFILTER_H__
