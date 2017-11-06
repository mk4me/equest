/********************************************************************
	created:  2014/08/19	09:17:34
	filename: HMDBSourceViewManager.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_HMDBCOMMUNICATION__HMDBSOURCEVIEWMANAGER_H__
#define __HEADER_GUARD_HMDBCOMMUNICATION__HMDBSOURCEVIEWMANAGER_H__

#include <plugins/hmdbCommunication/IHMDBSourceViewManager.h>
#include <utils/SmartPtr.h>

namespace hmdbCommunication
{
	class HMDBSourceViewManager : public IHMDBSourceViewManager
	{
	private:

		struct ViewDescription
		{
			utils::shared_ptr<IHMDBSourceContextView> viewPrototype;
			std::vector<ContextConfiguration> configurations;
			std::vector<utils::shared_ptr<IDataSourcePerspective>> perspectives;
			std::vector<utils::shared_ptr<IDataSourceContent>> contents;
			std::vector<utils::shared_ptr<ShallowCopyFilter>> filters;
		};

		typedef std::map<QString, ViewDescription> ViewDescriptions;
		typedef std::vector<IHierarchyPerspectivePtr> HierarchyPerspectives;


	public:
		//! Konstruktor domyslny
		HMDBSourceViewManager();
		//! Destruktor wirtualny
		virtual ~HMDBSourceViewManager();

		//! \param viewPrototype Prototyp widoku kontekstów
		virtual const bool registerViewPrototype(IHMDBSourceContextView * viewPrototype);
		//! \return Iloœc zarejestrowanych prototypów widoków
		virtual const unsigned int viewPrototypesCount() const;
		//! \param prototypeName Nawa prototypu
		//! \return Prototyp widoku
		virtual IHMDBSourceContextView * viewPrototype(const QString & prototypeName);
		//! \param prototypeName Nawa prototypu
		//! \return Prototyp widoku
		virtual const IHMDBSourceContextView * viewPrototype(const QString & prototypeName) const;
		//! \param idx Indeks prototypu widoku
		//! \return Prototyp widoku
		virtual IHMDBSourceContextView * viewPrototype(const unsigned int idx);
		//! \param idx Indeks prototypu widoku
		//! \return Prototyp widoku
		virtual const IHMDBSourceContextView * viewPrototype(const unsigned int idx) const;
		//! \param config Konfiguracja widoku
		//! \param prototypeName Nawa prototypu
		//! \return Czy uda³o siê zarejestrowaæ konfiguracjê (nazwa musi byæ unikalna!!)
		virtual const bool registerConfiguration(const ContextConfiguration & config,
			const QString & prototypeName = QString());
		//! \param prototypeName Nawa prototypu
		//! \return Iloœæ konfiguracji prototypu
		virtual const unsigned int configurationsCount(const QString & prototypeName = QString()) const;
		//! \param idx Indeks konfiguracji
		//! \param prototypeName Nawa prototypu
		//! \return Konfiguracja
		virtual const ContextConfiguration configuration(const unsigned int idx, const QString & prototypeName = QString()) const;
		//! \param perspective Perspektywa danych
		//! \param prototypeName Nawa prototypu
		//! \return Czy uda³o siê zarejestrowaæ perspektywê danych (nazwa musi byæ unikalna!!)
		virtual const bool registerPerspective(IDataSourcePerspective * perspective, const QString & prototypeName = QString());
		//! \param prototypeName Nawa prototypu
		//! \return Iloœæ perspektyw prototypu
		virtual const unsigned int perspectivesCount(const QString & prototypeName = QString()) const;
		//! \param idx Indeks perspektywy
		//! \param prototypeName Nawa prototypu
		//! \return Perspektywa
		virtual IDataSourcePerspective * perspective(const unsigned int idx, const QString & prototypeName = QString());
		//! \param idx Indeks perspektywy
		//! \param prototypeName Nawa prototypu
		//! \return Perspektywa
		virtual const IDataSourcePerspective * perspective(const unsigned int idx, const QString & prototypeName = QString()) const;
		//! \param viewPrototype Prototyp widoku
		//! \param content Zawartoœæ perspektywy
		//! \return Czy uda³o siê zarejestrowaæ content (nazwa musi byæ unikalna!!)
		virtual const bool registerContent(IDataSourceContent * content, const QString & prototypeName = QString());
		//! \param prototypeName Nawa prototypu
		//! \return Iloœæ contentó perspektywy
		virtual const unsigned int contentsCount(const QString & prototypeName = QString()) const;
		//! \param idx Indeks contentu
		//! \param prototypeName Nawa prototypu
		//! \return Content
		virtual IDataSourceContent * content(const unsigned int idx, const QString & prototypeName = QString());
		//! \param idx Indeks contentu
		//! \param prototypeName Nawa prototypu
		//! \return Content
		virtual const IDataSourceContent * content(const unsigned int idx, const QString & prototypeName = QString()) const;
		//! \param filter Filtr danych
		//! \param prototypeName Nawa prototypu
		//! \return Czy uda³o siê zarejestrowaæ filtr (nazwa musi byæ unikalna!!)
		virtual const bool registerFilter(ShallowCopyFilter * filter, const QString & prototypeName = QString());
		//! \param prototypeName Nawa prototypu
		//! \return Iloœæ filtrów
		virtual const unsigned int filtersCount(const QString & prototypeName = QString()) const;
		//! \param idx Indeks filtru
		//! \param prototypeName Nawa prototypu
		//! \return Filtr
		virtual ShallowCopyFilter * filter(const unsigned int idx, const QString & prototypeName = QString());
		//! \param idx Indeks filtru
		//! \param prototypeName Nawa prototypu
		//! \return Filtr
		virtual const ShallowCopyFilter * filter(const unsigned int idx, const QString & prototypeName = QString()) const;
		//! \param hierarchyPerspective Perspektywa dla analiz
		//! \return Czy uda³o siê dodaæ perspektywe dla analiz
		virtual const bool addHierarchyPerspective(IHierarchyPerspectivePtr hierarchyPerspective);
		//! \return Iloœc zarejestrownaych perspektyw dla analiz
		virtual const unsigned int hierarchyPerspectivesCount() const;
		//! \param idx Indeks perspektywy
		//! \return Perspektywa dla analiz
		virtual const IHierarchyPerspectivePtr hierarchyPerspective(const unsigned int idx);
		//! \param idx Indeks perspektywy
		//! \return Perspektywa dla analiz
		virtual const IHierarchyPerspectiveConstPtr hierarchyPerspective(const unsigned int idx) const;

	private:

		ViewDescription * find(const QString & name);
		const ViewDescription * find(const QString & name) const;

	private:
		//! Opis widoków
		ViewDescriptions viewDescriptions;
		//! Ogólne elementy dla wszystkich widoków
		ViewDescription generalViewElements;
		//! Hierarchie analiz
		HierarchyPerspectives hierarchyPerspectives;
	};
}

#endif	// __HEADER_GUARD_HMDBCOMMUNICATION__HMDBSOURCEVIEWMANAGER_H__