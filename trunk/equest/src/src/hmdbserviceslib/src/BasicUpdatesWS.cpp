#include <hmdbserviceslib/BasicUpdatesWS.h>
#include <hmdbserviceslib/DateTimeUtils.h>

namespace hmdbServices
{
	MotionBasicUpdatesWS::MotionBasicUpdatesWS(networkUtils::IWSDLServicePtr wsdlService)
		: wsdlService(wsdlService)
	{
	}

	MotionBasicUpdatesWS::~MotionBasicUpdatesWS()
	{
	}

	int MotionBasicUpdatesWS::createPerformer(const std::string & name,
		const std::string & surname)
	{
		wsdlService->setOperation("CreatePerformer");
		wsdlService->setValue("Name", name);
		wsdlService->setValue("Surname", surname);
		wsdlService->invoke(true);

		int ret = -1;
		wsdlService->getValue<int>("CreatePerformerResult", ret);

		return ret;
	}

	int MotionBasicUpdatesWS::createSession(const int labID, const std::string & motionKindName,
		const std::string & sessionDate, const std::string & sessionDescription,
		const std::set<int> & sessionGroupIDs)
	{
		wsdlService->setOperation("CreateSession");
		wsdlService->setValue("labID", labID);
		wsdlService->setValue("motionKindName", motionKindName);
		wsdlService->setValue("sessionDate", sessionDate);
		wsdlService->setValue("sessionDescription", sessionDescription);

		std::string ids;

		//TODO - generowanie identyfikatorów dla grup sesji

		wsdlService->setValue("sessionGroupIDs", ids);
		wsdlService->invoke(true);

		int ret = -1;
		wsdlService->getValue<int>("CreateSessionResult", ret);

		return ret;
	}

	int MotionBasicUpdatesWS::createTrial(const int sessionID, const std::string & trialDescription)
	{
		wsdlService->setOperation("CreateTrial");
		wsdlService->setValue("sessionID", sessionID);
		wsdlService->setValue("trialDescription", trialDescription);
		wsdlService->invoke(true);

		int ret = -1;
		wsdlService->getValue<int>("CreateTrialResult", ret);

		return ret;
	}

	int MotionBasicUpdatesWS::createMeasurementConfiguration(const std::string & mcName,
		const std::string & mcKind, const std::string & mcDescription)
	{
		wsdlService->setOperation("CreateMeasurementConfiguration");
		wsdlService->setValue("mcName", mcName);
		wsdlService->setValue("mcKind", mcKind);
		wsdlService->setValue("mcDescription", mcDescription);
		wsdlService->invoke(true);

		int ret = -1;
		wsdlService->getValue<int>("CreateMeasurementConfigurationResult", ret);

		return ret;
	}

	int MotionBasicUpdatesWS::assignSessionToGroup(const int sessionID, const int sessionsGroupID)
	{
		wsdlService->setOperation("AssignSessionToGroup");
		wsdlService->setValue("sessionID", sessionID);
		wsdlService->setValue("sessionsGroupID", sessionsGroupID);
		wsdlService->invoke(true);

		int ret = -1;
		wsdlService->getValue<int>("AssignSessionToGroupResult", ret);

		return ret;
	}

	int MotionBasicUpdatesWS::assignPerformerToSession(const int performerID, const int sessionID)
	{
		wsdlService->setOperation("AssignPerformerToSession");
		wsdlService->setValue("performerID", performerID);
		wsdlService->setValue("sessionID", sessionID);
		wsdlService->invoke(true);

		int ret = -1;
		wsdlService->getValue<int>("AssignPerformerToSessionResult", ret);

		return ret;
	}

	bool MotionBasicUpdatesWS::assignPerformerToMeasurement(const int performerID, const int measurementID)
	{
		wsdlService->setOperation("AssignPerformerToMeasurement");
		wsdlService->setValue("performerID", performerID);
		wsdlService->setValue("measurementID", measurementID);
		wsdlService->invoke(true);

		bool ret = false;
		wsdlService->getValue<bool>("AssignPerformerToMeasurementResult", ret);

		return ret;
	}

	void MotionBasicUpdatesWS::setPerformerAttribute(const int performerID, const std::string & attributeName,
		const std::string & attributeValue, const bool update)
	{
		wsdlService->setOperation("SetPerformerAttribute");
		wsdlService->setValue("performerID", performerID);
		wsdlService->setValue("attributeName", attributeName);
		wsdlService->setValue("attributeValue", attributeValue);
		wsdlService->setValue("update", update);
		wsdlService->invoke();
	}

	void MotionBasicUpdatesWS::setSessionAttribute(const int sessionID, const std::string & attributeName,
		const std::string & attributeValue, const bool update)
	{
		wsdlService->setOperation("SetSessionAttribute");
		wsdlService->setValue("sessionID", sessionID);
		wsdlService->setValue("attributeName", attributeName);
		wsdlService->setValue("attributeValue", attributeValue);
		wsdlService->setValue("update", update);
		wsdlService->invoke();
	}

	void MotionBasicUpdatesWS::setTrialAttribute(const int trialID, const std::string & attributeName,
		const std::string & attributeValue, const bool update)
	{
		wsdlService->setOperation("SetTrialAttribute");
		wsdlService->setValue("trialID", trialID);
		wsdlService->setValue("attributeName", attributeName);
		wsdlService->setValue("attributeValue", attributeValue);
		wsdlService->setValue("update", update);
		wsdlService->invoke();
	}

	void MotionBasicUpdatesWS::setMeasurementConfAttribute(const int measurementConfID, const std::string & attributeName,
		const std::string & attributeValue, const bool update)
	{
		wsdlService->setOperation("SetMeasurementConfAttribute");
		wsdlService->setValue("measurementConfID", measurementConfID);
		wsdlService->setValue("attributeName", attributeName);
		wsdlService->setValue("attributeValue", attributeValue);
		wsdlService->setValue("update", update);
		wsdlService->invoke();
	}

	void MotionBasicUpdatesWS::setPerformerConfAttribute(const int performerConfID, const std::string & attributeName,
		const std::string & attributeValue, const bool update)
	{
		wsdlService->setOperation("SetPerformerConfAttribute");
		wsdlService->setValue("performerConfID", performerConfID);
		wsdlService->setValue("attributeName", attributeName);
		wsdlService->setValue("attributeValue", attributeValue);
		wsdlService->setValue("update", update);
		wsdlService->invoke();
	}

	void MotionBasicUpdatesWS::setFileAttribute(const int fileID, const std::string & attributeName,
		const std::string & attributeValue, const bool update)
	{
		wsdlService->setOperation("SetFileAttribute");
		wsdlService->setValue("fileID", fileID);
		wsdlService->setValue("attributeName", attributeName);
		wsdlService->setValue("attributeValue", attributeValue);
		wsdlService->setValue("update", update);
		wsdlService->invoke();
	}

	void MotionBasicUpdatesWS::setFileTypedAttributeValue(const int resourceID, const std::string & entity,
		const std::string & attributeName, const int fileID, const bool update)
	{
		wsdlService->setOperation("SetFileTypedAttributeValue");
		wsdlService->setValue("resourceID", resourceID);
		wsdlService->setValue("entity", entity);
		wsdlService->setValue("attributeName", attributeName);
		wsdlService->setValue("fileID", fileID);
		wsdlService->setValue("update", update);
		wsdlService->invoke();
	}

	void MotionBasicUpdatesWS::clearAttributeValue(const int resourceID, const std::string & attributeName,
		const std::string & entity)
	{
		wsdlService->setOperation("ClearAttributeValue");
		wsdlService->setValue("resourceID", resourceID);
		wsdlService->setValue("attributeName", attributeName);
		wsdlService->setValue("entity", entity);
		wsdlService->invoke();
	}

	void MotionBasicUpdatesWS::setMyAnnotationStatus(const int trialID, const int status,
		const std::string & comment)
	{
		wsdlService->setOperation("SetMyAnnotationStatus");
		wsdlService->setValue("trialID", trialID);
		wsdlService->setValue("status", status);
		wsdlService->setValue("comment", comment);
		wsdlService->invoke();
	}

	void MotionBasicUpdatesWS::setAnnotationReview(const int trialID, const int userID,
		const int status,
		const std::string & note)
	{
		wsdlService->setOperation("SetAnnotationReview");
		wsdlService->setValue("trialID", trialID);
		wsdlService->setValue("userID", userID);
		wsdlService->setValue("status", status);
		wsdlService->setValue("note", note);
		wsdlService->invoke();
	}

	MedicalBasicUpdatesWS::MedicalBasicUpdatesWS(networkUtils::IWSDLServicePtr wsdlService)
		: wsdlService(wsdlService)
	{
	}

	MedicalBasicUpdatesWS::~MedicalBasicUpdatesWS()
	{
	}
}