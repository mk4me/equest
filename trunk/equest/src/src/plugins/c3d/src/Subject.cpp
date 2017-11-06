//#include <fstream>
//#include <boost/program_options.hpp>
//#include <boost/program_options/detail/config_file.hpp>
//#include <boost/program_options/parsers.hpp>
//#include <plugins/c3d/Subject.h>
//#include "SubjectParser.h"
//
//void SubjectParser::parseFile( core::IDataManager* dataManager, const boost::filesystem::path& path )
//{
//	std::ifstream filestream(path.string());
//	std::set<std::string> options;
//	options.insert("*");
//	std::map<std::string, std::string> dictionary;
//	
//	for (boost::program_options::detail::config_file_iterator i(filestream, options), e ; i != e; ++i) {
//		dictionary[i->string_key] = i->value[0];
//	}
//
//	SubjectPtr sub(new Subject());
//	sub->date =         dictionary["SUBJECT_INFO.CREATIONDATEANDTIME"];
//	sub->description1 = dictionary["SUBJECT_INFO.DESCRIPTION"];
//	sub->notes =		dictionary["SUBJECT_INFO.NOTES"];
//	sub->sex =			dictionary["PATIENT_INFO.PLEC"];
//	sub->birthdate =	dictionary["PATIENT_INFO.DATA_URODZENIA"];
//	sub->isIll =	    dictionary["PATIENT_INFO.CHORY"];
//	sub->isDiabetes =	dictionary["PATIENT_INFO.CUKRZYCA"];
//	sub->ilnesses =		dictionary["PATIENT_INFO.CHOROBY"];
//	sub->description2 = dictionary["PATIENT_INFO.DESCRIPTION"];
//	sub->name =			dictionary["PATIENT_INFO.IMIE"];
//	sub->surname =		dictionary["PATIENT_INFO.NAZWISKO"];
//	sub->status =		dictionary["PATIENT_INFO.STATUS"];
//
//	subject->set(sub);
//}
//
//core::IParser* SubjectParser::create()
//{
//	return new SubjectParser();
//}
//
//std::string SubjectParser::getSupportedExtensions() const
//{
//	return ".enf";
//}
//
//
//void SubjectParser::getObjects( std::vector<core::ObjectWrapperPtr>& objects )
//{
//	objects.push_back(subject);
//}
//
//SubjectParser::SubjectParser()
//{
//	subject = core::ObjectWrapper::create<Subject>();
//}
//
//SubjectParser::~SubjectParser()
//{
//
//}
