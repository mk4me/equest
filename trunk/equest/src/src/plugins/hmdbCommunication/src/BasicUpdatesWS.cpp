#include "BasicUpdatesWS.h"
#include <hmdbserviceslib/BasicUpdatesWS.h>

using namespace hmdbCommunication;
	
MotionBasicUpdatesWS::MotionBasicUpdatesWS(const WSDLServiceCreator creator)
	: creator(creator)
{
}

MotionBasicUpdatesWS::~MotionBasicUpdatesWS()
{
}

int MotionBasicUpdatesWS::createPerformer(const std::string & name,
	const std::string & surname)
{
	return hmdbServices::MotionBasicUpdatesWS(creator()).createPerformer(name, surname);
}

int MotionBasicUpdatesWS::createSession(const int labID, const std::string & motionKindName,
	const std::string & sessionDate, const std::string & sessionDescription,
	const std::set<int> & sessionGroupIDs)
{
	return hmdbServices::MotionBasicUpdatesWS(creator()).createSession(labID, motionKindName,
		sessionDate, sessionDescription, sessionGroupIDs);
}

int MotionBasicUpdatesWS::createTrial(const int sessionID, const std::string & trialDescription)
{
	return hmdbServices::MotionBasicUpdatesWS(creator()).createTrial(sessionID, trialDescription);
}

int MotionBasicUpdatesWS::createMeasurementConfiguration(const std::string & mcName,
	const std::string & mcKind, const std::string & mcDescription)
{
	return hmdbServices::MotionBasicUpdatesWS(creator()).createMeasurementConfiguration(mcName,
		mcKind, mcDescription);
}

int MotionBasicUpdatesWS::assignSessionToGroup(const int sessionID, const int sessionsGroupID)
{
	return hmdbServices::MotionBasicUpdatesWS(creator()).assignSessionToGroup(sessionID,
		sessionsGroupID);
}

int MotionBasicUpdatesWS::assignPerformerToSession(const int performerID, const int sessionID)
{
	return hmdbServices::MotionBasicUpdatesWS(creator()).assignPerformerToSession(performerID,
		sessionID);
}

bool MotionBasicUpdatesWS::assignPerformerToMeasurement(const int performerID, const int measurementID)
{
	return hmdbServices::MotionBasicUpdatesWS(creator()).assignPerformerToMeasurement(performerID,
		measurementID);
}

void MotionBasicUpdatesWS::setPerformerAttribute(const int performerID, const std::string & attributeName,
	const std::string & attributeValue, const bool update)
{
	hmdbServices::MotionBasicUpdatesWS(creator()).setPerformerAttribute(performerID,
		attributeName, attributeValue, update);
}

void MotionBasicUpdatesWS::setSessionAttribute(const int sessionID, const std::string & attributeName,
	const std::string & attributeValue, const bool update)
{
	hmdbServices::MotionBasicUpdatesWS(creator()).setSessionAttribute(sessionID,
		attributeName, attributeValue, update);
}

void MotionBasicUpdatesWS::setTrialAttribute(const int trialID, const std::string & attributeName,
	const std::string & attributeValue, const bool update)
{
	hmdbServices::MotionBasicUpdatesWS(creator()).setTrialAttribute(trialID,
		attributeName, attributeValue, update);
}

void MotionBasicUpdatesWS::setMeasurementConfAttribute(const int measurementConfID, const std::string & attributeName,
	const std::string & attributeValue, const bool update)
{
	hmdbServices::MotionBasicUpdatesWS(creator()).setMeasurementConfAttribute(measurementConfID,
		attributeName, attributeValue, update);
}

void MotionBasicUpdatesWS::setPerformerConfAttribute(const int performerConfID, const std::string & attributeName,
	const std::string & attributeValue, const bool update)
{
	hmdbServices::MotionBasicUpdatesWS(creator()).setPerformerConfAttribute(performerConfID,
		attributeName, attributeValue, update);
}

void MotionBasicUpdatesWS::setFileAttribute(const int fileID, const std::string & attributeName,
	const std::string & attributeValue, const bool update)
{
	hmdbServices::MotionBasicUpdatesWS(creator()).setFileAttribute(fileID,
		attributeName, attributeValue, update);
}

void MotionBasicUpdatesWS::setFileTypedAttributeValue(const int resourceID, const std::string & entity,
	const std::string & attributeName, const int fileID, const bool update)
{
	hmdbServices::MotionBasicUpdatesWS(creator()).setFileTypedAttributeValue(resourceID,
		entity, attributeName, fileID, update);
}

void MotionBasicUpdatesWS::clearAttributeValue(const int resourceID, const std::string & attributeName,
	const std::string & entity)
{
	hmdbServices::MotionBasicUpdatesWS(creator()).clearAttributeValue(resourceID,
		attributeName, entity);
}

void MotionBasicUpdatesWS::setMyAnnotationStatus(const int trialID, const int status,
	const std::string & comment)
{
	hmdbServices::MotionBasicUpdatesWS(creator()).setMyAnnotationStatus(trialID,
		status, comment);
}

void MotionBasicUpdatesWS::setAnnotationReview(const int trialID, const int userID,
	const int status,
	const std::string & note)
{
	hmdbServices::MotionBasicUpdatesWS(creator()).setAnnotationReview(trialID,
		userID, status, note);
}

MedicalBasicUpdatesWS::MedicalBasicUpdatesWS(const WSDLServiceCreator creator)
	: creator(creator)
{
}

MedicalBasicUpdatesWS::~MedicalBasicUpdatesWS()
{
}
