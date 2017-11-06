#ifndef HEADER_GUARD__PIN_H__
#define HEADER_GUARD__PIN_H__

#include <dfmlib/DFLMTypes.h>
#include <set>
#include <vector>
#include <functional>
#include <utils/SmartPtr.h>

////////////////////////////////////////////////////////////////////////////////
namespace dflm {
////////////////////////////////////////////////////////////////////////////////

//! Klasa reprezentująca punkt łączenia węzłów. Rozróżniamy 2 typy pinów - wejściowe i wyjściowe.
class Pin : public utils::enable_shared_from_this<Pin>
{
public:

    //! Typ określający typ pinu
	enum PinType {
        IN, //! Pin wejściowy
        OUT, //! Pin wyjściowy
        UNSET //! Pin niezdefiniowany
    };

    //! Typ określający statyczny stan pinu
	enum StaticStatus {
        OK,         //! Z pinem wszystko OK, neutralny stan pinu niepodłączonego bez żadnych wymagań (zależności, wymaganie podłaczenia)
        REQUIRED,   //! Pin jest wymagany a nie jest podłączony (dotyczy pinów wejściowych)
        INCOMPLETE, //! Pin ma niespełnione zależności - jeden z pinów od których jest zależny nie jest podłączony (dotyczy pinów wyjściowych)
        CONNECTED   //! Pin poprawnie podłączony
    };

    //! Typ określający dynamiczny stan pinu podczas łączenia
	enum DynamicStatus {
        COMPATIBLE,         //! Pin kompatybilny - można łączyć
        COMPATIBLE_CYCLE,   //! Pin kompatybilny ale tworzy cykl - nie można łączyć
        COMPATIBLE_FULL,    //! Pin kompatybilny ale docelowy pin wejściowy już zajęty - nie można łączyć
        INCOMPATIBLE        //! Pin nmiekompatybilny - nie można łączyć
    };

    //! Typ kolekcji połączeń
	//typedef std::set<ConnPtr> Connections;
    typedef std::vector<ConnPtr> Connections;

    //! Typ kolekcji pionów wymaganych
	typedef std::set<WPinPtr> ReqPinsSet;

    //! Typ iteratora do kolekcji połączeń
    typedef Connections::const_iterator iterator;

    //! Typ ilości połączeń w kolekcji
    typedef Connections::size_type size_type;

private:

    //! Funktor zwracający indeks pinu
    typedef std::function<int(const PinPtr &)> PinIndexFunc;

public:

    //! Zaprzyjaźniony typ węzła - one zarządzają swoimi pinami i na ich bazie są łaczone
	friend class Node;

    //! Zaprzyjaźniony typ modelu - zarządza, weryfikuje stan modelu, reguły jego łączenia między innymi przez piny
	friend class Model;

public:

    //! Konstruktor domyślny
    //! \param name nazwa pinu
    //! \param required Czy pin wymagany (dotyczy tylko pinów wejściowych)
    //! \param requiredPins Piny od których jest zależny aktualny pin (dotyczy pinów wyjściowych)
	Pin(const std::string & name = std::string(),
		bool required = false, const ReqPinsSet & requiredPins = ReqPinsSet());
	
    //! Wirtualny destruktor
	virtual ~Pin(void);

    //! \return Wskaźnik do rodzica lub nullptr jeśli pin nie jest dodany do węzła
	NPtr getParent() const;

    //! \return Iterator początka kolekcji połączeń
    iterator begin() const;

    //! \return Iterator końca kolekcji połączeń
    iterator end() const;

    //! \param idx Indeks połączenia
    //! \return Zwraca połaczenie o zadanym indeksie
    const ConnPtr & getConnection(int idx) const;

    //! \param connection Połaczenie którego indeksu szukamy
    //! \return Indeks połączenia w kolekcji lub -1 jeśli połączenia nie ma w kolekcji
    int getIndexOfConnection(const ConnPtr & connection) const;

    //! \return Czy kolekcja połączeń jest pusta
    bool empty() const;

    //! \return Ilość połączeń w kolekcji
    size_type size() const;

    //! \return Typ pinu (IN|OUT)
	PinType getType() const;

    //! \return Statyczny status pinu
	StaticStatus getStaticStatus() const;

    //! \param refPin Pin względem którego badamy stan dynamiczny pinu (podczas łączenia)
    //! \param model Model w obrębie którego się poruszamy definiując status dynamiczny
    //! \return Status dynamiczny pinu przed próbą połączenia
	DynamicStatus getDynamicStatus(const PinPtr & refPin, const MPtr & model) const;

    //! \param pin Pin którego kompatybilnośc testujemy
    //! \return Prawda jeśli piny są kompatybilne (aktualny pin potrafi obsłużyć testowany pin)
	virtual bool isCompatible(const CPinPtr & pin) const;
	
    //! \param pin Pin którego testujemy czy aktualny pin jest od niego zależny (dotyczy pinów wyjsciowych)
    //! \returen Prawda jeśli aktualny pin jest zależny od zadanego pinu
	bool dependsOnPin(const PinPtr & pin) const;

    //! \return Zwraca zbiór pinów od których jest uzależniony aktualny pin
	const ReqPinsSet & getDependantPins() const;

    //! \return Czy dany pin jest wymagany (dotyczy pinów wejściowych)
	bool isRequired() const;

    //! \return Czy wszystkie zależności pinu są spełnione (dotyczy pinów wyjściowych)
	bool isComplete() const;

    //! \return Nazwa pinu
	const std::string & getName() const;

 //   //! \param Nazwa pinu
	//void setPinName(const std::string & name);

    //! \return Indeks pinu w węźle (-1 jeśli pin nigdzie nie dodany, automatycznie ropoznaje typ pinu
    int getPinIndex() const;

private:

    //! \param name Nowa nazwa pinu
    void setName(const std::string & name);

    //! \param connection Połączenie dodawane do pinu
    void addConnection(const ConnPtr & connection);

    //! \param connection Połączenie usuwane z pinu
    void removeConnection(const ConnPtr & connection);

    //! Usuwa wszystkie połączenia pinu
	void clearConnections();

    //! \param type Typ pinu ustawiany podczas dołanczania go do węzła
	void setType(PinType type);

private:

    //! Zbiór połączeń pinu
	Connections connections;

    //! Węzeł rodzic pinu
	WNPtr parentNode;

    //! Czy pin wymagany (dla pin ów wejściowych)
	bool pinRequired;

    //! Piny od których uzależniony jest pin (dla pinów wyjściowych)
	ReqPinsSet requiredPinsDependency;

    //! Typ pinu (wejściowy|wyjściowy)
	PinType pinType;

    //! Nazwa pinu
	std::string name;

    //! Funktor zwracający indeks pinu
     PinIndexFunc pinIndexFunc;
};

}

#endif
