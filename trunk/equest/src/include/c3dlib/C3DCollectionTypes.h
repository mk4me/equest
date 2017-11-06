/********************************************************************
	created:	2015/03/20
	created:	10:23
	filename: 	C3DCollectionTypes.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_C3DLIB__C3DCollectionTypes_H__
#define HEADER_GUARD_C3DLIB__C3DCollectionTypes_H__

#include <viconformatslib/VskParser.h>
#include <c3dlib/IForcePlatform.h>
#include <c3dlib/C3DTypes.h>
#include <c3dlib/Export.h>

namespace  c3dlib {

	//! Prosta kolekcja przechowuj¹ca wszystkie zdarzenia z pliku c3d
	class C3DLIB_EXPORT C3DEventsCollection
	{
	public:
		//! wskaŸnik na zdarzenie wczytane z pliku C3D
		using EventPtr = utils::shared_ptr<c3dlib::C3DParser::IEvent>;
		//! niemodyfikowalny wskaŸnik na zdarzenie wczytane z pliku C3D
		using EventConstPtr = utils::shared_ptr<const c3dlib::C3DParser::IEvent>;
		//! kolekcja zdarzeñ wczytanych z pliku C3D
		using Collection = std::vector<EventPtr>;
		//! iterator kolekcji zdarzeñ wczytanych z pliku C3D
		using iterator = Collection::iterator;
		//! niemodyfikowalny iterator kolekcji zdarzeñ wczytanych z pliku C3D
		using const_iterator = Collection::const_iterator;
		//! typ zdarzenia wczytanego z pliku C3D
		using IEvent = c3dlib::C3DParser::IEvent;
		//! kontekst zdarzenia (strona lewa, prawa, ... )
		using Context =  IEvent::Context;

	private:
		//! kolekcja przechowuje zdarzenia wczytane z pliku c3d
		Collection events;
		// wyglada na to, ze VS2010 ma blad - wrzucenie nag³ówka zawieraj¹cego funkcje lambda
		// do nag³ówków prekompilowanych skutkuje uniemo¿liwieniem korzystania z lambdy wszedzie indziej
		// porownywanie eventów odbywa siê zatem w 'klasyczny' sposób
		struct C3DLIB_EXPORT EventFunctor : public std::binary_function < EventPtr, EventPtr, bool >
		{
			bool operator()(const EventPtr& e1, const EventPtr& e2);
		};

	public:
		//! Konstruktor
		C3DEventsCollection();
		//! Konstruktor kopiuj¹cy
		C3DEventsCollection(const C3DEventsCollection& es);
		C3DEventsCollection(C3DEventsCollection&& es);

	public:
		//! \return liczba wczytanych zdarzeñ
		int getNumEvents() const;
		//! pobranie zdarzenia
		//! \param index indeks z zakresu <0 , getNumEvents)
		EventPtr getEvent(int index);
		//! pobranie zdarzenia (wersja niemodyfikowalna)
		//! \param index indeks z zakresu <0 , getNumEvents)
		EventConstPtr getEvent(int index) const;
		//! \return przepisany, stl-owy iterator
		iterator begin();
		//! \return przepisany, stl-owy iterator
		iterator end();
		//! \return przepisany, stl-owy iterator
		const_iterator cbegin() const;
		//! \return przepisany, stl-owy iterator
		const_iterator cend() const;
		//! dodanie zdarzenia do kolekcji
		//! \param event
		void addEvent(EventPtr event);
		//! Zwraca event o okreœlonym kontekscie; nastepny do danego
		//! \param event konkretny event
		//! \param context konteks, który musi mieæ zwracany event
		//! \return event, który spe³nia za³o¿enia lub pusty wskaŸnik
		EventConstPtr getNextEvent(EventConstPtr event, Context context) const;
		//! Zwraca event dla podanego czasu, event musi mieæ odpowiedni kontekst,
		//! czas rozpoczêcia mniejszy lub rowny t i nie mo¿e byæ ostatnim eventem w kolekcji
		//! \param t czas, dla którego wyszukiwany jest event
		//! \param context kontekst, dla którego wyszukiwany jest event (lewy, prawy.. )
		//! \return event, który spe³nia za³o¿enia lub pusty wskaŸnik
		EventConstPtr getEvent(float t, Context context) const;
	};
	using EventsCollectionPtr = utils::shared_ptr<C3DEventsCollection>;
	using EventsCollectionConstPtr = utils::shared_ptr<const C3DEventsCollection>;

	//! Kolekcja obiektów EMG, obs³ugiwana w standardowy sposób
	using EMGCollection = dataaccessor::AccessorsCollection<EMGChannel>;
	using EMGCollectionPtr = utils::shared_ptr<EMGCollection>;
	using EMGCollectionConstPtr = utils::shared_ptr<const EMGCollection>;

	//! Kolekcja kana³ów opartych o trójwymiarowy wektor
	using VectorChannelCollection = dataaccessor::AccessorsCollection<VectorChannelReaderInterface>;
	using VectorChannelCollectionPtr = utils::shared_ptr<VectorChannelCollection >;
	using VectorChannelCollectionConstPtr = utils::shared_ptr<const VectorChannelCollection >;

	//! para liczb zmiennoprzecinkowych, s³u¿¹ca do okreœlenia przedzia³u czasowego zdarzenia
	using FloatPairPtr = utils::shared_ptr<std::pair<float, float>>;
	//! Metoda wydziela przedzia³y czasowe, dla których realizowana jest analiza
	//! \param events zdarzenia, z których beda wyci¹gane przedzia³y
	//! \param context kontekst kroku (lewy, prawy)
	C3DLIB_EXPORT std::vector<FloatPairPtr> getTimeSegments(EventsCollectionConstPtr events, C3DEventsCollection::Context context);

	//! Kolekcja dostarcza równie¿ informacji o p³ytach GFR oraz u³atwia pobranie konkretnego kana³u
	class C3DLIB_EXPORT GRFCollection : public dataaccessor::AccessorsCollection<GRFChannel>
	{
	public:
		//! \return wszystkie wczytane p³yty pomiarowe zwi¹zane z danymi GRF
		const IForcePlatformCollection& getPlatforms() const;
		//! Ustawienie p³yt pomiarowych
		//! \param val
		void setPlatforms(const IForcePlatformCollection& val);
		//! Metoda u³atwia pobranie kana³u GRF o odpowiednim typie
		//! \param type typ kana³u (np. M1 lub F2)
		GRFChannelConstPtr getGRFChannel(GRFChannel::Type type) const;

	private:
		//! Platformy pomiarowe zwi¹zane z t¹ sam¹ prób¹ pomiarow¹ co kana³y z kolekcji
		IForcePlatformCollection platforms;
	};
	using GRFCollectionPtr = utils::shared_ptr<GRFCollection>;
	using GRFCollectionConstPtr = utils::shared_ptr<const GRFCollection>;

	//! Kontener wszystkich markerów modelu, u³atwia obs³ugê danych, dodaje wsparcie o pliki VSK
	class C3DLIB_EXPORT MarkerCollection : public dataaccessor::AccessorsCollection<MarkerChannel>
	{
	public:
		//! Konstruktor
		//! \param vsk parser ze wczytanym plikiem vsk (dziêki niemu tworz¹ siê kolorowe po³¹czenia midzy markerami)
		MarkerCollection(vicon::VskPtr vsk = vicon::VskPtr());
		//! Zwraca nazwê markera na podstawie indeksu
		//! \param markerNo poprawny indeks, w przeciwnym razie poleci asercja
		virtual std::string getMarkerName(int markerNo) const;
		//! próbuje pobraæ kana³ na podstawie nazwy
		//! \param name nazwa kana³u, wielkoœæ liter ma znaczenie
		//! \return kana³ lub null jeœli takiego nie ma
		MarkerChannelConstPtr tryGetChannelByName(const std::string& name);
		//! \return parser vsk zwi¹zany z kolekcj¹
		vicon::VskConstPtr getVsk() const;

	private:
		//! parser vsk zwi¹zany z kolekcj¹
		vicon::VskPtr vsk;
	};
	using MarkerCollectionPtr = utils::shared_ptr<MarkerCollection>;
	using MarkerCollectionConstPtr = utils::shared_ptr<const MarkerCollection>;

	//typedef std::vector<double> MovieDelays;
	DEFINE_SMART_POINTERS(MovieDelays);

	//! makro u³atwia definicje dodatkowych kana³ów, osobne typy u³atwiaja otrzymanie konkretnych danych z DM
#define DEFINE_CHANNEL_COLLECTION(name)                                         \
class C3DLIB_EXPORT name##Collection : public VectorChannelCollection {};				        \
    using name##CollectionPtr = utils::shared_ptr<name##Collection>;				\
    using name##CollectionConstPtr = utils::shared_ptr<const name##Collection>;


	DEFINE_CHANNEL_COLLECTION(Force);
	DEFINE_CHANNEL_COLLECTION(Moment);
	DEFINE_CHANNEL_COLLECTION(Angle);
	DEFINE_CHANNEL_COLLECTION(Power);
}

#endif // HEADER_GUARD 
