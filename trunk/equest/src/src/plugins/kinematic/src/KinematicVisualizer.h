/********************************************************************
    created:  2011/07/15
    created:  15:7:2011   12:35
    filename: KinematicVisualizer.h
    author:   Mateusz Janiak

    purpose:
*********************************************************************/
#ifndef HEADER_GUARD___KINEMATICVISUALIZER_H__
#define HEADER_GUARD___KINEMATICVISUALIZER_H__

#include <QtCore/QObject>

#include <corelib/IVisualizer.h>
#include <osg/Geode>
#include <osgGA/OrbitManipulator>
#include <osgManipulator/Dragger>
#include <osgManipulator/Command>
#include <osgui/QOsgWidgets.h>
#include <QtGui/QIcon>
#include <QtWidgets/QMenu>
#include <QtWidgets/QWidget>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QComboBox>
#include "osgutils/OsgSchemeDrawer.h"
#include "Manipulators.h"
#include "TrajectoriesDialog.h"
#include "KinematicSerie.h"

#include <coreui/CoreWidgetAction.h>
#include "SchemeDialog.h"
#include "plugins/kinematic/IKinematicVisualizer.h"

class QDoubleSpinBox;
class AbstractSkeletonSerie;

namespace coreUI {
	class CoreAction;
	class CoreLabeledWidget;
}

//! Klasa dostarcza scenę 3d i zarządza obiektami na niej
//! \version 0.9.1
class KinematicVisualizer :  private QObject, public plugin::IVisualizer, public kinematic::IKinematicVisualizer
{
	// wszystkie możliwe serie przeniesione do osobnego pliku
	//friend class GRFSerie;
	friend class MarkerSerie;
	friend class SkeletonSerie;
	friend class AbstractSkeletonSerie;
	//friend class SkeletonSerie2;
    friend class KinematicDraggerCallback;
    typedef osg::ref_ptr<osg::Geode> GeodePtr;
    typedef osg::ref_ptr<osg::PositionAttitudeTransform> TransformPtr;
    Q_OBJECT;
    UNIQUE_ID("{E8B5DEB2-5C57-4323-937D-1FFD288B65B9}")
	CLASS_DESCRIPTION("Kinematic visualizer", "Kinematic visualizer")

private:

	typedef std::pair<coreUI::CoreLabeledWidget*, QDoubleSpinBox*> LabeledDoubleSpinBox;

public:

	class UpdateProxy
	{
	public:
		UpdateProxy(KinematicVisualizer * visualizer) : visualizer(visualizer) {}
		UpdateProxy(UpdateProxy && Other) : visualizer(Other.visualizer)
		{
			Other.visualizer = nullptr;
		}

		~UpdateProxy() { if(visualizer != nullptr) visualizer->requestUpdate(); }

		KinematicVisualizer* operator->() { return visualizer; }

	private:
		KinematicVisualizer * visualizer;
	};

public:
    KinematicVisualizer();

public:
    //! Update wywoływany przez EDR, sprawdza, czy zmienił się czas serii, jeśli tak, to konieczne jest odświeżenie
    //! \param deltaTime czas od ostatniego update'u
    virtual void update( double deltaTime );
	//! wizualizator nie ma ograniczeń w liczbie serii
	virtual int getMaxDataSeries() const;
    //! Fabryka serii, na podstawie typu danych we wrapperze tworzona jest konkretna seria
    //! \param data Obiekt z danymi
    //! \param name Nazwa danych
    virtual plugin::IVisualizer::ISerie *createSerie(const utils::TypeInfo & requestedType, const core::VariantConstPtr & data);
    //! Tworzenie serii na podstawie już istniejącej, nie zaimplementowane
    //! \param serie Kopiowana seria
    virtual plugin::IVisualizer::ISerie *createSerie(const plugin::IVisualizer::ISerie* serie);

	virtual plugin::IVisualizer::ISerie * createSerie(const plugin::IVisualizer::ISerie *,const utils::TypeInfo &,const core::VariantConstPtr &);

    QString getRootName( const core::VariantConstPtr & data, const QString& suffix );


    //! Usuwa serie z wizualizatora
    //! \param serie seria do usunięcia
    virtual void removeSerie(plugin::IVisualizer::ISerie *serie);

	//! Ustawia daną serię aktywną
	virtual void setActiveSerie(plugin::IVisualizer::ISerie *serie);
	//! \return Pobiera aktywną serię, nullptr gdy nie ma żadnej aktywnej
	virtual const plugin::IVisualizer::ISerie * getActiveSerie() const;
	virtual plugin::IVisualizer::ISerie * getActiveSerie();

	//! Tworzy pusty wizualizator
	virtual plugin::IVisualizer* create() const;
    //! Zwraca informacje o obsługiwanych danych
    //! \param info kolecja z typami danych
    virtual void getSupportedTypes(utils::TypeInfoList & supportedTypes) const;
    //! Tworzy widget w którym znajduje się
    //! \param manager Manager Flexi Bara
    virtual QWidget* createWidget();
    //! Tworzy ikonę
    virtual QIcon* createIcon();
    //! Tworzy pixmapę z aktualnym widokiem wizualizatora
    virtual QPixmap takeScreenshot() const;
	//! resetuje ustawienia sceny 3d
	void resetScene();

	virtual bool innerUpdateRequired();

	void requestUpdate();

	virtual bool getAxisXYZ() const { return AxisXYZ; }
	virtual void setAxisXYZ(bool val) { AxisXYZ = val; }

private:

	void updateIndicator();

    //! \return węzeł z siatką reprezentującą podlogę
    osg::Node* createFloor();
    //! \return aktywna seria, o ile taka została wybrana
    KinematicSerieBase* tryGetCurrentSerie();
	plugin::IVisualizer::ISerie* tryGetCurrentISerie();
    //! odświeża dane w spinboxach zawierających informcacje o transformacjach manipulatorów
    void refreshSpinboxes();
    //! Metoda ustala, do której serii należy dana geoda
    //! \param geode obiekt sceny, który chcemy sprawdzic
    //! \return seria, do której należy obiekt lub nullptr jeśli nie znaleziono
    KinematicSerieBase* getParentSerie(GeodePtr geode);
    //! Metoda ułatwiająca tworzenie spinwidgetów z opisem. Tworzony jest widget rodzica zawierający spinbox i label z opisem
    //! \param parent rodzic, do którego zostanie towrzony widget
    //! \param name opis spinboxa
    //! \param step krok dla strzałek
    //! \param visible czy ma być widoczny po utworzeniu
    //! \return para (widget, spinbox) widget jest rodzicem spinboxa
    LabeledDoubleSpinBox createSpinWidget(const QString & name, double step = 0.1);    
    //! Wymusza zmiane translacji węzła podlegającego wplywom manipulatorów
    //! \param serie seria, dla której ma być zmianiony węzeł
    //! \param index Indeks współrzędnej (0 - x, 1 - y, 2 - z)
    //! \param d ustawiana wartość translacji
    void setTranslation(KinematicSerieBase* serie, int index, double d );
    //! Wymusza zmiane obrotu węzła podlegającego wplywom manipulatorów
    //! \param serie seria, dla której ma być zmianiony węzeł
    //! \param index Indeks współrzędnej (0 - x, 1 - y, 2 - z)
    //! \param d ustawiana wartość katu Eulera
    void setRotation( KinematicSerieBase* serie, int index, double d );
    //! Wymusza zmiane skali węzła podlegającego wplywom manipulatorów
    //! \param serie seria, dla której ma być zmianiony węzeł
    //! \param index Indeks współrzędnej (0 - x, 1 - y, 2 - z)
    //! \param d ustawiana wartość skali
    void setScale(KinematicSerieBase* serie, int index, double d );
    //! Wymusza zmiane translacji węzła podlegającego wplywom manipulatorów
    //! \param serie seria, dla której ma być zmianiony węzeł
    //! \param t nowa translacja
    void setTranslation(KinematicSerieBase* serie, const osg::Vec3& t  );
    //! Wymusza zmiane obrotu węzła podlegającego wplywom manipulatorów
    //! \param serie seria, dla której ma być zmianiony węzeł
    //! \param r nowy obrot w postaci kątów Eulera
    void setRotation( KinematicSerieBase* serie, const osg::Vec3& r );
    //! Wymusza zmiane obrotu węzła podlegającego wplywom manipulatorów
    //! \param serie seria, dla której ma być zmianiony węzeł
    //! \param q nowy obrot
    void setRotation( KinematicSerieBase* serie, const osg::Quat& q);
    //! Wymusza zmiane skali węzła podlegającego wplywom manipulatorów
    //! \param serie seria, dla której ma być zmianiony węzeł
    //! \param s nowa skala
    void setScale(KinematicSerieBase* serie, const osg::Vec3& s );
    //! tworzy węzeł ze wskaźnikiem aktywnej serii danych
    //! \return utworzony węzeł
    TransformPtr createIndicator() const;
    //! Odwieza spinboxy translacji, jeśli ulegly one zmianie
    void refreshTranslateSpinboxes();
    //! Odwieza spinboxy rotacji, jeśli ulegly one zmianie
    void refreshRotateSpinboxes();
    //! Odwieza spinboxy skali, jeśli ulegly one zmianie
    void refreshScaleSpinboxes();

	static coreUI::CoreWidgetAction * createWidgetAction(QWidget * widget, QObject * parent, const QString & sectionName, coreUI::CoreTitleBar::SideType side);

	//! ustawienie aktywnej serii
	//! \param idx indeks serii, która ma stac się aktywna. Musi być z zakresu <0, liczbaSerii)
    void setActiveSerie(int idx);
	QStringList getMarkersNames(const c3dlib::MarkerCollectionConstPtr& ms) const;
    //QStringList getSkeletonNames( const kinematic::JointAnglesCollectionConstPtr& ms ) const;

private slots:
    //! Dialog z ustawieniami trajektorii stanie się widoczny
    void showTrajectoriesDialog();
    //! Dialog z ustawieniami widoczności węzłów stanie się widoczny
    void showSchemeDialog();
    //! Pobiera aktywna serie i zminia jej węzeł przeznaczony dla manipulatorów
    //! \param d wartość X nowej translacji
    void shiftX(double d);
    //! Pobiera aktywna serie i zminia jej węzeł przeznaczony dla manipulatorów
    //! \param d wartość Y nowej translacji
    void shiftY(double d);
    //! Pobiera aktywna serie i zminia jej węzeł przeznaczony dla manipulatorów
    //! \param d wartość Z nowej translacji
    void shiftZ(double d);
    //! Pobiera aktywna serie i zminia jej węzeł przeznaczony dla manipulatorów
    //! \param d wartość X nowej rotacji
    void rotateX(double d);
    //! Pobiera aktywna serie i zminia jej węzeł przeznaczony dla manipulatorów
    //! \param d wartość Y nowej rotacji
    void rotateY(double d);
    //! Pobiera aktywna serie i zminia jej węzeł przeznaczony dla manipulatorów
    //! \param d wartość Z nowej rotacji
    void rotateZ(double d);
    //! Pobiera aktywna serie i zminia jej węzeł przeznaczony dla manipulatorów
    //! \param d wartość X nowej skali
    void scaleX(double d);
    //! Pobiera aktywna serie i zminia jej węzeł przeznaczony dla manipulatorów
    //! \param d wartość Y nowej skali
    void scaleY(double d);
    //! Pobiera aktywna serie i zminia jej węzeł przeznaczony dla manipulatorów
    //! \param d wartość Z nowej skali
    void scaleZ(double d);
    //! usuwa działanie manipulatorów
    void resetTransform();
	//! Ustawia widok sceny - strona lewa
	void setLeft();
	//! Ustawia widok sceny - strona prawa
	void setRight();
	//! Ustawia widok sceny - od przodu
	void setFront();
	//! Ustawia widok sceny - z tylu
	void setBehind();
	//! Ustawia widok sceny - z gory
	void setTop();
	//! Ustawia widok sceny - od dolu
	void setBottom();
    //! zmienił się stan manipulatora
    void draggerTriggered();

    void showGhost(bool);
	void showJointsOrientations();

	void showBodyPlanes();

private:
    //! główny węzeł sceny 3d
    osg::ref_ptr<osg::Group> rootNode;
    //! węzeł, pod który podpinane są węzły serii
    TransformPtr transformNode;
    //! manipulator kamery (myszka + klawiatura)
	osg::ref_ptr<osgGA::OrbitManipulator> cameraManipulator;
    //! węzeł ze wskaźnikiem aktywnej serii
    TransformPtr indicatorNode;
    //! zwracany widget wizualizatora
    osg::ref_ptr<osgui::QOsgDefaultWidget> widget;
    //! pokazuje dialog z trajektoriami
    coreUI::CoreAction* actionTrajectories = nullptr;
    //! pokazuje dialog umożliwiający chowanie węzłów z geometria
    coreUI::CoreAction* actionScheme = nullptr;
    //! pokazuje ghosta dla markerów
    coreUI::CoreAction* actionGhost = nullptr;
    //! zamienia osie jeśli jest taka potrzeba
    coreUI::CoreAction* actionSwitchAxes = nullptr;
    //! wywołuje stan 'pickera' - można wybrać aktywna serie klikając na nia myszka
    coreUI::CoreAction* pickerAction = nullptr;
    //! wywołuje manipulator translacji
    coreUI::CoreAction* translateAction = nullptr;
    //! wywołuje manipulator rotacji
    coreUI::CoreAction* rotateAction = nullptr;
    //! wywołuje manipulator skali
    coreUI::CoreAction* scaleAction = nullptr;
    //! niweluje działanie manipulatorów
    coreUI::CoreAction* resetAction = nullptr;
    //! Dialog zarządzający trajektoriami
    TrajectoriesDialog* trajectoriesDialog = nullptr;
    //! Dialog zarządzający widocznością węzłów na scenie
    SchemeDialog* schemeDialog;
    //! kolecja z seriami danych podpiętymi pod wizualizator
    std::vector<KinematicSerieBase*> series;
    //! nr aktywnej serii
	int currentSerie;
    //! spinbox pokazujący translacje X
    LabeledDoubleSpinBox translateSpinWidgetX;
    //! spinbox pokazujący translacje Y
    LabeledDoubleSpinBox translateSpinWidgetY;
    //! spinbox pokazujący translacje Z
    LabeledDoubleSpinBox translateSpinWidgetZ;
    //! spinbox pokazujący rotacje X
    LabeledDoubleSpinBox rotateSpinWidgetX;
    //! spinbox pokazujący rotacje Y
    LabeledDoubleSpinBox rotateSpinWidgetY;
    //! spinbox pokazujący rotacje Z
    LabeledDoubleSpinBox rotateSpinWidgetZ;
    //! spinbox pokazujący skale X
    LabeledDoubleSpinBox scaleSpinWidgetX;
    //! spinbox pokazujący skale Y
    LabeledDoubleSpinBox scaleSpinWidgetY;
    //! spinbox pokazujący skale Z
    LabeledDoubleSpinBox scaleSpinWidgetZ;
    //! manipulator translacji
    DraggerContainerPtr translateDragger;
    //! manipulator rotacji
    DraggerContainerPtr rotationDragger;
    //! manipulator skali
    DraggerContainerPtr scaleDragger;
    //! obecnie wybrany manipulator lub nullptr jeśli takiego nie ma
    DraggerContainerPtr currentDragger;
    //! ostatnio ustawiony czas, pomocne przy odświeżaniu wizualizatora
    double lastTime;

	bool AxisXYZ = false;

	std::atomic<bool> innerUpdate;
};

//! Klasa pomocnicza do obsługi komunikatów pochodzących od manipulatorów
class KinematicDraggerCallback : public osgManipulator::DraggerCallback
{
public:
    //! Obiekt jest zaprzyjaźniony z KinematicVisualizer
    //! \param kv obsługiwany wizualizator
    KinematicDraggerCallback(KinematicVisualizer* kv) : visualizer(kv) {}

public:
    //! wywoływane, gdy stan manipulatora się zmienił
    //! \param mc informacja, jakiego typu zaszła zmiana
    virtual bool receive( const osgManipulator::MotionCommand& mc);
private:
    //! Obiekt, na którego scenie działają manipulatory
    KinematicVisualizer* visualizer;
};


#endif  //  HEADER_GUARD___KINEMATICVISUALIZER_H__
