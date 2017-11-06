#include "DicomPCH.h"
#include "LayeredImageVisualizer.h"
#include <plugins/dicom/ILayeredImage.h>
#include <QtGui/QPixmap>
#include <QtWidgets/QLabel>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QSlider>
#include <QtWidgets/QPushButton>
#include "LayeredImageVisualizerView.h"
#include <QtWidgets/QApplication>
#include "BackgroundLayer.h"
#include <boost/serialization/shared_ptr.hpp>
#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>
#include "LayeredImage.h"
#include "PointsLayer.h"
#include <corelib/PluginCommon.h>
#include <plugins/hmdbCommunication/IHMDBSource.h>
#include <corelib/ISourceManager.h>
#include <coreui/CorePopup.h>
#include <QtWidgets/QMessageBox>
#include "IDicomService.h"
#include <corelib/IServiceManager.h>
#include <plugins/hmdbCommunication/IHMDBSession.h>
#include <plugins/hmdbCommunication/IHMDBRemoteContext.h>
#include <plugins/hmdbCommunication/IHMDBShallowCopyContext.h>
#include <plugins/dicom/Annotations.h>
#include <hmdbserviceslib/IBasicUpdatesWS.h>
#include <hmdbserviceslib/IAuthorizationWS.h>
#include <hmdbserviceslib/IFileStoremanWS.h>
#include <fstream>
#include "DicomService.h"

using namespace dicom;

LayeredImageVisualizer::LayeredImageVisualizer()
: currentTrialID(-1), mainWidget(nullptr), isReviewer(false)
{
	
	currentSerie = -1;
}

LayeredImageVisualizer::~LayeredImageVisualizer()
{

}

hmdbCommunication::IHMDBShallowCopyContext * LayeredImageVisualizer::remoteShallowContext(const plugin::IVisualizer::ISerie * serie)
{
	if (serie != nullptr){
		const LayeredSerie* lserie = dynamic_cast<const LayeredSerie*>(serie);
		if (lserie != nullptr){
			return lserie->sourceContext();
		}
	}

	return nullptr;
}

const bool LayeredImageVisualizer::userIsReviewer() const
{
	return isReviewer;
}

plugin::IVisualizer* LayeredImageVisualizer::create() const
{
	return new LayeredImageVisualizer();
}

void LayeredImageVisualizer::setStatus(const hmdbServices::xmlWsdl::AnnotationStatus::Type type,
	const QString & comment)
{	
	auto remoteSrcContext = remoteShallowContext(getActiveSerie());

	if (!remoteSrcContext || !remoteSrcContext->shallowCopyRemoteContext()) {
		throw std::runtime_error("No session active");
	}
	try{

		auto resp = remoteSrcContext->shallowCopyRemoteContext()->remoteContext()->session()->authorization()->listUsers();
		auto usersList = hmdbServices::xmlWsdl::parseUsersList(resp);

		auto it = std::find_if(usersList.begin(), usersList.end(), [=](const hmdbServices::xmlWsdl::UserDetails & ud)
		{
			if (ud.login == currentLayerUser_){
				return true;
			}
			else{
				return false;
			}
		});

		if (it != usersList.end()){
			auto userid = it->id;
			auto updateService = remoteSrcContext->shallowCopyRemoteContext()->remoteContext()->session()->motionUpdate();

			if (userIsReviewer() == true){
				//lekarz
				if (type == hmdbServices::xmlWsdl::AnnotationStatus::Approved ||
					type == hmdbServices::xmlWsdl::AnnotationStatus::Rejected){

					updateService->setAnnotationReview(currentTrialID, userid, hmdbServices::xmlWsdl::AnnotationStatus::UnderReview, comment.toStdString());
				}

				updateService->setAnnotationReview(currentTrialID, userid, type, comment.toStdString());

			}
			else{
				//student
				UTILS_ASSERT(type == hmdbServices::xmlWsdl::AnnotationStatus::ReadyForReview, "User moze tylko przekazac dane do weryfikacji");
				//UTILS_ASSERT(userid == currentUser()->id(), "User musi sie zgadzac z aktualnie zalogowanym");
				updateService->setMyAnnotationStatus(currentTrialID, type, comment.toStdString());
			}

		}
	}
	catch (...)
	{

	}
	
	/*
	AnnotationStatus as;
	as.status = status;

	if (comm->userIsReviewer() == true){
		as.note = comment;
	}
	else{
		as.comment = comment;
	}

	annotations[id][trialID] = as;
	*/
}

const std::string LayeredImageVisualizer::getCurrentLayerUserName() const
{
	return currentLayerUser_;
}

const int LayeredImageVisualizer::currnetTrialID() const
{
	return currentTrialID;
}

const bool LayeredImageVisualizer::trySave()
{
	if (mainWidget->isWindowModified() == false){
		return true;
	}

	if (QMessageBox::question(mainWidget, tr("Data changed"), tr("Some data was changed. Would You like to save changes or discard them?"),
		QMessageBox::Save, QMessageBox::Discard) == QMessageBox::Save){
		try{
			saveSerie();
		}
		catch (...){
			return false;
		}
	}
	else{
		return true;
	}

	return true;
}

bool LayeredImageVisualizer::eventFilter(QObject * watched, QEvent * event)
{
	if (event->type() == QEvent::Close){
		//auto widget = qobject_cast<QWidget *>(watched);

		if (trySave() == true){
			event->accept();
		}
		else{
			event->ignore();
		}

		return true;
	}

	return QObject::eventFilter(watched, event);
}

plugin::IVisualizer::ISerie * LayeredImageVisualizer::createSerie(const utils::TypeInfo & requestedType, const core::VariantConstPtr& data)
{
	auto serie = new LayeredSerie(this, mainWidget);
	serie->setData(requestedType, data);

	//todo
	//pobranie z hmdbsource sourcecontext dla danych i zapamietanie go

	series.push_back(serie);

	if (series.size() == 1) {
		setActiveSerie(series[0]);
	}
	connect(serie->getLayersModel(), SIGNAL(changeSelection(const QModelIndex&)), this, SIGNAL(changeSelection(const QModelIndex&)));
	Q_EMIT serieChanged();

	return serie;
}

plugin::IVisualizer::ISerie * LayeredImageVisualizer::createSerie(const plugin::IVisualizer::ISerie * serie)
{
	return nullptr;
}

plugin::IVisualizer::ISerie* dicom::LayeredImageVisualizer::createSerie(const ISerie* serie, const utils::TypeInfo & requestedType, const core::VariantConstPtr & data)
{
	return nullptr;
}

void LayeredImageVisualizer::removeSerie(plugin::IVisualizer::ISerie *serie)
{
	auto active = getActiveSerie();
	auto it = std::find(series.begin(), series.end(), serie);
	if (it != series.end()) {
		series.erase(it);
	}

	if (active == serie) {
		currentSerie = series.empty() ? 0 : 1;
	}

	Q_EMIT serieChanged();
}

void LayeredImageVisualizer::setActiveSerie(plugin::IVisualizer::ISerie * serie)
{
	auto prevSerie = getActiveSerie();
	int count = static_cast<int>(series.size());
	for (int i = 0; i < count; ++i) {
		if (series[i] == serie) {
			currentSerie = i;
			if (prevSerie != series[i]) {
				mainWidget->setWindowTitle(QString::fromStdString(series[i]->getName()) + "[*]");
				gatherCommunicationInfo(series[i]);
				Q_EMIT serieChanged();
			}
			return;
		}
	}
}

const plugin::IVisualizer::ISerie * LayeredImageVisualizer::getActiveSerie() const
{
	return correctIndex(currentSerie) ? series[currentSerie] : nullptr;
}

plugin::IVisualizer::ISerie * dicom::LayeredImageVisualizer::getActiveSerie()
{
	return correctIndex(currentSerie) ? series[currentSerie] : nullptr;
}

QWidget* LayeredImageVisualizer::createWidget()
{
	if (mainWidget == nullptr){
		mainWidget = new LayeredImageVisualizerView(this);
	}
	return mainWidget;
}

QIcon* LayeredImageVisualizer::createIcon()
{
	return new QIcon();
}

int LayeredImageVisualizer::getMaxDataSeries() const
{
	return -1;
}

void LayeredImageVisualizer::getSupportedTypes(utils::TypeInfoList & supportedTypes) const
{
	supportedTypes.push_back(typeid(ILayeredImage));
}

void LayeredImageVisualizer::update(double deltaTime)
{
	if (correctIndex(currentSerie)) {
		auto serie = series[currentSerie];
		// nie jest to najszczesliwszy mechanizm, ale inicjalizacja jest mozliwa dopiero, gdy zna sie geometrie widgetow
		if (!serie->wasInitialized()) {
			serie->init();
		}
	}
}

QPixmap LayeredImageVisualizer::takeScreenshot() const
{
	if (correctIndex(currentSerie)) {
		return series[currentSerie]->getPixmap();
	}
	return QPixmap();
}

void dicom::LayeredImageVisualizer::setPrevSerie()
{
	if (correctIndex(currentSerie - 1)) {
		setActiveSerie(series[currentSerie - 1]);
	}
}

void dicom::LayeredImageVisualizer::setNextSerie()
{
	if (correctIndex(currentSerie + 1)) {
		setActiveSerie(series[currentSerie + 1]);
	}
}
//
void dicom::LayeredImageVisualizer::trySetSerie(int val)
{
	if (correctIndex(val)) {
		setActiveSerie(series[val]);
	}
}

bool dicom::LayeredImageVisualizer::correctIndex(int i) const
{
	return i >= 0 && i < series.size();
}

int dicom::LayeredImageVisualizer::getNumSeries() const
{
	return series.size();
}

int dicom::LayeredImageVisualizer::getCurrentSerieNo() const
{
	return currentSerie;
}

void dicom::LayeredImageVisualizer::saveSerie()
{
	if (correctIndex(currentSerie)) {
		series[currentSerie]->save();

		auto service = core::queryService<IDicomService>(plugin::getServiceManager());
		QIcon icon(":/dicom/file_done.png");
		service->updateItemIcon(series[currentSerie]->getImageFilename(), icon);

		mainWidget->setWindowModified(false);
	}
}

void dicom::LayeredImageVisualizer::editSerie(int tagIdx, int idx)
{
	if (correctIndex(currentSerie)) {
		series[currentSerie]->editLayer(tagIdx, idx);
	}
}

void dicom::LayeredImageVisualizer::removeLayer(int tagIdx, int idx)
{
	if (correctIndex(currentSerie)) {
		series[currentSerie]->removeLayer(tagIdx, idx);
	}
}

void dicom::LayeredImageVisualizer::selectLayer(int tagIdx, int idx)
{
	LayeredSerie* serie = dynamic_cast<LayeredSerie*>(getActiveSerie());
	if (serie) {
		auto selected = selectedLayer();
		ILayeredImagePtr img = serie->getImage();
		std::string tag = img->getTag(tagIdx);
		currentLayerUser_ = tag;
		currentTrialID = img->getTrialID();
		if (/*selected.second != -1 && */(tag != selected.first || idx != selected.second)) {
			if (idx > -1){
				auto li = img->getLayerItem(tag, idx);

				auto pl = boost::dynamic_pointer_cast<PointsLayer>(li);

				if (pl == nullptr){
					mainWidget->setDeletionButtonEnabled(false);
				}
				else{
					mainWidget->setDeletionButtonEnabled(true);
				}
			}

			serie->getGraphicsScene()->blockSignals(true);
			for(auto t : img->getTags()) {
				int count = img->getNumLayerItems(t);
				for (int i = 0; i < count; ++i) {
					auto li = img->getLayerItem(t, i);

					auto gli = boost::dynamic_pointer_cast<ILayerGraphicItem>(li);

					if (gli != nullptr){
						gli->setSelected(tag == t && i == idx);
					}
				}
			}
			serie->getGraphicsScene()->blockSignals(false);
			serie->getLayersModel()->refreshSelected();
		}
	}
}

std::pair<std::string, int> dicom::LayeredImageVisualizer::selectedLayer() const
{
	int idx = -1;
	std::string selectedTag;
	const LayeredSerie* serie = dynamic_cast<const LayeredSerie*>(getActiveSerie());
	if (serie) {
		ILayeredImageConstPtr img = serie->getImage();
		for(auto tag : img->getTags()) {
			int count = img->getNumGraphicLayerItems(tag);
			for (int i = 0; i < count; ++i) {
				if (img->getLayerGraphicItem(tag, i)->getSelected()) {
					if (idx == -1) {
						idx = i;
						selectedTag = tag;
					}
					else {
						// zaznaczono wiecej niz jedna serie danych !?
						UTILS_ASSERT(false);
						return std::make_pair(std::string(), -1);
					}
				}
			}
		}
	}

	return std::make_pair(selectedTag, idx);
}

void dicom::LayeredImageVisualizer::uploadSerie()
{
	
	saveSerie();
	if (correctIndex(currentSerie)) {

		if (currentTrialID < 0){
			//TODO
			//message coœ nie tak
			return;
		}
		
		utils::Filesystem::Path p(series[currentSerie]->getXmlOutputFilename());
		const auto fileName = p.filename().string();

		hmdbServices::ID fileID = -1;

		auto remoteSrcContext = remoteShallowContext(getActiveSerie());
		if (!remoteSrcContext || !remoteSrcContext->shallowCopyRemoteContext()) {
			return;
		}

		const auto & files = remoteSrcContext->shallowCopyDataContext()->shallowCopy()->motionShallowCopy.files;

		for (auto f : files)
		{
			if (f.second->fileName == fileName){
				fileID = f.second->fileID;
				break;
			}
		}		

		try{
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

			if (upload->status() != threadingUtils::IOperation::Finished){
				coreUI::CorePopup::showMessage(tr("Failed to upload file"), tr("Could not transfer data"));
			}
			else{
				coreUI::CorePopup::showMessage(tr("Upload complete"), tr("Data transfered successfully"));
			}

			if (fileID < 0){
				fileID = remoteSrcContext->shallowCopyRemoteContext()->remoteContext()->session()->motionFilestoreman()->storeTrialFile(currentTrialID, "/BDR/w", "", fileName);
			}
			else{
				remoteSrcContext->shallowCopyRemoteContext()->remoteContext()->session()->motionFilestoreman()->replaceFile(currentTrialID, "/BDR/w", fileName);
			}


		}
		catch (std::exception & e){
			PLUGIN_LOG_ERROR("Failed to upload file with error: " << e.what());
			coreUI::CorePopup::showMessage(tr("Failed to upload file"), tr("Errors while data transfer: ") + QString::fromStdString(e.what()));

		}
		catch (...){
			PLUGIN_LOG_ERROR("Failed to upload file - UNKNOWN error");
			coreUI::CorePopup::showMessage(tr("Failed to upload file"), tr("UNKNOWN error while data transfer"));
		}

		//TODO
		//zapis statusu adnotacji
	}
}

std::string dicom::LayeredImageVisualizer::getUserName() const
{
	return userName;	
}

void dicom::LayeredImageVisualizer::removeSelectedLayers()
{
	LayeredSerie* serie = dynamic_cast<LayeredSerie*>(getActiveSerie());
	if (serie) {
		auto image = serie->getImage();
		int tagsNo = image->getNumTags();
		for (int iTag = 0; iTag < tagsNo; ++iTag) {
			auto tag = image->getTag(iTag);
			for (int itm = image->getNumLayerItems(tag) - 1; itm >= 0; --itm) {
				auto li = image->getLayerItem(tag, itm);
				auto pl = boost::dynamic_pointer_cast<PointsLayer>(li);

				if (pl != nullptr && pl->getSelected() == true) {
					serie->removeLayer(iTag, itm);
				}
			}
		}
	}
}

const bool dicom::LayeredImageVisualizer::verifyCompletness() const
{
	bool ret = true;

	const LayeredSerie* serie = dynamic_cast<const LayeredSerie*>(getActiveSerie());

	auto image = serie->getImage();

	int minimumLayers = 2;

	if (image->isPowerDoppler() == true){
		minimumLayers = 3;
	}

	if (image->getNumLayerItems(currentLayerUser_) < minimumLayers)
	{
		ret = false;
	}

	return ret;
}

const bool dicom::LayeredImageVisualizer::verifyImflamatoryLevel() const
{
	const LayeredSerie* serie = dynamic_cast<const LayeredSerie*>(getActiveSerie());

	auto image = serie->getImage();

	auto s = image->getNumLayerItems(currentLayerUser_);

	for (int i = 0; i < s; ++i){
		auto l = image->getLayerItem(currentLayerUser_, i);
		if(l->getAdnotationIdx() == dicom::annotations::inflammatoryLevel){
			auto il = boost::dynamic_pointer_cast<const InflammatoryLevelLayer>(l);
			return il->value() != dicom::annotations::unknownInflammatoryLevel;
		}
	}

	return false;
}

const bool dicom::LayeredImageVisualizer::verifyBloodLevel() const
{
	const LayeredSerie* serie = dynamic_cast<const LayeredSerie*>(getActiveSerie());

	auto image = serie->getImage();

	if (image->isPowerDoppler() == false){
		return true;
	}

	auto s = image->getNumLayerItems(currentLayerUser_);

	for (int i = 0; i < s; ++i){
		auto l = image->getLayerItem(currentLayerUser_, i);
		if(l->getAdnotationIdx() == dicom::annotations::bloodLevel){
			auto bl = boost::dynamic_pointer_cast<const BloodLevelLayer>(l);
			return bl->value() != dicom::annotations::unknownBloodLevel;
		}
	}

	return false;
}

const bool dicom::LayeredImageVisualizer::verifyJointType() const
{
	const LayeredSerie* serie = dynamic_cast<const LayeredSerie*>(getActiveSerie());

	auto image = serie->getImage();

	auto s = image->getNumLayerItems(currentLayerUser_);

	for (int i = 0; i < s; ++i){
		auto l = image->getLayerItem(currentLayerUser_, i);
		if(l->getAdnotationIdx() == dicom::annotations::jointType){
			auto jt = boost::dynamic_pointer_cast<const JointTypeLayer>(l);
			return jt->value() != dicom::annotations::unknownJoint;
		}
	}

	return false;
}

const bool dicom::LayeredImageVisualizer::verifyFingerType() const
{
	const LayeredSerie* serie = dynamic_cast<const LayeredSerie*>(getActiveSerie());

	auto image = serie->getImage();

	auto s = image->getNumLayerItems(currentLayerUser_);

	for (int i = 0; i < s; ++i){
		auto l = image->getLayerItem(currentLayerUser_, i);
		if(l->getAdnotationIdx() == dicom::annotations::fingerType){
			auto ft = boost::dynamic_pointer_cast<const FingerTypeLayer>(l);
			return ft->value() != dicom::annotations::unknownFinger;
		}
	}

	return false;
}

void dicom::LayeredImageVisualizer::gatherCommunicationInfo(const plugin::IVisualizer::ISerie* serie)
{
	UTILS_ASSERT(serie);
	auto remoteSrcContext = remoteShallowContext(serie);
	if (remoteSrcContext != nullptr){
		userName = remoteSrcContext->shallowCopyLocalContext()->localContext()->dataContext()->userName();
		auto remote = remoteSrcContext->shallowCopyRemoteContext();
		isReviewer = remote ? DicomService::userIsReviewer(remote->remoteContext()->session().get()) : false;
	} else {
		UTILS_ASSERT(false, "No shallow context");
	}
}
