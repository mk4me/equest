/********************************************************************
	created:  2012/02/28
	created:  28:2:2012   22:32
	filename: IAdministrationWS.h
	author:   Mateusz Janiak

	purpose:
	*********************************************************************/
#ifndef HEADER_GUARD__HMDBSERVICES_IADMINISTRATIONWS_H__
#define HEADER_GUARD__HMDBSERVICES_IADMINISTRATIONWS_H__

#include <string>
#include <utils/SmartPtr.h>

namespace hmdbServices
{
	//! Interfejs serwisu z operacjami administracyjnymi
	class IAdministrationWS
	{
	public:
		//! Destruktor wirtualny
		virtual ~IAdministrationWS() {}

		//! \param groupName Nazwa grupy atrybutów
		//! \param entity Typ encji
		virtual void defineAttributeGroup(const std::string & groupName, const std::string & entity) = 0;

		//! \param groupName Nazwa grupy
		//! \param entity Typ encji
		virtual void removeAttributeGroup(const std::string & groupName, const std::string & entity) = 0;

		//! TODO - weryfikacja atrybutów i ich znaczenia!!
		//! \param attributName Nazwa atrybutu
		//! \param groupName Nazwa grupy atrybutów
		//! \param Typ encji
		//! \param isEnum Czy jest to atrybut wyliczeniowy
		//! \param pluginDescriptor Opis pluginu
		//! \param type Typ atrybutu
		//! \param unit Jednostka
		virtual void defineAttribute(const std::string & attributeName, const std::string & groupName,
			const std::string & entity, const bool isEnum, const std::string & pluginDescriptor,
			const std::string & type, const std::string & unit) = 0;

		//! \param attributName Nazwa atrybutu
		//! \param groupName Nazwa grupy
		//! \param entity Typ encji
		virtual void removeAttribute(const std::string & attributeName, const std::string & groupName,
			const std::string & entity) = 0;

		//! \param attributeName Nazwa atrybutu
		//! \param groupName Nazwa grupy
		//! \param entity Typ encji
		//! \param value Wartoœæ atrybutu
		//! \param clearExisting Czy ma usuwaæ wartoœæ ju¿ istniej¹c¹?
		virtual void addAttributeEnumValue(const std::string & attributeName, const std::string & groupName,
			const std::string & entity, const std::string & value, const bool clearExisting) = 0;

		//! \param olderThanMinutes Czas w minutach dla którego usuwa siê pliki z obszaru œci¹gania po stronie bazy danych
		virtual void downloadAreaCleanup(const int olderThanMinutes) = 0;
	};

	using AdministrationWSPtr = utils::shared_ptr<IAdministrationWS>;
	using AdministrationWSConstPtr = utils::shared_ptr<const IAdministrationWS>;
}

#endif	//	HEADER_GUARD__HMDBSERVICES_IADMINISTRATIONWS_H__
