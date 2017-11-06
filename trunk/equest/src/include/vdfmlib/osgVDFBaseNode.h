#ifndef HEADER_GUARD__VDFBASENODE_H__
#define HEADER_GUARD__VDFBASENODE_H__

#include <vdfmlib/osgVDFBasePin.h>
#include <osgWidget/Window>

////////////////////////////////////////////////////////////////////////////////
namespace osgVDF{
////////////////////////////////////////////////////////////////////////////////


    //! Klasa bazowa dla wizualnych reprezentacji węzłów logicznych z Data Flow.
class osgVDFBaseModel;

class osgVDFBaseNode : public osgWidget::Window
{
    friend class osgVDFBaseModel;

private:

    typedef std::set<osgVDFBaseNode*> VNodes;
    typedef std::vector<osgVDFBaseNode*> VNodesDifference;

public:
    //! Typ opisujący stan wizualny węzła w wyniku operacji na nim (wizualnych), wynikający z jego położenia
	enum VisualStatus {
        OK,         //! z węzłem wszystko ok
        COLLISION,  //! węzeł bierz eudział w kolizji - nachodzi się z innym węzęłm/ami
        ACTIVE      //! węzeł zaznaczony, nadrzędny status względem wszystkich innych, kiedy węzeł jest zaznaczony
    };


    //TODO - do zmian póxniej, musi to być bardziej elastyczne!!
    //! Typ opisujący charakter węzła, uzywany przy tworzeniu toolbara
	enum NodeType {
        SOURCE,    //! węzeł źródło
        VISUALIZE, //! węzeł wizualizujący
        PROCESS,   //! węzeł przetwarzający
        ANALYZE    //! węzeł analizujący dane
    };

    //! Zbiór pinów wizualnych
	typedef std::set<osg::ref_ptr<osgVDFBasePin> > Pins;

public:

	//META_Window(osgWidget::Window, osgVDFBaseNode);

	osgVDFBaseNode(const std::string & nodeName = std::string());
	virtual ~osgVDFBaseNode(void);

    //! \return Logiczny węzeł związany z tą graficzna reprezentacją
	const dflm::NPtr & getModelNode() const;

    //! \return Wizualny model, z którym skojarzony jest ten wizualny węzeł
	osgVDFBaseModel * getVisualModel() const;

    //! \return Kolekcja wizualnych pinów wejściowych
	const Pins & getInPins() const;

    //! \return Kolekcja wizualnych pinów wyjściowych
	const Pins & getOutPins() const;

	//for debug purpose only!!
	virtual void loggZCoordinates() const;

    //! \return Wizualną nazwę węzła
	virtual std::string getName() const = 0;

    //! \return Wizualny stan węzła
	VisualStatus getVisualStatus() const;

    //! \return Powierzchnia węzła
    osgWidget::point_type getArea() const;

    //! Usuwa węzeł z modelu, wcześniej wywołuje zadaną funkcję jeśli ustawiono
    void deleteNode();

private:

    //! \param inPin Wizualny pin wejściowy do dodania
    //! \param pinName Nazwa dodawanego wizualnego pina wejściowego
    void addInPin(osgVDFBasePin * inPin, const std::string & pinName = std::string());

    //! \param outPin Wizualny pin wyjsciowy do dodania
    //! \param pinName Nazwa dodawanego wizualnego pina wyjsciowego
    void addOutPin(osgVDFBasePin * outPin, const std::string & pinName = std::string());

    //! \param nodeVisualStatus Wizualny stan węzła ze względu na podjęte na nim akcje użytkownika lub jego statyczny stan w modelu logicznym
    void setVisualStatus(VisualStatus nodeVisualStatus);

    //! \param name Nowa wizualna nazwa węzła
    virtual void setName(const std::string & name) = 0;

    //! \param inPin Wizualny pin dodawany do węzła
    //! \param pinName Nazwa wizualnego pinu
	virtual void graphAddInPin(osgVDFBasePin * inPin, const std::string & pinName) = 0;

    //! \param outPin Wyjściowy wizualny pin dodawany do węzła
    //! \param pinName Nazwa wizualnego pinu dodawanego do węzła
	virtual void graphAddOutPin(osgVDFBasePin * outPin, const std::string & pinName) = 0;

    //! \param nodeVisualStatus Nowy wizualny status węzła
	virtual void graphSetStatus(VisualStatus nodeVisualStatus) = 0;

    //! Chwilowo wyłancza możliwość edycji i usuwania węzła
    virtual void lockForEditing(bool lock) = 0;

    //! \param area powierzchnia węzła
    void setArea(osgWidget::point_type area);

    void setCollisionNodes(const VNodes & vnodes);

    const VNodes & getCollisionNodes() const;

    protected:

    //! Aktualny status wizulany węzła
	VisualStatus visualStatus;

	//! Węzeł logiczny odpowiadający tej wizualnej reprezentacji
	dflm::NPtr modelNode;

	//! Piny wejściowe w wersji wizualnej
	Pins inPins;

    //! Piny wyjściowe w wersji wizualnej
	Pins outPins;

    //! Wizualny model węzła
	osgVDFBaseModel * visualModel;

    //! Obszar węzła
    osgWidget::point_type area;

    //! Wizualne węzły z którymi aktualnie jest w kolizji
    VNodes collisionNodes;
};

}

#endif
