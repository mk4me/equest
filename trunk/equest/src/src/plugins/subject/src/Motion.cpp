#include "SubjectPCH.h"
#include <mutex>
#include "SubjectService.h"
#include "Session.h"
#include "Subject.h"
#include "Motion.h"
#include <corelib/PluginCommon.h>

using namespace PluginSubject;

PluginSubject::SubjectID Motion::globalID = 0;

PluginSubject::SubjectID Motion::nextGlobalID()
{
	if(globalID == std::numeric_limits<PluginSubject::SubjectID>::max()){
		throw loglib::runtime_error("Motion overflow");
	}

	return globalID++;
}

Motion::Motion(const core::VariantConstPtr & session,
	const PluginSubject::SessionConstPtr & unpackedSession,
    PluginSubject::SubjectID localMotionID)
	: motionID(nextGlobalID()), session(session), unpackedSession(unpackedSession), localMotionID(localMotionID),
	name(generateName(motionID)), localName(generateLocalName(localMotionID, unpackedSession->getLocalName()))
{
    
}

Motion::~Motion()
{

}

std::string Motion::generateName(PluginSubject::SubjectID motionID)
{
	std::stringstream ss;
	ss.fill('0');
	ss.width(4);
	ss << motionID;

	return "Motion" + ss.str();
}

std::string Motion::generateLocalName(PluginSubject::SubjectID motionLocalID, const std::string & sessionLocalName)
{
	std::string localName = sessionLocalName + "-M";

	std::stringstream ss1;
	ss1.fill('0');
	ss1.width(2);
	ss1 << motionLocalID;

	localName += ss1.str();

	return localName;
}

const std::string & Motion::getName() const
{
    return name;
}

const std::string & Motion::getLocalName() const
{
    return localName;
}

SubjectID Motion::getID() const
{
    return motionID;
}

SubjectID Motion::getLocalID() const
{
    return localMotionID;
}


const core::VariantConstPtr & Motion::getSession() const
{
    return session;
}

const PluginSubject::SessionConstPtr & Motion::getUnpackedSession() const
{
	return unpackedSession;
}

void Motion::addData(const core::VariantConstPtr & data)
{
	storage.addData(data);
}

void Motion::removeData(const core::VariantConstPtr & data)
{
	storage.removeData(data);
}

const bool Motion::tryAddData(const core::VariantConstPtr & data)
{
	return storage.tryAddData(data);
}

const bool Motion::tryRemoveData(const core::VariantConstPtr & data)
{
	return storage.tryRemoveData(data);
}

void Motion::getObjects(core::ConstVariantsList & objects) const
{
	storage.getObjects(objects);
}

void Motion::getObjects(core::ConstVariantsList & objects, const utils::TypeInfo & type, bool exact) const
{
	storage.getObjects(objects, type, exact);
}

void Motion::getObjects(core::VariantsCollection& objects) const
{
	storage.getObjects(objects);
}

const bool Motion::isManaged(const core::VariantConstPtr & object) const
{
	return storage.isManaged(object);
}

const bool Motion::hasObject(const utils::TypeInfo & type, bool exact) const
{
	return storage.hasObject(type, exact);
}