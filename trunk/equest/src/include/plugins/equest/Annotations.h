/********************************************************************
	created:	2013/11/25
	created:	25:11:2013   8:52
	filename: 	Annotations.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_EQUEST__ANNOTATIONS_H__
#define HEADER_GUARD_EQUEST__ANNOTATIONS_H__

#include <string>
#include <boost/bimap.hpp>
//#include "qstring_serialization.h"
#include <utils/Filesystem.h>
#include <plugins/equest/Export.h>
#include <boost/lexical_cast.hpp>

namespace equest {

namespace annotations {

	//! Indeksy adnotacji
    enum annotationsIdx {
        unknownAnnotation = -1,
        otherAnnotation = 0, 
        skin = 1,  
        bone = 2,  
        tendon = 3,
        joint = 4, 
        inflammatory = 5,
        intensity = 6,
        noise = 7,
		bloodLevel = 8,
		inflammatoryLevel = 9,
		fingerType = 10,
		jointType = 11,
		imageType = 12
    };

	//! Mapowanie indesków adnotacji do ich nazw
    typedef boost::bimap<annotationsIdx, QString> AdnotationsType;
	typedef boost::shared_ptr<AdnotationsType> AdnotationsTypePtr;
	typedef boost::shared_ptr<const AdnotationsType> AdnotationsTypeConstPtr;

	//! Tworzy domyślne mapowanie indeksów
	PLUGIN_EQUEST_EXPORT AdnotationsTypePtr getDefault();
	//! \param p ścieżka pliku do którego zapisujemy  typy adnotacji
	//! \param adnotations Zapisywane typy adnotacji
	PLUGIN_EQUEST_EXPORT void save(const utils::Filesystem::Path& p, AdnotationsTypeConstPtr adnotations);
	//! \param p ścieżka z której wczytujemy typy adnotacji
	PLUGIN_EQUEST_EXPORT AdnotationsTypePtr load(const utils::Filesystem::Path& p);
	//! Zwraca instancje typów adnotacji
	PLUGIN_EQUEST_EXPORT AdnotationsTypePtr instance();
	//! Zapisuje aktualne typy adnotacji
	PLUGIN_EQUEST_EXPORT void autoSave();
	//! \return ścieżka pliku z typami adnotacji
	PLUGIN_EQUEST_EXPORT utils::Filesystem::Path adnotationsFile();
	
	//! Typ poziomu krwi
	enum bloodLevelDescriptor {

		unknownBloodLevel = -1,
		bloodLevel0,
		bloodLevel1,
		bloodLevel2,
		bloodLevel3
	};

	//! Mapowanie indeksów do nazw z ich opisem
	typedef boost::bimap<bloodLevelDescriptor, QString> BloodLevelsType;
	DEFINE_SMART_POINTERS(BloodLevelsType);

	//! \return Zwraca domyślne poziomy krwi
	PLUGIN_EQUEST_EXPORT BloodLevelsTypePtr getDefaultBloodLevels();
	//! Zwracaj instancję poziomów krwi
	PLUGIN_EQUEST_EXPORT BloodLevelsTypePtr instanceBloodLevels();

	//! Typy poziomów schorzenia
	enum inflammatoryLevelDescriptor {

		unknownInflammatoryLevel = -1,
		inflammatoryLevel0,
		inflammatoryLevel1,
		inflammatoryLevel2,
		inflammatoryLevel3
	};

	//! Mapowanie typów poziomów schorzenia do ich opisów
	typedef boost::bimap<inflammatoryLevelDescriptor, QString> InflammatoryLevelsType;
	DEFINE_SMART_POINTERS(InflammatoryLevelsType);

	//! \return Domyślne typy poziomów schorzenia
	PLUGIN_EQUEST_EXPORT InflammatoryLevelsTypePtr getDefaultInflammatoryLevels();
	//! \return Instancja poziomów schorzenia
	PLUGIN_EQUEST_EXPORT InflammatoryLevelsTypePtr instanceInflammatoryLevels();

	//! Nowe rzeczy

	//! Typ palca
	enum fingerTypeDescriptor {

		unknownFinger = -1,
		finger1,
		finger2,
		finger3,
		finger4,
		finger5
	};

	//! Mapiowanie indeksów do nazw z ich opisem
	typedef boost::bimap<fingerTypeDescriptor, QString> FingerType;
	DEFINE_SMART_POINTERS(FingerType);

	//! \return Zwraca domyślne poziomy krwi
	PLUGIN_EQUEST_EXPORT FingerTypePtr getDefaultFingerTypes();
	//! Zwracaj instancję poziomów krwi
	PLUGIN_EQUEST_EXPORT FingerTypePtr instanceFingerTypes();

	//! Typ jointa
	enum jointTypeDescriptor {

		unknownJoint = -1,
		LPIP,
		RPIP,
		LMCP,
		RMCP		
	};

	//! Mapiowanie indeksów do nazw z ich opisem
	typedef boost::bimap<jointTypeDescriptor, QString> JointType;
	DEFINE_SMART_POINTERS(JointType);

	//! \return Zwraca domyślne poziomy krwi
	PLUGIN_EQUEST_EXPORT JointTypePtr getDefaultJointTypes();
	//! Zwracaj instancję poziomów krwi
	PLUGIN_EQUEST_EXPORT JointTypePtr instanceJointTypes();

	//! Typ jakości obrazu
	enum imageTypeDescriptor {
		incorrect = 0,
		different = 1,
		identical = 2,
	};

	//! Mapiowanie indeksów do nazw z ich opisem
	typedef boost::bimap<imageTypeDescriptor, QString> ImageType;
	DEFINE_SMART_POINTERS(ImageType);

	//! \return Zwraca domyślne poziomy krwi
	PLUGIN_EQUEST_EXPORT ImageTypePtr getDefaultImageTypes();
	//! Zwracaj instancję poziomów krwi
	PLUGIN_EQUEST_EXPORT ImageTypePtr instanceImageTypes();

	template <typename T>
	QString annotationValueAsString(const T& val)
	{
		return QString::fromStdString(boost::lexical_cast<std::string>(val));
	}

	template <>
	inline QString annotationValueAsString(const equest::annotations::annotationsIdx& val)
	{
		return instance()->left.at(val);
	}

	template <>
	inline QString annotationValueAsString(const equest::annotations::bloodLevelDescriptor& val)
	{
        /*if (val == equest::annotations::unknownBloodLevel) {
            return "-1";
        }
		return instanceBloodLevels()->left.at(val);*/
        return QString("%1").arg((int)val);
	}

	template <>
	inline QString annotationValueAsString(const equest::annotations::inflammatoryLevelDescriptor& val)
	{
        /*if (val == equest::annotations::unknownInflammatoryLevel) {
            return "-1";
        }
		return instanceInflammatoryLevels()->left.at(val);*/
        return QString("%1").arg((int)val);
	}

	template <>
	inline QString annotationValueAsString(const equest::annotations::fingerTypeDescriptor& val)
	{
        /*if (val == equest::annotations::unknownFinger) {
            return "-1";
        }
		return instanceFingerTypes()->left.at(val);*/
        return QString("%1").arg((int)val);
	}

	template <>
	inline QString annotationValueAsString(const equest::annotations::jointTypeDescriptor& val)
	{
        if (val == equest::annotations::unknownJoint) {
            return "-1";
        }
		return instanceJointTypes()->left.at(val);
	}

	template <>
	inline QString annotationValueAsString(const equest::annotations::imageTypeDescriptor& val)
	{
		return instanceImageTypes()->left.at(val);
	}


}

}

#endif
