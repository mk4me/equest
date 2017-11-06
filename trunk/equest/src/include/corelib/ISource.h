/********************************************************************
    created:  2012/01/31
    created:  31:1:2012   8:58
    filename: ISource.h
    author:   Mateusz Janiak

    purpose:  Interfejs źródła danych mogący wstrzykiwać dane poprzez obiekty domenowe do aplikacji. Ma dostęp do MemoryDataManager i FileDataManager
*********************************************************************/
#ifndef HEADER_GUARD_CORE__ISOURCE_H__
#define HEADER_GUARD_CORE__ISOURCE_H__

#include <corelib/IIdentifiable.h>
#include <utils/SmartPtr.h>
#include <utils/TypeInfo.h>

class QWidget;

namespace core {

	class IDataManager;
	class IFileDataManager;
	class IStreamDataManager;
	class IServiceManager;
	class IDataHierarchyManager;

}

namespace plugin
{
    //! Interfejs źródła danych. Powinien to być jedeny obiekt inicjowany i mający dostęp do MemoryDM i FIleDM
    class ISource : public IIdentifiable, public IDescription
    {
    public:

		virtual ~ISource() {}

        //! Inicjalizacja źródła. Następuje już po wczytaniu pluginów i skonstruowaniu
        //! (nie zainicjalizowaniu!) wszystkich źródeł.
        virtual void init(core::IDataManager * memoryDM,
			core::IStreamDataManager * streamDM,
			core::IFileDataManager * fileDM,
			core::IDataHierarchyManager * hierarchyDM) = 0;

		//! Zobacz podobną metode dla IService
        //! Późna inicjalizacja źródła, następuje po wczytaniu i inicjalizacji wszystkich innych źródeł
        virtual bool lateInit() = 0;

		//! Metoda powinna w bezpieczny sposób zwalniac zasoby, mając na uwadze że niekoniecznie wszystkie usługi i zasoby pobrane z zewnątrz są jeszcze dostępne.
        //! Ta metoda w szczegolnoscis powinna zamknac wszystkie watki, które uruchomił serwis, może tez zwalniac pamieć przydzieloną dynamicznie
        //! Generalnie to taki bezpieczny destruktor uniezależniający dana usługę od pozostałych usług i przygotowujący ja na usunięcie
        virtual void finalize() = 0;

        //! Metoda aktualizująca pochodzi z wątku UI! Powinny tu być realizowane lekkie operacje odświeżania widgetów!!
        //! Jako parametr dostajemy przyrost czasu jaki minał od poprzedniego wywołania
        virtual void update(double deltaTime) = 0;

        //! Źródło nie musi mieć wizualnej reprezentacji.
        //! \return Widget tworzony przez źródło bądź NULL.
        virtual QWidget* getWidget() = 0;
		//! \return Widget kontrolujący zachowanie usługi/usług zależnych.
		virtual QWidget* getControlWidget() = 0;
		//! \return Widget dostarczający opcji związanych z usługą/usługami zależnymi.
		virtual QWidget* getSettingsWidget() = 0;
		//! \param offeredTypes Typy oferowane przez to źródło
		virtual void getOfferedTypes(utils::TypeInfoList & offeredTypes) const = 0;
    };

	DEFINE_SMART_POINTERS(ISource);

}

#endif //HEADER_GUARD_CORE__ISOURCE_H__
