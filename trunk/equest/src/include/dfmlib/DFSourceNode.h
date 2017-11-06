#ifndef HEADER_GUARD__DFSOURCENODE_H__
#define HEADER_GUARD__DFSOURCENODE_H__

#include <dfmlib/DFNode.h>

////////////////////////////////////////////////////////////////////////////////
namespace dflm{
////////////////////////////////////////////////////////////////////////////////

//! Klasa rozszerzająca węzeł DFNode o węzły źródłowe - nie mają one pinów wejściowych , oferują informację o dostepności kolejnych danych na wyjściach
class DFSourceNode :
	public virtual DFNode
{
public:

    //! \param name Nazwa węzła
    //! \param processingAllowed Czy przetwarzanie danych jest dozwolone
    //! \param propagatingAllowed Czy propagowanie danych jest dozwolone
    DFSourceNode(const std::string & name = std::string()/*, bool processingAllowed = true,
        bool propagatingAllowed = true*/);

    //! Wirtualny destruktor
	virtual ~DFSourceNode(void);

    //! Czysto wirtualna funkcja informująca o dostępności kolejnych danych w węźle
    //! \return Czy węzeł może zaoferować kolejną porcję danych
	virtual bool hasMoreData() const = 0;

protected:

    //! \return Zawsze true
	virtual bool checkInputPins() const;

    //! Nie robi nic
	virtual void resetInputPins();

private:

    //! Wywolanie tej metody zrzuca wyjątek - nie jest dozwolone dodawanie pinów wejściowych do źródeł
    //! \param newPin Pin wejściowy do dodania
    virtual void addInPin(const PinPtr & pin);
};

}

#endif
