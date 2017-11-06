/********************************************************************
    created:  2014/02/18
    created:  18:2:2014   16:52
    filename: LanguagesHelper.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD_CORE__LANGUAGESHELPER_H__
#define HEADER_GUARD_CORE__LANGUAGESHELPER_H__

#include <string>
#include <QtGui/QIcon>

namespace core
{
	//! Klasa pomocnicza przy ³adowaniu jêzyków
	class LanguagesHelper
	{
	public:

		//! Metoda inicjuj¹ca obs³ugê jêzyków
		//! Musi byæ wywo³ana przed innymi odwo³aniami do metod tej klasy
		//! Nale¿y j¹ wo³aæ tylko raz
		static void init();
		//! \param language Nazwa jêzyka po angielsku - z du¿ej litery
		//! \return Nazwa jêzyka w tym jêzyku, natywnie
		static const std::string nativeLanguageName(const std::string & language);
		//! \param language Nazwa jêzyka po angielsku - z du¿ej litery
		//! \return Ikona z flag¹ kraju dla danego jêzyka
		static const QIcon nationalFlagForLanguage(const std::string & language);
		//! \param code Kod jêzyka wg ISO639
		//! \return Nazwa jêzyka po angielsku odpowiadaj¹ca danemu kodowi lub pusty ci¹g jeœli nierozpoznano
		static std::string languageFromISO639Code(const std::string & code);
		//! \param locale Opis locale w formie lang_country wg ISO639 (jêzyk) i ISO 3166 (kraj)
		//! Jêzyk ma³ymi literami a kraj du¿ymi
		//! \return Nazwa jêzyka po angielsku dla zdekodowanego kodu jêzyka lub pusty ci¹g je¿eli nierozpoznano
		static std::string extractLanguageISO639Code(const std::string & locale);
	};
}

#endif	//	HEADER_GUARD_CORE__LANGUAGESHELPER_H__
