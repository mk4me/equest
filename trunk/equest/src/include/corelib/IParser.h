#ifndef HEADER_GUARD_CORE_IPARSER_H__
#define HEADER_GUARD_CORE_IPARSER_H__

#include <map>
#include <corelib/Variant.h>
#include <corelib/IIdentifiable.h>
#include <utils/SmartPtr.h>

namespace plugin
{
    //! Parser danych. Parser stanowi pomost pomiędzy plikiem a odwzorowaniem pliku
    //! w programie. Cykl na cykl życia parsera składa się stworzenie (metodą clone),
    //! przypisanie do pliku (parse), odwołania do obiektu danych (getObject),
    //! zniszczenie (destruktor). W tym cyklu nigdy nie następują zmiany, poza sytuacją,
    //! gdy parser z jakiegoś powodu po utworzeniu nie jest przypisany do pliku, tylko
    //! od razu niszczony.
	//! Musi dziedziczyć po ISourceParserCapabilities lub IStreamParserCapabilities
    class IParser : public ICoreElement
    {
	public:
		//! Typ opisujący wyrażenie
		struct ExpressionDescription
		{
			std::string description;			//! Opis wyrazenia regularnego opisującego dozwolone źródła danych
			utils::TypeInfoVector objectsTypes;	//! Typy możliwe do uzyskania wg pozycji dla tego źródła
		};

		//! Słownik rozszerzeń wpsieranych przez parser z ich opisem
		typedef std::map<std::string, ExpressionDescription> Expressions;
		//! Indeksy obiektów które chcemy by parser wyciągnął
		typedef std::set<utils::TypeInfoVector::size_type> ObjectsIndexes;

	public:
		//! Destruktor wirtualny
		virtual ~IParser() {}
		//! \retrun Lista rozszerzeń, które parser obsługuje. Muszą być oddzielone średnikiem.
		//! Obsługiwany format rozszerzenia: [ { *. | . } ]ext
		virtual void acceptedExpressions(Expressions & expressions) const = 0;
        //! \return Instancja tego samego typu.
        virtual IParser* create() const = 0;
		//! \param object [out] Obiekty danych parsera w kolejności jak w opisie ExpressionDescription::types dla danego źródła!!
		//! \param idx Indeks obiektu wg opisu dla danego źródła, który chcemy pobrać
		virtual void getObject(core::Variant& object, const core::VariantsVector::size_type idx) const = 0;
		//! Metoda zwalniająca sparsowane zasoby parsera
		virtual void reset() = 0;
    };

	DEFINE_SMART_POINTERS(IParser);    

	//! Interfejs parsera obsługującego we własnym zakresie I/O na podstawie ścieżki źródła
	//! Po tej klasie musi dziedziczyć IParser jeśli wspiera taką funkcjonalność
	class ISourceParser : public virtual IParser
	{
	public:
		//! Destruktor wirtualny
		virtual ~ISourceParser() {}
		//! Przyporządkowanie parsera do konkretnego pliku.
        //! Na każdej instancji parsera ta metoda wywoływana jest maksymalnie jeden raz.
        //! \param source Ścieżka do źródła danych - plik, url, id urządzenia, ...
        virtual void parse(const std::string & source) = 0;
	};

	DEFINE_SMART_POINTERS(ISourceParser);

	//! Interfejs parsera z własnym I/O potrafiącego selektywnie wyciągać dane z pliku, nie wszystkie za jednym razem
	class IOptimizedSourceParser : public ISourceParser
	{
	public:
		//! \param source Ścieżka do źródła danych - plik, url, id urządzenia, ...
		//! \param objectsIndexes Jakie typy obiektów wg opisu dla danego źródła parser powinien wydobyć
		virtual void parse(const std::string & source, const IParser::ObjectsIndexes & objectsIndexes) = 0;
	};

	DEFINE_SMART_POINTERS(IOptimizedSourceParser);

	//! Interfejs parsera działąjącego w oparciu o strumienie STL i ścieżkę opisującą typ danych w strumieniu
	//! np. pamięć współdzielona pomiędzy procesami, dostęp do ukrytych zasobów (SQLCIPHER) implementujących strumienie
	//! Po tej klasie musi dziedziczyć IParser jeśli wspiera taką funkcjonalność
	class IStreamParser : public virtual IParser
	{
	public:
		//! Wskaźnik strumienia
		typedef utils::shared_ptr<std::istream> IStreamPtr;

	public:
		//! Destruktor wirtualny
		virtual ~IStreamParser() {}
		//! \param stream Strumień wejściowy z którego dostarczamy danych
		//! \param source Ścieżka źródła identyfikująca zawartość strumienia - np. rozszerzenie, protokół, adres
		virtual void parse(const IStreamPtr stream, const std::string & source) = 0;
	};

	DEFINE_SMART_POINTERS(IStreamParser);

	//! Interfejs parsera strumieni potrafiącego selektywnie wyciągać dane ze strumienia, nie wszystkie za jednym razem
	class IOptimizedStreamParser : public IStreamParser
	{
	public:
		//! \param stream Strumień wejściowy z którego dostarczamy danych
		//! \param source Ścieżka do źródła danych - plik, url, id urządzenia, ...
		//! \param objectsIndexes Jakie typy obiektów wg opisu dla danego źródła parser powinien wydobyć
		virtual void parse(const IStreamPtr stream, const std::string & source, const ObjectsIndexes & objectsIndexes) = 0;
	};

	DEFINE_SMART_POINTERS(IOptimizedStreamParser);
}

#endif
