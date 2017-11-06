#include <hmdbserviceslib/BasicQueriesWS.h>
#include <hmdbserviceslib/DateTimeUtils.h>

namespace hmdbServices
{
	std::string simpleXMLQuerry(const std::string & operation,
		const networkUtils::IWSDLServicePtr wsdlService)
	{
		//networkUtils::IWSDLService * c = const_cast<networkUtils::IWSDLService*>(wsdlService);
		wsdlService->setOperation(operation);
		wsdlService->invoke();
		return wsdlService->xmlResponse();
	}

	std::string simpleIDXMLQuerry(const int id, const std::string & operation,
		const networkUtils::IWSDLServicePtr wsdlService,
		const std::string & idName = std::string("id"))
	{
		//networkUtils::IWSDLService * c = const_cast<networkUtils::IWSDLService*>(wsdlService);
		wsdlService->setOperation(operation);
		wsdlService->setValue(idName, id);
		wsdlService->invoke();
		return wsdlService->xmlResponse();
	}

	std::string simpleSubjectXMLQuerry(const int subjectID, const std::string & subjectType,
		const std::string & operation, const networkUtils::IWSDLServicePtr wsdlService)
	{
		//networkUtils::IWSDLService * c = const_cast<networkUtils::IWSDLService*>(wsdlService);
		wsdlService->setOperation(operation);
		wsdlService->setValue("subjectID", subjectID);
		wsdlService->setValue("subjectType", subjectType);
		wsdlService->invoke();
		return wsdlService->xmlResponse();
	}

	GeneralBasicQueriesWS::GeneralBasicQueriesWS(networkUtils::IWSDLServicePtr wsdlService)
		: wsdlService(wsdlService)
	{
	}

	GeneralBasicQueriesWS::~GeneralBasicQueriesWS()
	{
	}

	DateTime GeneralBasicQueriesWS::dataModificationTime() const
	{
		wsdlService->setOperation("GetDBTimestamp");
		wsdlService->invoke(true);

		std::string time;
		wsdlService->getValue("GetDBTimestampResult", time);

		return toTime(time);
	}

	DateTime GeneralBasicQueriesWS::metadataModificationTime() const
	{
		wsdlService->setOperation("GetMetadataTimestamp");
		wsdlService->invoke(true);

		std::string time;
		wsdlService->getValue("GetMetadataTimestampResult", time);

		return toTime(time);
	}

	MotionBasicQueriesWS::MotionBasicQueriesWS(networkUtils::IWSDLServicePtr wsdlService)
		: wsdlService(wsdlService)
	{
	}

	MotionBasicQueriesWS::~MotionBasicQueriesWS()
	{
	}

	std::string MotionBasicQueriesWS::getPerformerByIdXML(const int id) const
	{
		return simpleIDXMLQuerry(id, "GetPerformerByIdXML", wsdlService);
	}

	std::string MotionBasicQueriesWS::getSessionByIdXML(const int id) const
	{
		return simpleIDXMLQuerry(id, "GetSessionByIdXML", wsdlService);
	}

	std::string MotionBasicQueriesWS::getSessionLabel(const int id) const
	{
		wsdlService->setOperation("GetSessionLabel");
		wsdlService->setValue("id", id);
		wsdlService->invoke(true);

		std::string ret;

		try{
			wsdlService->getValue("SessionLabel", ret);
		}
		catch (...) {
		}

		return ret;
	}

	std::string MotionBasicQueriesWS::getTrialByIdXML(const int id) const
	{
		return simpleIDXMLQuerry(id, "GetTrialByIdXML", wsdlService);
	}

	std::string MotionBasicQueriesWS::getMeasurementConfigurationByIdXML(const int id) const
	{
		return simpleIDXMLQuerry(id, "GetMeasurementConfigurationByIdXML", wsdlService);
	}

	std::string MotionBasicQueriesWS::getPerformerConfigurationByIdXML(const int id) const
	{
		return simpleIDXMLQuerry(id, "GetPerformerConfigurationByIdXML", wsdlService);
	}

	std::string MotionBasicQueriesWS::listPerformersXML() const
	{
		return simpleXMLQuerry("ListPerformersXML", wsdlService);
	}

	std::string MotionBasicQueriesWS::listPerformersWithAttributesXML() const
	{
		return simpleXMLQuerry("ListPerformersWithAttributesXML", wsdlService);
	}

	std::string MotionBasicQueriesWS::listLabPerformersWithAttributesXML(const int labID) const
	{
		return simpleIDXMLQuerry(labID, "ListLabPerformersWithAttributesXML", wsdlService, "labID");
	}

	std::string MotionBasicQueriesWS::listSessionPerformersWithAttributesXML(const int sessionID) const
	{
		return simpleIDXMLQuerry(sessionID, "ListSessionPerformersWithAttributesXML", wsdlService, "sessionID");
	}

	std::string MotionBasicQueriesWS::listPerformerSessionsXML(const int performerID) const
	{
		return simpleIDXMLQuerry(performerID, "ListPerformerSessionsXML", wsdlService, "performerID");
	}

	std::string MotionBasicQueriesWS::listPerformerSessionsWithAttributesXML(const int performerID) const
	{
		return simpleIDXMLQuerry(performerID, "ListPerformerSessionsWithAttributesXML", wsdlService, "performerID");
	}

	std::string MotionBasicQueriesWS::listLabSessionsWithAttributesXML(const int labID) const
	{
		return simpleIDXMLQuerry(labID, "ListLabSessionsWithAttributesXML", wsdlService, "labID");
	}

	std::string MotionBasicQueriesWS::listGroupSessionsWithAttributesXML(const int sessionGroupID) const
	{
		return simpleIDXMLQuerry(sessionGroupID, "ListGroupSessionsWithAttributesXML", wsdlService, "sessionGroupID");
	}

	std::string MotionBasicQueriesWS::listMeasurementConfigSessionsWithAttributesXML(const int measurementConfID) const
	{
		return simpleIDXMLQuerry(measurementConfID, "ListMeasurementConfigSessionsWithAttributesXML", wsdlService, "measurementConfID");
	}

	std::string MotionBasicQueriesWS::listSessionSessionGroups(const int sessionID) const
	{
		return simpleIDXMLQuerry(sessionID, "ListSessionSessionGroups", wsdlService, "sessionID");
	}

	std::string MotionBasicQueriesWS::listSessionTrialsXML(const int sessionID) const
	{
		return simpleIDXMLQuerry(sessionID, "ListSessionTrialsXML", wsdlService, "sessionID");
	}

	std::string MotionBasicQueriesWS::listSessionTrialsWithAttributesXML(const int sessionID) const
	{
		return simpleIDXMLQuerry(sessionID, "ListSessionTrialsWithAttributesXML", wsdlService, "sessionID");
	}

	std::string MotionBasicQueriesWS::listMeasurementConfigurationsWithAttributesXML() const
	{
		return simpleXMLQuerry("ListMeasurementConfigurationsWithAttributesXML", wsdlService);
	}

	std::string MotionBasicQueriesWS::listSessionPerformerConfsWithAttributesXML(const int sessionID) const
	{
		return simpleIDXMLQuerry(sessionID, "ListSessionPerformerConfsWithAttributesXML", wsdlService, "sessionID");
	}

	std::string MotionBasicQueriesWS::listFilesXML(const int subjectID, const std::string & subjectType) const
	{
		return simpleSubjectXMLQuerry(subjectID, subjectType, "ListFilesXML", wsdlService);
	}

	std::string MotionBasicQueriesWS::listFilesWithAttributesXML(const int subjectID, const std::string & subjectType) const
	{
		return simpleSubjectXMLQuerry(subjectID, subjectType, "ListFilesWithAttributesXML", wsdlService);
	}

	std::string MotionBasicQueriesWS::listFileAttributeDataXML(const int subjectID, const std::string & subjectType) const
	{
		return simpleSubjectXMLQuerry(subjectID, subjectType, "ListFileAttributeDataXML", wsdlService);
	}

	std::string MotionBasicQueriesWS::listFileAttributeDataWithAttributesXML(const int subjectID, const std::string & subjectType) const
	{
		return simpleSubjectXMLQuerry(subjectID, subjectType, "ListFileAttributeDataWithAttributesXML", wsdlService);
	}

	std::string MotionBasicQueriesWS::listAttributesDefined(const std::string & attributeGroupName,
		const std::string & entityKind) const
	{
		wsdlService->setOperation("ListAttributesDefined");
		wsdlService->setValue("attributeGroupName", attributeGroupName);
		wsdlService->setValue("entityKind", entityKind);
		wsdlService->invoke();
		return wsdlService->xmlResponse();
	}

	std::string MotionBasicQueriesWS::listAttributeGroupsDefined(const std::string & entityKind) const
	{
		wsdlService->setOperation("ListAttributeGroupsDefined");
		wsdlService->setValue("entityKind", entityKind);
		wsdlService->invoke();
		return wsdlService->xmlResponse();
	}

	std::string MotionBasicQueriesWS::listSessionGroupsDefined() const
	{
		return simpleXMLQuerry("ListSessionGroupsDefined", wsdlService);
	}

	std::string MotionBasicQueriesWS::listMotionKindsDefined() const
	{
		return simpleXMLQuerry("ListMotionKindsDefined", wsdlService);
	}

	std::string MotionBasicQueriesWS::listEnumValues(const std::string & attributeName,
		const std::string & entityKind) const
	{
		wsdlService->setOperation("ListEnumValues");
		wsdlService->setValue("attributeName", attributeName);
		wsdlService->setValue("entityKind", entityKind);
		wsdlService->invoke();
		return wsdlService->xmlResponse();
	}

	std::string MotionBasicQueriesWS::listMyAnnotationsXML() const
	{
		return simpleXMLQuerry("ListMyAnnotationsXML", wsdlService);
	}

	std::string MotionBasicQueriesWS::listAnnotationsXML() const
	{
		return simpleXMLQuerry("ListAnnotationsXML", wsdlService);
	}

	std::string MotionBasicQueriesWS::listMyAwaitingAnnotationsXML() const
	{
		return simpleXMLQuerry("ListMyAwaitingAnnotationsXML", wsdlService);
	}

	std::string MotionBasicQueriesWS::listMyReviewedAnnotationsXML() const
	{
		return simpleXMLQuerry("ListMyReviewedAnnotationsXML", wsdlService);
	}

	DateTime MotionBasicQueriesWS::dataModificationTime() const
	{
		return GeneralBasicQueriesWS(wsdlService).dataModificationTime();
	}

	DateTime MotionBasicQueriesWS::metadataModificationTime() const
	{
		return GeneralBasicQueriesWS(wsdlService).metadataModificationTime();
	}

	MedicalBasicQueriesWS::MedicalBasicQueriesWS(networkUtils::IWSDLServicePtr wsdlService)
		: wsdlService(wsdlService)
	{
	}

	MedicalBasicQueriesWS::~MedicalBasicQueriesWS()
	{
	}

	DateTime MedicalBasicQueriesWS::dataModificationTime() const
	{
		return GeneralBasicQueriesWS(wsdlService).dataModificationTime();
	}

	DateTime MedicalBasicQueriesWS::metadataModificationTime() const
	{
		return GeneralBasicQueriesWS(wsdlService).metadataModificationTime();
	}
}