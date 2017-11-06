/********************************************************************
	created:  2012/02/16
	created:  16:2:2012   13:47
	filename: IDataSourcePerspective.h
	author:   Mateusz Janiak

	purpose:
	*********************************************************************/
#ifndef HEADER_GUARD_HMDBCOMMUNICATION__IDATASOURCEPERSPECTIVE_H__
#define HEADER_GUARD_HMDBCOMMUNICATION__IDATASOURCEPERSPECTIVE_H__

#include <plugins/hmdbCommunication/ShallowCopy.h>
#include <plugins/hmdbCommunication/IDataSourceContent.h>

class QTreeWidget;
class QTreeWidgetItem;

namespace hmdbCommunication
{
	//! Interfejs perspektywy danych, ich hierarchi
	class IDataSourcePerspective : public IDataSourceContent
	{
	public:
		//! Wirtualny destruktor
		virtual ~IDataSourcePerspective() {}
		//! \param treeWidget Drzewo danych dla którego budujemy perspektywę
		//! \param shallowCopy Płytka kopia danych dla której generujemy perspektywę
		virtual void rebuildPerspective(QTreeWidgetItem * rootItem, const ShallowCopy & shallowCopy) = 0;

		//! Pusta implementacja funkcjonalności contentu - perspektywa może generować własne itemy-agregaty danych o których content nic nie wie

		//! \param item Element który chcemy wypełnić właściwą zawartością
		//! \param shallowCopy Płytka kopia związana z aktualnym kontekstem - można z niej pobrać dodatkowe info
		//! \return Czy wypełnilismy element
		virtual const bool fillContent(QTreeWidgetItem * item, const ShallowCopy & shallowCopy) const { return false; };
		//! \param item Element dla którego chcemy pobrać nagłówek
		//! \param headers [out] Nagłowek dla zadanego elementu
		//! \return Czy wypełnilismy nagłówek
		virtual const bool headers(const QTreeWidgetItem * item, QStringList & headers) const { return false; };
		//! \param headers [out] Domyslne nagłówki kiedy nie ma danych
		virtual void defaultHeaders(QStringList & headers) const { headers << name(); }
		//! \param item Element dla którego chcemy pobrać pole w którym zamieścimy jego status
		//! \reuturn Numer pola ze statusem
		virtual const int statusField(const QTreeWidgetItem * item) const { return -1; };
		//! \param item Element dla którego chcemy pobrać pole w którym zamieścimy info o aktualnym pobieraniu
		//! \reuturn Numer pola z info o pobieraniu
		virtual const int downloadField(const QTreeWidgetItem * item) const { return -1; };
		//! Czyścimy dane contentu (np. bo zmieniamy perspektywę)
		virtual void release() {}
	};

	DEFINE_SMART_POINTERS(IDataSourcePerspective)
}

#endif	//	HEADER_GUARD_HMDBCOMMUNICATION__IDATASOURCEPERSPECTIVE_H__
