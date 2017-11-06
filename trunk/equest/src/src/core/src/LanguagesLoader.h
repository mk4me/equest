/********************************************************************
    created:  2014/02/18
    created:  18:2:2014   12:12
    filename: LanguagesLoader.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD_CORE__LANGUAGESLOADER_H__
#define HEADER_GUARD_CORE__LANGUAGESLOADER_H__

#include <string>
#include <vector>
#include <utils/Filesystem.h>

namespace core
{
	class LanguagesManager;

	//! Klasa realizuj¹ca ³adowanie t³umaczeñ wg przyjêtego w aplikacji i skryptach CMake
	//! schematu nazewniczego
	//! TODO
	//! Wymaga formalnego opisania i doprecyzowania
	//! 
	//! TODO
	//! uwzglednic filtrowanie dla tlumaczen glownego okna - projektu ktory faktycznie startuje
	//! np. IMU, MEDUSA, MDE, old _view przy ladowaniu tlumaczen dla core
	class LanguagesLoader
	{
	public:
		//! \param paths [out] Œcie¿ki do t³umaczeñ które nale¿y zweryfikowaæ,
		//! podczas ladowania usuwane sa sciezki zaladowane
		//! \param languageManager Manager t³umaczeñ zasilany znalezionymi t³umaczeniami
		static void loadCoreTranslations(utils::Filesystem::PathsList & paths,
			LanguagesManager * languagesManager);

		//! \param paths [out] Œcie¿ki do t³umaczeñ które nale¿y zweryfikowaæ,
		//! podczas ladowania usuwane sa sciezki zaladowane
		//! \param pluginName Nazwa pluginu która musi wyst¹piæ w nazwie t³umaczenia
		//! \param languageManager Manager t³umaczeñ zasilany znalezionymi t³umaczeniami
		static void loadPluginTranslations(utils::Filesystem::PathsList & paths,
			const std::string & pluginName,	LanguagesManager * languagesManager);

		//! \param pluginName Nazwa pluginu która musi wyst¹piæ w nazwie t³umaczenia
		//! \param defaultLanguageCode Domyœlny jêzyk plugina, dla ktorego nie ma tlumaczenia
		//! \param languageManager Manager t³umaczeñ zasilany znalezionymi t³umaczeniami
		static void loadPluginDefaultTranslation(const std::string & pluginName,
			const std::string & defaultLanguageCode,
			LanguagesManager * languagesManager);
	};
}

#endif	//	HEADER_GUARD_CORE__LANGUAGESLOADER_H__
