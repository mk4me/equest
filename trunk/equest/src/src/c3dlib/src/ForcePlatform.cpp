#include <c3dlib/ForcePlatform.h>
#include <list>
#include <c3dlib/IForcePlatform.h>
#include <c3dlib/C3DTypes.h>
#include <dataaccessorlib/Adapters.h>

namespace c3dlib {

ForcePlatform::ForcePlatform(ForcePlatformStructPtr platform) :
    platform(*platform)
{
    
}

ForcePlatform::StepsConstRange ForcePlatform::getSteps() const
{
    return boost::make_iterator_range(steps.cbegin(), steps.cend());
}

osg::Vec3 ForcePlatform::getCenter() const 
{
    osg::Vec3 c;
    c += platform.corners[0];
    c += platform.corners[1];
    c += platform.corners[2];
    c += platform.corners[3];
    c *= 0.25f;
    c += platform.origin;
    return c;
}

float ForcePlatform::getWidth() const {
    return (platform.corners[0] - platform.corners[1]).length();
}

float ForcePlatform::getLength() const {
    return (platform.corners[1] - platform.corners[2]).length();
}

float ForcePlatform::getSignX() const {
    return (platform.corners[1].x() - platform.corners[0].x()) >= 0.0f ? 1.0f : -1.0f;
}

float ForcePlatform::getSignY() const {
    return (platform.corners[2].y() - platform.corners[1].y()) >= 0.0f ? 1.0f : -1.0f;
}


void ForcePlatform::computeSteps(MarkerCollectionPtr markers, EventsCollectionConstPtr events )
{
    GRFChannelConstPtr f1 = force;
	auto ltoe = markers->tryGetChannelByName("LTOE");
	auto rtoe = markers->tryGetChannelByName("RTOE");
	auto lhee = markers->tryGetChannelByName("LHEE");
	auto rhee = markers->tryGetChannelByName("RHEE");

	if (!(ltoe && rtoe && lhee && rhee)) {
		return;
	}

    auto getAndRemoveLastStep = [&](std::list<Step>& steps, Context context, Step& ret) -> bool
    {
        for (auto it = steps.begin(); it != steps.end(); ++it) {
            if (it->getContext() == context && (it->getStartTime() >= 0.0f && it->getEndTime() < 0.0f)) {
                ret = *it;
                steps.erase(it);
                return true;
            }
        }

        return false;
    };

	auto fltoe = dataaccessor::DiscreteFunctionAccessorAdapter<c3dlib::MarkerChannel::value_type, c3dlib::MarkerChannel::argument_type>(*ltoe);
	auto frtoe = dataaccessor::DiscreteFunctionAccessorAdapter<c3dlib::MarkerChannel::value_type, c3dlib::MarkerChannel::argument_type>(*rtoe);
	auto flhee = dataaccessor::DiscreteFunctionAccessorAdapter<c3dlib::MarkerChannel::value_type, c3dlib::MarkerChannel::argument_type>(*lhee);
	auto frhee = dataaccessor::DiscreteFunctionAccessorAdapter<c3dlib::MarkerChannel::value_type, c3dlib::MarkerChannel::argument_type>(*rhee);

    std::list<Step> possibleSteps;
    for (auto it = events->cbegin(); it != events->cend(); ++it) {
        const auto e = *it;
        double time = e->getTime();		
		osg::Vec3 ltoePos = fltoe.value(time);
		osg::Vec3 rtoePos = frtoe.value(time);
		osg::Vec3 lheePos = flhee.value(time);
		osg::Vec3 rheePos = frhee.value(time);
        Context context = e->getContext();
        if (context == IEvent::Left && (isInsideXY(ltoePos) || isInsideXY(lheePos))) {
            std::string l = e->getLabel();
            if (l == "Foot Strike") {
                Step step(e->getTime(), -1.0f, context);
                possibleSteps.push_back(step);
            } else if (l == "Foot Off") {
                Step step;
                if (getAndRemoveLastStep(possibleSteps, context, step)) {
                    possibleSteps.push_back(Step(step.getStartTime(), e->getTime(), context));
                } else {
                    possibleSteps.push_back(Step(-1.0, e->getTime(), context));
                }
            }
        } 
        
        if (context == IEvent::Right && (isInsideXY(rtoePos) || isInsideXY(rheePos))) {
            std::string l = e->getLabel();
            if (l == "Foot Strike") {
                Step step(e->getTime(), -1.0, context);
                possibleSteps.push_back(step);
            } else if (l == "Foot Off") {
                Step step;
                if (getAndRemoveLastStep(possibleSteps, context, step)) {
                    possibleSteps.push_back(Step(step.getStartTime(), e->getTime(), context));
                } else {                    
                    possibleSteps.push_back(Step(-1.0, e->getTime(), context));
                }
            }
        }
    }

    if (!possibleSteps.empty()) {
        possibleSteps.sort([&](const Step& s1, const Step& s2)->bool 
        {
            if (s1.isValid() && s2.isValid()) {
                double dur1 = s1.getEndTime() - s1.getStartTime();
                double dur2 = s2.getEndTime() - s2.getStartTime();
                return dur1 > dur2;
            } else if (s1.isValid()) {
                return true;
            } else if (s2.isValid()) {
                return false;
            } else {
                return (s1.getStartTime() < s2.getStartTime());
            }
        }
        );

        for (auto it = possibleSteps.begin(); it != possibleSteps.end(); ++it) {
            if (it->isValid()) {
                float time1 = it->getStartTime();
                float time2 = it->getEndTime();				
                it->setStartPoint(it->getContext() == IEvent::Left ? flhee.value(time1) : frhee.value(time1));
                //f1->dataStart = time1;				
				it->setEndPoint(it->getContext() == IEvent::Left ? flhee.value(time2) : frtoe.value(time2));
                //f1->dataEnd = time2;
            } else {
                float time1 = it->getStartTime();
                float time2 = it->getEndTime();
                if (time1 < 0.0) {
                    time1 = f1->hasStartEndData() ? f1->getDataStartTime() : std::max<float>(time2 - 1.0f, 0.001f);
                }
                if (time2 < 0.0) {
					//TODO
					//do sprawdzenia
                    time2 = f1->hasStartEndData() ? f1->getDataEndTime() : std::min<float>(time1 + 1.0f, f1->argument(f1->size() - 1));
                }

                it->setStartTime(time1);
                it->setEndTime(time2);				
				it->setStartPoint(it->getContext() == IEvent::Left ? flhee.value(time1) : frhee.value(time1));				
				it->setEndPoint(it->getContext() == IEvent::Left ? fltoe.value(time2) : frtoe.value(time2));
            }

            steps.push_back(IForcePlatform::IStepPtr(new Step(*it)));
        }
        
    } else {

    }
}

bool ForcePlatform::isInsideXY( const osg::Vec3& v) const
{
    float minx, miny, maxx, maxy;
    minx = miny = std::numeric_limits<float>::max();
    maxx = maxy = -std::numeric_limits<float>::max();
    for (int i = 0; i < 4; ++i) {
        if (platform.corners[i].x() < minx) {
            minx = platform.corners[i].x();
        }
        if (platform.corners[i].y() < miny) {
            miny = platform.corners[i].y();
        }
        if (platform.corners[i].x() > maxx) {
            maxx = platform.corners[i].x();
        }
        if (platform.corners[i].y() > maxy) {
            maxy = platform.corners[i].y();
        }
    }
    if (v.x() >= minx && v.x() <= maxx && v.y() >= miny && v.y() <= maxy) {
        return true;
    }
    return false;
}

GRFChannelConstPtr ForcePlatform::getForceChannel() const
{
    return force;
}

GRFChannelConstPtr ForcePlatform::getMomentChannel() const
{
    return moment;
}

}