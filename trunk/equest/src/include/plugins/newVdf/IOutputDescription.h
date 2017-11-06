/********************************************************************
	created:	2011/05/12
	created:	12:5:2011   9:14
	filename: 	IOutputDescription.h
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_CORE__IOUTPUTDESCRIPTION_H__
#define HEADER_GUARD_CORE__IOUTPUTDESCRIPTION_H__

#include <set>
#include <vector>
#include <string>
#include <utils/TypeInfo.h>
#include <utils/SmartPtr.h>

namespace vdf {

//! Interfejs służący do opisu typów danych wyjściowcyh oferowanych przez element przetwarzający.
class IOutputDescription
{
public:
    //! Struktura opisująca typy danych wyjściowe.
    struct OutputInfo
    {
        //! Typ opisujący zależne elementy wejściowe, które są wymagane do generowana danego typu wyjściowego
        typedef std::set<int> DependentInput;
        //! Nazwa wyjścia.
        std::string name;
        //! Typ wyjścia.
        utils::TypeInfo type;
        //! Zależne wejścia
        DependentInput dependentInput;

        //!
        OutputInfo() {}

        //!
        OutputInfo(const std::string& name, const utils::TypeInfo& type, const DependentInput & dependentInput = DependentInput()) :
        name(name), type(type), dependentInput(dependentInput)
        {}

        OutputInfo & operator= (const OutputInfo & other)
        {
            name = other.name;
            type = other.type;
            dependentInput = other.dependentInput;
            return *this;
        }
    };

public:
    //! Pobiera informacje o typach wyjść. Kolejność obowiązuje potem w metodzie process.
    //! \param output Obiekt umożliwiający zapis danych wyjsciowych - zapis zawsze jest możliwy!!
    virtual void getOutputInfo( std::vector<OutputInfo> & output ) const = 0;

    virtual ~IOutputDescription() {}
};
DEFINE_SMART_POINTERS(IOutputDescription);
}

#endif
