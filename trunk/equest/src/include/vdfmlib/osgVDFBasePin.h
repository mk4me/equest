#ifndef HEADER_GUARD__VDFBASEPIN_H__
#define HEADER_GUARD__VDFBASEPIN_H__

#include <osgWidget/Widget>
#include <dfmlib/DFPin.h>
#include <map>
#include <osgui/Utils2D.h>

////////////////////////////////////////////////////////////////////////////////
namespace osgVDF{
////////////////////////////////////////////////////////////////////////////////

class osgVDFBaseModel;
class osgVDFBaseNode;

class osgVDFBasePin : public osgWidget::Widget
{
	friend class osgVDFBaseModel;
	friend class osgVDFBaseNode;

private:

    typedef std::function<void(osg::Geode *, const osgWidget::XYCoord &)> VisualConnectionsUpdater;

public:

    //! Typ reprezentujący wizualny stan pinu - aktywny i statyczny stan
	enum VisualStatus{
        OK,             //! z pinem wszystko ok, ewentualnie można łączyć piny
		ACTIVE,         //! pin aktualnie aktywny, aktualnie wskazywany
		INCOMPATIBLE,   //! pin niekompatybilny podczas łączenia
		REQUIRED,       //! pin wymagany
		CONNECTED,      //! pin podłączony
		INCOMPLETE      //! pin, którego zależności nie zostasły w pełni spełnione
    };

    //! Mapa łącząca wizualne połączenie z pinami wizualnymi po drugiej stornie tych połączeń
	typedef std::map<osg::Geode*, osgVDFBasePin*> Connections;

public:
	//META_Object(osgWidget::Widget, osgVDFBasePin);

	osgVDFBasePin (const std::string& name = std::string());
    osgVDFBasePin (const osgVDFBasePin& pin, const osg::CopyOp& co);

	~osgVDFBasePin(void);

    //!
    virtual void positioned();

    //! \param wm WindowManager który nami zarządza
	virtual void managed(osgWidget::WindowManager* wm);

    //! \return Pin logiczny który reprezentujemy
	const dflm::PinPtr & getModelPin() const;

    //! \return Wizualny węzeł będący naszym rodzicem
	osgVDFBaseNode * getVisualNode() const;

    //! \return Aktualny statyczny stantus
	VisualStatus getStaticVisualStatus() const;

    //! \param refPin Pin względem którego sprawdzxamy nasz dynamiczny status
    //! \param model Model logiczny względem którego sprawdzamy nasz stan
	VisualStatus getDynamicVisualStatus(const dflm::PinPtr & refPin, const dflm::MPtr & model) const;

    //! \param refPin Pin względem którego sprawdzxamy nasz dynamiczny status
    //! \param model Model logiczny względem którego sprawdzamy nasz stan
	VisualStatus getDynamicVisualStatus(osgVDFBasePin * refPin, const dflm::MPtr & model) const;

    //! \return Centralny punkt pinu z punktu widzenia przyłanczania połączeń
	virtual const osgWidget::XYCoord & getCenterPosition();
    //! \return Centralny punkt pinu z punktu widzenia przyłanczania połączeń
	virtual osgWidget::XYCoord getCenterPosition() const;
	
    //! \return Mapa połączeń wizualnych pinu
	const Connections & getConnections() const;

    //! \return Zwraca aktualny wizualny status pinu
	VisualStatus getVisualStatus() const;

private:

    //! \param pinVisualStatus Wizualny status pinu oparty na podjętych na nim akcjach bądź jego statycznemu statusowi
    virtual void graphSetStatus(VisualStatus pinVisualStatus) = 0;

    //! \param pinVisualStatus Nowy wizualny status pinu,
    //! wiąże się ze zmianą wyglądu pinu przez dostepną metodę wirtualną implementowaną przez konkretną reprezentację wizualną pinu
    void setVisualStatus(VisualStatus pinVisualStatus);

    //! \param connection Połączenie wizualne które dodajemy
    //! \param other Pin wizualny po drugiej stronie połączenia
	void addConnection(osg::Geode * connection, osgVDFBasePin * other);

    //! \param connection Połączenie wizualne, które usuwamy
	void removeConnection(osg::Geode * connection);

private:

    //! modelPin logiczny, którego ta wizualna reprezentacja dotyczy
	dflm::PinPtr modelPin;

    //! Mapa połączeń i poinów po drugiej stornie połączenia
	Connections connections;

    //! Wizualny węzeł w którym występuje ten wizulany modelPin
	osgVDFBaseNode * visualParentNode;

    //! Wizualny model w kontekście którego występuje ten wizualny modelPin
	osgVDFBaseModel * visualModel;

    //! Wiazualny status tego pinu
	VisualStatus visualStatus;

    //! Pozycja śroka pinu potrzebna przy realizacji wizualnych połączeń
	osgWidget::XYCoord centerPosition;

    //! Czy pozycję centralną trzeba ponownie wyznaczyć, bo modelPin zmienił pozycję
	bool centerChanged;

    //! Funktor nadany przez model do aktualizacji połączeń vizualnych
    VisualConnectionsUpdater connectionsUpdater;
};

}

#endif
