/********************************************************************
    created:  2013/03/25
    created:  25:3:2013   14:43
    filename: ConverterManager.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___CONVERTERMANAGER_H__
#define HEADER_GUARD___CONVERTERMANAGER_H__

#include <corelib/IConverterManager.h>

namespace core {

	//! Implementacja managera konwerterów w aplikacji
	class ConverterManager : public plugin::IConverterManager
	{
	private:
		//! Typ mapuj¹cy typy danych do indeksów na których te typy wystêpuj¹
		typedef std::map<utils::TypeInfo, std::set<int>> DataTypeToIndex;

		//! Struktura opisuj¹ca konwerter
		struct ConverterData
		{
			//! Sumaryczny rozmiar wejœcia (ile danych faktycznie potrzeba by konwertowaæ)
			unsigned int inputSize;
			//! Mapowanie typów danych wyjsciowych do ich indeksów
			DataTypeToIndex outputTypesIndexes;
			//! Mapowanie typów danych wejsciowych do ich indeksów
			DataTypeToIndex inputTypesIndexes;
			//! Zbiór typów wejœciowych
			utils::TypeInfoSet inputTypes;
			//! Zbiór typów wyjœciowych
			utils::TypeInfoSet outputTypes;
		};

		//! Mapa konwerterów i ich opisu
		typedef std::map<plugin::IConverterPtr, ConverterData> ConvertersDescription;

		//! Mapa typów do zwi¹zanych z nimi konwerterów
		typedef std::map<utils::TypeInfo, Converters> ConvertersByTypes;

		//! Mapowanie danych wg typow
		typedef std::map<utils::TypeInfo, utils::ConstObjectsList> DataByTypes;

	private:
		//! Kolekcja zarejestrowanych parserów
		ConvertersDescription converters_;
		//! Mapa konwerterów wg przyjmowanych wejœæ
		ConvertersByTypes convertersByInputs_;
		//! Mapa konwerterów wg oferownaych wyjœæ
		ConvertersByTypes convertersByOutputs_;

	private:

		//! \param converter Konwerter dla którego przygotowujemy opis
		//! \param data [out] Opis konwertera który wype³niamy
		static void fillConverterData(const plugin::IConverterPtr & converter, ConverterData & data);

		//! \param first Para typ -> zbiór wejœæ z typem
		//! \param first Para typ -> zbiór wejœæ z typem
		//! \return Prawda jeœli typy i ich iloœc siê zgadzaj¹
		static const bool compareInputs(const DataTypeToIndex::value_type & first, const DataTypeToIndex::value_type & second);

		//! \param converter Konwerter którego chcemy u¿yæ
		//! \param data Dane do dopasowania
		//! \param conversionData [out] Dane gotowe do zasilenia konwertera
		static void generateConverterInputs(const ConverterData & converterDescription, const utils::ConstObjectsList & data, std::list<utils::ConstObjectsVector> & conversionData);

		//! \param input Dane pogrupowane wg typow obslugiwanych przez wejscia
		//! \param conversionData [out] dane do konwersji
		static void generateConverterInputs(const DataByTypes & input, std::list<utils::ConstObjectsVector> & conversionData);

	public:

		ConverterManager();

		//! Wirtualny destruktor
		virtual ~ConverterManager();

		//! \param input Dane wejœciowe do konwersji
		//! \param output [out] Wyniki konwersji	
		//! \param recursive Czy wyniki maj¹ byæ generowane rekurencyjnie równie¿ na bazie wyników z poprzednich iteracji
		virtual void convert(const utils::ConstObjectsList & input, utils::ConstObjectsList & output, bool recursive = false) const;

		//! \param input Dane wejœciowe do konwersji
		//! \param output [out] Wyniki konwersji	
		//! \param requestedTypes Oczekiwane typy wyjœciowe
		//! \param recursive Czy wyniki maj¹ byæ generowane rekurencyjnie równie¿ na bazie wyników z poprzednich iteracji
		virtual void convert(const utils::ConstObjectsList & input, utils::ConstObjectsList & output, const utils::TypeInfoSet & requestedTypes, bool recursive = false) const;

		//! \param converters [out] Wszystkie zarejestrowane konwertery danych domenowych
		virtual void converters(Converters & converters) const;

		//! \param inTypes Typy danych wejœciowych które chcemy konwertowaæ
		//! \param converters [out] Zarejestrowane konwertery danych domenowych które potrafi¹ operowaæ na danym zbiorze i jego podzbiorach
		virtual void convertersForInputs(const utils::TypeInfoList & inTypes, plugin::IConverterManager::Converters & converters) const;
		//! \param inTypes Typy danych wejœciowych które chcemy uzyskac po konwersji
		//! \param converters [out] Zarejestrowane konwertery danych domenowych które potrafi¹ generowaæ dane zadanego typu
		virtual void convertersWithOutputs(const utils::TypeInfoSet & outTypes, plugin::IConverterManager::Converters & converters) const;

		//! \param converter Konwerter do zarejestrowania
		void registerConverter(const plugin::IConverterPtr & converter);
	};

}

#endif	//	HEADER_GUARD___CONVERTERMANAGER_H__
