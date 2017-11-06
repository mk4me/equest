/********************************************************************
	created:  2012/03/23
	created:  23:3:2012   13:47
	filename: IDataSourceContent.h
	author:   Mateusz Janiak

	purpose:
	*********************************************************************/
#ifndef HEADER_GUARD_HMDBCOMMUNICATION__IDATASOURCECONTENT_H__
#define HEADER_GUARD_HMDBCOMMUNICATION__IDATASOURCECONTENT_H__

#include <hmdbserviceslib/ShallowCopy.h>
#include <plugins/hmdbCommunication/ShallowCopy.h>
#include <QtWidgets/QTreeWidgetItem>

namespace hmdbCommunication
{
	//! Typ zawartości
	enum ContentType {		
		PatientContent,			// pacjent
		SubjectContent,			// subject
		SessionContent,			// sesja
		MotionContent,			// motion
		FileContent,			// plik		
		PatientsGroupContent,	// grupa pacjentów
		SubjectsGroupContent,	// grupa subjectów
		SessionsGroupContent,	// grupa sesji
		MotionsGroupContent,	// grupa motionów
		FilesGroupContent,		// grupa plików
		CustomContent			// własna zawartość
	};

	//! Interfejs bazowy dla elementów realizujących zawartość
	class TreeWidgetContentItem : public QTreeWidgetItem
	{
	public:

		explicit TreeWidgetContentItem(const ContentType contentType,
			const hmdbServices::ID id, int type = Type) : QTreeWidgetItem(type),
			contentType_(contentType), id_(id)
		{

		}

		TreeWidgetContentItem(const ContentType contentType,
			const hmdbServices::ID id, const QStringList &strings,
			int type = Type) : QTreeWidgetItem(strings, type),
			contentType_(contentType), id_(id)
		{

		}

		explicit TreeWidgetContentItem(const ContentType contentType,
			const hmdbServices::ID id, QTreeWidget *view,
			int type = Type) : QTreeWidgetItem(view, type),
			contentType_(contentType), id_(id)
		{

		}

		TreeWidgetContentItem(const ContentType contentType,
			const hmdbServices::ID id, QTreeWidget *view,
			const QStringList &strings, int type = Type) : QTreeWidgetItem(view, strings, type),
			contentType_(contentType), id_(id)
		{

		}

		TreeWidgetContentItem(const ContentType contentType,
			const hmdbServices::ID id, QTreeWidget *view,
			QTreeWidgetItem *after, int type = Type) : QTreeWidgetItem(view, after, type),
			contentType_(contentType), id_(id)
		{

		}

		explicit TreeWidgetContentItem(const ContentType contentType,
			const hmdbServices::ID id, QTreeWidgetItem *parent,
			int type = Type) : QTreeWidgetItem(parent, type),
			contentType_(contentType), id_(id)
		{

		}

		TreeWidgetContentItem(const ContentType contentType,
			const hmdbServices::ID id, QTreeWidgetItem *parent,
			const QStringList &strings, int type = Type) : QTreeWidgetItem(parent, strings, type),
			contentType_(contentType), id_(id)
		{

		}

		TreeWidgetContentItem(const ContentType contentType,
			const hmdbServices::ID id, QTreeWidgetItem *parent,
			QTreeWidgetItem *after, int type = Type) : QTreeWidgetItem(parent, after, type),
			contentType_(contentType), id_(id)
		{

		}

		TreeWidgetContentItem(const TreeWidgetContentItem &other) : QTreeWidgetItem(other),
			contentType_(other.contentType_), id_(other.id_)
		{

		}

		//! Wirtualny destruktor
		virtual ~TreeWidgetContentItem() {}
		//! \return Typ contentu zawieranego przez dany element
		inline const ContentType contentType() const { return contentType_; };
		//! \return Identyfikator obiektu jak w płytkiej kopii lub -1 dla złożonych obiektów i własncyh danych
		inline const hmdbServices::ID id() const { return id_; }

	private:

		const ContentType contentType_;
		const hmdbServices::ID id_;
	};

	//! Interfejs podstawowej funkcjonalności generatorów zawartości - zarówno perspektywa jak i generatory go implementują
	class IDataSourceContentFunctionality
	{
	public:
		//! Wirtualny destruktor
		virtual ~IDataSourceContentFunctionality() {}
		//! \param item Element który chcemy wypełnić właściwą zawartością
		//! \param shallowCopy Płytka kopia związana z aktualnym kontekstem - można z niej pobrać dodatkowe info
		//! \return Czy wypełnilismy element
		virtual const bool fillContent(QTreeWidgetItem * item, const ShallowCopy & shallowCopy) const = 0;
		//! \param item Element dla którego chcemy pobrać nagłówek
		//! \param headers [out] Nagłowek dla zadanego elementu
		//! \return Czy wypełnilismy nagłówek
		virtual const bool headers(const QTreeWidgetItem * item, QStringList & headers) const = 0;
		//! \param item Element dla którego chcemy pobrać pole w którym zamieścimy jego status
		//! \reuturn Numer pola ze statusem
		virtual const int statusField(const QTreeWidgetItem * item) const = 0;
		//! \param item Element dla którego chcemy pobrać pole w którym zamieścimy info o aktualnym pobieraniu
		//! \reuturn Numer pola z info o pobieraniu
		virtual const int downloadField(const QTreeWidgetItem * item) const = 0;
		//! Czyścimy dane contentu (np. bo zmieniamy perspektywę)
		virtual void release() = 0;
		//! \param c Znak symbolizujący płeć - z hmdbServices::MedicalShallowCopy::Patient, m,M => Mężczyzna, f,F,w,W,k,K => Kobieta
		//! \return Przetłumaczona nazwa płci
		static const QString decodeGender(const hmdbServices::xmlWsdl::Gender::Type g)
		{
			QString gender;

			switch (g){
			case hmdbServices::xmlWsdl::Gender::Male:
				gender = QObject::tr("Man");
				break;
			case hmdbServices::xmlWsdl::Gender::Female:
				gender = QObject::tr("Woman");
				break;
			default:
				gender = QObject::tr("Not specified");
				break;
			}

			return gender;
		}
	};

	//! Interfejc właściwy contentu - dodaje nazwę contentu do jego podstawowej funkcjonalności
	class IDataSourceContent : public IDataSourceContentFunctionality
	{
	public:
		//! Wirtualny destruktor
		virtual ~IDataSourceContent() {}
		//! \return Nazwa contentu
		virtual const QString name() const = 0;
	};

	typedef utils::shared_ptr<IDataSourceContent> DataSourceContentPtr;
}

#endif	//	HEADER_GUARD_HMDBCOMMUNICATION__IDATASOURCECONTENT_H__
