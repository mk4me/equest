/********************************************************************
	created:	2011/08/29
	created:	29:8:2011   9:25
	filename: 	GRFDrawer.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_KINEMATIC__GRFSERIE_H__
#define HEADER_GUARD_KINEMATIC__GRFSERIE_H__

#include <list>
#include <corelib/IVisualizer.h>
#include <c3dlib/IForcePlatform.h>
#include <osg/Geode>
#include "KinematicVisualizer.h"

//! empirycznie dobrany współczynnik długości wizualizowanych sił
const float grfScale = 0.0008f;

//! Seria danych odpowiedzialna za wizualizacje płyt naciskowych i sił na nie działających
class GRFSerie : public KinematicTimeSerie
{
public:
	typedef osg::ref_ptr<osg::Geode> GeodePtr;
	typedef osg::ref_ptr<osg::Group> GroupPtr;
	typedef osg::ref_ptr<osg::Geometry> GeometryPtr;
	typedef osg::ref_ptr<osg::PositionAttitudeTransform> TransformPtr;
	typedef osg::ref_ptr<osg::ShapeDrawable> ShapeDrawablePtr;

public:
	//! Konstruktor
	//! \param visualizer wizualizator tworzący ta serie
	GRFSerie(KinematicVisualizer * visualizer, const utils::TypeInfo & requestedType,
		const core::VariantConstPtr & data);

public:
	//! Ustawia nazwę serii
	//! \param name ustawiana nazwa 
	virtual void setName(const std::string & name);
    //! \return nazwa serii
    virtual const std::string getName() const;

	virtual void update();
    //! \return ustawione dane
    virtual const core::VariantConstPtr & getData() const;

	virtual const utils::TypeInfo & getRequestedDataType() const;
	//! \return długość kanału w sekundach
	virtual double getLength() const;
	virtual double getBegin() const;
	virtual double getEnd() const;
	//! Czas zawiera się między 0 a getLength()
	//! \param time Aktualny, lokalny czas kanału w sekundach
	virtual void setLocalTime(double time);

	virtual osg::Matrix getInitialMatrix() const;

private:
    //! Struktura pomocna przy wizualizacji wektora siły.
    //! Wektor składa się z prostopadloscianu i stozka
	struct Arrow 
	{
        //! główny węzeł z wektorem
		TransformPtr mainPtr;
        //! węzeł z prostopadloscianem
		TransformPtr boxPtr;
        //! węzeł ze stozkiem
		TransformPtr conePtr;
        //! geometria prostopadloscianu
		ShapeDrawablePtr boxShape; 
        //! geometria stozka
		ShapeDrawablePtr coneShape;

		//! Ustawienie transformacji, w taki sposób, aby wektor znalazl się pomiędzy dwoma punktami
		//! \param from punkt, od którego prowadzony jest wektor
		//! \param to punkt, do którego prowadzone jest wektor
		void setArrow(osg::Vec3 from, osg::Vec3 to);
		//! Ustawienie koloru wektora
		//! \param color kolor do ustawienia RGBA (0 - 1)
		void setColor(const osg::Vec4& color);
		//! \return aktualny kolor wektora
		const osg::Vec4& getColor() const;
	};
	typedef utils::shared_ptr<Arrow> ArrowPtr;
	typedef utils::shared_ptr<const Arrow> ArrowConstPtr;

	typedef std::map<c3dlib::IForcePlatformConstPtr, osg::ref_ptr<osg::ShapeDrawable> > Platform2Shape;

    //! Klasa ułatwia rysowanie "ducha" strzałki. Wizualizowane są poprzednie pozycje strzałki. Im starcza pozycja tym mniejsza alfa.
	class GhostStack
	{
	public:
        //! Stan strzałki, pozycje - od - do
		typedef std::pair<osg::Vec3, osg::Vec3> ArrowState;
		//! Konstruktor
		//! \param maxSize maksymalny rozmiar stosu strzałek
		//! \param hookNode węzeł, pod który beda podczepiane strzałki
		//! \param color kolor strzałek
		GhostStack(int maxSize, GroupPtr hookNode, const osg::Vec4& color, osg::StateSet * stateset);
        //! Dodanie strzałki do stosu i usuwa najstarsza strzałke, jeśli jest taka potrzeba
        //! \param state 
        void addState(const ArrowState& state);
		//! Aktualizacja kolorów strzałek, zarządzanie widocznością węzłów
		void update();

	private:
        //! Kolor strzałek
		osg::Vec4 color;
        //! Stos ze strzałkami
		std::list<ArrowPtr> stackArrows;
        //! węzeł, pod który podpięte są strzałki
		GroupPtr hookNode;
        //! maksymalny rozmiar stosu
		int maxSize;
	};
	typedef utils::shared_ptr<GhostStack> GhostStackPtr;
	typedef utils::shared_ptr<const GhostStack> GhostStackConstPtr;

private:
	//! Tworzy węzeł z geometria wszystkich płyt GRF
	//! \param platforms kolecja platform pochodzących z C3D
    //! \return utworzony węzeł
	GroupPtr createPlatformsGroup(const c3dlib::IForcePlatformCollection& platforms);
	//! Tworzy "skrzydla motyla" czyli graficzna reprezentacje zmiany siły nacisku konkretnej płyty w czasie
	//! \param grf dane płyty GRF
	//! \param maxLength tutaj zwracana jest maksymalna długość siły, jest to pomocne przy kolorowaniu wektora
    //! \return utworzony węzeł
	GroupPtr createButterfly(c3dlib::GRFCollectionConstPtr grf, float& maxLength) const;
    //! Tworzy pojedyncze "skrzydla motyla" czyli graficzna reprezentacje zmiany siły nacisku konkretnej płyty w czasie
    //! \param step wykryty wczesniej krok w obrębie płyty pomiarowej
    //! \param maxLength maksymalna długość siły, jest to pomocne przy kolorowaniu wektora
    //! \param platform plaforma, dla której tworzona jest geometria
    //! \return utworzony węzeł 
	GeodePtr createStep(c3dlib::IForcePlatform::IStepConstPtr step, float &maxLength, c3dlib::IForcePlatformConstPtr platform, osg::StateSet * stateset) const;
    //! Tworzy węzeł z geometria pojedynczej płyty GRF
    //! \param texture tekstura, która ma być na prostopadloscianie, może być nullptr
    //! \param origin punkt, w którym znajduje się lewy górny róg płyty
    //! \param width szerokość płyty
    //! \param lenght długość płyty
    //! \param height wysokość płyty
    //! \return utworzony węzeł
    TransformPtr createPlatformTransform(osg::Texture2D* texture, const osg::Vec3& origin, float width, float lenght, float height) const;
	//! Statyczna metoda, tworzy strukture z geometria i opisem strzałki
    //! \return utworzony węzeł
	static ArrowPtr createArrow(osg::StateSet * stateset);
	//! Pobiera teksture z resource'ów
	//! \param number obecnie 1 lub 2
    //! \return zaladowana tekstura, wyjątek w przypadku bledu
	static osg::ref_ptr<osg::Texture2D> getTexture(int number);
    //! próbuje pobrać teksture z resource'ów
    //! \param ret tutaj trafi pobrana tekstura
    //! \param number obecnie 1 lub 2
    //! \return czy operacja się udala
    static bool tryGetTexture(osg::ref_ptr<osg::Texture2D>& ret, int number);
	
private:
    //! Wizualizator, który utworzył serie
	KinematicVisualizer * visualizer;
    //! Dane GRF dostarczone serii
	c3dlib::GRFCollectionConstPtr grfCollection;
    //! maksymalna długość siły, pomocne przy kolorowaniu wektora
	float maxLength;
    //! mapa (krok -> (strzałka, jej ghost) pomocne przy rysowaniu i odświeżaniu wizualizacji
	std::map<c3dlib::IForcePlatform::IStepConstPtr, std::pair<ArrowPtr, GhostStackPtr>> stepsData;
    //! pobrana z zasobów tekstura dla pierwszej płyty
	static osg::ref_ptr<osg::Texture2D> texture1;
    //! pobrana z zasobów tekstura dla drugiej płyty
    static osg::ref_ptr<osg::Texture2D> texture2;
    //! wrapper przekazany serii
    core::VariantConstPtr data;

	utils::TypeInfo requestedType;
    //! nazwa serii
    std::string name;
    //! mapa, która ułatwia pobranie geometrii na postawie platformy z C3D
    Platform2Shape platform2Shape;

	double minTime;

	double maxTime;
};




#endif
