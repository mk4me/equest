#include "NewChartPCH.h"
#include "NewChartEvents.h"
#include "NewChartSeriesData.h"
#include "NewChartEventStateData.h"

const QColor leftColor1(255, 0, 0, 15);
const QColor leftColor2(128, 0, 0, 15);
const QColor rightColor1(0, 255, 0, 15);
const QColor rightColor2(0, 128, 0, 15);

EventsPlotItem::EventsPlotItem(c3dlib::EventsCollectionConstPtr events) :
    events(events) 
{
    UTILS_ASSERT(events);
}


void EventsPlotItem::draw( QPainter *painter, const QwtScaleMap &xMap, const QwtScaleMap &yMap, const QRectF &canvasRect ) const
{
    int count = events->getNumEvents();
	c3dlib::C3DEventsCollection::EventConstPtr lastLeftEvent;
	c3dlib::C3DEventsCollection::EventConstPtr lastRightEvent;

    int half = (canvasRect.bottom() - canvasRect.top())/ 2 + canvasRect.top();

    for (int i = 0; i < count; ++i) {
		c3dlib::C3DEventsCollection::EventConstPtr event = events->getEvent(i);
		if (event->getContext() != c3dlib::C3DEventsCollection::IEvent::Left && event->getContext() != c3dlib::C3DEventsCollection::IEvent::Right) {
            continue;
        }
        int x = static_cast<int>(xMap.transform(event->getTime()));

		bool left = event->getContext() == c3dlib::C3DEventsCollection::IEvent::Left;
        int top = left ? canvasRect.top() : half;
        int bottom = left ? half : canvasRect.bottom();

        painter->setFont(QFont("Tahoma", 8));
        painter->setPen(QPen(QColor(0, 0, 0, 70)));
        painter->drawLine(x, top, x, bottom);

        painter->save();
        painter->translate(x + 10, bottom - 30);
        painter->rotate(-90); // or 270
        painter->drawText(0, 0, QObject::tr(event->getLabel().c_str()));
        painter->restore();

        if (lastLeftEvent && left) {
            painter->setBrush(QBrush((i % 2) ? leftColor1 : leftColor2));
            int lastX = static_cast<int>(xMap.transform(lastLeftEvent->getTime()));
            painter->drawRect(lastX, top, x - lastX, bottom - top);
        } else if (lastRightEvent && !left) {
            painter->setBrush(QBrush((i % 2) ? rightColor1 : rightColor2));
            int lastX = static_cast<int>(xMap.transform(lastRightEvent->getTime()));
            painter->drawRect(lastX, top, x - lastX, bottom - top);
        } 
		if (event->getContext() == c3dlib::C3DEventsCollection::IEvent::Left) {
            lastLeftEvent = event;
        } else {
            lastRightEvent = event;
        }
    }

    painter->setFont(QFont("Tahoma", 32));  
    painter->save();

    painter->translate(canvasRect.left() + 30, half - 10);
    painter->rotate(-90); 
    painter->setPen(QPen(QColor(255, 0, 0, 55)));
    painter->drawText(0, 0, ("Left"));
    painter->restore();

    painter->save();
    painter->translate(canvasRect.left() + 30, canvasRect.bottom() - 10);
    painter->rotate(-90); 
    painter->setPen(QPen(QColor(0, 255, 0, 55)));
    painter->drawText(0, 0, ("Right"));
    painter->restore();
}



void EventsHelper::createSegments(std::vector<SegmentPtr>& collection, c3dlib::C3DEventsCollection::Context context)
{
    // te translacje, używane są później, w trakcie rysowania
    // nazwy te pochodza z pliku c3d
    QObject::tr("Foot Strike");
    QObject::tr("Foot Off"   );
    SegmentPtr currentSegment;
	auto uaf = scalar->template feature<dataaccessor::UniformArgumentsFeature>();
	std::function<dataaccessor::NearestArgumentsFinder::Range(const dataaccessor::IDiscreteAccessorT<c3dlib::ScalarChannelReaderInterface::value_type, c3dlib::ScalarChannelReaderInterface::argument_type> &, const c3dlib::ScalarChannelReaderInterface::argument_type &)> range = static_cast<dataaccessor::NearestArgumentsFinder::Range(*)(const dataaccessor::IDiscreteAccessorT<c3dlib::ScalarChannelReaderInterface::value_type, c3dlib::ScalarChannelReaderInterface::argument_type>&,
		const c3dlib::ScalarChannelReaderInterface::argument_type&)>(&dataaccessor::NearestArgumentsFinder::range<c3dlib::ScalarChannelReaderInterface::value_type, c3dlib::ScalarChannelReaderInterface::argument_type>);

	if (uaf != nullptr) {

		range = std::bind(static_cast<dataaccessor::NearestArgumentsFinder::Range(*)(const dataaccessor::IDiscreteAccessorT<c3dlib::ScalarChannelReaderInterface::value_type, c3dlib::ScalarChannelReaderInterface::argument_type>&,
			const c3dlib::ScalarChannelReaderInterface::argument_type&, const c3dlib::ScalarChannelReaderInterface::argument_type&)>(&dataaccessor::NearestArgumentsFinder::range<c3dlib::ScalarChannelReaderInterface::value_type, c3dlib::ScalarChannelReaderInterface::argument_type>),
			std::placeholders::_1, std::placeholders::_2, uaf->argumentsInterval());
	}

    for( auto it = events->cbegin(); it != events->cend(); ++it) {
        EventConstPtr event = *it;
        if (event->getContext() == context) {
            if (event->getLabel() == "Foot Strike") {
                if (currentSegment) {
                    UTILS_ASSERT(currentSegment->event1);
                    currentSegment->end = event->getTime();					
					range(*scalar, currentSegment->end).second;
                    collection.push_back(currentSegment);
                }

                currentSegment.reset(new Segment());
                currentSegment->event1 = event;
                currentSegment->begin = event->getTime();
				currentSegment->scalar = scalar;
				currentSegment->beginIdx = range(*scalar, currentSegment->begin).first;
            } else if (currentSegment && event->getLabel() == "Foot Off") {
                currentSegment->event2 = event;
            }
        } 
    }

    for (unsigned int i = 0; i < collection.size(); ++i) {		
        SegmentPtr segment = collection[i];
		auto df = segment->scalar->feature<dataaccessor::DescriptorFeature>();
		std::string sname;
		if (df != nullptr){
			sname = df->name();
		}
        QString name = QString("%1:%2").arg(sname.empty() == false? sname.c_str() : "Unknown").arg(i);
        segment->normalizedCurve = PlotCurvePtr(new QwtPlotCurve(name));
        segment->normalizedCurve->setData(new NewChartEventStateData(scalar, segment->begin, segment->end));
        segment->normalizedCurve->setRenderHint(QwtPlotItem::RenderAntialiased, true);
        segment->normalizedCurve->setPaintAttribute(QwtPlotCurve::ClipPolygons, false);
        segment->normalizedCurve->setItemAttribute(QwtPlotItem::AutoScale, true);
        segment->normalizedCurve->setItemAttribute(QwtPlotItem::Legend, true);
    }
}

EventsHelper::EventsHelper(c3dlib::EventsCollectionConstPtr events, c3dlib::ScalarChannelReaderInterfaceConstPtr scalar) :
    events(events),
    scalar(scalar),
    eventsItem(new EventsPlotItem(events))
{
	createSegments(leftSegments, c3dlib::C3DEventsCollection::IEvent::Left);
	createSegments(rightSegments, c3dlib::C3DEventsCollection::IEvent::Right);
   
}

EventsHelper::SegmentConstPtr EventsHelper::getSegment(timeType time, c3dlib::C3DEventsCollection::Context context)
{
	UTILS_ASSERT(context == c3dlib::C3DEventsCollection::IEvent::Left || context == c3dlib::C3DEventsCollection::IEvent::Right);
	std::vector<SegmentPtr>& segments = context == c3dlib::C3DEventsCollection::IEvent::Left ? leftSegments : rightSegments;
    for (auto it = segments.cbegin(); it != segments.cend(); ++it) {
        if (time >= (*it)->begin && time <= (*it)->end) {
            return *it;
        }
    }
    return SegmentConstPtr();
}
