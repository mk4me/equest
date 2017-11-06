#include "CommunicationPCH.h"
#include <plugins/hmdbCommunication/DataSourceDefaultContent.h>
#include <QtCore/QObject>

using namespace hmdbCommunication;

DataSourceDefaultContent::DataSourceDefaultContent()
{

}

DataSourceDefaultContent::~DataSourceDefaultContent()
{

}

const QString DataSourceDefaultContent::name() const
{
	return QObject::tr("Default Content");
}

const bool DataSourceDefaultContent::fillContent(QTreeWidgetItem * item, const ShallowCopy & shallowCopy) const
{
	bool ret = false;
	auto content = dynamic_cast<TreeWidgetContentItem*>(item);
	if (content != nullptr){
		//wystarczy switch + dynamic_cast
		switch (content->contentType()){

		case PatientContent:
		{
			auto it = shallowCopy.medicalShallowCopy.patients.find(content->id());
			if (it != shallowCopy.medicalShallowCopy.patients.end()){
				content->setText(0, QString::fromUtf8((it->second->surname + ", " + it->second->name).c_str()));
				ret = true;
			}
		}
			break;
		case SubjectContent:
		{
			auto it = shallowCopy.motionShallowCopy.performers.find(content->id());
			if (it != shallowCopy.motionShallowCopy.performers.end()){
				content->setText(0, QObject::tr("Subject") + " " + QString::number(it->second->performerID));
				ret = true;
			}
		}
			break;
		case SessionContent:
		{
			auto it = shallowCopy.motionShallowCopy.sessions.find(content->id());
			if (it != shallowCopy.motionShallowCopy.sessions.end()){
				content->setText(0, QString::fromStdString(it->second->sessionName));
				ret = true;
			}			
		}
			break;
		case MotionContent:
		{
			auto it = shallowCopy.motionShallowCopy.trials.find(content->id());
			if (it != shallowCopy.motionShallowCopy.trials.end()){
				content->setText(0, QString::fromStdString(it->second->trialName));
				ret = true;
			}			
		}
			break;
		case FileContent:
		{
			auto it = shallowCopy.motionShallowCopy.files.find(content->id());
			if (it != shallowCopy.motionShallowCopy.files.end()){
				content->setText(0, QString::fromStdString(it->second->fileName));
				ret = true;
			}
		}
			break;
		}
	}

	return ret;
}

const bool DataSourceDefaultContent::headers(const QTreeWidgetItem * item, QStringList & headers) const
{
	bool ret = false;
	auto content = dynamic_cast<const TreeWidgetContentItem*>(item);
	if (content != nullptr){
		//wystarczy switch + dynamic_cast
		switch (content->contentType()){

		case PatientContent:
			headers << QObject::tr("Patient");
			ret = true;
			break;
		case SubjectContent:
			headers << QObject::tr("Subject");
			ret = true;
			break;
		case SessionContent:
			headers << QObject::tr("Session");
			ret = true;
			break;
		case MotionContent:
			headers << QObject::tr("Motion");
			ret = true;
			break;
		case FileContent:
			headers << QObject::tr("File");
			ret = true;
			break;
		}
	}

	return ret;
}

const int DataSourceDefaultContent::statusField(const QTreeWidgetItem * item) const
{
	return 0;
}

const int DataSourceDefaultContent::downloadField(const QTreeWidgetItem * item) const
{
	return 1;
}

void DataSourceDefaultContent::release()
{

}