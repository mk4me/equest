/********************************************************************
	created:	2011/07/04
	created:	4:7:2011   14:55
	filename: 	EventVisualizer.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_PLUGINC3D__EVENTVISUALIZER_H__
#define HEADER_GUARD_PLUGINC3D__EVENTVISUALIZER_H__

#include <string>
#include <core/IObjectSource.h>
#include <QtCore/QObject>
#include <core/IVisualizer.h>
#include <core/PluginCommon.h>
#include <core/IIdentifiable.h>
#include <QtGui/QWidget>
#include <QtGui/QPlainTextEdit>
#include <QtGui/QLabel>
#include <core/IDataManager.h>
#include <core/ObjectWrapper.h>
#include <plugins/c3d/C3DChannels.h>

//class EventVisualizer : public core::IVisualizer
//{
//	UNIQUE_ID("{04DB408B-57FF-473C-8C42-AE93406C76F0}", "Event visualizer");
//
//	std::string name;
//	std::auto_ptr<QLabel> widget;
//
//public:
//	EventVisualizer() :
//	  name("EventVisualizer")
//	  {
//	  }
//
//	  class EventVisualizerSerie : public SerieBase
//	  {
//	  public:
//		  EventVisualizerSerie(EventVisualizer * visualizer)
//			  : visualizer(visualizer)
//		  {
//
//		  }
//
//	  protected:
//		  virtual void setSerieName(const std::string & name)
//		  {
//			  //TODO
//			  //obecnie nazwy serii nie sa obslugiwane ale musimy pomyslec o tym i ewentualnie dodac!!
//		  }
//
//		  virtual void setSerieData(const core::ObjectWrapperConstPtr & data)
//		  {
//			    std::stringstream txt;
//
//				EventsCollectionConstPtr events = data->get();
//				txt << "<ul>";
//				for (auto it = events->cbegin(); it != events->cend(); it++) {
//					txt << "<li><b>" << (*it)->getLabel() << "</b></li>";
//					txt << "<ul>";
//					txt << "<li><i> Subject: </i>"     << (*it)->getSubject()     << "</li>";
//					txt << "<li><i> Context: </i>"     << (*it)->getContext()     << "</li>";
//					txt << "<li><i> Time:    </i>"     << (*it)->getTime()        << "</li>";
//					txt << "<li><i> Description: </i>" << (*it)->getDescription() << "</li>";
//					txt << "</ul>";
//				}
//				txt << "</ul>";
//			    visualizer->widget->setText(QString(txt.str().c_str()));
//		  }
//
//	  private:
//		  EventVisualizer* visualizer;
//	  };
//
//	  virtual IVisualizer* createClone() const
//	  {
//		  return new EventVisualizer();
//	  }
//
//	  virtual const std::string& getName() const
//	  {
//		  static std::string name = "EventVisualizer";
//		  return name;
//	  }
//
//	  core::IVisualizer::SerieBase *createSerie(const core::ObjectWrapperConstPtr & data, const std::string & name)
//	  {
//		  core::IVisualizer::SerieBase * ret = new EventVisualizerSerie(this);
//		  ret->setName(name);
//		  ret->setData(data);
//
//		  return ret;
//	  }
//
//	  virtual void removeSerie(core::IVisualizer::SerieBase *serie)
//	  {
//		  widget->setText("serie removed");
//	  }
//
//	  virtual QWidget* createWidget(std::vector<QObject*>& actions)
//	  {
//		  widget.reset(new QLabel(nullptr));
//		  widget->setStyleSheet(  "QLabel { background: white; }" );
//		  return widget.get();
//	  }
//
//	  virtual QIcon* createIcon()
//	  {
//		  return nullptr;
//	  }
//
//	  virtual int getMaxDataSeries(void) const
//	  {
//		  return 1;
//	  }
//
//	  virtual void getInputInfo( std::vector<core::IInputDescription::InputInfo>& info)
//	  {
//		  core::IInputDescription::InputInfo input;
//
//		  input.name = "event";
//		  input.type = typeid(C3DEventsCollection);
//		  input.required = true;
//		  input.modify = false;
//
//		  info.push_back(input);
//	  }
//
//	  virtual void setUp(core::IObjectSource* source) 
//	  {
//	  }
//
//	  virtual void update(double deltaTime)
//	  {
//	  }
//	  virtual void reset()
//	  {
//
//	  }
//};


#endif
