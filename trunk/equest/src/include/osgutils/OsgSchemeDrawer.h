#ifndef HEADER_GUARD_OSGSCHEMEDRAWER_H__
#define HEADER_GUARD_OSGSCHEMEDRAWER_H__

#include <osg/Node>
#include <osg/Switch>
#include <osg/ShapeDrawable>
#include <osg/ref_ptr>
#include <utils/SmartPtr.h>

namespace osgutils
{

//! Typ definiujący indeksy na połączonych punktach
typedef std::pair<unsigned int, unsigned int> SegmentRange;

//! Opis segmentu
struct SegmentDescriptor
{
	float length;			//! Długość połączenia
	SegmentRange range;		//! indeksy punktów opisujacych segment
};

typedef std::vector<SegmentDescriptor> SegmentsDescriptors;

osg::Vec3Array * convert(const std::vector<osg::Vec3> & input);

//! Interfejs bazowego drawera
class IOsgSchemeDrawer
{
public:
	//! Destruktor wirtualny
	virtual ~IOsgSchemeDrawer() {}
	//! \return Zwracać węzeł drawera
	virtual osg::ref_ptr<osg::Node> getNode() = 0;
};
DEFINE_SMART_POINTERS(IOsgSchemeDrawer);

class IBaseDrawerScheme
{
public:
	//! \param color Kolor
	virtual void setColor(const osg::Vec4 & color) = 0;
	//! \param idx Indeks obiektu
	//! \param color Kolor
	virtual void setColor(const unsigned int idx, const osg::Vec4 & color) = 0;
	//! \param idx Indeks obiektu
	//! \return Kolor obiektu
	virtual const osg::Vec4 & color(const unsigned int idx) const = 0;
	//! \param visible Czy węzeł jest widoczny
	virtual void setVisible(const bool visible) = 0;
	//! \param idx Indeks obiektu
	//! \param visible Czy węzeł jest widoczny
	virtual void setVisible(const unsigned int idx, const bool visible) = 0;
	//! \param idx Indeks obiektu
	//! \return Widocznośc obiektu
	virtual const bool visible(const unsigned int idx) const = 0;
	//! \param size Rozmiar
	virtual void setSize(const float size) = 0;
	//! \param idx Indeks obiektu
	//! \param size Rozmiar
	virtual void setSize(const unsigned int idx, const float size) = 0;
	//! \param idx Indeks obiektu
	//! \return Rozmiar
	virtual const float size(const unsigned int idx) const = 0;
};
DEFINE_SMART_POINTERS(IBaseDrawerScheme)

class IPointsSchemeDrawer : public IOsgSchemeDrawer, public IBaseDrawerScheme
{
public:
	//! Destruktor wirtualny
	virtual ~IPointsSchemeDrawer() {}

	//! \param pointsNum Ilość punktów do wizualizacji
	virtual void init(const unsigned int pointsNum) = 0;
	//! \param positions Pozycje punktów
	virtual void update(const std::vector<osg::Vec3> & positions) = 0;
};
DEFINE_SMART_POINTERS(IPointsSchemeDrawer);
class PointInstance;

//! Drawer dla chmury punktów
class PointsDrawer : public IPointsSchemeDrawer
{
public:
	//! \param complexity Złożoność punktów
	PointsDrawer(const unsigned int complexity);

	//! \param pointsNum Ilość punktów
	virtual void init(const unsigned int pointsNum);
	//! \return Zwracać węzeł drawera
	virtual osg::ref_ptr<osg::Node> getNode();
	//! \param positions Pozycje punktów
	virtual void update(const std::vector<osg::Vec3> & positions);

	virtual void setSize(const float size);
	virtual void setColor(const osg::Vec4 & color);
	virtual void setVisible(const bool visible);
	virtual void setSize(const unsigned int idx, const float size);
	virtual void setColor(const unsigned int idx, const osg::Vec4 & color);
	virtual void setVisible(const unsigned int idx, const bool visible);

	virtual const osg::Vec4 & color(const unsigned int idx) const;
	virtual const bool visible(const unsigned int idx) const;
	virtual const float size(const unsigned int idx) const;

private:
		
	std::vector<utils::shared_ptr<PointInstance>> pointsInstances;

	unsigned int complexity;

	osg::ref_ptr<osg::Switch> node;
};
DEFINE_SMART_POINTERS(PointsDrawer);

class IConnectionsSchemeDrawer : public IOsgSchemeDrawer, public IBaseDrawerScheme
{
public:
	//! \param connections Schemat połączeń
	virtual void init(const SegmentsDescriptors & connections) = 0;
	//! Pozycje punktów dla których rysujemy połaczenia
	virtual void update(const std::vector<osg::Vec3> & positions) = 0;
};
DEFINE_SMART_POINTERS(IConnectionsSchemeDrawer)
typedef std::pair<IConnectionsSchemeDrawerPtr, SegmentsDescriptors> IConnectionDrawerWithDescriptors;
class ConnectionSphereInstance;

//! Drawer dla połączeń
class ConnectionsSphereDrawer : public IConnectionsSchemeDrawer
{
public:

	ConnectionsSphereDrawer(const unsigned int complexity);

	//! \param connections Schemat połączeń
	virtual void init(const SegmentsDescriptors & connections);
	//! \return Zwracać węzeł drawera
	virtual osg::ref_ptr<osg::Node> getNode();
	//! Pozycje punktów dla których rysujemy połaczenia
	virtual void update(const std::vector<osg::Vec3> & positions);

	virtual void setSize(const float size);
	virtual void setColor(const osg::Vec4 & color);
	virtual void setVisible(const bool visible);
	virtual void setSize(const unsigned int idx, const float size);
	virtual void setColor(const unsigned int idx, const osg::Vec4 & color);
	virtual void setVisible(const unsigned int idx, const bool visible);

	virtual const osg::Vec4 & color(const unsigned int idx) const;
	virtual const bool visible(const unsigned int idx) const;
	virtual const float size(const unsigned int idx) const;

private:
	typedef osg::ref_ptr<osg::ShapeDrawable> ShapeDrawablePtr;	
	osg::ref_ptr<osg::Switch> node;
	unsigned int complexity;
	std::vector<utils::shared_ptr<ConnectionSphereInstance>> connectionsInstances;
};
DEFINE_SMART_POINTERS(ConnectionsSphereDrawer);

class ConnectionInstance;
// TODO wydzielicz czesc wspolna dla ConnectionsDrawer i ConnectionsSphereDrawer
//! Drawer dla połączeń
class ConnectionsDrawer : public IConnectionsSchemeDrawer
{
public:

	ConnectionsDrawer(const unsigned int complexity);

	//! \param connections Schemat połączeń
	virtual void init(const SegmentsDescriptors & connections);
	//! \return Zwracać węzeł drawera
	virtual osg::ref_ptr<osg::Node> getNode();
	//! Pozycje punktów dla których rysujemy połaczenia
	virtual void update(const std::vector<osg::Vec3> & positions);

	virtual void setSize(const float size);
	virtual void setColor(const osg::Vec4 & color);
	virtual void setVisible(const bool visible);
	virtual void setSize(const unsigned int idx, const float size);
	virtual void setColor(const unsigned int idx, const osg::Vec4 & color);
	virtual void setVisible(const unsigned int idx, const bool visible);

	virtual const osg::Vec4 & color(const unsigned int idx) const;
	virtual const bool visible(const unsigned int idx) const;
	virtual const float size(const unsigned int idx) const;

private:
	//! Schemat połączeń
	std::vector<utils::shared_ptr<ConnectionInstance>> connectionsInstances;

	unsigned int complexity;

	std::map<unsigned int, std::pair<osg::Vec3, osg::Vec3>> updateCache;

	osg::ref_ptr<osg::Switch> node;
};
DEFINE_SMART_POINTERS(ConnectionsDrawer);

//! Interfejs drawera o zmienny zakresie danych
class IRangeDrawer
{
public:
	//! Zakres dadnych
	//! Początek - Koniec
	typedef std::pair<int, int> Range;

public:
	//! Destruktor wirtualny
	virtual ~IRangeDrawer() {}
	//! \param range Zakres danych
	virtual void setRange(const Range & range) = 0;
};
DEFINE_SMART_POINTERS(IRangeDrawer);

class TrajectoryInstance;

//! Drawer dla trajektorii
class TrajectorySchemeDrawer : public IOsgSchemeDrawer, public IRangeDrawer
{
public:
    TrajectorySchemeDrawer();
	void init(const std::vector<osg::Vec3> & points);
	//! \return Zwracać węzeł drawera
	virtual osg::ref_ptr<osg::Node> getNode();

	void update();

	void setColor(const osg::Vec4 & color);
	void setWidth(const float width);
	virtual void setRange(const Range & range);

	const osg::Vec4 & color() const;
	const float width() const;

    Range range() const;
    int count() const;

private:
    int pointCount;
	utils::shared_ptr<TrajectoryInstance> trajectory;
};

class GhostInstance;

//! Drawer dla ducha
class GhostSchemeDrawer : public IOsgSchemeDrawer, public IRangeDrawer
{
public:

	GhostSchemeDrawer(const unsigned int pointComplexity,
		const unsigned int connectionComplexity);

	void init(const std::vector<std::vector<osg::Vec3>> & points,
		const SegmentsDescriptors & connections);
	//! \return Zwracać węzeł drawera
	virtual osg::ref_ptr<osg::Node> getNode();	

	virtual void setRange(const Range & range);

	IBaseDrawerScheme * connectionsDrawer();
	IBaseDrawerScheme * pointsDrawer();

private:
	const unsigned int pointComplexity;
	const unsigned int connectionComplexity;
	utils::shared_ptr<GhostInstance> ghostInstance;
};
}
#endif //HEADER_GUARD_OSGSCHEMEDRAWER_H__
