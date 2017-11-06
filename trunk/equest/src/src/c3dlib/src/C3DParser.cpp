#include <utils/SmartPtr.h>
#include <utils/StreamTools.h>
#include <c3dlib/C3DParser.h>
#include <sstream>
#include <btkEvent.h>
#include <btkC3DFileIO.h>
#include <btkAcquisitionFileReader.h>
#include <btkAcquisitionFileWriter.h>
#include <stdexcept>
#include <btkProcessObject.h>
#include <btkAcquisition.h>
#include <btkSeparateKnownVirtualMarkersFilter.h>
#include <btkForcePlatformsExtractor.h>
#include <btkGroundReactionWrenchFilter.h>
#include <btkDownsampleFilter.h>
#include <btkPointCollection.h>
#include <btkForcePlatformCollection.h>
#include <btkWrenchCollection.h>
#include <btkAcquisitionFileIO.h>
#include <btkMetaDataUtils.h>
#include <list>
#include <btkMergeAcquisitionFilter.h>
#include <utils/Debug.h>

namespace c3dlib {

//! kanał analogowy, implementacja przykrywa obiekt btk::Analog
class Analog : public C3DParser::IAnalog
{
private:
    //! przez tą wartość zostaną przemnożone próbki
	double scale;
    //! wskaźnik do obiektu z biblioteki btk
	btk::Analog::ConstPointer analog;

public:
	//! Domyslny konstruktor - pusty
	Analog() : scale(1.0) {}

	//! Destruktor wirtualny
	virtual ~Analog() {}

	//! ustawia kanał anaglogowy
	//! \param val wczytany z pliku kanał w formacie btk
	void setAnalog(btk::Analog::ConstPointer val)	  { analog = val; }
	//! \return etykieta (nazwa) danego kanału
	virtual const std::string& getLabel() const		  { return analog->GetLabel(); }
	//! \return opis danego kanału
	virtual const std::string& getDescription() const { return analog->GetDescription(); }
	//! \return nazwa jednostki danego kanału
	virtual const std::string& getUnit() const	      { return analog->GetUnit(); }
	//! \return skala kanału
	virtual double getScale() const					  { return analog->GetScale(); }
	//! \return wartość konkretnej próbki
	virtual double getValue(int index) const		  { return analog->GetValues()[index]; }
};
DEFINE_SMART_POINTERS(Analog);

// pojedynczy punkt c3d
class Point : public C3DParser::IPoint
{
private:
    //! punkt w formacie btk wczytany z pliku
	btk::Point::ConstPointer point;
    //! liczba dostępnych próbek
	int numOfFrames;
    //! typ punktu c3d (marker, siła, ...)
	Type type;
	//! przez tą wartość zostaną przemnożone pobierane wartości
	float scale;
    //! nazwa jednostki danego kanału
    std::string unit;

public:
	Point() :
	  //point(nullptr),
	  numOfFrames(-1),
	  scale(1.0f),
      unit("unknown")
	  {

	  }

public:
    //! \return punkt w formacie btk wczytany z pliku
	btk::Point::ConstPointer getPoint() const { return point; }
    //! ustawia typ punktu c3d (marker, siła, ...)
	void setType(Point::Type type) { this->type = type; }
    //! ustawia punkt w formacie btk wczytany z pliku
	void setPoint(btk::Point::ConstPointer val) { point = val; }
    //! ustawia liczbę dostępnych próbek
	void setNumberOfFrames(int val) { numOfFrames = val; }
    //! \return skala danego kanału
	float getScale() const { return scale; }
    //! ustawia skalę danego kanału
	void setScale(float val) { scale = val; }
    //! ustawia nazwę jednostki dla kanału
    void setUnit(const std::string& val) { unit = val; }

public:
    //! \return Etykieta danych akwizycji
	virtual const std::string& getLabel() const		  { return point->GetLabel(); }
    //! \return Opis danych akwizycji
	virtual const std::string& getDescription() const { return point->GetDescription(); }
    //! \return nazwa jednostki dla kanału
    virtual const std::string& getUnit() const { return unit; }
    //! \return typ punktu c3d (marker, siła, ...)
	virtual Point::Type getType() const { return type; }
    //! \return wartość konkretnej próbki
	virtual osg::Vec3 getValue(int index) const
	{
		const auto& value = point->GetValues();
		return osg::Vec3(value[index ], value[index + numOfFrames], value[index + 2 * numOfFrames]) * scale;
	}
};
DEFINE_SMART_POINTERS(Point);

//! dostarcza danych o zdarzeniu zapisanym w c3d (np. dotknięcie stopą podłogi)
class Event : public C3DParser::IEvent
{
private:
    //! wkaźnik na obiekt btk zawierający informacje o zdarzeniu
	btk::Event::ConstPointer eventPtr;
    //! offset wynikający z przesuniecia w pliku c3d (first frame)
    double offset;

public:
    //! konstruktor
    Event() : offset(0.0), eventPtr()
    {}

public:
    //! \return kontekst zdarzenia (strona lewa, prawa ... )
	virtual Context getContext() const
	{
        std::string ctx = eventPtr->GetContext();
        if (ctx == "Left") {
            return Left;
        } else if (ctx == "Right") {
            return Right;
        } else {
            return General;
        }
	}
    //! \return nazwa pacjenta (zapisana w c3d)
	virtual std::string getSubject() const
	{
		return eventPtr->GetSubject();
	}
    //! \return czas, w którym wystąpiło zdarzenie
	virtual double getTime() const
	{
		return eventPtr->GetTime() - offset;
	}
    //! \return nr próbki, dla której wystąpiło zdarzenie
	virtual int getFrame() const
	{
		return eventPtr->GetFrame();
	}
    //! \return Etykieta danych akwizycji
	virtual const std::string& getLabel() const
	{
		return eventPtr->GetLabel();
	}
    //! \return Opis danych akwizycji
	virtual const std::string& getDescription() const
	{
		return eventPtr->GetDescription();
	}
    //! \return głęboka kopia obiektu
	virtual IEvent* clone() const
	{        
		return new Event(*this);
	}
    //! ustawia offset wynikający z przesuniecia w pliku c3d (first frame)
    void setOffset(double val) 
    {
        offset = val;
    }
public:
    //! \return wkaźnik na obiekt btk zawierający informacje o zdarzeniu
	btk::Event::ConstPointer getEventPtr() const { return eventPtr; }
    //! ustawia wkaźnik na obiekt btk zawierający informacje o zdarzeniu
	void setEventPtr(btk::Event::ConstPointer val) { eventPtr = val; }
};
DEFINE_SMART_POINTERS(Event)

//! Pola tej klasy agreguja te dane wykorzystywane wewnatrz klasy C3DParser,
//! które nie moga być widoczne w nagłówku
class C3DParser::__data
{
public:
    //! wskaźnik do danych akwizycji
    btk::Acquisition::Pointer aquisitionPointer;
    //! wskaźnik do klasy reprezentującej plik c3d
    btk::C3DFileIO::Pointer c3dPointer;
    //! obiekt służacy do odzielenia obiektów (punktów) wyestymowanych od pomierzonych
	btk::SeparateKnownVirtualMarkersFilter::Pointer virtualMarkersSeparator;
    //! obiekt służący do pobrania informacji o płytach pomiarowych
	btk::ForcePlatformsExtractor::Pointer forcePlatformsExtractor;
    //! region of intrest (ustawiane na pierwszą i ostatnią próbkę)
    int roi[2];
    //! obiekt pomocniczy, przechowuje nr pierwszej próbkę
    int firstFrame;
    //! obiekt pomocniczy, przechowuje nr ostatniej próbkę
    int lastFrame;

    __data() :
        firstFrame(0),
        lastFrame(0)
    {
        roi[0] = firstFrame;
        roi[1] = lastFrame;

        virtualMarkersSeparator = btk::SeparateKnownVirtualMarkersFilter::New();
        forcePlatformsExtractor = btk::ForcePlatformsExtractor::New();
    }
};

C3DParser::C3DParser() :
    data( new __data)
{

}

C3DParser::~C3DParser()
{
    delete data;
}


void C3DParser::save( const std::string& filename )
{
	UTILS_ASSERT(data->aquisitionPointer);
	btk::AcquisitionFileWriter::Pointer writer = btk::AcquisitionFileWriter::New();
	writer->SetFilename(filename);
	writer->SetInput(data->aquisitionPointer);
}

void C3DParser::load(const std::string& filename)
{
    std::string warnings;
	std::vector<std::string> filenames;
	filenames.push_back(filename);
	importFrom(filenames, warnings);
}

void C3DParser::importFrom( const std::vector<std::string>& filenames, std::string& importWarnings )
{
    // wczytywanie plików (niekoniecznie c3d)
    std::vector<btk::AcquisitionFileReader::Pointer> readers;
    try
    {
        for (unsigned int i = 0 ; i < filenames.size() ; ++i)
        {
            btk::AcquisitionFileReader::Pointer reader = btk::AcquisitionFileReader::New();
            reader->SetFilename(filenames[i]);
            reader->Update();
            readers.push_back(reader);
        }
    }
    catch (btk::Exception& e)
    {
        throw std::runtime_error(e.what());
    }
    // Check if the original acquisition need to be cropped
    if (this->data->aquisitionPointer)
    {
        if ((this->data->roi[0] != this->data->firstFrame) || (this->data->roi[1] != this->data->lastFrame))
        {
            // Resize the data.
            this->data->aquisitionPointer->ResizeFrameNumber(this->data->aquisitionPointer->GetPointFrameNumber() - (this->data->lastFrame - this->data->roi[1]));
            this->data->aquisitionPointer->ResizeFrameNumberFromEnd(this->data->aquisitionPointer->GetPointFrameNumber() - (this->data->firstFrame - this->data->roi[0]));
            // Remove the events outside of the bounds
            btk::EventCollection::Iterator it = this->data->aquisitionPointer->BeginEvent();
            while (it != this->data->aquisitionPointer->EndEvent())
            {
                if (((*it)->GetFrame() < this->data->roi[0]) || ((*it)->GetFrame() > this->data->roi[1]))
                    it = this->data->aquisitionPointer->RemoveEvent(it);
                else
                    ++it;
            }
        }
    }

    // łączenie danych z plików
    int shift = !this->data->aquisitionPointer ? 0 : 1;
	utils::StringStreamBufferGrabber err(std::cerr);
    btk::MergeAcquisitionFilter::Pointer merger = btk::MergeAcquisitionFilter::New();
    merger->SetInput(0, this->data->aquisitionPointer);
    for (unsigned int i = 0 ; i < readers.size() ; ++i) {
        merger->SetInput(i+shift, readers[i]->GetOutput());
    }
    merger->Update();
    this->data->aquisitionPointer = merger->GetOutput();
	// pobranie danych akwizycji
    this->loadAcquisition();
    importWarnings = err.str();
}

void C3DParser::loadAcquisition()
{
    std::string labelPrefix = "";
    btk::MetaData::ConstIterator itSubjects = this->data->aquisitionPointer->GetMetaData()->FindChild("SUBJECTS");
    if (itSubjects != this->data->aquisitionPointer->GetMetaData()->End())
    {
        btk::MetaDataInfo::Pointer labelPrefixesInfo = (*itSubjects)->ExtractChildInfo("LABEL_PREFIXES", btk::MetaDataInfo::Char, 2, false);
        if (labelPrefixesInfo)
        {
            labelPrefix = labelPrefixesInfo->ToString(0);
            labelPrefix = labelPrefix.erase(labelPrefix.find_last_not_of(' ') + 1);
            labelPrefix = labelPrefix.erase(0, labelPrefix.find_first_not_of(' '));
        }
    }

	btk::MetaData::ConstIterator itPoints = this->data->aquisitionPointer->GetMetaData()->FindChild("POINT");
	if (itPoints != this->data->aquisitionPointer->GetMetaData()->End())
	{
		btk::MetaDataInfo::Pointer movieDelays = (*itPoints)->ExtractChildInfo("MOVIE_DELAY", btk::MetaDataInfo::Real, 1, false);
		btk::MetaDataInfo::Pointer movieIDs = (*itPoints)->ExtractChildInfo("MOVIE_ID", btk::MetaDataInfo::Char, 2, false);
		if (movieDelays && movieIDs)
		{
			auto delays = movieDelays->ToDouble();
			auto ids = movieIDs->ToString();
			if (delays.size() == ids.size()) {
				for(auto& str : ids) {
					str.erase(std::remove_if(str.begin(), str.end(), ::isspace), str.end());
				}
				std::transform(ids.begin(), ids.end(), delays.begin(), std::inserter(movieDelaysVector, movieDelaysVector.end()), [](const std::string& a, double b) {
					return std::make_pair(a, b);
				});
			}
		}
	}

    this->data->virtualMarkersSeparator->SetInput(this->data->aquisitionPointer->GetPoints());
    this->data->forcePlatformsExtractor->SetInput(this->data->aquisitionPointer);

    this->data->virtualMarkersSeparator->SetLabelPrefix(labelPrefix);
    this->data->virtualMarkersSeparator->Update();

	this->data->forcePlatformsExtractor->Update();
	btk::ForcePlatformCollection::Pointer fp = this->data->forcePlatformsExtractor->GetOutput();
	float scale = getUnitScale("mm");
	for (btk::ForcePlatformCollection::ConstIterator it = fp->Begin(); it != fp->End(); ++it) {
		// system jest przygotowany dla 4 wierzchołków
		// nie zakłada się, ze platformy moga być innym wielokatem.
		if ((*it)->GetCorners().size() == 12) {
			ForcePlatformStructPtr platform(new ForcePlatformStruct());
			platform->origin[0] = (*it)->GetOrigin()[0];
			platform->origin[1] = (*it)->GetOrigin()[1];
			platform->origin[2] = (*it)->GetOrigin()[2];
			platform->origin *= scale;
			for (int i = 0; i < 4; ++i) {
				platform->corners[i].set(
					(*it)->GetCorner(i)(0,0),
					(*it)->GetCorner(i)(1,0),
					(*it)->GetCorner(i)(2,0)
					);
				platform->corners[i] *= scale;

			}
            platform->type = (*it)->GetType();
            int count = (*it)->GetChannelNumber();
            for (int i = 0; i < count; ++i) {
                auto channel = (*it)->GetChannel(i);
                platform->channelLabels.insert(channel->GetLabel());
            }
			forcePlatforms.push_back(platform);
		}
	}

    this->data->firstFrame = this->data->aquisitionPointer->GetFirstFrame();
    this->data->lastFrame = this->data->aquisitionPointer->GetLastFrame();
    this->data->roi[0] = this->data->firstFrame;
    this->data->roi[1] = this->data->lastFrame;

    btk::PointCollection::Pointer points = this->data->virtualMarkersSeparator->GetOutput(0);
    for (btk::PointCollection::ConstIterator it = points->Begin() ; it != points->End() ; ++it)
    {
        PointPtr p = utils::make_shared<Point>();
		p->setPoint(*it);
		p->setNumberOfFrames(this->getNumPointFrames());
		p->setType(Point::Marker);
		p->setScale(scale);
        p->setUnit(data->aquisitionPointer->GetPointUnit((*it)->GetType()));
		this->points.push_back(p);
    }
    // Virtual markers (CoM, CoG, ...)

    points = this->data->virtualMarkersSeparator->GetOutput(2);
    for (btk::PointCollection::ConstIterator it = points->Begin() ; it != points->End() ; ++it) {
        PointPtr p = utils::make_shared<Point>();
		p->setPoint(*it);
		p->setNumberOfFrames(this->getNumPointFrames());
		p->setType(Point::VirtualMarker);
		p->setScale(scale);
        p->setUnit(data->aquisitionPointer->GetPointUnit((*it)->GetType()));
        this->points.push_back(p);
    }
    // Virtual markers used to define frames
    points = this->data->virtualMarkersSeparator->GetOutput(1);
    for (btk::PointCollection::ConstIterator it = points->Begin() ; it != points->End() ; ++it) {
        PointPtr p = utils::make_shared<Point>();
		p->setPoint(*it);
		p->setNumberOfFrames(this->getNumPointFrames());
        p->setUnit(data->aquisitionPointer->GetPointUnit((*it)->GetType()));
		p->setType(Point::VirtualMarkerForFrame);
        this->points.push_back(p);
    }
    // Other points
    points = this->data->virtualMarkersSeparator->GetOutput(3);

    for (btk::PointCollection::ConstIterator it = points->Begin() ; it != points->End() ; ++it) {
        PointPtr p = utils::make_shared<Point>();
		p->setPoint(*it);
		p->setNumberOfFrames(this->getNumPointFrames());
		switch((*it)->GetType()) {
		case btk::Point::Angle: p->setType(Point::Angle); break;
		case btk::Point::Force: p->setType(Point::Force); break;
		case btk::Point::Moment: p->setType(Point::Moment); break;
		case btk::Point::Power: p->setType(Point::Power); break;
		case btk::Point::Scalar: p->setType(Point::Scalar); break;
		}
        p->setUnit(data->aquisitionPointer->GetPointUnit((*it)->GetType()));
        this->points.push_back(p);
    }


    for (btk::Acquisition::AnalogIterator it = this->data->aquisitionPointer->BeginAnalog() ; it != this->data->aquisitionPointer->EndAnalog() ; ++it) {
        AnalogPtr a = utils::make_shared<Analog>();
		a->setAnalog(*it);
		this->analogs.push_back(a);
    }
        
    double pointFrq = getPointFrequency();
    double offset = this->data->firstFrame / pointFrq;
    // Event
    for (btk::Acquisition::EventIterator it = this->data->aquisitionPointer->BeginEvent() ; it != this->data->aquisitionPointer->EndEvent() ; ++it) {
        EventPtr e = utils::make_shared<Event>();
        e->setEventPtr(*it);
        e->setOffset(offset);
        this->events.push_back(e);
    }
}

double C3DParser::getAnalogFrequency() const
{
	double res = this->data->aquisitionPointer->GetAnalogFrequency();
	return res;
}

C3DParser::IAnalogConstPtr C3DParser::getAnalog( int index ) const
{
	UTILS_ASSERT(index >= 0 && index < getNumAnalogs() && this->analogs[index]);
	return this->analogs[index];
}

int C3DParser::getNumAnalogs() const
{
	return analogs.size();
}

int C3DParser::getNumAnalogFrames() const
{
	return getNumPointFrames() * getNumberAnalogSamplePerFrame();
}

int C3DParser::getNumberAnalogSamplePerFrame() const
{
	int spf = data->aquisitionPointer->GetNumberAnalogSamplePerFrame();
	return spf;
}


C3DParser::IPointConstPtr C3DParser::getPoint( int index ) const
{
	UTILS_ASSERT(index >= 0 && index < getNumPoints() && this->points[index]);
	return this->points[index];
}


double C3DParser::getPointFrequency() const
{
	double tmp = data->aquisitionPointer->GetPointFrequency();
	return tmp;
}


int C3DParser::getNumPoints() const
{
	return this->data->aquisitionPointer->GetPointNumber();
}

int C3DParser::getNumPointFrames() const
{
	int tmp = data->aquisitionPointer->GetPointFrameNumber();
	return tmp;
}

int C3DParser::getNumEvents() const
{
	return events.size();
}

C3DParser::IEventPtr C3DParser::getEvent( int index ) const
{
	UTILS_ASSERT(index >= 0 && index < getNumEvents());
	return events[index];
}

float C3DParser::getUnitScale( const std::string& unit ) const
{
	if (unit == "m") return 1.0f;
	if (unit == "mm") return 0.001f;
	throw std::runtime_error("Unknown unit");
}

const MovieDelays& C3DParser::getMovieDelays() const
{
	return movieDelaysVector;
}

}
