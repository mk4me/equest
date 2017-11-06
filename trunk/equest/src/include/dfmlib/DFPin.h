#ifndef HEADER_GUARD__DFPIN_H__
#define HEADER_GUARD__DFPIN_H__

#include <dfmlib/Pin.h>

////////////////////////////////////////////////////////////////////////////////
namespace dflm {
////////////////////////////////////////////////////////////////////////////////

//! Klasa pinów świadomych istnienia danych i wymiany tych danych. Działają z węzłami świadomymi danych - DFNode
class DFPin : public Pin
{
public:

    //! \param pinName nazwa pinu
    //! \param required Czy pin jest wymagany? (dotyczy tylko pinów wejściowych)
    //! \param requiredPins Zbiór pinów od których jest uzależnione występowanie informacji na danym pinie (dotyczy pinów wyjściowych)
    DFPin(const std::string & pinName = std::string(), bool required = false,
        const ReqPinsSet & requiredPins = ReqPinsSet());

    //! Wirtualny destruktor
	virtual ~DFPin(void);

    //! \return Czy pin został zaktualizowany ( załadowano mu dane )
	bool isUpdated() const;

    //! Resetuje stan update pinu po przekazaniu danych (węzeł wyjściowy - do innego pina / węzeł wejściowy - do węzła przetwarzającego)
	void reset();

    //! \return Czy dane w pinie są gotowe do odczytu
	void update();

    //! \param pin Pin do sprawdzenia czy jest typu DFPin
    //! \return Prawda jeśli pin jest typu DFPin, inaczej false
	static bool isDFPin(const CPinPtr & pin);

    //! \param pin Pin do konwertowania do typu DFPin
    //! \return Wskaźnik do pinu typu DFPin lub nullptr (pusty wskaźnik)
	static DFPinPtr getDFPin(const PinPtr & pin);

    //! \param pin Pin do konwertowania do typu DFPin
    //! \return Wskaźnik do pinu typu const DFPin lub nullptr (pusty wskaźnik)
    static CDFPinPtr getDFPin(const CPinPtr & pin);

protected:

    //! Metoda odpalana przy każdym wywołaniu update()
	virtual void onUpdate();

    virtual void onReset();

    //! Metoda pozwalająca kopiować dane pomiędzy węzłami - kopiowanie inicjuje węzeł wyjsciowy i wywołuje tą metodę na węźle wejściowym
    //! Mechanizm ten przenosi odpowiedzialność kopiowania danych na węzeł wejściowy który może miesz szerszą funkcjonalność (kompatybilność typów) niż węzeł wyjściowy
    //! \param pin Pin z którego węzeł wejściowy powinien skopiować dane
	virtual void copyDataFromPin(const DFPinPtr & pin);

private:

    //! Czy węzeł został zaktualizowany - załadowano nowe dane
	bool updated;
};

}

#endif
