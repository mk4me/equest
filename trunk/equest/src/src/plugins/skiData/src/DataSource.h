/********************************************************************
	created:  2016/07/18
	filename: DataSource.h
	author:	  Mateusz Janiak
	
	purpose:  
*********************************************************************/
#ifndef __HEADER_GUARD_SKIDATA__DATASOURCE_H__
#define __HEADER_GUARD_SKIDATA__DATASOURCE_H__

#include <corelib/ISource.h>
#include <utils/Filesystem.h>
#include <map>
#include <list>
#include <corelib/Variant.h>
#include <corelib/IHierarchyItem.h>

namespace skidata
{
	class SkiDataSource : public plugin::ISource
	{
		UNIQUE_ID("{441BB894-1019-4382-BACA-F18A511A49CB}");
		CLASS_DESCRIPTION("Ski data source", "Ski data source");

	private:

		struct RunDescription
		{
			std::list<std::string> files;
			core::VariantsList generatedData;
			bool loaded;
			unsigned int idx;
			core::IHierarchyItemPtr hierarchyItem;
		};

	public:
		//! Konstruktor
		SkiDataSource();
		//! Destruktor wirtualny
		virtual ~SkiDataSource();

		//! Inicjalizacja ürÛd≥a. NastÍpuje juø po wczytaniu pluginÛw i skonstruowaniu
		//! (nie zainicjalizowaniu!) wszystkich ürÛde≥.
		virtual void init(core::IDataManager * memoryDM,
			core::IStreamDataManager * streamDM,
			core::IFileDataManager * fileDM,
			core::IDataHierarchyManager * hierarchyDM);

		//! Zobacz podobnπ metode dla IService
		//! PÛüna inicjalizacja ürÛd≥a, nastÍpuje po wczytaniu i inicjalizacji wszystkich innych ürÛde≥
		virtual bool lateInit();

		//! Metoda powinna w bezpieczny sposÛb zwalniac zasoby, majπc na uwadze øe niekoniecznie wszystkie us≥ugi i zasoby pobrane z zewnπtrz sπ jeszcze dostÍpne.
		//! Ta metoda w szczegolnoscis powinna zamknac wszystkie watki, ktÛre uruchomi≥ serwis, moøe tez zwalniac pamieÊ przydzielonπ dynamicznie
		//! Generalnie to taki bezpieczny destruktor uniezaleøniajπcy dana us≥ugÍ od pozosta≥ych us≥ug i przygotowujπcy ja na usuniÍcie
		virtual void finalize();

		//! Metoda aktualizujπca pochodzi z wπtku UI! Powinny tu byÊ realizowane lekkie operacje odúwieøania widgetÛw!!
		//! Jako parametr dostajemy przyrost czasu jaki mina≥ od poprzedniego wywo≥ania
		virtual void update(double deltaTime);

		//! èrÛd≥o nie musi mieÊ wizualnej reprezentacji.
		//! \return Widget tworzony przez ürÛd≥o bπdü NULL.
		virtual QWidget* getWidget();
		//! \return Widget kontrolujπcy zachowanie us≥ugi/us≥ug zaleønych.
		virtual QWidget* getControlWidget();
		//! \return Widget dostarczajπcy opcji zwiπzanych z us≥ugπ/us≥ugami zaleønymi.
		virtual QWidget* getSettingsWidget();
		//! \param offeredTypes Typy oferowane przez to ürÛd≥o
		virtual void getOfferedTypes(utils::TypeInfoList & offeredTypes) const;

		unsigned int addRun(const utils::Filesystem::Path & path);
		void removeRun(const utils::Filesystem::Path & path);
		std::list<utils::Filesystem::Path> currentRuns() const;

		void loadRun(const utils::Filesystem::Path & path);
		void unloadRun(const utils::Filesystem::Path & path);

		bool isLoaded(const utils::Filesystem::Path & path);

	private:

		static unsigned int runIdx;
		core::IHierarchyItemPtr hierarchyRoot;
		std::map<utils::Filesystem::Path, RunDescription> data;

		core::IDataManager * memoryDM;			
		core::IFileDataManager * fileDM;
		core::IDataHierarchyManager * hierarchyDM;
	};
}

#endif  // __HEADER_GUARD_SKIDATA__DATASOURCE_H__