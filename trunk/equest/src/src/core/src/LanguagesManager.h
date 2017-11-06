/********************************************************************
    created:  2014/02/17
    created:  17:2:2014   18:45
    filename: LanguagesManager.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD_CORE__LANGUAGESMANAGER_H__
#define HEADER_GUARD_CORE__LANGUAGESMANAGER_H__

#include <corelib/ILanguagesManager.h>
#include <corelib/BaseDataTypes.h>
#include <utils/SmartPtr.h>
#include <map>
#include <vector>

class QTranslator;

namespace core
{
	class LanguagesManager : public ILanguagesManager
	{
	public:
		//! Wskaznik translatora
		typedef utils::shared_ptr<QTranslator> TranslatorPtr;

	private:
		//! Mapa lokalizacji do t³umaczy
		typedef std::map<std::string, TranslatorPtr> LocalTranslators;
		//! Mapa modu³ów do ich t³umaczeñ
		typedef std::map<std::string, LocalTranslators> ModuleTranslators;

	public:
		//! Konstruktor domyœlny
		LanguagesManager();
		//! Destruktor wirtualny
		virtual ~LanguagesManager();
		//! \return Zwraca nazwe aktualnie ustawionego jêzyka po angielsku
		virtual const std::string currentLanguage() const;
		//! \param language Jêzyk aplikacji w wersji angielskiej
		virtual void setLanguage(const std::string & language);
		//! \return Zbiór zarejestrowanych jêzyków po angielsku
		virtual const StringSet registeredLanguages() const;
		//! \return Zbiór zarejestrowanych modu³ów aplikacji z t³umaczeniami
		//! Jako modu³ rozumiemy pluginy, okno g³ówne, core i qt jako framework okien
		virtual const StringSet registeredModules() const;
		//! \param module Modul o ktorego jêzyki odpytujemy
		//! \return Lista jezykow tlumaczen dla danego modulu
		virtual const StringSet moduleLanguages(const std::string & module) const;
		//! \return Nazwa danego jêzyka w tym jêzyku
		virtual const std::string nativeLanguageName(const std::string & language) const;
		//! \param language Jezyk o ktory pytamy
		//! \return Czy tlumaczenie jezyka kompletne?
		virtual const bool languageComplete(const std::string & language) const;
		//! \return Ikona z flag¹ kraju (najwiekszego w ktorym dany jezyk jest glownym jezykiem)
		virtual const QIcon nationalFlagForLanguage(const std::string & language) const;
		//! Metoda rejestruje translator dla zadanego modulu i jêzyka
		//! \param module Modu³ aplikacji dla którego rejestrujemy t³umacz
		//! \param language Jêzyk dla którego instalujemy t³umacz
		//! \param translator Instalowany t³umacz
		void registerTranslator(const std::string & module, const std::string & language,
			TranslatorPtr translator);

		//! \param module Modu³ aplikacji dla którego usuwamy wszystkie t³umacze
		void unregisterTranslators(const std::string & module);

	private:
		//! Wy³¹cza t³umaczenia aplikacji dla aktualnie ustawionego jêzyka
		void reloadCurrentLanguage(const std::string & language);

		//! \param language Jezyk ktory probuje usunaæ
		void tryRemoveUnusedLanguage(const std::string & language);

	private:
		//! Zarejestrowane translatory
		ModuleTranslators translators;
		//! Aktualny jêzyk
		std::string currentLanguage_;
		//! Zarejestrowane jezyki
		StringSet languages_;
	};
}

#endif	//	HEADER_GUARD_CORE__LANGUAGESMANAGER_H__
