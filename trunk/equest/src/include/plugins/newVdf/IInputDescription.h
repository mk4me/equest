/********************************************************************
	created:	2011/05/12
	created:	12:5:2011   9:12
	filename: 	IInputDescription.h
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_CORE__IINPUTDESCRIPTION_H__
#define HEADER_GUARD_CORE__IINPUTDESCRIPTION_H__

#include <utils/TypeInfo.h>
#include <vector>
#include <string>

namespace vdf {

class IInputDescription
{
public:
    virtual ~IInputDescription() 
    {
    }

    //! Struktura opisująca typy danych wejściowe.
    struct InputInfo
    {
        //! Nazwa wejścia.
        std::string name;
        //! Typy wspierane przez wejście.
        utils::TypeInfo type;
        //! Czy wejściew wymagane
        bool required;
        //! Czy dane na wejsciu będza modyfikowane
        bool modify;
    };

public:
    //! Pobiera informacje o wejściu. Gdy danego wejścia nie ma wynikowa lista typów powinna
    //! być pusta.
    //! \param inputNo Numer wejścia. 
    //! \param name Nazwa wejścia.
    //! \param type Typy wspierane na danym wejściu.
    virtual void getInputInfo(std::vector<InputInfo>& info) const = 0;
};
DEFINE_SMART_POINTERS(IInputDescription);
}

#endif
