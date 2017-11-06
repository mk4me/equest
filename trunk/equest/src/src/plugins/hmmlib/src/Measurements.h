/********************************************************************
	created:	2012/03/30
	created:	30:3:2012   12:11
	filename: 	Measurements.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_HMM__MEASUREMENTS_H__
#define HEADER_GUARD_HMM__MEASUREMENTS_H__

#include <QtCore/QString>
#include <map>
#include <vector>
#include <utils/SmartPtr.h>
#include "tinyxml2.h"
//#include <boost/bimap.hpp>
#include <tinyxml2.h>

//class tinyxml2::XMLElement;

//! W klasie przechowywana jest konkretna konfiguracja EMG
class MeasurementConfig
{
    friend class MeasurementsParser;

public:
	//! 
	MeasurementConfig();
    //! 
    virtual ~MeasurementConfig() {}

public:
    //! tlumaczenie wpisu w podobny sposób, jak to robi Qt
    //! \param text tekst do przetlumaczenia
    //! \return przetlumaczony tekst lub tekst wejsciowy, jeśli nie znaleziono wpisu
    QString tr(const QString& text) const;
    //! czy konfigurator ma wpis
    //! \param text wpis do sprawdzenia
    //! \return true, jeśli znaleziono
    bool hasEntry(const QString& text) const;
    //! pobiera id (identyfikator miesnia, potrzebny np. w graficznym konfiguratorze EMG)
    //! \param name nieprzetlumaczona nazwa, dla którego pobieramy id
    //! \return znalezione id lub pusy string
    QString getIdByName(const QString& name) const;
    //! \return nazwa konfigu
    const QString& getName() const { return name; }
    //! \return unikalny numer konfigu
    int getNumber() const { return number; }

private:
    //! nazwa obrazka, unikalny identyfikator elementu
    struct EntryData { QString name; QString id; };
    //! mapa [element konfiguracji (np. elektroda) -> wpis z odpowiadającą nazwą + ew. nazwą obrazka]
    typedef std::pair<QString, EntryData> MapEntry;
	
private:
    //! nazwa konfigu
    QString name;
    //! unikalny numer konfigu
    int number;
    //! mapa z wpisami
    std::map<QString, EntryData> entry2Data;
};
typedef utils::shared_ptr<MeasurementConfig> MeasurementConfigPtr;
typedef utils::shared_ptr<const MeasurementConfig> MeasurementConfigConstPtr;

//! klasa przechowuje wszystkie konfigi oraz ułatwia dostęp do nich
class Measurements
{
public:
    static utils::shared_ptr<const Measurements> get();
public:
    Measurements() {}
    virtual ~Measurements() {}
   
public:
    //! dodaje konfig do kolekcji
    //! \param config dodawany konfig EMG
    void addConfig(MeasurementConfigPtr config);
    //! pobiera konfig na podstawie jego nazwy
    //! \param name nazwa konfiga
    //! \return konfig lub wyjątek, gdy nie znaleziono
    MeasurementConfigConstPtr getConfig(const QString& name) const;
    //! pobiera konfig na podstawie jego numeru
    //! \param number numer konfiga
    //! \return konfig lub wyjątek, gdy nie znaleziono
    MeasurementConfigConstPtr getConfig(int number) const;
    //! probuje pobrać konfig na podstawie jego nazwy
    //! \param name 
    MeasurementConfigConstPtr tryGetConfig(const QString& name) const;
    //! probuje pobrać konfig na podstawie jego numeru
    //! \param number 
    MeasurementConfigConstPtr tryGetConfig(int number) const;
    //! czy dodano wpis o podanej nazwie
    //! \param name 
    bool hasConfig(const QString& name) const;
    //! czy dodano wpis o podanym numerze
    //! \param number 
    bool hasConfig(int number) const;

private:
    //! konfiguracja po jej numerze
    std::map<int, MeasurementConfigPtr> configsByNumber;
    //! konfiguracja po jej nazwie
    std::map<QString, MeasurementConfigPtr> configsByName;

private:
    //! kolekcja z konfiguracjami pomiarowymi
    static utils::shared_ptr<const Measurements> measurements;
};
typedef utils::shared_ptr<Measurements> MeasurementsPtr;
typedef utils::shared_ptr<const Measurements> MeasurementsConstPtr;


//! parser xml-a z konfigami EMG
class MeasurementsParser
{
public:
    MeasurementsParser();
	virtual ~MeasurementsParser() {}

public:
    //! parsowanie xmla z konfiguracjami
    //! \param filename 
    void parse(const std::string& filename);
    //! \return czy dane zostały już poprawnie zaladowane
    bool isLoaded() const { return loaded; }
    //! \return kolecja z konfigami EMG
    MeasurementsConstPtr getMeasurments();

private:
    //! Wczytuje caly konfig
    //! \param element 
    //! \param config 
    void readMeasurment(tinyxml2::XMLElement* element, MeasurementConfigPtr config );
    //! wczytuje wpisy konfigu
    //! \param A 
    //! \param entry 
    void readAttribute(tinyxml2::XMLElement* A, MeasurementConfig::MapEntry& entry); 

private:
    //! czy dane zostały już poprawnie zaladowane
    bool loaded;
    //! kolekcja z konfigami EMG
    MeasurementsPtr measurments;
};
typedef utils::shared_ptr<MeasurementsParser> MeasurementsParserPtr;
typedef utils::shared_ptr<const MeasurementsParser> MeasurementsParserConstPtr;


#endif
