#include "CorePCH.h"
#include "LanguagesHelper.h"

#include <boost/bimap.hpp>
#include <boost/algorithm/string.hpp>
#include <map>

//! Mapowanie kodu ISO639 do angielskiej nazwy jêzyka
typedef boost::bimap<std::string, std::string> LanguagesCodeMapping;

//! Struktura opisujaca jezyk
struct LanguageDescription
{
public:
	//! Konstruktor
	//! \param nativeName Nazwa danego jezyka w tym jêzyku - UTF-8
	//! \param nationalFlag Ikona z flag¹ danego kraju odpowiadaj¹cego temu jêzykowi
	LanguageDescription(const std::string & nativeName,
		const QIcon & nationalFlag) : nativeName(nativeName),
		nationalFlag(nationalFlag) {}

	std::string nativeName;	//! Natywan nazwa (UTF-8)
	QIcon nationalFlag;		//! Ikona z flaga
};

//! Mapa t³umaczeñ z nazw angielskich jêzyków na ich natywn¹ nazwê,
//! np. Polish -> Polski
typedef std::map<std::string,LanguageDescription> LanguagesDescriptions;

//! Opisy jêzyków - ikony flag krajów i ich natywne nazwy
static LanguagesDescriptions languagesDescriptions = LanguagesDescriptions();
//! Mapowanie kodów ISO639 do nazw angielskich jêzyków
static LanguagesCodeMapping languagesCodesMapping = LanguagesCodeMapping();

//! Metoda rejestruje jêzyk
//! \param code Kod jêzyka wg ISO639
//! \param name Angielska nazwa jêzyka
//! \param nativeName Nazwa jêzyka w tym jêzyku
//! \param flagIconsPath Œcie¿ka do ikony z flag¹ dla jêzyka
void registerLanguage(const std::string & code, const std::string & name,
	const std::string & nativeName, const std::string & flagIconPath)
{
	languagesCodesMapping.insert(LanguagesCodeMapping::value_type(code,name));
	languagesDescriptions.insert(LanguagesDescriptions::value_type(name,
		LanguageDescription(nativeName, QIcon(QPixmap(flagIconPath.c_str())))));
}

using namespace core;

void LanguagesHelper::init()
{
	//dodaæ translacje natywne, kody i ikony flag dla wszystkich jêzyków
	// Ÿród³o: http://en.wikipedia.org/wiki/List_of_indigenous_language_names
	
	registerLanguage("pl", "Polish", "Polski", ":/PolishFlag.png");
	registerLanguage("en", "English", "English", ":/UnitedKingdomFlag.png");
	registerLanguage("de", "German", "Deutsch", ":/GermanFlag.png");
}

const std::string LanguagesHelper::nativeLanguageName(const std::string & language)
{
	auto it = languagesDescriptions.find(language);

	if(it == languagesDescriptions.end()){
		return std::string();
	}

	return it->second.nativeName;
}

const QIcon LanguagesHelper::nationalFlagForLanguage(const std::string & language)
{
	auto it = languagesDescriptions.find(language);

	if(it == languagesDescriptions.end()){
		return QIcon();
	}

	return it->second.nationalFlag;
}	

std::string LanguagesHelper::languageFromISO639Code(const std::string & code)
{
	auto it = languagesCodesMapping.left.find(code);
	if(it == languagesCodesMapping.left.end()){
		return std::string();
	}

	return it->second;
}

std::string LanguagesHelper::extractLanguageISO639Code(const std::string & locale)
{
	if(locale.empty() == true || locale.size() > 7){
		return std::string();
	}

	std::vector<std::string> strs;
	boost::split(strs, locale, boost::is_any_of("_.-"));

	if(strs.empty() == true || strs.size() > 2 || strs[0].empty() == true
		|| strs[0].size() > 2){
			//nierozpoznany format
			return std::string();
	}		

	// próbuje dekodowaæ z tego co mam
	return languageFromISO639Code(strs[0]);
}