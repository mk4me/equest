/********************************************************************
	created:	2011/09/02
	created:	2:9:2011   9:28
	filename: 	MarkerSerie.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_KINEMATIC__MARKERSERIE_H__
#define HEADER_GUARD_KINEMATIC__MARKERSERIE_H__

#include <corelib/IVisualizer.h>
#include "KinematicSerie.h"
#include <plugins/c3d/C3DChannels.h>
#include <osgutils/OsgSchemeDrawer.h>
#include "TrajectoriesDrawer.h"

class KinematicVisualizer;
class TrajectoryDrawerManager;

//! Seria danych wizualizatora 3D wizualizująca markery z pliku C3D
//! \version 0.9.1
class MarkerSerie : public QObject, public KinematicTimeSerie, public IGhostSerie
{
	Q_OBJECT;
    
public:
    //! Konstuktor
    //! \param visualizer wizualizator, który stworzył serie danych
	MarkerSerie(KinematicVisualizer * visualizer,
		const utils::TypeInfo & requestedType,
		const core::VariantConstPtr & data);

public:
    //! Ustawia nowa nazwę serii
    virtual void setName(const std::string & name);
    //! \return nazwa serii
    virtual const std::string getName() const;
    //! \return ustawione dane
    virtual const core::VariantConstPtr & getData() const;

	virtual void update();

	virtual const utils::TypeInfo & getRequestedDataType() const;
	//! \return długość kanału w sekundach
	virtual double getLength() const;
	virtual double getBegin() const;
	virtual double getEnd() const;

    //! \return macierz serii z transformacją, która nie została zmieniona przez manipulatory
    virtual osg::Matrix getInitialMatrix() const;

	virtual const bool ghostVisible() const;
	virtual void setGhostVisible(const bool visible);
    TrajectoryDrawerManagerPtr getTrajectoriesManager() const;

	osgutils::IPointsSchemeDrawerPtr getPointsDrawer() const;
	osgutils::IConnectionDrawerWithDescriptors getConnectionsDrawer() const;

private:

	//! Oblicza typowa długość pomiędzy dwoma markerami, odległości liczone są 100 razy dla animacji, tworzony jest histogram
	//! \param channel1 pierwszy marker
	//! \param channel2 drugi marker
	//! \param epsilon przedział histogramu
	//! \return typowa długość między markerami
	static float getStickLength(c3dlib::VectorChannelReaderInterfaceConstPtr channel1, c3dlib::VectorChannelReaderInterfaceConstPtr channel2, float epsilon = 0.001f);

	const std::vector<std::vector<osg::Vec3>> createPointsPositions(const unsigned int density) const;

protected:
	//! Czas zawiera się między 0 a getLength()
	//! \param time Aktualny, lokalny czas kanału w sekundach
	virtual void setLocalTime(double time);
	
private:
    //! Wizualizator, który utworzył serie
	KinematicVisualizer * visualizer;
    //! wrapper przekazany serii
    core::VariantConstPtr data;
	//! Wypakowane markery
	c3dlib::MarkerCollectionConstPtr markersCollection;
	//! Czas faktycznej aktualizacji
	double lastUpdateTime;
	//! Informacja o zadanym typie
	utils::TypeInfo requestedType;
    //! nazwa serii
    std::string name;
	//! Konfiguracja połączeń
	osgutils::SegmentsDescriptors connectionsConfigurations;
	//! Obiekt rysujący punkty
	utils::shared_ptr<osgutils::IPointsSchemeDrawer> pointsDrawer;
    //! Obiekt rysujący połączenia
	utils::shared_ptr<osgutils::IConnectionsSchemeDrawer> connectionsDrawer;
    //! Klasa pomocnicza przy rysowaniu ducha
	utils::shared_ptr<osgutils::GhostSchemeDrawer> ghostDrawer;
	//! Klasa pomocnicza przy rysowaniu trajektorii
	TrajectoryDrawerManagerPtr trajectoriesManager;
	//!
	float sampleDuration;
};
typedef utils::shared_ptr<MarkerSerie> MarkerSeriePtr;
typedef utils::shared_ptr<const MarkerSerie> MarkerSerieConstPtr;


#endif
