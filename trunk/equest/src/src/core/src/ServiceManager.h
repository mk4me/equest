#ifndef SERVICE_MANAGER_H
#define SERVICE_MANAGER_H

#include <map>
#include <vector>

#include <corelib/IServiceManager.h>
#include <corelib/IService.h>

namespace core {

class ServiceManager: public IServiceManager
{
public:
    //! Słownik usług.
    typedef std::map<UniqueID, plugin::IServicePtr> ServicesMap;
    //! Sekwencja usług.
    typedef std::vector<plugin::IServicePtr> ServicesList;

private:
    //! Słownik usług.
    ServicesMap servicesMap; 
    //! Sekwencja usług.
    ServicesList servicesList;

public:
    ServiceManager();
	virtual ~ServiceManager(void);

public:

    void update(double deltaTime);

    //! Metoda finalizująca wszystkie serwisy  wywoływana przed niszczeniem managera
    void finalizeServices();

// IServiceManager
public:
    //! Rejestruje usługę. Usługi muszą mieć unikatowe ID!
    //! \param service Usłguga do rejestracji.
    virtual void registerService(plugin::IServicePtr service);
    //! \return Liczba usług.
    virtual int getNumServices() const;
    //! \param idx Indeks usługi.
    //! \return Usługa o zadanym indeksie.
    virtual plugin::IServicePtr getService(int idx);
    //! \param id ID usługi do wyszukania.
    //! \return Odnaleziona usługa bądź NULL.
    virtual plugin::IServicePtr getService(UniqueID id);
};

}

#endif //SERVICE_MANAGER_H
