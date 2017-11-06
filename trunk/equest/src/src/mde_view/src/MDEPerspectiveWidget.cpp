#include "MDEPerspectiveWidget.h"
#include <plugins/hmdbCommunication/GeneralSourceViewWidget.h>
#include <plugins/hmdbCommunication/DataViewWidget.h>
#include <plugins/hmdbCommunication/DefaultPatientCardWidget.h>
#include <QtWidgets/QHBoxLayout>

MDEPerspectiveWidget::MDEPerspectiveWidget(hmdbCommunication::IHMDBShallowCopyContextPtr shallowCopyContext,
	QWidget * parent, Qt::WindowFlags f)
	: QWidget(parent, f), patientCardWidget(new DefaultPatientCardWidget),
	generalViewWidget(new GeneralSourceViewWidget(shallowCopyContext))
{
	auto l = new QHBoxLayout;
	l->addWidget(generalViewWidget);
	l->addWidget(patientCardWidget);
	setLayout(l);

	auto view = generalViewWidget->dataView();

	connect(view, SIGNAL(shallowCopyChanged()), this, SLOT(onShallowCopyChange()));
	connect(view, SIGNAL(currentItemChanged(char, int, char, int)), this, SLOT(onItemChange(char, int, char, int)));
}

MDEPerspectiveWidget::~MDEPerspectiveWidget()
{

}

GeneralSourceViewWidget * MDEPerspectiveWidget::generalDataView()
{
	return generalViewWidget;
}

void MDEPerspectiveWidget::onShallowCopyChange()
{
	shallowCopy_ = generalViewWidget->dataView()->completeShallowCopy();
}

void MDEPerspectiveWidget::onItemChange(char currentItem, int currentID, char previousItem, int previousID)
{
	hmdbServices::MotionShallowCopy::Performer * performer = nullptr;
	hmdbServices::MedicalShallowCopy::Patient * patient = nullptr;
	if (shallowCopy_ != nullptr && currentItem != -1 && currentID != -1){
		hmdbCommunication::DataType df = (hmdbCommunication::DataType)currentItem;
		switch (df)
		{
		case hmdbCommunication::FileType:
		{
			auto it = shallowCopy_->motionShallowCopy.files.find(currentID);
			if (it != shallowCopy_->motionShallowCopy.files.end()){
				if (it->second->isSessionFile() == true){
					performer = it->second->session->performerConf->performer;
				}
				else{
					performer = it->second->trial->session->performerConf->performer;
				}
			}
		}
			break;
		case hmdbCommunication::MotionType:
		{
			auto it = shallowCopy_->motionShallowCopy.trials.find(currentID);
			if (it != shallowCopy_->motionShallowCopy.trials.end()){
				performer = it->second->session->performerConf->performer;
			}
		}
			break;
		case hmdbCommunication::SessionType:
		{
			auto it = shallowCopy_->motionShallowCopy.sessions.find(currentID);
			if (it != shallowCopy_->motionShallowCopy.sessions.end()){
				performer = it->second->performerConf->performer;
			}
		}
			break;
		case hmdbCommunication::SubjectType:
		{
			auto it = shallowCopy_->motionShallowCopy.performers.find(currentID);
			if (it != shallowCopy_->motionShallowCopy.performers.end()){
				performer = it->second;
			}
		}
			break;
		case hmdbCommunication::PatientType:
		{
			auto it = shallowCopy_->medicalShallowCopy.patients.find(currentID);
			if (it != shallowCopy_->medicalShallowCopy.patients.end()){
				patient = it->second;
			}
		}
			break;
		default:
			break;
		}

		if (performer != nullptr){
			patient = performer->patient;
		}
		else if (patient != nullptr){
			performer = patient->performer;
		}
	}

	patientCardWidget->setPatient(patient, performer);
}