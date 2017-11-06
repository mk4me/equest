#ifndef HEADER_GUARD__DFINTERFACE_H__
#define HEADER_GUARD__DFINTERFACE_H__

////////////////////////////////////////////////////////////////////////////////
namespace dflm{
////////////////////////////////////////////////////////////////////////////////

//! Klasa wprowadzająca obsługe przepływu danych w data flow.
class DFInterface
{
public:

    //! Konstruktor domyślny
    //! \param enable Czy obiekt moze przetwarzać dane podczas notyfikacji
	DFInterface(bool enable = true);

    //! Wirtualny destruktor
	virtual ~DFInterface(void);

    //! \param enable Czy obiekt moze przetwarzać dane w momencie notyfikacji
    //! false blokuje przetwarzanie danych
    void setEnable(bool enable);

    //! \return Czy obiekt moze przetwarzać informacje
	bool isEnable() const;

    //! Metoda resetująca stan obiektu - wywoływana po każdym pełnym cyklu danych
	virtual void reset();

    //! Metoda informująca o zmianie otoczenia obiektu - obiekt sam musi zweryfikować zmianę i na nią zareagować
	void notify();

protected:

    //! Metoda czysto wirtualna - tutaj obiekt implementuje przetwarzanie danych
	virtual void process() = 0;

private:

    //! Czy obiekt może przetwarzać dane
	bool objectEnable;
};

}

#endif
