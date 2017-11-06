#include "BasicQueriesWS.h"
#include <hmdbserviceslib/BasicQueriesWS.h>
#include <hmdbserviceslib/DateTimeUtils.h>

using namespace hmdbCommunication;

GeneralBasicQueriesWS::GeneralBasicQueriesWS(const WSDLServiceCreator creator)
	: creator(creator)
{
}

GeneralBasicQueriesWS::~GeneralBasicQueriesWS()
{
}

hmdbServices::DateTime GeneralBasicQueriesWS::dataModificationTime() const
{
	return hmdbServices::GeneralBasicQueriesWS(creator()).dataModificationTime();
}

hmdbServices::DateTime GeneralBasicQueriesWS::metadataModificationTime() const
{
	return hmdbServices::GeneralBasicQueriesWS(creator()).metadataModificationTime();
}

MotionBasicQueriesWS::MotionBasicQueriesWS(const WSDLServiceCreator creator)
	: creator(creator)
{
}

MotionBasicQueriesWS::~MotionBasicQueriesWS()
{
}

std::string MotionBasicQueriesWS::getPerformerByIdXML(const int id) const
{
	return hmdbServices::MotionBasicQueriesWS(creator()).getPerformerByIdXML(id);
}

std::string MotionBasicQueriesWS::getSessionByIdXML(const int id) const
{
	return hmdbServices::MotionBasicQueriesWS(creator()).getSessionByIdXML(id);
}

std::string MotionBasicQueriesWS::getSessionLabel(const int id) const
{
	return hmdbServices::MotionBasicQueriesWS(creator()).getSessionLabel(id);
}

std::string MotionBasicQueriesWS::getTrialByIdXML(const int id) const
{
	return hmdbServices::MotionBasicQueriesWS(creator()).getTrialByIdXML(id);
}

std::string MotionBasicQueriesWS::getMeasurementConfigurationByIdXML(const int id) const
{
	return hmdbServices::MotionBasicQueriesWS(creator()).getMeasurementConfigurationByIdXML(id);
}

std::string MotionBasicQueriesWS::getPerformerConfigurationByIdXML(const int id) const
{
	return hmdbServices::MotionBasicQueriesWS(creator()).getPerformerConfigurationByIdXML(id);
}

std::string MotionBasicQueriesWS::listPerformersXML() const
{
	return hmdbServices::MotionBasicQueriesWS(creator()).listPerformersXML();
}

std::string MotionBasicQueriesWS::listPerformersWithAttributesXML() const
{
	return hmdbServices::MotionBasicQueriesWS(creator()).listPerformersWithAttributesXML();
}

std::string MotionBasicQueriesWS::listLabPerformersWithAttributesXML(const int labID) const
{
	return hmdbServices::MotionBasicQueriesWS(creator()).listLabPerformersWithAttributesXML(labID);
}

std::string MotionBasicQueriesWS::listSessionPerformersWithAttributesXML(const int sessionID) const
{
	return hmdbServices::MotionBasicQueriesWS(creator()).listSessionPerformersWithAttributesXML(sessionID);
}

std::string MotionBasicQueriesWS::listPerformerSessionsXML(const int performerID) const
{
	return hmdbServices::MotionBasicQueriesWS(creator()).listPerformerSessionsXML(performerID);
}

std::string MotionBasicQueriesWS::listPerformerSessionsWithAttributesXML(const int performerID) const
{
	return hmdbServices::MotionBasicQueriesWS(creator()).listPerformerSessionsWithAttributesXML(performerID);
}

std::string MotionBasicQueriesWS::listLabSessionsWithAttributesXML(const int labID) const
{
	return hmdbServices::MotionBasicQueriesWS(creator()).listLabSessionsWithAttributesXML(labID);
}

std::string MotionBasicQueriesWS::listGroupSessionsWithAttributesXML(const int sessionGroupID) const
{
	return hmdbServices::MotionBasicQueriesWS(creator()).listGroupSessionsWithAttributesXML(sessionGroupID);
}

std::string MotionBasicQueriesWS::listMeasurementConfigSessionsWithAttributesXML(const int measurementConfID) const
{
	return hmdbServices::MotionBasicQueriesWS(creator()).listMeasurementConfigSessionsWithAttributesXML(measurementConfID);
}

std::string MotionBasicQueriesWS::listSessionSessionGroups(const int sessionID) const
{
	return hmdbServices::MotionBasicQueriesWS(creator()).listSessionSessionGroups(sessionID);
}

std::string MotionBasicQueriesWS::listSessionTrialsXML(const int sessionID) const
{
	return hmdbServices::MotionBasicQueriesWS(creator()).listSessionTrialsXML(sessionID);
}

std::string MotionBasicQueriesWS::listSessionTrialsWithAttributesXML(const int sessionID) const
{
	return hmdbServices::MotionBasicQueriesWS(creator()).listSessionTrialsWithAttributesXML(sessionID);
}

std::string MotionBasicQueriesWS::listMeasurementConfigurationsWithAttributesXML() const
{
	return hmdbServices::MotionBasicQueriesWS(creator()).listMeasurementConfigurationsWithAttributesXML();
}

std::string MotionBasicQueriesWS::listSessionPerformerConfsWithAttributesXML(const int sessionID) const
{
	return hmdbServices::MotionBasicQueriesWS(creator()).listSessionPerformerConfsWithAttributesXML(sessionID);
}

std::string MotionBasicQueriesWS::listFilesXML(const int subjectID, const std::string & subjectType) const
{
	return hmdbServices::MotionBasicQueriesWS(creator()).listFilesXML(subjectID, subjectType);
}

std::string MotionBasicQueriesWS::listFilesWithAttributesXML(const int subjectID, const std::string & subjectType) const
{
	return hmdbServices::MotionBasicQueriesWS(creator()).listFilesWithAttributesXML(subjectID, subjectType);
}

std::string MotionBasicQueriesWS::listFileAttributeDataXML(const int subjectID, const std::string & subjectType) const
{
	return hmdbServices::MotionBasicQueriesWS(creator()).listFileAttributeDataXML(subjectID, subjectType);
}

std::string MotionBasicQueriesWS::listFileAttributeDataWithAttributesXML(const int subjectID, const std::string & subjectType) const
{
	return hmdbServices::MotionBasicQueriesWS(creator()).listFileAttributeDataWithAttributesXML(subjectID, subjectType);
}

std::string MotionBasicQueriesWS::listAttributesDefined(const std::string & attributeGroupName,
	const std::string & entityKind) const
{
	return hmdbServices::MotionBasicQueriesWS(creator()).listAttributesDefined(attributeGroupName, entityKind);
}

std::string MotionBasicQueriesWS::listAttributeGroupsDefined(const std::string & entityKind) const
{
	return hmdbServices::MotionBasicQueriesWS(creator()).listAttributeGroupsDefined(entityKind);
}

std::string MotionBasicQueriesWS::listSessionGroupsDefined() const
{
	return hmdbServices::MotionBasicQueriesWS(creator()).listSessionGroupsDefined();
}

std::string MotionBasicQueriesWS::listMotionKindsDefined() const
{
	return hmdbServices::MotionBasicQueriesWS(creator()).listMotionKindsDefined();
}

std::string MotionBasicQueriesWS::listEnumValues(const std::string & attributeName,
	const std::string & entityKind) const
{
	return hmdbServices::MotionBasicQueriesWS(creator()).listEnumValues(attributeName, entityKind);
}

std::string MotionBasicQueriesWS::listMyAnnotationsXML() const
{
	return hmdbServices::MotionBasicQueriesWS(creator()).listMyAnnotationsXML();
}

std::string MotionBasicQueriesWS::listAnnotationsXML() const
{
	return hmdbServices::MotionBasicQueriesWS(creator()).listAnnotationsXML();
}

std::string MotionBasicQueriesWS::listMyAwaitingAnnotationsXML() const
{
	return hmdbServices::MotionBasicQueriesWS(creator()).listMyAwaitingAnnotationsXML();
}

std::string MotionBasicQueriesWS::listMyReviewedAnnotationsXML() const
{
	return hmdbServices::MotionBasicQueriesWS(creator()).listMyReviewedAnnotationsXML();
}

hmdbServices::DateTime MotionBasicQueriesWS::dataModificationTime() const
{
	return hmdbServices::MotionBasicQueriesWS(creator()).dataModificationTime();
}

hmdbServices::DateTime MotionBasicQueriesWS::metadataModificationTime() const
{
	return hmdbServices::MotionBasicQueriesWS(creator()).metadataModificationTime();
}

MedicalBasicQueriesWS::MedicalBasicQueriesWS(const WSDLServiceCreator creator)
	: creator(creator)
{
}

MedicalBasicQueriesWS::~MedicalBasicQueriesWS()
{
}

hmdbServices::DateTime MedicalBasicQueriesWS::dataModificationTime() const
{
	return hmdbServices::MedicalBasicQueriesWS(creator()).dataModificationTime();
}

hmdbServices::DateTime MedicalBasicQueriesWS::metadataModificationTime() const
{
	return hmdbServices::MedicalBasicQueriesWS(creator()).metadataModificationTime();
}