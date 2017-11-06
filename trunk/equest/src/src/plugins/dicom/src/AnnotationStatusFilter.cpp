#include <DicomPCH.h>
#include <plugins/dicom/AnnotationStatusFilter.h>
#include <plugins/hmdbCommunication/IHMDBSession.h>
#include <plugins/hmdbCommunication/ShallowCopy.h>
#include <hmdbserviceslib/IBasicQueriesWS.h>
#include <hmdbserviceslib/Entity.h>
#include "DicomService.h"

using namespace dicom;

AnnotationStatusFilter::AnnotationStatusFilter(const QString & name,
	const bool accept, const bool missingDefault, const GeneralStatus status,
	hmdbCommunication::IHMDBSession * session)
	: name_(name), accept(accept), missingDefault(missingDefault),
	session(session), status(status)
{

}

AnnotationStatusFilter::~AnnotationStatusFilter()
{

}

AnnotationStatusFilter * AnnotationStatusFilter::create(hmdbCommunication::IHMDBSession * session) const
{
	return new AnnotationStatusFilter(name_, accept, missingDefault, status, session);
}

QString AnnotationStatusFilter::name() const
{
	return name_;
}


void AnnotationStatusFilter::beforeFiltering()
{
	if (session != nullptr){
		auto mq = session->motionQueries();
		if (mq != nullptr){	
			auto modTime = mq->dataModificationTime();
			if (modTime > lastUpdateTime){
				refreshIdentifiers();
				lastUpdateTime = modTime;
			}
		}
	}
}

void AnnotationStatusFilter::groupAnnotations(hmdbCommunication::IHMDBSession * session,
	AnnotationStatusFilter::Identifiers & inEdition,
	AnnotationStatusFilter::Identifiers & inVerification,
	AnnotationStatusFilter::Identifiers & verified)
{
	try{
		bool userIsReviewer = dicom::DicomService::userIsReviewer(session);
		if (userIsReviewer == true){
			auto resp = session->motionQueries()->listAnnotationsXML();
			auto annotations = hmdbServices::xmlWsdl::parseAnnotations(resp);

			std::map<int, std::map<int, hmdbServices::xmlWsdl::AnnotationStatus::Type>> annotationsByUsers;

			for (const hmdbServices::xmlWsdl::Annotation & a : annotations){
				annotationsByUsers[a.trialID][a.userID] = a.status;
			}

			for (auto trialAnnotations : annotationsByUsers){

				int verifiedC = 0;
				int inEditionC = 0;
				int waitingC = 0;

				for (auto annotation : trialAnnotations.second){

					switch (annotation.second){
					case hmdbServices::xmlWsdl::AnnotationStatus::Approved:
						++verifiedC;
						break;

					case hmdbServices::xmlWsdl::AnnotationStatus::ReadyForReview:
						++waitingC;
						break;

					default:
						++inEditionC;
						break;
					}
				}

				if (inEditionC > 0){
					inEdition.insert(trialAnnotations.first);
				}
				else if (waitingC > 0){
					inVerification.insert(trialAnnotations.first);
				}
				else if (verifiedC > 0){
					verified.insert(trialAnnotations.first);
				}
			}
		}
		else{

			auto resp = session->motionQueries()->listMyAnnotationsXML();
			auto annotations = hmdbServices::xmlWsdl::parseAnnotations(resp);

			for (auto annotation : annotations){
				switch (annotation.status){
				case hmdbServices::xmlWsdl::AnnotationStatus::Approved:
					verified.insert(annotation.trialID);
					break;

				case hmdbServices::xmlWsdl::AnnotationStatus::ReadyForReview:
					inVerification.insert(annotation.trialID);
					break;

				default:
					inEdition.insert(annotation.trialID);
					break;
				}
			}
		}
	}
	catch (...){

	}
}

void AnnotationStatusFilter::refreshIdentifiers()
{
	AnnotationStatusFilter::Identifiers inEdition;
	AnnotationStatusFilter::Identifiers inVerification;
	AnnotationStatusFilter::Identifiers verified;

	groupAnnotations(session, inEdition, inVerification, verified);

	switch (status)
	{
	case dicom::AnnotationStatusFilter::InEdition:
		identifiers = inEdition;
		break;
	case dicom::AnnotationStatusFilter::InVerification:
		identifiers = inVerification;
		break;
	case dicom::AnnotationStatusFilter::Verified:
		identifiers = verified;
		break;
	default:
		break;
	}
}

bool AnnotationStatusFilter::filterMotion(const hmdbServices::MotionShallowCopy::Trial* motion) const
{
	return (identifiers.find(motion->trialID) != identifiers.end()) ? accept : missingDefault;
}