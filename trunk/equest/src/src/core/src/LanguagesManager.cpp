#include "CorePCH.h"
#include "LanguagesManager.h"
#include "ApplicationCommon.h"
#include "LanguagesHelper.h"
#include <QtCore/QCoreApplication>
#include "LanguageChangeFilter.h"

using namespace core;

LanguagesManager::LanguagesManager()
{
	
}

LanguagesManager::~LanguagesManager()
{

}

const std::string LanguagesManager::currentLanguage() const
{
	return currentLanguage_;
}

void LanguagesManager::setLanguage(const std::string & language)
{
	if(languages_.find(language) == languages_.end()){
		throw std::invalid_argument("No such language");
	}

	if(language != currentLanguage_){
		reloadCurrentLanguage(language);
		currentLanguage_ = language;
		CORE_LOG_DEBUG("Application language changed to " << language);
	}
}

void LanguagesManager::reloadCurrentLanguage(const std::string & language)
{
	LanguageChangeFilter filter;

	//ide po wszystkich tlumaczeniach modulow
	for(auto it = translators.begin(); it != translators.end(); ++it){
		auto IT = it->second.find(currentLanguage_);
		//najpierw wywalam wszystkie translatory dla danego jêzyka jeœli s¹ zainstalowane (niezerowe)
		if(IT != it->second.end() && IT->second != nullptr){
			qApp->removeTranslator(IT->second.get());
		}

		//teraz instaluje translatory dla danego jêzyka jeœli s¹ niezainstalowane zainstalowane (niezerowe)
		IT = it->second.find(language);
		if(IT != it->second.end() && IT->second != nullptr){
			qApp->installTranslator(IT->second.get());
		}
	}
}

const LanguagesManager::StringSet LanguagesManager::registeredLanguages() const
{
	return languages_;
}

const LanguagesManager::StringSet LanguagesManager::registeredModules() const
{
	StringSet ret;
	for(auto it = translators.begin(); it != translators.end(); ++it){
		ret.insert(it->first);
	}

	return ret;
}

const LanguagesManager::StringSet LanguagesManager::moduleLanguages(const std::string & module) const
{
	StringSet ret;

	auto it = translators.find(module);

	if(it != translators.end()){

		for(auto IT = it->second.begin(); IT != it->second.end(); ++IT){
			ret.insert(IT->first);
		}
	}

	return ret;
}

const bool LanguagesManager::languageComplete(const std::string & language) const
{
	if(languages_.find(language) == languages_.end()){
		return false;
	}

	bool ret = true;

	for(auto it = translators.begin(); it != translators.end(); ++it){
		if(it->second.find(language) == it->second.end()){
			ret = false;
			break;
		}
	}

	return ret;
}



const std::string LanguagesManager::nativeLanguageName(const std::string & language) const
{
	return LanguagesHelper::nativeLanguageName(language);
}

const QIcon LanguagesManager::nationalFlagForLanguage(const std::string & language) const
{
	return LanguagesHelper::nationalFlagForLanguage(language);
}

void LanguagesManager::registerTranslator(const std::string & module, const std::string & language,
	TranslatorPtr translator)
{
	auto it = translators.find(module);
	if(it == translators.end()){
		translators[module][language] = translator;
	}else{
		auto IT = it->second.find(language);
		if(IT != it->second.end()){			
			CORE_LOG_INFO("Overriding already registered " << language << " translator for module " << module);
		}

		it->second[language] = translator;
	}

	languages_.insert(language);

	if(translator != nullptr && language == currentLanguage_){
		qApp->installTranslator(translator.get());
	}
}

void LanguagesManager::unregisterTranslators(const std::string & module)
{
	auto it = translators.find(module);
	if(it != translators.end()){
		
		StringSet langsToDelete;

		for(auto IT = it->second.begin(); IT != it->second.end(); ++IT){
			auto lang = IT->first;
			langsToDelete.insert(lang);
			if(lang == currentLanguage_ && IT->second != nullptr){
				qApp->removeTranslator(IT->second.get());
			}
		}

		translators.erase(it);

		for(auto IT = langsToDelete.begin(); IT != langsToDelete.end(); ++IT){
			tryRemoveUnusedLanguage(*IT);
		}
	}
}

void LanguagesManager::tryRemoveUnusedLanguage(const std::string & language)
{
	bool found = false;

	for(auto it = translators.begin(); it != translators.end(); ++it){

		if(it->second.find(language) != it->second.end()){
			found = true;
			break;
		}
	}

	if(found == false){
		languages_.erase(language);
	}
}