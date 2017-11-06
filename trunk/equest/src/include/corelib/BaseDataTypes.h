#ifndef BASE_DATA_TYPES_H
#define BASE_DATA_TYPES_H

#include <corelib/Export.h>
#include <string>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

namespace core {

//! Typ unikalnego identyfikatora
typedef boost::uuids::uuid UniqueID;

//! Klasa generująca unikalne identyfikatory dla żądanych ciągów znaków
//! zapewnia jednoznaczne przyporządkowanie ciąg znaków <-> id,
//! dla tego samego ciągu znaków zwraca ten sam id. Wymagane jest więc dostarczanie z zewnątrz
//! unikalnych ciągów znaków - polecamy jakiś GUID generator (np. z Visual Studio)
//! Ta klasa i definicja typu dla identyfikatora normalizują różne możliwe podejścia do unikalnych identyfikatorów w aplikacji
class CORELIB_EXPORT UID 
{
public:
//! Statyczna metoda
//! \param uuid Ciąg znaków dla którego generujemy unikalny identyfikator
//! \return Unikalny identyfikator
inline static const UniqueID GenerateUniqueID(const std::string& uuid) 
{
    return boost::uuids::string_generator()(uuid);
}
};

//! Interfejs obserwatora zmian dla managerów
template <class Changes>
class IChangesObserver
{
public:

	virtual ~IChangesObserver() {}

	//! \param previousValue 
	//! \param currentVal 
	//! \param type 
	//! \param modyfication 
	virtual void observe(const Changes & changes) = 0;
};

}

#endif //BASE_SERVICE_H
