#include "SubjectPCH.h"
#include "Session.h"
#include <plugins/subject/IMotion.h>
#include <plugins/subject/ISubject.h>
#include <corelib/DataAccessors.h>

using namespace PluginSubject;

PluginSubject::SubjectID Session::globalID = 0;

PluginSubject::SubjectID Session::nextGlobalID()
{
	if(globalID == std::numeric_limits<PluginSubject::SubjectID>::max()){
		throw loglib::runtime_error("Session overflow");
	}

	return globalID++;
}

Session::Session(const core::VariantConstPtr & subject, const PluginSubject::SubjectConstPtr & unpackedSubject,
	SubjectID localSessionID) : sessionID(nextGlobalID()), subject(subject), unpackedSubject(unpackedSubject),
	localSessionID(localSessionID), currentMotionID(0), name(generateName(sessionID)),
	localName(generateLocalName(localSessionID, unpackedSubject->getName()))
{
	
}

Session::~Session()
{

}

std::string Session::generateName(PluginSubject::SubjectID sessionID)
{
	std::stringstream ss;
	ss.fill('0');
	ss.width(4);
	ss << sessionID;

	return "Session" + ss.str();
}

std::string Session::generateLocalName(PluginSubject::SubjectID localSessionID, const std::string & subjectName)
{
	std::stringstream ss;
	ss.fill('0');
	ss.width(4);
	ss << localSessionID;

	return subjectName + "-S" + ss.str();
}

const std::string & Session::getName() const
{
    return name;
}

const std::string & Session::getLocalName() const
{
    return localName;
}

SubjectID Session::getID() const
{
    return sessionID;
}

SubjectID Session::getLocalID() const
{
    return localSessionID;
}

void Session::getMotions(core::ConstVariantsList & motions) const
{
	motionStorage.getObjects(motions, typeid(PluginSubject::IMotion), false);
}

void Session::addMotion(const core::VariantConstPtr & motion)
{
	utils::ObjectWrapper::Types types;
	motion->data()->getSupportedTypes(types);
	if (std::find(types.begin(), types.end(), typeid(PluginSubject::IMotion)) == types.end()){
		throw loglib::runtime_error("Data type does not support PluginSubject::IMotion type");
	}

	motionStorage.addData(motion);
}

void Session::removeMotion(const core::VariantConstPtr & motion)
{
	motionStorage.removeData(motion);
}

const core::VariantConstPtr & Session::getSubject() const
{
    return subject;
}

const PluginSubject::SubjectConstPtr & Session::getUnpackedSubject() const
{
	return unpackedSubject;
}

PluginSubject::SubjectID Session::nextMotionID() const
{
	return currentMotionID++;
}

void Session::addData(const core::VariantConstPtr & data)
{
	dataStorage.addData(data);
}

void Session::removeData(const core::VariantConstPtr & data)
{
	dataStorage.removeData(data);
}

const bool Session::tryAddData(const core::VariantConstPtr & data)
{
	return dataStorage.tryAddData(data);
}

const bool Session::tryRemoveData(const core::VariantConstPtr & data)
{
	return dataStorage.tryRemoveData(data);
}

void Session::getObjects(core::ConstVariantsList & objects) const
{
	dataStorage.getObjects(objects);
}

void Session::getObjects(core::ConstVariantsList & objects, const utils::TypeInfo & type, bool exact) const
{
	dataStorage.getObjects(objects, type, exact);
}

void Session::getObjects(core::VariantsCollection& objects) const
{
	dataStorage.getObjects(objects);
}

const bool Session::isManaged(const core::VariantConstPtr & object) const
{
	return dataStorage.isManaged(object);
}

const bool Session::hasObject(const utils::TypeInfo & type, bool exact) const
{
	return dataStorage.hasObject(type, exact);
}