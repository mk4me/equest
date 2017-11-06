/********************************************************************
	created:	2011/05/30
	created:	30:5:2011   8:59
	filename: 	C3DParser.h
	author:		Wojciech Knieć

	purpose:
*********************************************************************/

#ifndef HEADER_GUARD_C3DLIB__C3DPARSER_H__
#define HEADER_GUARD_C3DLIB__C3DPARSER_H__

#include <string>
#include <vector>
#include <map>
#include <set>
#include <osg/Vec3>
#include <c3dlib/Export.h>
#include <utils/SmartPtr.h>

namespace c3dlib {

using MovieDelays = std::map<std::string, double>;

//! Opis platformy GRF
struct C3DLIB_EXPORT ForcePlatformStruct
{
	//! wierzchołki pojedynczej płyty
	osg::Vec3 corners[4];
	//! przesunięcie płyty w układzie odniesienia zgodnym z markerami
	osg::Vec3 origin;
    //! typ płyty, w aplikacji obslugiwany jest 2
    int type;
    //! lista indeksów kanałów analogowych podpiętych do płyty (np. F1 i M1 to kanały 1-6)
    std::set<std::string> channelLabels;
};
DEFINE_SMART_POINTERS(ForcePlatformStruct);
typedef std::vector<ForcePlatformStructPtr> ForcePlatformStructCollection;

//! Klasa służy do pobierania informacji z pliku C3D, przykrywa biblioteke btk
class C3DLIB_EXPORT C3DParser
{
public:
	//! Interfejs dostarcza opisu dla danych akwizycji (jest to część wspólna dla danych analogowych, punktów i zdarzeń)
	class IAquisitionEntry
	{
	public:
		virtual ~IAquisitionEntry() {}
	public:
		//! \return Etykieta danych akwizycji
		virtual const std::string& getLabel() const = 0;

		//! \return Opis danych akwizycji
		virtual const std::string& getDescription() const = 0;
	};
	DEFINE_SMART_POINTERS(IAquisitionEntry);

	//! Interfejs dostarcza potrzebnych danych nt. punktu c3d
    class IPoint : public IAquisitionEntry
    {
	public:
        enum Type {Marker, VirtualMarker, VirtualMarkerForFrame, Angle, Force, Moment, Power, Scalar};
	public:
        //! \return Typ punktu c3d
        virtual Type getType() const = 0;
        //! \return jednostka dla danych analogowych (np. mm)
		virtual const std::string& getUnit() const = 0;
		//! \param index nr próbki
        //! \return wartość próbki
		virtual osg::Vec3 getValue(int index) const = 0;
    };
	DEFINE_SMART_POINTERS(IPoint)

	//! Dostarcza danych dla kanału analogowego
    class IAnalog : public IAquisitionEntry
    {
	public:
        //! \return jednostka dla danych analogowych (np. mm)
		virtual const std::string& getUnit() const = 0;
        //! \return użyta skala w pliku C3D
		virtual double getScale() const = 0;
		//! \param index nr próbki
		//! pobiera wartość próbki
		virtual double getValue(int index) const = 0;
	};
	DEFINE_SMART_POINTERS(IAnalog);
    
	//! dostarcza danych o zdarzeniu zapisanym w c3d (np. dotknięcie stopą podłogi)
    class IEvent : public IAquisitionEntry
    {
    public:
        //! konteksty dla zdarzenia
        enum Context
        {
            //! zdarzenie nie związane z żadną stroną
            General,
            //! zdarzenia związane z lewą stroną
            Left,
            //! zdarzenia związane z prawą stroną
            Right
        };

	public:
        //! return kontekst zdarzenia (lewy, prawy)
        virtual Context getContext() const = 0;
        //! return dodatkowa informacja - nazwa badanej osoby
        virtual std::string getSubject() const = 0;
        //! return czas, dla którego wystąpiło zdarzenie
        virtual double getTime() const = 0;
		//! zwraca kopię obiektu, kopia musi być później zwolniona !
		virtual IEvent* clone() const = 0;
    };
	DEFINE_SMART_POINTERS(IEvent);

public:
    C3DParser();
    virtual ~C3DParser();

public:
    //! Wczytanie pliku
    //! \param filename
    void load(const std::string& filename);
    //! Zapis pliku
    //! \param filename
    void save(const std::string& filename);

    //! Wczytanie plików
    //! \param filenames kolekcja z nazwami plików
    //! \param importWarnings zwraca warningi podczas wczytywania plików
    void importFrom(const std::vector<std::string>& filenames, std::string& importWarnings);

public:
	//! \return czestotliwość dla wszystkich danych analogowych
	double getAnalogFrequency() const;
	//! \param indeks w kolekcji pobieranego kanału analogowego
	//! \return kanał analogowy
	IAnalogConstPtr getAnalog(int index) const;
	//! \return liczba kanałów analogowych
	int getNumAnalogs() const;
	//! \return liczba pojedynczych próbek w kanałach analogowych
	int getNumAnalogFrames() const;
	//! \return liczba próbek analogowych przypadających na próbę punktu
	int getNumberAnalogSamplePerFrame() const;
	//! \param index indeks w kolekcji pobieranego punktu
	//! \return punkt z c3d
	IPointConstPtr getPoint(int index) const;
	//! \return liczba punktów w kolekcji
	int getNumPoints() const;
	//! \return liczba zapisanych próbek dla punktow
	int getNumPointFrames() const;
	//! \return czestotliwość zapisu danych dla punktow
    double getPointFrequency() const;
    //! \return liczba eventów wczytanych z pliku c3d
	int getNumEvents() const;
	const MovieDelays& getMovieDelays() const;
    //! \return event o konkrentym indeksie (zgodnie z kolejnością w c3d)
	IEventPtr getEvent(int index) const;
    //! \return kolekcja z informacjami o platformach pomiarowych zapisanych w pliku 
	const ForcePlatformStructCollection & getForcePlatformsStruct() const { return forcePlatforms; }

private:
    //! realizacja idiomu "Cashire cat"
    //! http://en.wikipedia.org/wiki/Opaque_pointer
    class __data;
    //! wskaźnik do danych, które zostaną całkowicie ukryte
    //! przed użytkownikami biblioteki.
    __data* data;
	//! kolekcja punktów c3d
    std::vector<IPointPtr> points;
	//! kolekcja kanałów analogowych
    std::vector<IAnalogPtr> analogs;
	//! kolekcja zdarzeń
    std::vector<IEventPtr> events;
	//! płyty GRF
	ForcePlatformStructCollection forcePlatforms;
    //! przesunięcia video, zgodnie z kolejnością
	MovieDelays movieDelaysVector;
private:
	//! Zapewnia właściwe wczytywanie danych akwizycji
    void loadAcquisition();
	//! zwraca mnożnik, dzięki któremu można przekonwertować wartość do SI
	float getUnitScale(const std::string& unit) const;
};


}

#endif
