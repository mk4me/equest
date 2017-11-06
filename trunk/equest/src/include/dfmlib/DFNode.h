#ifndef HEADER_GUARD__DFNODE_H__
#define HEADER_GUARD__DFNODE_H__

#include <functional>
#include <dfmlib/Node.h>
#include <dfmlib/DFInterface.h>

////////////////////////////////////////////////////////////////////////////////
namespace dflm{
////////////////////////////////////////////////////////////////////////////////

class DFModel;

//! Klasa reprezentująca węzeł z możliwością przetwarzania i przekazywania danych na bazie DFPinów w DFModel
class DFNode :
	public Node, public DFInterface
{

    friend class DFModel;

private:

    typedef std::function<void(void)> OnLeafProcessedCallback;

public:

    //! \param nodeName Nazwa węzła
    //! \param processingAllowed Czy przetwarzanie danych aktywne
    //! \param propagatingAllowed Czy propagowanie danych aktywne
    DFNode(const std::string & nodeName = std::string()/*, bool processingAllowed = true,
        bool propagatingAllowed = true*/);

    //! Wirtualny destruktor
	virtual ~DFNode(void);

    //! \param allow Zezwala/wylancza przetwarzanie danych
	void allowProcessing(bool allow);

    //! \param allow Zezwala/wylancza propagowanie danych dalej
	void allowPropagating(bool allow);

    //! \return Czy przetwarzanie jest dozwolone
	bool isProcessingAllowed() const;

    //! \return Czy propagowanie danych dalej jest dozwolone
	bool isPropagatingAllowed() const;

    //! \param node Węzeł podstawowy do "podniesienia" na DFNode
    //! \return DFNode dla danego węzła jeśli jest jego pochodną lub nullptr jeśli konwersja niemożliwa
	static DFNPtr getDFNode(const NPtr & node);

protected:

    //! ---------------- Interfejs DFInterface ---------------------------

	virtual void process();

    //! ---------------- DFNode -------------------------------------------

    //! \return Czy piny wejściowe mają załadowane dane poprawnie
	virtual bool checkInputPins() const;

    //! Aktualizuje status pinów wyjściowych - notify()
	virtual void updateOutputPins();

    //! Ustawia dane pinów wyjściowych po przetworzeniu danych
    virtual void setOutputData();

    //! Przetwarza dane właściwe
    virtual void processData();

private:

    //! Czy przetwarzanie danych aktywne
	bool processingAllowed;

    //! Czy Propagowanie danych aktywne
	bool propagatingAllowed;

    //! Akcja wywoływana na rzecz modelu przez węzeł liść
    OnLeafProcessedCallback onLeafProcessedCallback;
};

}

#endif
