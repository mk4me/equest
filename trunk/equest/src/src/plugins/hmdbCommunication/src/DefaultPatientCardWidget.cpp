#include "CommunicationPCH.h"
#include <plugins/hmdbCommunication/DefaultPatientCardWidget.h>
#include "AntropometricDataWidget.h"
#include "PersonalDataWidget.h"
#include "DisordersDataWidget.h"
#include "NotesWidget.h"
#include "QMultiToolBox.h"

#include <QtWidgets/QTextEdit>
#include <QtWidgets/QTreeWidget>
#include <QtWidgets/QLabel>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QListWidget>
//#include "DataSourceUserData.h"
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QVBoxLayout>
#include <plugins/hmdbCommunication/IDataSourceContent.h>
#include <QtWidgets/QGroupBox>
#include <hmdbserviceslib/DateTimeUtils.h>

using namespace hmdbCommunication;
using namespace hmdbServices;

QString getDate(const hmdbServices::Date & date)
{
	QString ret;

	if (date.getYear() >= 1800){
		ret = QString::fromStdString(date.toString());
	}

	return ret;
}

class SessionTreeItem : public QTreeWidgetItem
{
public:

	SessionTreeItem(const MotionShallowCopy::Attrs & attributes) : QTreeWidgetItem(), attributes_(attributes)
	{
	}

	virtual ~SessionTreeItem()
	{
	}

	const MotionShallowCopy::Attrs & attributes()  const
	{
		return attributes_;
	}

private:

	MotionShallowCopy::Attrs attributes_;
};

DefaultPatientCardWidget::DefaultPatientCardWidget(QWidget * parent) : QFrame(parent), sessionsWidget(new QTreeWidget()),
personalDataWidget(new PersonalDataWidget()), disordersDataWidget(new DisordersDataWidget()),
antropomtricDataWidget(new AntropometricDataWidget()), notesWidget(new NotesWidget())
{
	QStringList headers;

	headers << tr("Session Group");

	sessionsWidget->setHeaderLabels(headers);
	connect(sessionsWidget, SIGNAL(currentItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)), this, SLOT(currentSessionChanged(QTreeWidgetItem*, QTreeWidgetItem*)));

	personalDataWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
	disordersDataWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
	sessionsWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
	notesWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

	QGroupBox * groupA = new QGroupBox(tr("Personal data"));
	auto hBLayout = new QHBoxLayout();
	groupA->setLayout(hBLayout);
	groupA->layout()->addWidget(personalDataWidget);
	hBLayout->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Fixed));

	disordersDataWidget->setMaximumHeight(250);

	QGroupBox * groupB = new QGroupBox(tr("Disorders"));
	groupB->setLayout(new QHBoxLayout());
	groupB->layout()->addWidget(disordersDataWidget);

	sessionsWidget->setMaximumHeight(250);

	QGroupBox * groupC = new QGroupBox(tr("Sessions"));
	groupC->setLayout(new QHBoxLayout());
	groupC->layout()->addWidget(sessionsWidget);

	QGroupBox * groupD = new QGroupBox(tr("Notes"));
	groupD->setLayout(new QHBoxLayout());
	groupD->layout()->addWidget(notesWidget);

	QWidget * leftWidget = new QWidget();
	leftWidget->setObjectName(QString::fromUtf8("leftWidget"));
	QVBoxLayout * vLayout = new QVBoxLayout();
	vLayout->setContentsMargins(0, 0, 0, 0);
	vLayout->addWidget(groupA);
	vLayout->addWidget(groupB);
	vLayout->addWidget(groupC);
	vLayout->addWidget(groupD);

	leftWidget->setLayout(vLayout);

	leftWidget->setMinimumWidth(450);
	leftWidget->setContentsMargins(0, 0, 0, 0);

	antropomtricDataWidget->setMinimumWidth(450);
	QGroupBox * groupE = new QGroupBox(tr("Anthropometric Data"));
	groupE->setLayout(new QHBoxLayout());
	groupE->layout()->addWidget(antropomtricDataWidget);

	QHBoxLayout * layout = new QHBoxLayout();

	layout->addWidget(leftWidget);
	layout->addWidget(groupE);

	setLayout(layout);

	innerSetPatient(nullptr, QPixmap(), nullptr);
}

DefaultPatientCardWidget::~DefaultPatientCardWidget()
{
}

void DefaultPatientCardWidget::setPatient(const hmdbServices::MedicalShallowCopy::Patient * patient,
	const hmdbServices::MotionShallowCopy::Performer * subject,
	const QPixmap & photo)
{
	innerSetPatient(patient, photo, subject);
}

void DefaultPatientCardWidget::setPersonalData(const MedicalShallowCopy::Patient * patient, const QPixmap & photo)
{
	if (patient == nullptr){
		personalDataWidget->reset();
	}
	else{
		personalDataWidget->setName(QString::fromUtf8(patient->name.c_str()));
		personalDataWidget->setSurname(QString::fromUtf8(patient->surname.c_str()));
		personalDataWidget->setGender(hmdbCommunication::IDataSourceContent::decodeGender(patient->gender));
		personalDataWidget->setBirthdate(getDate(patient->birthDate));
		personalDataWidget->setPhoto(photo);
	}
}

void DefaultPatientCardWidget::setDisordersData(const MedicalShallowCopy::PatientDisorders & patientDisorders)
{
	disordersDataWidget->setAutoUpdate(false);

	if (patientDisorders.empty() == false){
		int s = (std::max)(patientDisorders.size(), static_cast<MedicalShallowCopy::PatientDisorders::size_type>(2));
		disordersDataWidget->setDisordersCount(s);

		int i = 0;
		for (auto it = patientDisorders.begin(); it != patientDisorders.end(); ++it){
			disordersDataWidget->setDisorder(i++, QString::fromUtf8(it->second.disorder->name.c_str()),
				getDate(it->second.diagnosisDate), QString::fromUtf8(it->second.focus.c_str()));
		}

		for (; i < s; ++i){
			disordersDataWidget->setDisorder(i, QString(), QString(), QString());
		}
	}
	else{
		int emptyRows = 2;
		disordersDataWidget->setDisordersCount(emptyRows);

		for (int i = 0; i < emptyRows; ++i){
			disordersDataWidget->setDisorder(i, QString(), QString(), QString());
		}
	}

	disordersDataWidget->setAutoUpdate(true);
}

void DefaultPatientCardWidget::currentSessionChanged(QTreeWidgetItem * current, QTreeWidgetItem * prev)
{
	SessionTreeItem * item = dynamic_cast<SessionTreeItem*>(current);
	QStringList headers;

	if (item == nullptr){
		setAntropometricData(MotionShallowCopy::Attrs());
		headers << tr("Session group");
		headers << tr("Date");
	}
	else{
		setAntropometricData(item->attributes());

		headers << tr("Session");
		headers << tr("Laboratory");
		headers << tr("Owner ID");
		headers << tr("Motion kind");
		headers << tr("Tags");
		headers << tr("Attributes");
	}

	sessionsWidget->setHeaderLabels(headers);
	for (int i = 0; i < sessionsWidget->columnCount(); ++i){
		sessionsWidget->resizeColumnToContents(i);
	}
}

void DefaultPatientCardWidget::setSessionsData(const MotionShallowCopy::PerformerConfs & subjectConfigurations)
{
	//czyścimy zawartość sesji
	sessionsWidget->clear();

	if (subjectConfigurations.empty() == false){
		//wypełniamy sesjami

		//grupujemy wykresy na przed/po badaniu + upperBody, lowerBody
		std::map<hmdbServices::DateTime, std::map<int, const MotionShallowCopy::Session *>> groupedData;

		for (auto it = subjectConfigurations.begin(); it != subjectConfigurations.end(); ++it){
			auto session = it->second->session;
			groupedData[session->sessionDate][session->sessionID] = session;
		}

		if (groupedData.begin()->second.empty() == false){
			QTreeWidgetItem * item = createBranch(QObject::tr("Before treatment"), groupedData.begin()->second);
			sessionsWidget->addTopLevelItem(item);
		}

		if (groupedData.begin()->first != groupedData.rbegin()->first && groupedData.rbegin()->second.empty() == false){
			QTreeWidgetItem * item = createBranch(tr("After treatment"), groupedData.rbegin()->second);
			sessionsWidget->addTopLevelItem(item);
		}
	}
	else{
		setAntropometricData(MotionShallowCopy::Attrs());
	}

	sessionsWidget->expandAll();
	for (int i = 0; i < sessionsWidget->columnCount(); ++i){
		sessionsWidget->resizeColumnToContents(i);
	}

	if (sessionsWidget->topLevelItemCount() > 0 && sessionsWidget->topLevelItem(0)->childCount() > 0){
		sessionsWidget->setCurrentItem(sessionsWidget->topLevelItem(0)->child(0));
	}
}

QTreeWidgetItem * DefaultPatientCardWidget::createBranch(const QString & name, const std::map<int, const MotionShallowCopy::Session *> & sessions)
{
	QTreeWidgetItem * ret = new QTreeWidgetItem();

	ret->setText(0, name);
	ret->setText(1, getDate(sessions.begin()->second->sessionDate));

	//górna partia
	if (sessions.begin()->first % 2 == 0){
		SessionTreeItem * item = new SessionTreeItem(sessions.begin()->second->performerConf->attrs);
		item->setText(0, tr("Upper body") + " - " + sessions.begin()->second->sessionName.c_str());
		item->setText(1, QString::number(sessions.begin()->second->labID));
		item->setText(2, QString::number(sessions.begin()->second->userID));
		item->setText(3, QString::fromStdString(sessions.begin()->second->motionKind));
		item->setText(4, QString::fromStdString(sessions.begin()->second->tags));

		std::string attr;

		for (auto it = sessions.begin()->second->attrs.begin(); it != sessions.begin()->second->attrs.end(); ++it){
			attr += it->first + "=" + it->second + ";";
		}

		item->setText(5, QString::fromStdString(attr));

		ret->addChild(item);
	}

	QString sname(tr("Lower body"));

	//górna partia
	if (sessions.begin()->first % 2 == 1){
		sname = tr("Upper body");
	}

	SessionTreeItem * item = new SessionTreeItem(sessions.begin()->second->performerConf->attrs);
	item->setText(0, sname + " - " + QString::fromStdString(sessions.begin()->second->sessionName));
	item->setText(1, QString::number(sessions.begin()->second->labID));
	item->setText(2, QString::number(sessions.begin()->second->userID));
	item->setText(3, QString::fromStdString(sessions.begin()->second->motionKind));
	item->setText(4, QString::fromStdString(sessions.begin()->second->tags));

	std::string attr;

	for (auto it = sessions.begin()->second->attrs.begin(); it != sessions.begin()->second->attrs.end(); ++it){
		attr += it->first + "=" + it->second + ";";
	}

	item->setText(5, QString::fromStdString(attr));

	ret->addChild(item);

	if (sessions.rbegin()->first % 2 == 1){
		return ret;
	}

	sname = tr("Lower body");

	item = new SessionTreeItem(sessions.rbegin()->second->performerConf->attrs);
	item->setText(0, sname + " - " + QString::fromStdString(sessions.rbegin()->second->sessionName));
	item->setText(1, QString::number(sessions.rbegin()->second->labID));
	item->setText(2, QString::number(sessions.rbegin()->second->userID));
	item->setText(3, QString::fromStdString(sessions.rbegin()->second->motionKind));
	item->setText(4, QString::fromStdString(sessions.rbegin()->second->tags));

	std::string().swap(attr);

	for (auto it = sessions.rbegin()->second->attrs.begin(); it != sessions.rbegin()->second->attrs.end(); ++it){
		attr += it->first + "=" + it->second + ";";
	}

	item->setText(5, QString::fromStdString(attr));

	ret->addChild(item);

	return ret;
}

void DefaultPatientCardWidget::setNotes(const MedicalShallowCopy::Patient * patient,
	const hmdbServices::MotionShallowCopy::Performer * subject)
{
	notesWidget->setCurrentPatient(patient == nullptr ? -1 : patient->patientID);
}

void DefaultPatientCardWidget::setAntropometricData(const MotionShallowCopy::Attrs & attributes)
{
	antropomtricDataWidget->setAutoUpdate(false);

	antropomtricDataWidget->setBodyMass(getAttribute(attributes, "BodyMass"));
	antropomtricDataWidget->setBodyHeight(getAttribute(attributes, "Height"));
	antropomtricDataWidget->setASIS(getAttribute(attributes, "InterAsisDistance"));

	antropomtricDataWidget->setLeftLegLength(getAttribute(attributes, "LeftLegLength"));
	antropomtricDataWidget->setRightLegLenght(getAttribute(attributes, "RightLegLenght"));

	antropomtricDataWidget->setLeftKneeWidth(getAttribute(attributes, "LeftKneeWidth"));
	antropomtricDataWidget->setRightKneeWidth(getAttribute(attributes, "RightKneeWidth"));

	antropomtricDataWidget->setLeftAnkleWidth(getAttribute(attributes, "LeftAnkleWidth"));
	antropomtricDataWidget->setRightAnkleWidth(getAttribute(attributes, "RightAnkleWidth"));

	antropomtricDataWidget->setLeftCircuitThigh(getAttribute(attributes, "LeftCircuitThigh"));
	antropomtricDataWidget->setRightCircuitThight(getAttribute(attributes, "RightCircuitThight"));

	antropomtricDataWidget->setLeftCircuitShank(getAttribute(attributes, "LeftCircuitShank"));
	antropomtricDataWidget->setRightCircuitShank(getAttribute(attributes, "RightCircuitShank"));

	antropomtricDataWidget->setLeftShoulderOffset(getAttribute(attributes, "LeftShoulderOffset"));
	antropomtricDataWidget->setRightShoulderOffset(getAttribute(attributes, "RightShoulderOffset"));

	antropomtricDataWidget->setLeftElbowWidth(getAttribute(attributes, "LeftElbowWidth"));
	antropomtricDataWidget->setRightElbowWidth(getAttribute(attributes, "RightElbowWidth"));

	antropomtricDataWidget->setLeftWristWidth(getAttribute(attributes, "LeftWristWidth"));
	antropomtricDataWidget->setRightWristWidth(getAttribute(attributes, "RightWristWidth"));

	antropomtricDataWidget->setLeftWristThickness(getAttribute(attributes, "LeftWristThickness"));
	antropomtricDataWidget->setRightWristThickness(getAttribute(attributes, "RightWristThickness"));

	antropomtricDataWidget->setLeftHandWidth(getAttribute(attributes, "LeftHandWidth"));
	antropomtricDataWidget->setRightHandWidth(getAttribute(attributes, "RightHandWidth"));

	antropomtricDataWidget->setLeftHandThickness(getAttribute(attributes, "LeftHandThickness"));
	antropomtricDataWidget->setRightHandThickness(getAttribute(attributes, "RightHandThickness"));

	antropomtricDataWidget->setAutoUpdate(true);
}

QString DefaultPatientCardWidget::getAttribute(const MotionShallowCopy::Attrs & attributes, const std::string & name, const QString & defaultVal)
{
	QString value(defaultVal);
	auto it = attributes.find(name);
	if (it != attributes.end()){
		value = QString::fromStdString(it->second);
	}

	return value;
}

void DefaultPatientCardWidget::innerSetPatient(const hmdbServices::MedicalShallowCopy::Patient * patient, const QPixmap & photo, const hmdbServices::MotionShallowCopy::Performer * subject)
{
	setPersonalData(patient, photo);
	setDisordersData(patient == nullptr ? MedicalShallowCopy::PatientDisorders() : patient->disorders);

	if (subject == nullptr){
		setSessionsData(MotionShallowCopy::PerformerConfs());
	}
	else{
		setSessionsData(subject->performerConfs);
	}

	setNotes(patient, subject);
}

DefaultPatientCard::DefaultPatientCard() : patientCardWidget(new DefaultPatientCardWidget())
{
}

DefaultPatientCard::~DefaultPatientCard()
{
}

const std::string DefaultPatientCard::name()
{
	return "Default Patient Card";
}

QWidget * DefaultPatientCard::widget()
{
	return patientCardWidget;
}

void DefaultPatientCard::setPatient(const MedicalShallowCopy::Patient * patient, const hmdbServices::MotionShallowCopy::Performer * subject,
	const QPixmap & photo)
{
	patientCardWidget->setPatient(patient, subject, photo);
}