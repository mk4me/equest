#include "EquestPCH.h"
#include "EquestAnnotationVisualizer.h"
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QDoubleSpinBox>
#include <utils/StringConversionTools.h>
#include <corelib/PluginCommon.h>
#include <hmdbserviceslib/IAuthorizationWS.h>

#include <QtGui/QIcon>
#include <QtWidgets/QWidget>
#include <QtWidgets/QLayout>
#include <QtWidgets/QLabel>
#include <QtCore/QTimer>
#include <QtWidgets/QToolButton>
#include <plugins/equest/Equest.h>
#include "EquestAnnotation.h"
#include <iomanip>
#include "ui_toolbar.h"
#include "corelib/IServiceManager.h"
#include "plugins/newTimeline/ITimelineService.h"
#include <QtCore/QTime>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QFileDialog>
#include "utils/TimeUtils.h"
#include "plugins/hmdbCommunication/IHMDBShallowCopyContext.h"
#include "hmdbserviceslib/ShallowCopy.h"
#include "plugins/hmdbCommunication/IHMDBStorage.h"
#include <iosfwd>
#include "coreui/CorePopup.h"
#include "corelib/ISourceManager.h"
#include "plugins/hmdbCommunication/IHMDBSource.h"
#include "hmdbserviceslib/IFileStoremanWS.h"
#include "c3dlib/C3DTypes.h"
#include "plugins/equest/EquestPerspective.h"
#include "plugins/subject/IMotion.h"

using namespace core;
using namespace equest;



EquestAnnotationVisualizer::AnnotationSerie::AnnotationSerie(EquestAnnotationVisualizer * visualizer) :
	visualizer(visualizer),
	editable(false)
{

}

EquestAnnotationVisualizer::AnnotationSerie::~AnnotationSerie()
{

}

void EquestAnnotationVisualizer::AnnotationSerie::setName(const std::string & name)
{
	this->name = name;
}

const std::string EquestAnnotationVisualizer::AnnotationSerie::getName() const
{
	return name;
}


void EquestAnnotationVisualizer::AnnotationSerie::setData(const utils::TypeInfo & requestedType, const core::VariantConstPtr & data)
{
	bool success = false;

	this->data = data;
	this->requestedType = requestedType;
	if (data->data()->isSupported(typeid(EquestAnnotations))) {
		EquestAnnotationsConstPtr annotation;
		data->tryGet(annotation);

		QWidget * widget = new QWidget();
		ui.setupUi(widget);

		ui.tableWidget->setColumnCount(4);
		QStringList headers;
		headers << "Start time" << "End time" << "Annotation" << "Go to...";
		//ui.tableWidget->setSelectionMode(QAbstractItemView::MultiSelection);
		ui.tableWidget->setHorizontalHeaderLabels(headers);

		invalidateTable();
		visualizer->widget->layout()->addWidget(widget);
		QObject::connect(ui.annotationButton, SIGNAL(clicked()), visualizer, SLOT(addAnnotation()));
		QObject::connect(ui.tableWidget, SIGNAL(cellDoubleClicked(int, int)), visualizer, SLOT(cellDoubleClicked(int, int)));
		QObject::connect(ui.tableWidget, SIGNAL(cellChanged(int, int)), visualizer, SLOT(cellChanged(int, int)));
	}
	
}

void equest::EquestAnnotationVisualizer::AnnotationSerie::addAnnotation()
{
	EquestAnnotationsConstPtr cptr;
	data->tryGet(cptr);

	EquestAnnotationsPtr annotation = utils::const_pointer_cast<EquestAnnotations>(cptr);
	auto time = visualizer->currentTime;
	annotation->annotations.push_back({ time, time + 1.0, "Annotation" });
	invalidateTable();
	//ui.tableWidget->
}

void equest::EquestAnnotationVisualizer::AnnotationSerie::invalidateCell(int row, int col, double value)
{
	UTILS_ASSERT(col < 2);
	EquestAnnotationsPtr ptr = getAnnotations();
	double& field = col == 0 ? ptr->annotations[row].startTime : ptr->annotations[row].endTime;
	field = value;
}

void equest::EquestAnnotationVisualizer::AnnotationSerie::invalidateCell(int row, int col)
{
	UTILS_ASSERT(col == 2);
	QString value = ui.tableWidget->item(row, col)->text();
	EquestAnnotationsPtr ptr = getAnnotations();
	ptr->annotations[row].text = value;
}

void equest::EquestAnnotationVisualizer::AnnotationSerie::invalidateTable()
{
	EquestAnnotationsConstPtr ptr;
	data->tryGet(ptr);
	auto count = ptr->annotations.size();
	ui.tableWidget->setRowCount(count);

	for (auto i = 0; i < count; ++i) {
		auto& entry = ptr->annotations[i];
		//ui.tableWidget->setItem(i, 0, new QTableWidgetItem(utils::convertToTimeString(entry.startTime)));
		//ui.tableWidget->setItem(i, 1, new QTableWidgetItem(utils::convertToTimeString(entry.endTime)));
		ui.tableWidget->setCellWidget(i, 0, createTableSpinbox(i, 0, entry.startTime));
		ui.tableWidget->setCellWidget(i, 1, createTableSpinbox(i, 1, entry.endTime));
		ui.tableWidget->setItem(i, 2, new QTableWidgetItem(entry.text));
		ui.tableWidget->setCellWidget(i, 3, createTableButton(i, 3));
	}
	

	//ui.tableWidget->setRowCount(static_cast<int>(getLength()/step) + 1);
	//ui.tableWidget->setColumnCount(1);
	//QStringList strings;
	//double start = 0.0;
	//while (start < getLength()) {
	//	strings << convertD(start);
	//	start += this->step;
	//}
	//strings << convertD(getLength());
	//ui.tableWidget->setVerticalHeaderLabels(strings);
	//ui.tableWidget->setHorizontalHeaderLabels(QStringList("Annotation"));
}

EquestAnnotationsConstPtr equest::EquestAnnotationVisualizer::AnnotationSerie::getAnnotations() const
{
	EquestAnnotationsConstPtr ptr;
	data->tryGet(ptr);
	return ptr;
}

EquestAnnotationsPtr equest::EquestAnnotationVisualizer::AnnotationSerie::getAnnotations()
{
	EquestAnnotationsConstPtr ptr;
	data->tryGet(ptr);
	return utils::const_pointer_cast<EquestAnnotations>(ptr);
}

QDoubleSpinBox* equest::EquestAnnotationVisualizer::AnnotationSerie::createTableSpinbox(int row, int column, double value)
{
	QDoubleSpinBox* spin = new QDoubleSpinBox();
	spin->setValue(value);
	spin->setSingleStep(0.1);
	QObject::connect(spin, SIGNAL(valueChanged(double)), visualizer, SLOT(cellValueChanged(double)));
	spin->setProperty("row", row);
	spin->setProperty("column", column);
	return spin;
}

QToolButton* equest::EquestAnnotationVisualizer::AnnotationSerie::createTableButton(int row, int column)
{
	QToolButton* button = new QToolButton();
	QObject::connect(button, SIGNAL(clicked()), visualizer, SLOT(moveTimeline()));
	button->setProperty("row", row);
	button->setProperty("column", column);
	return button;
}

core::VariantConstPtr equest::EquestAnnotationVisualizer::AnnotationSerie::getRelatedC3D()
{
	auto transaction = plugin::getDataManagerReader()->transaction();
	core::ConstVariantsList objects;
	this->motion->getObjects(objects, typeid(c3dlib::VectorChannelReaderInterface), false);
	std::string source;
	std::string name;
	for (auto& var : objects) {
		return var;

	}
	return core::VariantConstPtr();
}

std::string equest::EquestAnnotationVisualizer::AnnotationSerie::getTempFilename() const
{
	std::string filename = this->motionLabel + "." + getUserName(nullptr) + ".xml";
	utils::Filesystem::Path dir = plugin::getPaths()->getUserDataPath() / "EquestAnnotations";
	if (!utils::Filesystem::pathExists(dir)) {
		utils::Filesystem::createDirectory(dir);
	}

	return (dir / filename).string();
}

const utils::TypeInfo & EquestAnnotationVisualizer::AnnotationSerie::getRequestedDataType() const
{
	return requestedType;
}


void EquestAnnotationVisualizer::AnnotationSerie::update()
{	

}

const core::VariantConstPtr & EquestAnnotationVisualizer::AnnotationSerie::getData() const
{
	return data;
}


void EquestAnnotationVisualizer::AnnotationSerie::setTime(double serieTime)
{
	double time = serieTime;

	ui.timeLabel->setText(utils::convertToTimeString(time));

	EquestAnnotationsConstPtr ptr = getAnnotations();
	ui.tableWidget->clearSelection();
	auto count = ptr->annotations.size();
	for (int i = 0; i < count; ++i) {
		auto& entry = ptr->annotations[i];
		if (entry.startTime <= time && entry.endTime >= time) {
			ui.tableWidget->selectRow(i);
			break;
		}
	}
	
}

EquestAnnotationVisualizer::EquestAnnotationVisualizer() :
	widget(nullptr),
	currentSerie(nullptr),
	currentTime(-1.0),
	timer(nullptr)
{

}

EquestAnnotationVisualizer::~EquestAnnotationVisualizer()
{
}

plugin::IVisualizer* EquestAnnotationVisualizer::create() const
{
	return new EquestAnnotationVisualizer();
}

void EquestAnnotationVisualizer::getSupportedTypes(utils::TypeInfoList & supportedTypes) const
{
	supportedTypes.push_back(typeid(EquestAnnotations));
}

void EquestAnnotationVisualizer::update(double deltaTime)
{

}


QWidget* EquestAnnotationVisualizer::createWidget()
{
	widget = new QWidget();
	ui.setupUi(widget);
	connect(ui.saveButton, SIGNAL(clicked()), this, SLOT(save()));
	connect(ui.uploadButton, SIGNAL(clicked()), this, SLOT(upload()));
	timer = new QTimer(widget);
	connect(timer, SIGNAL(timeout()), this, SLOT(updateTime()));
	timer->start(100);
	return widget;
}

QIcon* EquestAnnotationVisualizer::createIcon()
{
	return new QIcon(QString::fromUtf8(":/video/icons/video.png"));
}

int EquestAnnotationVisualizer::getMaxDataSeries() const
{
	return 1;
}

plugin::IVisualizer::ISerie* EquestAnnotationVisualizer::createSerie(const utils::TypeInfo & requestedType, const VariantConstPtr & data)
{
	AnnotationSerie* ret = new AnnotationSerie(this);
	ret->setName("Annotation");
	ret->setData(requestedType, data);
	auto username = getUserName(data);
	auto metaname = getUserNameFromMetadata(data);
	ret->setEditable(username == metaname);
	return ret;
}

plugin::IVisualizer::ISerie* EquestAnnotationVisualizer::createSerie(const plugin::IVisualizer::ISerie * serie)
{
	return nullptr;
}

plugin::IVisualizer::ISerie* EquestAnnotationVisualizer::createSerie(const plugin::IVisualizer::ISerie * serie, const utils::TypeInfo & requestedType, const core::VariantConstPtr & data)
{
	return nullptr;
}

void EquestAnnotationVisualizer::removeSerie(plugin::IVisualizer::ISerie* serie)
{
	currentSerie = nullptr;
}


QPixmap EquestAnnotationVisualizer::takeScreenshot() const
{
	return QPixmap::grabWidget(widget);
}

void EquestAnnotationVisualizer::setActiveSerie(plugin::IVisualizer::ISerie * serie)
{
	auto as = dynamic_cast<AnnotationSerie*>(serie);
	if (as) {
		currentSerie = as;
		bool enabled = as ? as->getEditable() : false;
		this->ui.saveButton->setEnabled(enabled);
		this->ui.uploadButton->setEnabled(enabled);
		this->ui.label->setText(QString::fromStdString(as->motionLabel));
		as->ui.annotationButton->setEnabled(enabled);
	}
	else if (serie && !as) {
		throw std::runtime_error("Unknown serie type");
	}
}

const plugin::IVisualizer::ISerie * EquestAnnotationVisualizer::getActiveSerie() const
{
	return currentSerie;
}

plugin::IVisualizer::ISerie * EquestAnnotationVisualizer::getActiveSerie()
{
	return currentSerie;
}

void equest::EquestAnnotationVisualizer::moveTimeline()
{
	QToolButton* button = qobject_cast<QToolButton*>(sender());
	if (button && currentSerie)
	{
		int row = button->property("row").toInt();
		int col = button->property("column").toInt();
		auto& entry = currentSerie->getAnnotations()->annotations[row];
		auto timeline = core::queryService<ITimelineService>(plugin::getServiceManager());
		if (timeline != nullptr) {
			timeline->setTime(entry.startTime);
		}

	}

}

void equest::EquestAnnotationVisualizer::addAnnotation()
{
	if (currentSerie) {
		currentSerie->addAnnotation();
	}
}

void equest::EquestAnnotationVisualizer::save()
{
	try {
		if (currentSerie) {
			std::string path = currentSerie->getTempFilename();
			if (!path.empty()) {
				EquestAnnotationsConstPtr ptr = currentSerie->getAnnotations();
				EquestAnnotations::save(path, *ptr);
			}
		}
	}
	catch (...) {
		QMessageBox::warning(widget, "Warning", "Save failed");
	}
}

hmdbCommunication::IHMDBShallowCopyContext * equest::EquestAnnotationVisualizer::sourceContextForData(const core::VariantConstPtr data)
{
	hmdbCommunication::IHMDBShallowCopyContext * ret = nullptr;

	auto source = core::querySource<hmdbCommunication::IHMDBSource>(plugin::getSourceManager());
	if (source != nullptr) {
		auto srcContext = source->shallowContextForData(data);
		ret = srcContext.get();
	}

	return ret;
}

void equest::EquestAnnotationVisualizer::upload()
{
	//QMessageBox::warning(widget, "Warning", "Not yet implemented");
	//return;
	save();
	utils::Filesystem::Path p = currentSerie->getTempFilename(); //("C:/EQuest/_out/src/equest_view/2017-05-29-B1022-S01-T01.equest0.xml");// = getXmlOutputFilename();

	/*int trialID = -1;
	{
		auto ss = stem.string();
		const hmdbCommunication::IHMDBShallowCopyContextPtr shallow = comm->shallowContextForData(wrapper);
		auto & shallowCopy = shallow->shallowCopyDataContext()->shallowCopy()->motionShallowCopy;
		auto& trials = shallowCopy.trials;
		for (auto it = trials.begin(); it != trials.end(); ++it) {
			auto* trial = it->second;
			if (ss.find(trial->trialName) != std::string::npos) {
				trialID = trial->trialID;
				break;
			}
		}
	}
	ncimg->setTrialID(trialID);*/
	core::VariantConstPtr data = this->currentSerie->getRelatedC3D();

	const auto fileName = p.filename().string();

	hmdbServices::ID fileID = -1;
	// data to jakis zarzadzany plik triala
	//this->shallowContext_ = sourceContextForData(data);

	//auto remoteSrcContext = remoteShallowContext(getActiveSerie());
	auto remoteSrcContext = sourceContextForData(data);
	auto source = core::querySource<hmdbCommunication::IHMDBSource>(plugin::getSourceManager());
	if (!remoteSrcContext || !remoteSrcContext->shallowCopyRemoteContext()) {
		return;
	}
	int currentTrialID = -1;
	{
		auto stem = p.stem().stem();
		auto ss = stem.string();
		const hmdbCommunication::IHMDBShallowCopyContextPtr shallow = source->shallowContextForData(data);
		auto & shallowCopy = shallow->shallowCopyDataContext()->shallowCopy()->motionShallowCopy;
		auto& trials = shallowCopy.trials;
		for (auto it = trials.begin(); it != trials.end(); ++it) {
			auto* trial = it->second;
			if (ss.find(trial->trialName) != std::string::npos) {
				currentTrialID = trial->trialID;
				break;
			}
		}
	}

	const auto & files = remoteSrcContext->shallowCopyDataContext()->shallowCopy()->motionShallowCopy.files;

	for (auto f : files)
	{
		if (f.second->fileName == fileName) {
			fileID = f.second->fileID;
			break;
		}
	}

	try {
		/// TODO : pobrac dane z OW
		hmdbCommunication::IHMDBStorageOperations::IStreamPtr stream(new std::ifstream(p.string(), std::ios::binary));
		const auto fileSize = utils::Filesystem::size(p);
		auto remote = remoteSrcContext->shallowCopyRemoteContext()->remoteContext();
		if (!remote) {
			coreUI::CorePopup::showMessage(tr("Failed to upload serie"), tr("Remote context was not found"));
			return;
		}
		auto upload = remote->prepareFileUpload(fileName, "/BDR/w", stream, hmdbCommunication::IHMDBRemoteContext::Motion, fileSize);
		upload->start();
		upload->wait();

		if (upload->status() != threadingUtils::IOperation::Finished) {
			coreUI::CorePopup::showMessage(tr("Failed to upload file"), tr("Could not transfer data"));
		}
		else {
			coreUI::CorePopup::showMessage(tr("Upload complete"), tr("Data transfered successfully"));
		}

		if (fileID < 0) {
			auto resp = remoteSrcContext->shallowCopyRemoteContext()->remoteContext()->session()->authorization()->listUsers();
			fileID = remoteSrcContext->shallowCopyRemoteContext()->remoteContext()->session()->motionFilestoreman()->storeTrialFile(currentTrialID, "/BDR/w", "", fileName);
		}
		else {
			remoteSrcContext->shallowCopyRemoteContext()->remoteContext()->session()->motionFilestoreman()->replaceFile(currentTrialID, "/BDR/w", fileName);
		}


	}
	catch (std::exception & e) {
		PLUGIN_LOG_ERROR("Failed to upload file with error: " << e.what());
		coreUI::CorePopup::showMessage(tr("Failed to upload file"), tr("Errors while data transfer: ") + QString::fromStdString(e.what()));

	}
	catch (...) {
		PLUGIN_LOG_ERROR("Failed to upload file - UNKNOWN error");
		coreUI::CorePopup::showMessage(tr("Failed to upload file"), tr("UNKNOWN error while data transfer"));
	}

}


void equest::EquestAnnotationVisualizer::cellDoubleClicked(int row, int column)
{
	if (currentSerie) {
		EquestAnnotationsConstPtr ptr = currentSerie->getAnnotations();
		const auto& vec = ptr->annotations;
		if (!vec.empty() && row < vec.size()) {
			double start = vec[row].startTime;
			auto timeline = core::queryService<ITimelineService>(plugin::getServiceManager());
			if (timeline != nullptr) {
				timeline->setTime(start);
			}
		}
	}
}

void equest::EquestAnnotationVisualizer::cellChanged(int row, int column)
{
	if (currentSerie) {
		currentSerie->invalidateCell(row, column);
	}
}

void equest::EquestAnnotationVisualizer::cellValueChanged(double value)
{
	QDoubleSpinBox* spin = qobject_cast<QDoubleSpinBox*>(sender());
	if (spin && currentSerie)
	{
		int row = spin->property("row").toInt();
		int col =  spin->property("column").toInt();
		currentSerie->invalidateCell(row, col, value);
	}
}

void equest::EquestAnnotationVisualizer::updateTime()
{
	if (currentSerie) {
		double time = 0.0;
		// chcemy czas taki jak na timeline, bo wtedy nie trzeba sie martwic o zmiane dl. czasu dla serii adnotacji
		auto timeline = core::queryService<ITimelineService>(plugin::getServiceManager());
		if (timeline != nullptr) {
			time = timeline->getTime();
		}
		if (time != currentTime) {
			currentTime = time;
			currentSerie->setTime(currentTime);
		}
	}
}


