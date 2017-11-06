#ifndef HEADER_GUARD__VDFBASEMODEL_H__
#define HEADER_GUARD__VDFBASEMODEL_H__

#include <mutex>
#include <osgWidget/Widget>
#include <osgWidget/WindowManager>
#include <osgGA/GUIEventAdapter>
#include <osgGA/GUIEventHandler>
#include <boost/bimap.hpp>

#include <osgui/KeyboardMapper.h>

#include <osgui/Buttonized.h>
#include <osgui/Borderized.h>

#include <dfmlib/DFLMTypes.h>
#include <dfmlib/Connection.h>
#include <dfmlib/Model.h>

#include <vdfmlib/osgVDFNodeTypeDescriptor.h>

#include <osgui/Toolbar.h>
#include <osgui/ContextMenu.h>
#include <osgui/TooltipHost.h>

////////////////////////////////////////////////////////////////////////////////
namespace osgVDF{
////////////////////////////////////////////////////////////////////////////////

class osgVDFBaseNode;
class osgVDFBasePin;
class osgVDFContextMenu;

class osgVDFBaseModel : public osgWidget::WindowManager, public osgui::KeyboardMapper, public osgui::TooltipHost
{

//friend class osgVDFBaseNode;

public:
    //! Typ akcji na potrzeby realizacji usuwania połączeń przez zewnętrzne menu kontekstowe
    typedef std::function<void(void)> ConnectionAction;

    //! Para akcji - podświetlenia danego połączenia wizualnego i jego usunięcia na potrzeby realizacji tego zadania przez zewnętrzne menu kontekstowe - np Qt
    typedef std::pair<ConnectionAction, ConnectionAction> ConnectionActions;

    //! Typ mapujący nazwę połączenia i akcje z nim związane
    typedef std::map<std::string, ConnectionActions> ConnectionsActionsMapping; 

    //! Typ akcji węzła
    typedef std::function<void(const osgVDFBaseNode *, const osgWidget::XYCoord &)> NodeAction;

    //! Akcja usuwająca wezel
    typedef std::function<void(const osgVDFBaseNode *)> NodeDeleteAction;

protected:
    
    typedef std::lock_guard<std::recursive_mutex> ScopeLock;

    //! Typ służący do ładowania konfiguracji węzłów
    typedef std::map<dflm::NPtr, osgWidget::XYCoord> NodesPositions;

    //! Mapa łącząca węzły logiczne z ich wizualnymi reprezentacjami
    typedef std::map<dflm::NPtr, osg::ref_ptr<osgVDFBaseNode>> NodesMapping;

    //! Mapa łącząca piny logiczne z ich wizualnymi reprezentacjami
    typedef std::map<dflm::PinPtr, osgVDFBasePin*> PinsMapping;

    //typedef boost::bimap< set_of<dflm::ConnPtr>, set_of<osg::ref_ptr<osg::Geode> > > CoonMapp;

    //! Mapa łącząca logiczne połączenia z ich wizualnymi reprezentacjami
    typedef std::map<dflm::ConnPtr, osg::ref_ptr<osg::Geode> > ConnectionsMapping;

    //! Mapa łącząca wizualne połączenia z ich logicznymi reprezentacjami
    typedef std::map<osg::ref_ptr<osg::Geode>, dflm::ConnPtr> RevConnectionsMapping;

    //! Zbiór wizualnych węzłów
    typedef osgVDFBaseNode::VNodes VNodes;

    //! Mapa poprzednich wizualnych statusów wizualnych węzłów
    typedef std::map<osgVDFBaseNode*, osgVDFBaseNode::VisualStatus> VNodesPrevVisualStatus;

    //! Mapa poprzednich wizualnych statusów wizualnych pinów
    typedef std::map<osgVDFBasePin*, osgVDFBasePin::VisualStatus> VPinPrevVisualStatus;

    //! Mapa kompatybilności pinów wizualnych
    typedef std::map<osgVDFBasePin*, osgVDFBaseNode::Pins> VPinsCompatibility;

private:

    typedef osgVDFBasePin::VisualConnectionsUpdater VisualConnectionUpdater;

    //! Typ przycisku w OSG na potrzeby realizacji wewnętrznego toolbara z węzłami
    typedef osgui::Buttonized<osgui::Borderized<osgWidget::Label>, osgui::TextStylePolicy> Button;

    //! Klasa przechwytująca zmianę rozmiaru okna i aktualizująca ten rozmiar dla VDF
    class ModelResizeHandler : public osgGA::GUIEventHandler{

    public:
        ModelResizeHandler(osgVDFBaseModel * m);
        virtual ~ModelResizeHandler();

        virtual bool handle(const osgGA::GUIEventAdapter& gea,
            osgGA::GUIActionAdapter&      gaa,
            osg::Object*                  obj,
            osg::NodeVisitor*             nv
            );

    protected:
        osg::observer_ptr<osgVDFBaseModel> model;
    };

    friend class ModelResizeHandler;


    //! Klasa odpowiedzialna za zarządzanie zaznaczeniami wizualnych węzłów
    class VisualSelectionManager : public osgGA::GUIEventHandler{

    public:
        VisualSelectionManager(osgVDFBaseModel * m);

        virtual ~VisualSelectionManager();

        virtual bool handle(const osgGA::GUIEventAdapter& gea,
            osgGA::GUIActionAdapter&      gaa,
            osg::Object*                  obj,
            osg::NodeVisitor*             nv
            );

    protected:
        osg::observer_ptr<osgVDFBaseModel> model;
        bool visualSelectionStarted;
    };

    friend class VisualSelectionManager;

    //! Klasa odpowiadająca za obsługe akcji użytkownika na przestrzeni VDF - tam gdzie osadzone są węzły, toolbar i połączenia
    class UserSpaceClick : public osgGA::GUIEventHandler{
    public:
        UserSpaceClick(osgVDFBaseModel * m);

        virtual ~UserSpaceClick();

        virtual bool handle(const osgGA::GUIEventAdapter& gea,
            osgGA::GUIActionAdapter&      gaa,
            osg::Object*                  obj,
            osg::NodeVisitor*             nv
            );

    protected:
        static void DistanceFromLine(double cx, double cy, double ax, double ay ,
            double bx, double by, double &distanceSegment,
            double &distanceLine);

        const osgVDFBaseNode* getTopNodeInPoint(const osgWidget::XYCoord & pos) const;

    protected:
        osg::observer_ptr<osgVDFBaseModel> model;
    };


    friend class NodeDescriptorClick;
    friend class osgVDFBaseNode;
    friend class osgVDFBasePin;


public:

    osgVDFBaseModel(osgViewer::View* view = 0, osgWidget::point_type width = 0.0f, osgWidget::point_type height = 0.0f,
        unsigned int nodeMask = 0, unsigned int flags = 0, const dflm::MPtr & model = dflm::MPtr());

	~osgVDFBaseModel(void);

    virtual void setModel(const dflm::MPtr & model, const NodesPositions & nodesPositions = NodesPositions());

    const dflm::MPtr & getModel() const;

    //! \param dewscriptor Klasa opisująca typ węzła dodawanego do defualtowego toolbara
    void registerNodeType(osgVDFNodeTypeDescriptor * descriptor);

    //! \param lock Czy zablokować do edycji logiki i właściwości modelu - tylko wizualna zmiana layoutu możliwa
    void lockForModelEdition(bool lock);

    //! \return Czy zablokowano do edycji logiki
    bool isLockedForModelEdition() const;

    //! \param lock Czy zablokować do jakiejkolwiek edycji
    void lockForVisualEdition(bool lock);

    //! \return Czy zablokowano do jakiejkolwiek edycji
    bool isLockedForVisualEdition() const;

    //-------------------------- NODES LAYOUT MANAGMENT ------------------------------------

    //! \param pos Pozycja która testujemy czy jest wolna
    //! \return true jeśli podana pozycja jest wolna, nizajęta przez żaden węzeł wizualny
	bool isPointFree(const osgWidget::XYCoord & pos) const;

    //! \param area Obszar z którego węzły chcemy pobrać
    //! \return Kolekcja węzłów z zadanego obszaru
    dflm::Model::Nodes getNodesInArea(const osg::BoundingBox & area) const;

    //! \param node Węzęł dla którego poszukujemy pustego miejsca
    //! \posToStart Pozycja startowa od której powinniśmy zacząć szukać
    //! \return Pozycja znalezionego wolnego miejsca lub pozycja startowa jeśli nic nie znaleziono
    virtual osgWidget::XYCoord getFreePlaceForNode(const dflm::NPtr & node, const osgWidget::XYCoord & posToStart) const;

    //----------------------------- LOGIC EDITION ------------------------------------------

    //! \param node Węzeł logiczny do doania
    //! \param image Obrazek związany z podanym węzłem (powinien być obslugiwany przez implementację węzła wizualnego)
    //! \param name Nazwa węzła
    //! \param pos Pozycja do wstawienia węzła
	void addNode(const dflm::NPtr & node, osg::ref_ptr<osg::Image> image = osg::ref_ptr<osg::Image>(),
		const std::string &  name = std::string(), const osgWidget::XYCoord & pos = osgWidget::XYCoord(-1,-1));
	
    //! \param node Węzeł do usunięcia
    void removeNode(const dflm::NPtr & node);

    //! Rozłancza węzeł, usuwa wszystkie jego połączenia
    //! \param node Węzeł do rozłączenia
    void disconnectNode(const dflm::NPtr & node);

    //! \param src Pin który chcemy łączyć
    //! \param dest Pin który chcemy łączyć
    //! \return Połączenie między pinami
    virtual dflm::ConnPtr connect(const dflm::PinPtr & src, const dflm::PinPtr & dest);

    //! \param connection Połączenie które usuwamy
    virtual void disconnect(const dflm::ConnPtr & connection);

    //! Usuwa wszystkie węzły i ich połączenia
    void removeAllNodes();

    //! Usuwa wszystkie połączenia
    void removceAllConnections();
    
    //----------------------- SELECTION ---------------------------------------------

    //! \node Węzeł do zaznaczenia
	void selectNode(const dflm::NPtr & node);

    //! \param rect Obszar w którym zaznaczamy wszystkie węzły
    void selectNodes(const osg::BoundingBox & rect);

    //! Zaznacza wszystkie węzłyu
	void selectAllNodes();

    //! \param node Węzeł do odznaczenia
	void deselectNode(const dflm::NPtr & node);

    //! \param Obszar w którym odznaczamy wszystkie węzły
    void deselectNodes(const osg::BoundingBox & rect);
    
    //! Odznacza wszystkie węzły
	void deselectAllNodes();

	//! \return Zaznaczone węzły
    const dflm::Model::Nodes & getSelectedNodes() const;

    //! \param node Węzeł który testujemy pod kątem zaznaczenia
    //! \return true jeśli węzeł zaznaczony
    bool isNodeSelected(const dflm::NPtr & node) const;

    //------------------------------ VISUAL NODES PROPERTIES ---------------------------------

    //! \param node Węzeł który testujemy czy jest w kolizji
    //! \return true jeśli węzeł bvierze udział w kolizji
	bool isNodeInCollision(const dflm::NPtr & node) const;

    //! \param node Węzeł który testujemy na występowanie w widzialnym obszarze
    //! \return true jeśli węzeł jest choć częsciowo zawarty w widocznym obszarze
	bool isNodeInVisibleArea(const dflm::NPtr & node) const;

    //! \param Węzeł który testujmey czy jest całkowicie widzoczny
    //! \return true jeśli węzeł całkowicie widoczny
	bool isNodeFullyInVisibleArea(const dflm::NPtr & node) const;

    //! \param node Węzeł którego pozycję chcemy pobrać
    //! \return Pozycja węzła
	osgWidget::XYCoord getNodePosition(const dflm::NPtr & node);

    //! \param node Węzeł którego rozmiar chcemy pobrać
    //! \return Rozmiar danego węzła
	osgWidget::XYCoord getNodeSize(const dflm::NPtr & node);

    //! \param nodes Kolekcja węzłów których obszar chcemy dostać
    //! \return obszar na bazie węzłów które występuja w modelu
    osg::BoundingBox getNodesBoundingBox(const dflm::Model::Nodes & nodes ) const;

    //! \param node Węzeł któy przesuwamy względem jego aktualnej pozycji
    //! \param moveStep Przesunięcie węzła względem jego aktualnej pozycji
	void setNodeRelativePosition(const dflm::NPtr & node, const osgWidget::XYCoord & moveStep);

    //! \param node Węzeł któy ustawiamy bezwzględnie
    //! \param nodePos Nowa pozycja węzła
	void setNodeAbsolutePosition(const dflm::NPtr & node, const osgWidget::XYCoord & nodePos);

    //---------------------------------- VMODEL CONFIGURATION --------------------------------------

    //! \param show Czy pokazywać domyślny toolbar
	void showDefaultToolbar(bool show);

    //! \return Minimalny obszar węzła jaki trzeba zaznaczyć aby wezel był uznany za zaznaczony/odznaczony
    //! wartość procentowa powierzchni wezła z przedzialu [0..1], gdzie 0 oznacza zaznaczenie na kontakt z węzłem a 1 wymaga objęcia całego węzła
	osgWidget::point_type getMinSelectedNodeAreaRatioToSelect() const;

    //! \param areaRatio Procentowy obszar węzła jaki należy objąć by zaznaczyć/odznaczyć węzeł
	void setMinSelectedNodeAreaRatioToSelect(osgWidget::point_type areaRatio);

    //! \return Zbiór klawiszy modyfikujących sposób zaznaczania
	const osgui::KeyboardMapper::Keys & getSelectionActionKeys() const;

    //! \param keys Zbiór klawisdzy modyfikujący sposób zaznaczania
	void setSelectionActionKeys(const osgui::KeyboardMapper::Keys & keys);

    //! \return Mnimalny dystans pomiedzy aktualną pozycją a połączeniem aby uznać połaczenie jako potencjalnie wybrane
	osgWidget::point_type getMinDistToDelConnection() const;

    //! \param dist Minimalny dystans kursora do połączenia aby uznać połączenie jako potencjalnie wybrane
	void setMinDistToDelConnection(osgWidget::point_type dist);

    //! \param Normalny kolor połączenia
	osgWidget::Color getVisualConnectionNormalColor() const;

    //! \param color Normalny kolor połączenia
	void setVisualConnectionNormalColor( const osgWidget::Color & color);
	
    //! \return Kolor zaznaczonego połączenia
	osgWidget::Color getVisualConnectionHighlightColor() const;
    //! \param color Kolor zaznaczonego połaczenia
	void setVisualConnectionHighlightColor( const osgWidget::Color & color);

    //--------------------------------------- NODES ACTIONS -----------------------------------------------------------

    void setNodeClickAction(const NodeAction & clickAction);

    const NodeAction & getNodeClickAction() const;

    void setNodeDoubleClickAction(const NodeAction & doubleClickAction);

    const NodeAction & getNodeDoubleClickAction() const;

    void setNodeDeleteAction(const NodeDeleteAction & deleteAction);

    const NodeDeleteAction & getNodeDeleteAction() const;

    void setNodeContextAction(const NodeAction & contextAction);

    const NodeAction & getNodeContextAction() const;

private:

    //user interaction - connection managment
    virtual osg::Geode* createVisualConnection() = 0;
    virtual void setVisualConnectionEnd(osg::Geode * connection, const osgWidget::XYCoord & endPos) = 0;
    virtual void setVisualConnectionStart(osg::Geode * connection, const osgWidget::XYCoord & startPos) = 0;
    virtual void setVisualConnectionZ(osg::Geode * connection, osgWidget::point_type z) = 0;
    virtual void setVisualConnectionColor(osg::Geode * connection, const osgWidget::Color & color ) = 0;

    //user interaction - selection managment
    virtual osg::Geode* createVisualSelection(const osgWidget::XYCoord & startPos) = 0;
    virtual void continueVisulaSelection(osg::Geode * selection, const osgWidget::XYCoord & continuePos) = 0;

    //visual components delivery    
    virtual osgVDFBaseNode * createVisualNode(osg::ref_ptr<osg::Image> image = osg::ref_ptr<osg::Image>(),
        const std::string &  name = std::string()) = 0;

    virtual osgVDFBasePin * createVisualPin(const std::string &  name = std::string()) = 0;

    virtual void showConnectionsDeleteContextMenu(const ConnectionsActionsMapping & toDelete, const ConnectionAction & leaveContextMenu, const osgWidget::XYCoord & pos);

    //-----------------------------------------------------------------------------------------------------------------

    osgWidget::XYCoord innerGetNodePosition( const dflm::NPtr & node );
    bool innerIsPointFree( const osgWidget::XYCoord &posToStart ) const;
    bool innerIsNodeSelected(osgVDFBaseNode * vnode) const;
    bool innerIsNodeSelected(const dflm::NPtr & node) const;

	osgWidget::point_type getMinNodesZ();
	osgWidget::point_type getMaxNodesZ();

	void hideConnections();

	static void refreshPinVisualState(osgVDFBasePin * pin);
	static void addConnectionToPin(osgVDFBasePin * pin, osg::Geode * connection, osgVDFBasePin * other);
	static void removeConnectionFromPin(osgVDFBasePin * pin, osg::Geode * connection);

	void configureVisualNode(osgVDFBaseNode * node);
	void configureVisualPin(osgVDFBasePin * pin);

	static void refreshOutputPins(osgVDFBaseNode * node);

	osgVDFBaseNode* getVisualNode(const dflm::NPtr & node) const;

	void refreshFocused(osgWidget::Window * toFocus = 0);

    void saveCurrentVisualNodesStates();

    void restorePreviousVisualNodeStates();

    void updateVisualNodeStatus(osgVDFBaseNode * vnode, const VNodes & nodes);

    void deselectVisualNodes(const VNodes & vnodes, const VNodes & refNodes);

    void deselectAllVisualNodes();

    VNodes getCollisionNodes(osgVDFBaseNode * vnode, const VNodes & nodes) const;

    dflm::Model::Nodes filterNodes(const dflm::Model::Nodes & nodes) const;

    VNodes getVNodesFromNodes(const dflm::Model::Nodes & nodes) const;
    dflm::Model::Nodes getNodesFromVNodes(const VNodes & vnodes) const;

    osg::BoundingBox innerGetNodesBoundingBox( const VNodes & vnodes) const;

    VNodes getVisualNodesInArea(const osg::BoundingBox & area) const;

    void positionNode( osgVDFBaseNode* vnode, const osgWidget::XYCoord &nodePos );

    void moveNode( osgVDFBaseNode* vnode, const osgWidget::XYCoord &moveStep );
    void moveNodes( const VNodes & vnodes, const osgWidget::XYCoord &moveStep );

    void clearVisualSelections();
    void clearVisualNodes();
    void clearVisualConnections();

    void innerSelectNode(const dflm::NPtr & node);
    void innerDeselectNode(const dflm::NPtr & node);

    void innerSelectNode(osgVDFBaseNode * node);
    void innerDeselectNode(osgVDFBaseNode * node);

    void selectVisualNode(osgVDFBaseNode * node);
    void deselectVisualNode(osgVDFBaseNode * node, const VNodes & nodes);


    void selectNodes(const dflm::Model::Nodes & nodes);
    
    void deselectNodes(const dflm::Model::Nodes & nodes);

    void init( osgViewer::View* view );


	void updatePinsConnections(osgVDFBasePin * pin);

	void removeNode(const dflm::NPtr & node, bool clearEvent);
	void deleteNodeSelf(osgVDFBaseNode * node);

	bool onNodeClick(osgWidget::Event& ev);
	bool onNodeDrag(osgWidget::Event& ev);
	bool onNodeRelease(osgWidget::Event& ev);
	void updateCollisions();
	VNodes getSelectedNodesCollisions();

	bool onPinClick(osgWidget::Event& ev);
	bool onPinDrag(osgWidget::Event& ev);
	bool onPinRelease(osgWidget::Event& ev);

	bool onPinEnter(osgWidget::Event& ev);
	bool onPinLeave(osgWidget::Event& ev);

	bool onNodeEnter(osgWidget::Event& ev);
	bool onNodeLeave(osgWidget::Event& ev);

	std::vector<std::string> getPinErrors(const dflm::PinPtr & pin);
	std::vector<std::string> getNodeErrors(const dflm::NPtr & node);

	void savePinsStatus(const osgVDFBaseNode::Pins & pins);
	void highlightCompatiblePins(osgVDFBasePin * startPin, const osgVDFBaseNode::Pins & pins);
	void restorePinsStatus();

	bool onNodeDescriptorPush(osgWidget::Event& ev);

	void fillToolbarWithGroups(osgui::Toolbar * toolbar);
    void graphAddNodeTypeToToolbar(osgVDFNodeTypeDescriptor* nodeTypeDescriptor);

    void highlightConnection(osg::Geode * connection);

    void clearHighlightedConnection();

    bool onLeaveContextMenu(osgWidget::Event& ev);

    static void contextMenuActionWrapper(const ConnectionAction & action, const std::string& name, bool state);

	void removeConnection(const dflm::ConnPtr & connection, const std::string & connectionName, bool checked);
	void onCloseContextMenu(osgui::ContextMenu * menu);

protected:

	osgVDFNodeTypeDescriptor * currentNodeDescriptor;
	std::map<osgWidget::Widget*,osgVDFNodeTypeDescriptor*> controlsToNodeTypes;
	std::map<osgVDFNodeTypeDescriptor*,osgWidget::Widget*> nodeTypesToControls;
	std::set<osgVDFNodeTypeDescriptor*> nodeTypesMissing;

protected:
	dflm::MPtr logicModel;
	
	NodesMapping nodesLogicalToGraph;
	VNodes visualNodes;
	PinsMapping pinsLogicalToGraph;
	VPinsCompatibility compatiblePins;

	ConnectionsMapping connectionsLogicalToGraph;
	RevConnectionsMapping connectionsGraphToLogical;

	VNodes selectedVNodes;
    VNodes deselectedVNodes;
	KeyboardMapper::Keys selectionActionKeys;
	osgWidget::point_type minIntersectRatioToSelect;

	//connection events
	osgWidget::point_type maxDistToDelConnection;

	osgWidget::Color visualConnectionNormalColor;
	osgWidget::Color visualConnectionHighlightColor;
	
private:
	VNodesPrevVisualStatus selectionPrevNodesStatus;	
	osgWidget::XYCoord selectionStartPose;
	osgVDFBaseNode * focusedNode;

	osg::ref_ptr<osg::Geode> visualSelection;
	osg::ref_ptr<osg::Geode> visualConnection;

	//node events
	osg::ref_ptr<osgWidget::Callback> nodePushEvent;
	osg::ref_ptr<osgWidget::Callback> nodeDragEvent;
	osg::ref_ptr<osgWidget::Callback> nodeReleaseEvent;
	osg::ref_ptr<osgWidget::Callback> nodeEnterEvent;
	osg::ref_ptr<osgWidget::Callback> nodeLeaveEvent;

    bool moveStartedWithViewCollision;
	osgWidget::XYCoord initialMoveCoords;
	bool moveStarted;
	VNodesPrevVisualStatus movePrevNodesStatus;
	VNodes collisionNodes;
	osg::BoundingBox modelBoundingBox;
	osg::BoundingBox selectionBoundingBox;

	//pin events
	osg::ref_ptr<osgWidget::Callback> pinPushEvent;
	osg::ref_ptr<osgWidget::Callback> pinDragEvent;
	osg::ref_ptr<osgWidget::Callback> pinReleaseEvent;
	osg::ref_ptr<osgWidget::Callback> pinEnterEvent;
	osg::ref_ptr<osgWidget::Callback> pinLeaveEvent;

	bool connectingStarted;
	osgWidget::XYCoord connectingCurrentPose;
	osgVDFBasePin* connectingStartPin;
	osgVDFBasePin* connectingCurrentPin;
	VPinPrevVisualStatus connectingPrevPinsStatus;
	osgVDFBasePin::VisualStatus conectingPrevStartPinStatus;
	osgVDFBasePin::VisualStatus conectingPrevCurrentPinStatus;
	
    VisualConnectionUpdater connectionContinue;

	VisualConnectionUpdater connectionStartUpdate;
	VisualConnectionUpdater connectionEndUpdate;
    
    dflm::Model::Nodes selectedNodes;

	osg::ref_ptr<osgui::ContextMenu> contextMenu;
	osg::Geode * lastHighlightedConnection;
	bool contextMenuOn;

	std::set<osg::ref_ptr<osgui::Tooltip> > tooltips;
	osg::ref_ptr<osgui::Tooltip> currentNodeTooltip;
	osg::ref_ptr<osgui::Tooltip> currentPinTooltip;

	osg::ref_ptr<osgui::Toolbar> toolbar;
	bool toolbarVisible;
	Button * lastButton;

    mutable std::recursive_mutex stateMutex;

    NodeDeleteAction deleteNodeAction;

    NodeAction clickNodeAction;

    NodeAction doubleClickNodeAction;

    NodeAction contextNodeAction;
};


}

#endif
