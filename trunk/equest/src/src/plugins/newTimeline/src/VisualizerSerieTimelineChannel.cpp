#include "TimelinePCH.h"
#include <plugins/newTimeline/VisualizerSerieTimelineChannel.h>
#include <plugins/newTimeline/ITimelineService.h>

VisualizerSerieTimelineChannel::VisualizerSerieTimelineChannel(core::Visualizer * visualizer, core::Visualizer::Serie * serie)
    : visualizer(visualizer), serie(serie), timeAvareSerieFeatures(serie->serieFeatures<plugin::IVisualizer::ITimeAvareSerieFeatures>()), 
	timeEditableSerieFeatures(serie->serieFeatures<plugin::IVisualizer::ITimeEditableSerieFeatures>())
{
	if(timeAvareSerieFeatures == nullptr){
		throw loglib::runtime_error("Serie not aware of time");
	}
}

VisualizerSerieTimelineChannel::~VisualizerSerieTimelineChannel()
{

}

void VisualizerSerieTimelineChannel::setTime(double time)
{
    timeAvareSerieFeatures->setTime(time);
}

double VisualizerSerieTimelineChannel::getLength() const
{
    return timeAvareSerieFeatures->getLength();
}

void VisualizerSerieTimelineChannel::offsetChanged(double newOffset)
{
	if(timeEditableSerieFeatures != nullptr){
		timeEditableSerieFeatures->setOffset(newOffset);
	}
}

void VisualizerSerieTimelineChannel::scaleChanged(double newScale)
{
	if(timeEditableSerieFeatures != nullptr){
		timeEditableSerieFeatures->setScale(newScale);
	}	
}

VisualizerSerieTimelineChannel * VisualizerSerieTimelineChannel::clone() const
{
    //TODO
    //można tutaj wprowadzic zarządzanie klonowaniem serii

    return nullptr;
}

const core::Visualizer::Serie * VisualizerSerieTimelineChannel::getSerie() const
{
    return serie;
}

const core::Visualizer * VisualizerSerieTimelineChannel::getVisualizer() const
{
    return visualizer;
}

VisualizerSerieTimelineMultiChannel::VisualizerSerieTimelineMultiChannel(const VisualizersSeries & visualizersSeries)
    : visualizersSeries(visualizersSeries), length(-1)
{
    UTILS_ASSERT((visualizersSeries.empty() == false), "Nie podano żadnych serii dla kanalu");

	std::list<plugin::IVisualizer::ITimeAvareSerieFeatures*> locTasfs;
	std::list<plugin::IVisualizer::ITimeEditableSerieFeatures*> locTesfs;

	for(auto it = visualizersSeries.begin(); it != visualizersSeries.end(); ++it){

		auto tasf = (*it)->serieFeatures<plugin::IVisualizer::ITimeAvareSerieFeatures>();

		if(tasf == nullptr){
			throw loglib::runtime_error("Serie not aware of time");
		}

		locTasfs.push_back(tasf);

		auto tesf = (*it)->serieFeatures<plugin::IVisualizer::ITimeEditableSerieFeatures>();
		if(tesf != nullptr){
			locTesfs.push_back(tesf);
		}
	}

	tasfs = locTasfs;
	tesfs = locTesfs;
}

VisualizerSerieTimelineMultiChannel::~VisualizerSerieTimelineMultiChannel()
{

}

void VisualizerSerieTimelineMultiChannel::setTime(double time)
{
    for(auto it = tasfs.begin(); it != tasfs.end(); ++it){
		(*it)->setTime(time > (*it)->getLength() ? (*it)->getLength() : time);
    }
}

double VisualizerSerieTimelineMultiChannel::getLength() const
{
	double length = 0;

	for(auto it = tasfs.begin(); it != tasfs.end(); ++it){
		length = std::max(length, (*it)->getLength());
	}

	return length;
}

VisualizerSerieTimelineMultiChannel * VisualizerSerieTimelineMultiChannel::clone() const
{
    //TODO
    //można tutaj wprowadzic zarządzanie klonowaniem serii

    return nullptr;
}

const VisualizerSerieTimelineMultiChannel::VisualizersSeries & VisualizerSerieTimelineMultiChannel::getVisualizersSeries() const
{
    return visualizersSeries;
}

void VisualizerSerieTimelineMultiChannel::offsetChanged(double newOffset)
{
	for(auto it = tesfs.begin(); it != tesfs.end(); ++it){
		(*it)->setOffset(newOffset);
	}
}

void VisualizerSerieTimelineMultiChannel::scaleChanged(double newScale)
{
	for(auto it = tesfs.begin(); it != tesfs.end(); ++it){
		(*it)->setScale(newScale);
	}
}

VisualizerTimelineHelper::VisualizerTimelineHelper(ITimelineService * timeline) : timeline(timeline)
{

}

VisualizerTimelineHelper::~VisualizerTimelineHelper()
{
	for(auto it = seriesToChannels.begin(); it != seriesToChannels.end(); ++it){
		timeline->removeChannel(boost::lexical_cast<std::string>(it->second.first));
		it->first->visualizer()->removeObserver(this);
	}
}

void VisualizerTimelineHelper::update(core::Visualizer::Serie * serie, core::Visualizer::SerieModyfication modyfication)
{
	static int idx = 0;
	if(modyfication == core::Visualizer::ADD_SERIE){
		//utwórz kanał
		auto channel = utils::shared_ptr<timeline::IChannel>(new VisualizerSerieTimelineChannel(serie->visualizer(), serie));
		//dodaj do timeline
		timeline->addChannel(boost::lexical_cast<std::string>(idx), channel);
		//zapamiętaj
		seriesToChannels[serie] = std::make_pair(idx++, channel);
	}else{
		//usuń kanał
		auto it = seriesToChannels.find(serie);
		timeline->removeChannel(boost::lexical_cast<std::string>(it->second.first));
		seriesToChannels.erase(serie);
	}
}