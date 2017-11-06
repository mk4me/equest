#include "C3DPCH.h"
#include "C3DParser.h"

#include <fstream>
#include <string>
#include <vector>
#include <boost/tuple/tuple.hpp>
#include <c3dlib/C3DParser.h>
#include <utils/Filesystem.h>
#include <plugins/c3d/C3DChannels.h>
#include <plugins/c3d/C3DCollections.h>
#include "C3DParser.h"
#include <c3dlib/ForcePlatform.h>

C3DParser::C3DParser()
{
    
}

C3DParser::~C3DParser()
{

}

void C3DParser::initObjects(utils::ObjectsVector & objects)
{
	// sztywne stworzenie obiektów, zachowanie kompatybilności
	for (int i = 0; i < 4; ++i){
		objects.push_back(utils::ObjectWrapper::create<c3dlib::GRFChannel>());
	}
	for (int i = 0; i < 16; ++i){
		objects.push_back(utils::ObjectWrapper::create<c3dlib::EMGChannel>());
	}

	// GRF i EMG są dosepne tez poprzez standardowe kolekcje
	objects.push_back(utils::ObjectWrapper::create<c3dlib::GRFCollection>());
	objects.push_back(utils::ObjectWrapper::create<c3dlib::EMGCollection>());

	objects.push_back(utils::ObjectWrapper::create<c3dlib::C3DEventsCollection>());
	objects.push_back(utils::ObjectWrapper::create<c3dlib::MovieDelays>());

	// reszta kolekcji już bez udziwnien
	objects.push_back(utils::ObjectWrapper::create<c3dlib::MarkerCollection>());
	objects.push_back(utils::ObjectWrapper::create<c3dlib::ForceCollection>());
	objects.push_back(utils::ObjectWrapper::create<c3dlib::AngleCollection>());
	objects.push_back(utils::ObjectWrapper::create<c3dlib::MomentCollection>());
	objects.push_back(utils::ObjectWrapper::create<c3dlib::PowerCollection>());
}

void C3DParser::parse( const std::string & source  )
{
	utils::Filesystem::Path path(source);
	c3dlib::C3DParser parser;

    std::vector<std::string> files;
    files.push_back(path.string());
	std::string importWarnings;
    parser.importFrom(files, importWarnings);

	utils::ObjectsVector localData;

	initObjects(localData);

	auto ff = dataaccessor::FunctionFeature::feature(true);

    // wczytanie danych analogowych
	c3dlib::GRFCollectionPtr grfs(new c3dlib::GRFCollection());
	c3dlib::EMGCollectionPtr e(new c3dlib::EMGCollection());
    if (parser.getNumAnalogs() == 28)
    {
		//uniform + bounded args + function		
		auto uf = utils::make_shared<dataaccessor::UniformArgumentsFeature<c3dlib::GRFChannel::argument_type>>(1.0 / parser.getAnalogFrequency());
		auto abf = utils::make_shared<dataaccessor::BoundedArgumentsFeature<c3dlib::GRFChannel::argument_type>>(0.0, (parser.getNumAnalogFrames() - 1) / parser.getAnalogFrequency());				
        for (int i = 0; i < 4; ++i) {		

			c3dlib::GRFChannelPtr ptr(new c3dlib::GRFChannel(parser, i));		

			ptr->attachFeature(uf);
			ptr->attachFeature(abf);
			ptr->attachFeature(ff);

            localData[i]->set(ptr);			
            grfs->addAccessor(ptr);
        }

		localData[20]->set(grfs);        
		//uniform + bounded args
        for (int i = 12; i < 28; ++i) {
			c3dlib::EMGChannelPtr ptr(new c3dlib::EMGChannel(parser, i));
			ptr->attachFeature(uf);
			ptr->attachFeature(abf);

            localData[4+i-12]->set(ptr);			
			e->addAccessor(ptr);
        }
        
		localData[21]->set(e);        
    }

    // wczytanie eventów
	int count = parser.getNumEvents();
	c3dlib::EventsCollectionPtr allEventsCollection(new c3dlib::C3DEventsCollection());
	for (int i = 0; i < count; ++i) {
		c3dlib::C3DParser::IEventPtr event = parser.getEvent(i);
		c3dlib::C3DEventsCollection::EventPtr e(event->clone());
        allEventsCollection->addEvent(e);
	}
    
	localData[22]->set(allEventsCollection);	

	c3dlib::MovieDelaysPtr delays = utils::make_shared<c3dlib::MovieDelays>(parser.getMovieDelays());
	localData[23]->set(delays);	

    // wczytanie plików *vsk, które dostarczaja opis do markerów
    utils::Filesystem::Path dir = path.parent_path();
    auto vskFiles = utils::Filesystem::listFiles(dir, false, ".vsk");
    vicon::VskPtr vsk;
    if (vskFiles.size() == 1) {
		std::ifstream ifs(vskFiles.front().string());
		if (ifs.is_open() == true) {
			vsk = utils::make_shared<vicon::Vsk>(vicon::VskParser::parse(ifs));
		}
    }
	c3dlib::MarkerCollectionPtr markers(new c3dlib::MarkerCollection(vsk));
	c3dlib::ForceCollectionPtr forces(new c3dlib::ForceCollection);
	c3dlib::AngleCollectionPtr angles(new c3dlib::AngleCollection);
	c3dlib::MomentCollectionPtr moments(new c3dlib::MomentCollection);
	c3dlib::PowerCollectionPtr powers(new c3dlib::PowerCollection);

	localData[24]->set(markers);
	localData[25]->set(forces);
	localData[26]->set(angles);
	localData[27]->set(moments);
	localData[28]->set(powers);	

	const int markersCount = parser.getNumPoints();
	//uniform + bounded args + function		
	auto uf = utils::make_shared<dataaccessor::UniformArgumentsFeature<c3dlib::GRFChannel::argument_type>>(1.0 / parser.getPointFrequency());
	auto abf = utils::make_shared<dataaccessor::BoundedArgumentsFeature<c3dlib::GRFChannel::argument_type>>(0.0, (parser.getNumPointFrames() - 1) / parser.getPointFrequency());

	for (int i = 0; i < markersCount; ++i) {

		switch (parser.getPoint(i)->getType()) {
			case c3dlib::C3DParser::IPoint::Angle: {
				c3dlib::AngleChannelPtr ptr(new c3dlib::AngleChannel(parser, i));

				ptr->attachFeature(uf);
				ptr->attachFeature(abf);
				ptr->attachFeature(ff);

				angles->addAccessor(ptr);
            } break;
            case c3dlib::C3DParser::IPoint::Marker: {
				c3dlib::MarkerChannelPtr ptr(new c3dlib::MarkerChannel(parser, i));

				ptr->attachFeature(uf);
				ptr->attachFeature(abf);
				ptr->attachFeature(ff);

				markers->addAccessor(ptr);
            } break;
            case c3dlib::C3DParser::IPoint::Force: {
				c3dlib::ForceChannelPtr ptr(new c3dlib::ForceChannel(parser, i));

				ptr->attachFeature(uf);
				ptr->attachFeature(abf);
				ptr->attachFeature(ff);

				forces->addAccessor(ptr);
            } break;
            case c3dlib::C3DParser::IPoint::Moment: {
				c3dlib::MomentChannelPtr ptr(new c3dlib::MomentChannel(parser, i));

				ptr->attachFeature(uf);
				ptr->attachFeature(abf);
				ptr->attachFeature(ff);

				moments->addAccessor(ptr);
            } break;
            case c3dlib::C3DParser::IPoint::Power: {
				c3dlib::PowerChannelPtr ptr(new c3dlib::PowerChannel(parser, i));

				ptr->attachFeature(uf);
				ptr->attachFeature(abf);
				ptr->attachFeature(ff);

				powers->addAccessor(ptr);
            } break;
		}
	}

    try {
		c3dlib::IForcePlatformCollection platforms;
        auto parsedPlatforms = parser.getForcePlatformsStruct();
        int count = static_cast<int>(parsedPlatforms.size());
        for (int i = 0; i < count;  ++i) {
			c3dlib::ForcePlatformPtr fp(new c3dlib::ForcePlatform(parsedPlatforms[i]));
            // HACK ! POWINIEN BYC KANAL NA PODST. NR PLATFORMY!
			fp->setForceChannel(grfs->getGRFChannel(i == 0 ? c3dlib::GRFChannel::F1 : c3dlib::GRFChannel::F2));
			fp->setMomentChannel(grfs->getGRFChannel(i == 0 ? c3dlib::GRFChannel::M1 : c3dlib::GRFChannel::M2));
            fp->computeSteps(markers, allEventsCollection);
            platforms.push_back(fp);
        }
		grfs->setPlatforms(platforms);
	} catch(...) {}
	this->path = source;
	data.swap(localData);
}

void C3DParser::reset()
{
	std::string().swap(path);
	utils::ObjectsVector().swap(data);
}

void C3DParser::getObject(core::Variant & object, const core::VariantsVector::size_type idx) const
{
	object.set(data[idx]);
	object.setMetadata("core/source", path);
	if (idx < 4){
		c3dlib::GRFChannelPtr ptr = data[idx]->get();
		if (ptr) {
			auto df = ptr->feature<dataaccessor::DescriptorFeature>();
			if (df != nullptr){
				object.setMetadata("core/name", df->name());
			}			
		}
	}
	else if(idx < 20){
		c3dlib::EMGChannelPtr ptr = data[idx]->get();
		if (ptr) {
			auto df = ptr->feature<dataaccessor::DescriptorFeature>();
			if (df != nullptr){
				object.setMetadata("core/name", df->name());
			}
		}
	}
	else if (idx < 22){
		object.setMetadata("core/name", path);
	}
}

plugin::IParser* C3DParser::create() const
{
    return new C3DParser();
}

void C3DParser::acceptedExpressions(Expressions & expressions) const
{
    ExpressionDescription expDesc;
    expDesc.description = "C3D format";

	utils::ObjectsVector localData;
	initObjects(localData);
	expDesc.objectsTypes.reserve(localData.size());

	for (auto it = localData.begin(); it != localData.end(); ++it){
		expDesc.objectsTypes.push_back((*it)->getTypeInfo());
	}	

    expressions.insert(Expressions::value_type(".*\\.c3d$", expDesc));
}
