#ifndef HEADER_GUARD__NODE_H__
#define HEADER_GUARD__NODE_H__

#include <dfmlib/DFLMTypes.h>
#include <utils/SmartPtr.h>
#include <set>
#include <vector>
#include <stdexcept>

////////////////////////////////////////////////////////////////////////////////
namespace dflm{
	////////////////////////////////////////////////////////////////////////////////

	//class EditableNode;

	//! Klasa reprezentująca wezeł w modelu data flow. Zarządza on pinami wejściowymi i wyjściowymi oraz
	class Node : public utils::enable_shared_from_this<Node>
	{
	public:
		//! Zaprzyjaźniona klasa modelu
		friend class Model;

		//! Jeśli węzeł zezwala na edycję
		//friend class EditableNode;

	public:
		//! Typ opisujący zbiór pinów
		//typedef std::set<PinPtr> Pins;
		typedef std::vector<PinPtr> Pins;

		//! Typ itertatora do kolekcji pinów
		typedef Pins::const_iterator iterator;

		//! Typ rozmiaru kolekcji pinów
		typedef Pins::size_type size_type;

	protected:
		//! Interfejs klienta podczas inicjalizacji węzła, do dodawania pinów (klient sam powienien trzymać wewnętrznie wskaźniki do odpowiednich pinów)
		class IPinsAdder
		{
		public:
			//! \param pin Pin do dodania do pinów wejśicowych
			virtual void addInPin(const PinPtr & pin) = 0;

			//! \pin Pin do dodania do pinów wyjściowych
			virtual void addOutPin(const PinPtr & pin) = 0;
		};

		//! Interfejs służący do usuwania pinów z węzła
		class IPinsDeleter
		{
		public:
			//! \param pin Pin do usunięcia z pinów wejśicowych
			virtual void removeInPin(const PinPtr & pin) = 0;

			//! \param pin Pin do usunięcia z pinów wyjściowych
			virtual void removeOutPin(const PinPtr & pin) = 0;
		};

		//! Wskaźnik inteligętny do interfejsu dodającego piny do węzła
		typedef utils::shared_ptr<IPinsAdder> PinsAdderPtr;

		//! Wskaźnik inteligętny do interfejsu usuwającego piny z węzła
		typedef utils::shared_ptr<IPinsDeleter> PinsDeleterPtr;

	private:

		//! Prywatna, bazowa implementacja interfejsu dodającego piny do węzła używana na potrzeby inicjalizacji węzła
		class PinsAdder : public IPinsAdder
		{
		public:
			//! \param node Węzeł do którego dodajemy piny
			PinsAdder(Node * node) : node(node) {}
			~PinsAdder() {}

			//! Przekierowuje wywołanie do prywatenej metody węzła
			virtual void addInPin(const PinPtr & pin)
			{
				if (node->initialized == true){
					throw std::runtime_error("Node was already initialized!");
				}
				node->addInPin(pin);
			}

			//! Przekierowuje wywołanie do prywatenej metody węzła
			virtual void addOutPin(const PinPtr & pin)
			{
				if (node->initialized == true){
					throw std::runtime_error("Node was already initialized!");
				}
				node->addOutPin(pin);
			}

		private:
			//! Węzeł na którym dzxiałamy
			Node * node;
		};

		//! Zaprzyjaźnienie aby PinAdder miał dostęp do prywatnych metod węzła
		friend class PinsAdder;

	public:

		//! \param name nazwa węzła
		Node(const std::string & name = std::string());

		//! Wirtualny destruktor
		virtual ~Node(void);

		//! \return Czy węzeł został zainicjalizowany
		bool isInitialized() const;

		MPtr getModel() const;

		//! \return Iterator początku kolekcji pinów wejściowych
		iterator beginIn() const;

		//! \return Iterator końca kolekcji pinów wejściowych
		iterator endIn() const;

		//! \param idx Indeks pinu wejściowego, zgłasza wyjątek jeśli niepoprawny index
		//! \return Pin wejściowy spod wskazanego indeksu
		const PinPtr & getInPin(int idx) const;

		//! \param pin Pin wejściowy którego indeks chcemy dostać, potrzebne w przypadku zarządzania pinami - indeksy mogą się zmieniać!
		//! \return Indeks pinu wejściowego lub -1 jeśli pin wejściowy nie istnieje
		int indexOfInPin(const PinPtr & pin) const;

		//! \return Ilość pinów wejściowych
		size_type sizeIn() const;

		//! \return Iterator początku kolekcji pinów wyjsciowych
		iterator beginOut() const;

		//! \return Iterator końca kolekcji pinów wyjsciowych
		iterator endOut() const;

		//! \param idx Indeks pinu wyjsciowego, zgłasza wyjątek jeśli niepoprawny index
		//! \return Pin wyjściowy spod wskazanego indeksu
		const PinPtr & getOutPin(int idx) const;

		//! \param pin Pin wyjściowy którego indeks chcemy dostać, potrzebne w przypadku zarządzania pinami - indeksy mogą się zmieniać!
		//! \return Indeks pinu wyjściowego lub -1 jeśli pin wyjściowy nie istnieje
		int indexOfOutPin(const PinPtr & pin) const;

		//! \return Ilość pinów wyjściowych
		size_type sizeOut() const;

		//! \param node Węzeł któremu sprawdza się poprawność połączeń pinów wejściowych
		//! \return Prawda jeśli połączenia pinów wejściowych są poprawne
		static bool validInPinsConnection(const CNPtr & node);

		//! \param node Węzeł któremu sprawdza się poprawność połączeń pinów wyjściowych
		//! \return Prawda jeśli połączenia pinów wyjsciowych są poprawne
		static bool validOutPinsConnection(const CNPtr & node);

		//! \param node Węzeł któremu sprawdza się poprawność podłączeń
		//! \return Prawda jeśli węzeł jest podłączony poprawnie
		static bool validConnection(const CNPtr & node);

		//! \param node Węzeł któremu sprawdza się czy jakikolwiek pin wejściowy został podłączony
		//! \return Prawda jeśli conajmniej jeden pin wejściowy jest podłączony
		static bool anyInPinConnected(const CNPtr & node);

		//! \param node Węzeł któremu sprawdza się czy jakikolwiek pin wyjściowy został podłączony
		//! \return Prawda jeśli conajmniej jeden pin wyjsciowy jest podłączony
		static bool anyOutPinConnected(const CNPtr & node);

		//! \param node Węzeł któremu sprawdza się czy jakikolwiek pin został podłączony
		//! \return Prawda jeśli conajmniej jeden pin jest podłączony
		static bool anyPinConnected(const CNPtr & node);

		//! \return Nazwa węzła
		const std::string & getName() const;

		//! Metoda pozwalająca podpiąć metody konfiguracji węzła
		virtual void configure();

	protected:

		//! Inicjalizacja do implementacji przez klienta
		virtual void doInitialization(const PinsAdderPtr & pinsAdder);

		//! Resetuje węzeł - do implementacji przez klienta
		//virtual void onReset();

	private:

		//! \param name Nowa nazwa węzła
		void setName(const std::string & name);

		//! \param pin Pin do dodania do pinów wejściowych
		virtual void addInPin(const PinPtr & pin);

		//! \param pin Pin do dodania do pinów wyjsciowych
		virtual void addOutPin(const PinPtr & pin);

		//! Inicjalizacja
		void initialize();

		//! \param pins Zbiór pinów wśród ktróych poszukujemy zadanego pinu
		//! \param pin Pin którego indeksu poszukujemy w zadanym zbiorze
		//! \return Indeks pinu w zadanym zbiorze lub -1 jeśli pinu nie znaleziono
		static int indexOfPin(const Pins & pins, const PinPtr & pin);

		//! \param pins Zbiór pinów do sprawdzenia
		//! \return Prawda jeśli przynajmniej jeden pin jest podłączony
		static bool anyPinConnected(const Pins & pins);

	private:
		//! Zbiór pinów wejściowych
		Pins inPins;

		//! Zbiór pinów wyjściowych
		Pins outPins;

		//! Nazwa węzła
		std::string name;

		//! Czy węzeł zainicjalizowano
		bool initialized;

		//! Model do którego należy węzeł
		WMPtr model;

		//! Obiekt na potrzby inicjalizacji węzła
		PinsAdderPtr pinsAdder;
	};

	//class EditableNode : public Node
	//{
	//
	//public:
	//
	//    EditableNode(const MPtr & model);
	//    ~EditableNode();
	//
	//    MPtr getModel() const;
	//
	//protected:
	//
	//    virtual void onInPinRemove(const PinPtr & pin);
	//
	//    virtual void onInPinAdd(const PinPtr & pin);
	//
	//    virtual void onOutPinRemove(const PinPtr & pin);
	//
	//    virtual void onOutPinAdd(const PinPtr & pin);
	//
	//private:
	//    //! \param newPin Pin do dodania do pinów wejściowych
	//    void addInPin(const PinPtr & pin)
	//    {
	//        addPin(inPins, pin);
	//        onInPinAdd(pin);
	//    }
	//
	//    //! \param newPin Pin do dodania do pinów wyjsciowych
	//    void addOutPin(const PinPtr & pin)
	//    {
	//        addPin(outPins, pin);
	//        onOutPinAdd(pin);
	//    }
	//
	//    //! \param newPin Pin do dodania do pinów wejściowych
	//    void removeInPin(const PinPtr & pin)
	//    {
	//        removePin(inPins, pin);
	//        onInPinRemove(pin);
	//    }
	//
	//    //! \param newPin Pin do dodania do pinów wyjsciowych
	//    void removeOutPin(const PinPtr & pin)
	//    {
	//        removePin(outPins, pin);
	//        onOutPinRemove(pin);
	//    }
	//
	//    static void addPin(Node::Pins & pins, const PinPtr & pin)
	//    {
	//        if(std::find(pins.begin(), pins.end(), pin) != pins.end()){
	//            throw std::runtime_error("Pin already exists");
	//        }
	//
	//        pins.push_back(pin);
	//    }
	//
	//    static void removePin(Node::Pins & pins, const PinPtr & pin)
	//    {
	//        auto it = std::find(pins.begin(), pins.end(), pin);
	//        if(it == pins.end()){
	//            throw std::runtime_error("Pin not exists");
	//        }
	//
	//        pins.erase(it);
	//    }
	//
	//private:
	//
	//    WMPtr model;
	//
	//};
}

#endif
