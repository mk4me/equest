/********************************************************************
created:  2010/04/21
created:  21:4:2010   17:47
filename: Enum.h
author:	  Piotr Gwiazdowski

purpose:
*********************************************************************/
#ifndef __MOTIONUTIL_ENUM_H__
#define __MOTIONUTIL_ENUM_H__

//------------------------------------------------------------------------------
#include <utils/Config.h>
#include <vector>
#include <string>
#include <sstream>
#include <map>
#include <utils/Macros.h>
#include <stdexcept>
#include <algorithm>

////////////////////////////////////////////////////////////////////////////////
namespace utils {
////////////////////////////////////////////////////////////////////////////////

/**
* Makro kojarzące z wyliczeniem dodatkowe informacje. Nie może być używane w
* nagłówkach (tworzy zmienną statyczną).
*
* Przykład użycia:
* enum Color {
*  ColorRed  = 0x01,
*  ColorBlue,
*  ColorPink = ColorRed | ColorBlue,
* };
* #define COLOR_ENUM_DEF \
*  ColorRed  = 0x01,\
*  ColorBlue,\
*  ColorPink = ColorRed | ColorBlue
* DECLARE_ENUM_ENCHANCEMENT(Color, COLOR_ENUM_DEF)
*
* Inaczej:
* #define COLOR_ENUM_DEF \
*  ColorRed  = 0x01,\
*  ColorBlue,\
*  ColorPink = ColorRed | ColorBlue
* enum Color { COLOR_ENUM_DEF };
* DECLARE_ENUM_ENCHANCEMENT(Color, COLOR_ENUM_DEF);
*
* \param TYPE      Typ już istniejącego wyliczenia.
* \param ENUM_DEF  Definicja wyliczenia podana w postaci makra. Definicja musi
*                  zawierać tylko to, co znajduje się między klamrami.
*                  Mogą w niej występować komentarze w stylu C (komentarzy w stylu C++ nie
*                  da się umieścić w makrze).
*/
#define DECLARE_ENUM_ENCHANCEMENT(TYPE, ENUM_DEF)                   \
namespace utils {                                                   \
template <>                                                         \
class __EnumInitializer<TYPE>                                       \
{                                                                   \
public:                                                             \
    /*Dane enuma.*/                                                 \
    __EnumData<TYPE> data;                                          \
    __EnumInitializer()                                             \
    {                                                               \
        /*pobieramy wartości enuma*/                                \
        std::vector<__EnumInitHelper::value_type> values;           \
        /*inicjalizacja*/                                           \
        __EnumInitHelper::pValues = &values;                        \
        __EnumInitHelper::lastValue = __EnumInitHelper::initValue;  \
        /*właściwe dane*/                                           \
        __EnumInitHelper ENUM_DEF;                                  \
        /*zerujemy dla bezpieczeństwa*/                             \
        __EnumInitHelper::lastValue = __EnumInitHelper::initValue;  \
        __EnumInitHelper::pValues = NULL;                           \
        /**/                                                        \
        /*inicjalizacja danych*/                                    \
        const char * raw = COMA_AWARE_STRINGIZE(ENUM_DEF);          \
        data.init(values, raw);                                     \
    }                                                               \
};                                                                  \
}

//------------------------------------------------------------------------------

//! Klasa używana do inicjalizacji. Używana jedynie w drodze specjalizacji.
template <class T>
class __EnumInitializer;

//------------------------------------------------------------------------------

//! Pomocnicza klasa służąca rekonstrukcji wartości wyliczeń. Konstrukcja
//! tych obiektów odbywa się przez C++ runtime.
struct __EnumInitHelper
{
    //! Typ używanych wartości.
    typedef unsigned value_type;

    //! Wartość.
    value_type value;
    //! Ostatnia użyta wartość.
    static value_type lastValue;
    //! Wskaźnik na kontener wartości.
    static std::vector<value_type> * pValues;
    //! Wartość "zerująca"
    static const value_type initValue = value_type(-1);

    //! Inkrementacja względem poprzedniej wartości.
    __EnumInitHelper() { init(lastValue+1); }
    //! Jawnie podana wartość.
    __EnumInitHelper(value_type val) { init(val); }
    //! Kopia.
    __EnumInitHelper(const __EnumInitHelper& val) { init(val.value); }
    //! Rzutowanie na typ wartości.
    operator value_type() { return value; }

    //! Metoda inicjująca.
    void init(value_type val)
    {
        lastValue = value = val;
        pValues->push_back(value);
    }
};

//------------------------------------------------------------------------------

//! Klasa zawierająca informacje odnośnie wyliczeń. Singleton.
template <class T>
class __EnumData
{
public:
    //! Tym wyliczenia.
    typedef T EnumType;
    //! Typ słownika z kluczem będącym wartością.
    typedef std::multimap<EnumType, std::string> DictValues;
    //! Typ słownika z kluczem będącym łańcuchem
    typedef std::map<std::string, EnumType> DictNames;

    //! Typ rozmiaru (nazwa w stylu STLa)
    typedef typename DictValues::size_type size_type;
    //!
    typedef typename DictValues::value_type value_type;

public:
    //! Nazwy enumów.
    DictValues byValues;
    //! Wartości enumów.
    DictNames byNames;

public:
    //! \param Wartość dla której zwrócić nazwę.
    //! \return Nazwa.
    //! \throws std::invalid_argument
    const std::string& getName(EnumType value) const
    {
        typename DictValues::const_iterator it = byValues.find(value);
        if ( it != byValues.end() ) {
            return it->second;
        } else {
            std::ostringstream msg;
            msg << "Value not found: " << value;
            throw std::invalid_argument(msg.str());
        }
    }

    //! \param Wartość dla której zwrócić nazwę.
    //! \return Nazwa.
    //! \throws std::invalid_argument
    EnumType parse(const std::string& name) const
    {
        typename DictNames::const_iterator it = byNames.find(name);
        if ( it != byNames.end() ) {
            return it->second;
        } else {
            std::ostringstream msg;
            msg << "Unable to parse: " << name;
            throw std::invalid_argument(msg.str());
        }
    }

    //! \return Liczba enumów.
    size_type size() const
    {
        return byNames.size();
    }

    //! Parsuje łańcuch.
    //! \param values Wartości wyliczeń.
    //! \param input Łańcuch reprezentujący wyliczenie.
    //! \return true jeśli się udało.
    bool init(const std::vector<__EnumInitHelper::value_type>& values, std::string input)
    {
//         // usuwamy komentarze (w stylu C) oraz białe znaki
//         boost::regex removeComments("/\\*(.|[\r\n])*?\\*/|[\\s\n\r]");
//         input = boost::regex_replace(input, removeComments, std::string(""));
// 
//         // teraz szukamy poszczególnych wartości
//         boost::regex enrtySearch("(\\w+).*?(,|$)");
// 
//         boost::sregex_iterator it(input.begin(), input.end(), enrtySearch);
//         boost::sregex_iterator end;
//         for (size_t i = 0; it != end; ++it, ++i) {
//             // pobieramy nazwę i wartoćś
//             const std::string & name = (*it)[1];
//             EnumType value = static_cast<EnumType>(values[i]);
//             // uzupełniamy słowniki
//             byNames[name] = value;
//             byValues.insert( std::make_pair(value, name) );
//         }
//         return true;

        // odpowiednik wyszukiwania z użyciem klasycznych metod
        // usuwamy białe znaki
        std::string::iterator lastSign = std::remove_if(input.begin(), input.end(), isspace);
        input.resize( std::distance(input.begin(), lastSign) );

        // ustawiamy zmienne stanu
        size_t entryBegin = 0;
        size_t entryEnd = input.find_first_of(',');
        size_t equalPos = input.find_first_of('=');
        size_t entryNo = 0;

        while (entryEnd != std::string::npos) {
            // koniec tokenu
            size_t nameEnd = entryEnd;
            if ( equalPos != std::string::npos && equalPos < entryEnd ) {
                // przechodzimy na kolejny znak równości
                nameEnd = equalPos;
                equalPos = input.find_first_of('=', equalPos + 1);
            }
            // pobieramy token
            std::string name = input.substr(entryBegin, nameEnd - entryBegin);
            EnumType value = static_cast<EnumType>(values[entryNo]);
            byNames[name] = value;
			byValues.insert({ value, name });
            
            // przechodzimy na następny wpis
            entryBegin = entryEnd + 1;

            // sprawdzamy, czy to nie przypadkiem koniec łańcucha
            if ( entryEnd != input.size() ) {
                entryEnd = input.find_first_of(",\0", entryBegin);
                if ( entryEnd == std::string::npos ) {
                    entryEnd = input.size();
                }
            } else {
                entryEnd = std::string::npos;
            }
            ++entryNo;
        }
        return true;
    }
};

//------------------------------------------------------------------------------

template <class T>
class Enum {
public:
    typedef __EnumInitializer<T> EnumInitializer;
    //! Typ wyliczenia.
    typedef T EnumType;
    //! Typ danych.
    typedef __EnumData<T> EnumData;
    //! Typ rozmiaru.
    typedef typename EnumData::size_type size_type;

public:
    //! Definicja stałego iteratora (w stylu STL).
    typedef typename EnumData::DictValues::const_iterator const_iterator;
    //! Definicja stałego odwrotnego iteratora    (w stylu STL).
    typedef typename EnumData::DictValues::const_reverse_iterator const_reverse_iterator;
    //! Definicja wartości interatorów (w stylu STL).
    typedef typename EnumData::DictValues::value_type value_type;

public:
    //! Same metody statyczne.
    Enum() {}

public:
    //! \param Wartość dla której zwrócić nazwę.
    //! \return Nazwa.
    //! \throws std::invalid_argument
    static EnumType parse(const std::string& name)
    {
        return getInitializer().data.parse(name);
    }
    //! \param Wartość dla której zwrócić nazwę.
    //! \return Nazwa.
    //! \throws std::invalid_argument
    static const std::string& getName(EnumType value)
    {
        return getInitializer().data.getName(value);
    }
    //! \return Liczba enumów.
    static size_type size()
    {
        return getInitializer().data.size();
    }

    //! \return Iterator.
    const_iterator begin()
    {
        return getInitializer().data.byValues.begin();
    }
    //! \return Iterator.
    const_iterator end()
    {
        return getInitializer().data.byValues.end();
    }
    //! \return Iterator.
    const_reverse_iterator rbegin()
    {
        return const_reverse_iterator(end());
    }
    //! \return Iterator.
    const_reverse_iterator rend()
    {
        return const_reverse_iterator(begin());
    }

private:
    static EnumInitializer& getInitializer()
    {
        static EnumInitializer initializer = EnumInitializer();
        return initializer;
    }
};

////////////////////////////////////////////////////////////////////////////////
} // namespace utils
////////////////////////////////////////////////////////////////////////////////

#endif  // __MOTIONUTIL_ENUM_H__
