#include "MdePCH.h"
#include "ImageTableModel.h"
#include "corelib/PluginCommon.h"
#include <plugins/dicom/ILayeredImage.h>
#include <plugins/dicom/Annotations.h>
#include "plugins/dicom/ILayer.h"
#include "utils/SmartPtr.h"
#include "corelib/ISourceManager.h"
#include "plugins/hmdbCommunication/IHMDBSource.h"
#include <hmdbserviceslib/BasicQueriesWS.h>
#include "plugins/hmdbCommunication/IHMDBShallowCopyContext.h"
#include "plugins/hmdbCommunication/IHMDBSession.h"
#include <hmdbserviceslib/IAuthorizationWS.h>
#include <hmdbserviceslib/Entity.h>
#include <queue>
#include <stack>


ImageTableModel::ImageTableModel(QObject *parent /*= 0*/) : QAbstractTableModel(parent)
{
	qRegisterMetaType<ImageTableRow>("ImageTableRow");
}


QVariant ImageTableModel::headerData(int section, Qt::Orientation orientation, int role /*= Qt::DisplayRole*/) const
{
	if (role == Qt::DisplayRole) {
		if (orientation == Qt::Horizontal) {
			switch (section) {
				case 0: return tr("File name");
				case 1: return tr("User name");
				case 2: return tr("No of inflammations");
				case 3: return tr("Areas of inflammations");
				case 4: return tr("Annotation status");
			}
		} else {
			return section + 1;
		}
	}
	return QVariant();
}


int ImageTableModel::rowCount(const QModelIndex &parent /*= QModelIndex()*/) const
{
	return rows.size();
}

int ImageTableModel::columnCount(const QModelIndex &parent /*= QModelIndex()*/) const
{
	return 5;
}


core::ConstVariantsList ImageTableModel::getLayersObjects()
{
	core::IDataManagerReader* dm = plugin::getDataManagerReader();
	core::ConstVariantsList objects;
	dm->transaction()->getObjects(objects, typeid(dicom::LayersVector), false);
	return objects;
}


QVariant ImageTableModel::data(const QModelIndex &index, int role /*= Qt::DisplayRole*/) const
{
	if (role == Qt::DisplayRole) {
		std::vector<ImageTableRow>::size_type idx = index.row();
		switch (index.column()) {
			case 0: return rows[idx].imgName;
			case 1: return rows[idx].userName;
			case 2: return rows[idx].numberOfSynovitis;
			case 3: return rows[idx].regions;
			case 4: return rows[idx].status;
		}
	}
	return QString("ARG.%1.%2").arg(index.row()).arg(index.column());
}


QRect getBoundingRect(const std::vector<QPointF>& pixelsBoundary)
{
	QRect rect;
	if (!pixelsBoundary.empty()) {
		auto p = pixelsBoundary[0];
		rect.setCoords(p.x(), p.y(), p.x(), p.y());
		for (auto& p : pixelsBoundary) {
			if (p.x() < rect.left()) rect.setLeft(p.x());
			if (p.x() > rect.right()) rect.setRight(p.x());
			if (p.y() < rect.top()) rect.setTop(p.y());
			if (p.y() > rect.bottom()) rect.setBottom(p.y());
		}
	}
	return rect;
}

const uint BOUNDARY_PIX = 0xFFFF0000;
const uint VISITED_PIX  = 0xFF00FF00;
const uint EMPTY_PIX    = 0xFFFFFFFF;

QPointF getSecondPixelCandidate(const QImage& image)
{
	auto width = image.size().width();
	auto height = image.size().height();
	for (auto x = 0; x < width; ++x) {
		for (auto y = 0; y < height; ++y) {
			auto pix = image.pixel(x, y);
			if (pix != BOUNDARY_PIX && pix != VISITED_PIX) {
				return QPointF(x, y);
			}
		}		
	}

	throw std::runtime_error("There are no pixels which meet requirements");
}

QPointF getFirstPixelCandidate(const std::vector<QPointF>& pixelsBoundary, const QImage& image)
{
	auto size = image.size();
	for (auto& p : pixelsBoundary) {
		auto sx = p.x() > size.width() / 2 ? -1 : 1;
		auto sy = p.y() > size.height() / 2 ? -1 : 1;
		QPoint np(p.x() + sx, p.y() + sy);
		auto pix = image.pixel(np);
		if (pix == EMPTY_PIX) {
			return np;
		}
	}

	return getSecondPixelCandidate(image);
}

int floodFill(QImage& image, const QPoint& start)
{	
	int counter = 0;
	std::stack<QPoint> toCheck;
	toCheck.push(start);
	while (toCheck.empty() == false) {
		auto pos = toCheck.top();
		toCheck.pop();
		if (!image.rect().contains(pos)) {
			return -1;
		}

		auto p = image.pixel(pos);
		if (p != EMPTY_PIX) {
			continue;;
		}
		++counter;
		image.setPixel(pos, VISITED_PIX);
		toCheck.push(QPoint(pos.x() - 1, pos.y()));
		toCheck.push(QPoint(pos.x() + 1, pos.y()));
		toCheck.push(QPoint(pos.x(), pos.y() - 1));
		toCheck.push(QPoint(pos.x(), pos.y() + 1));
	}
	return counter;
}

int getPixelCount(std::vector<QPointF> pixelsBoundary)
{
	QRect boundingRect = getBoundingRect(pixelsBoundary);
	QImage img(boundingRect.size(), QImage::Format::Format_ARGB32);
	img.fill(EMPTY_PIX);
	auto sx = boundingRect.left();
	auto sy = boundingRect.top();
	for (auto& p : pixelsBoundary) {
		p.setX(p.x() - sx);
		p.setY(p.y() - sy);
	}

	for (auto& p : pixelsBoundary) {
		img.setPixel(p.x(), p.y(), BOUNDARY_PIX);
	}
	//static int nr = 0;
	//nr++;
	//img.save(QString("E:/testA%1.png").arg(nr));
	
	int count = -1;
	try {
		while (count == -1) {
			auto first = getFirstPixelCandidate(pixelsBoundary, img);
			count = floodFill(img, QPoint(first.x(), first.y()));
		}
	}
	catch (...) {
		//img.save(QString("E:/testC%1.png").arg(nr));
	}
	//img.save(QString("E:/testB%1.png").arg(nr));
	return count + pixelsBoundary.size();
}

std::pair<int, QString> getSynovitisInfo(const dicom::LayersVector* vec)
{
	int count = 0;
	QString regions;
	for (auto& layer : *vec) {
		if (layer->getAdnotationIdx() == dicom::annotations::inflammatory) {
			count++;
			auto gitem = boost::dynamic_pointer_cast<dicom::ILayerGraphicItem>(layer);
			auto size = getPixelCount(gitem->getPixelPointsCloud());
			regions += count == 1 ? QString("%1").arg(size) : QString(", %1").arg(size);
		}
	}
	return std::make_pair(count, regions);
}

hmdbCommunication::IHMDBShallowCopyContext* getShallowCopyContext()
{
	auto source = core::querySource<hmdbCommunication::IHMDBSource>(plugin::getSourceManager());
	if (source != nullptr) {
		auto srcContext = source->shallowContext(0);
		return srcContext.get();
	}
	return nullptr;
}

hmdbCommunication::IHMDBSession * getSession(hmdbCommunication::IHMDBShallowCopyContext* srcContext)
{
	if (srcContext) {
		auto remoteContext = srcContext->shallowCopyRemoteContext();
		if (remoteContext) {
			auto rc = remoteContext->remoteContext();
			if (rc) {
				auto session = rc->session() ? rc->session().get() : nullptr;
				return session;
			}
		}
	}
	return nullptr;
}


std::map<QString, int> getTrials(hmdbCommunication::IHMDBShallowCopyContext* shallow)
{
	std::map<QString, int> res;
	auto & shallowCopy = shallow->shallowCopyDataContext()->shallowCopy()->motionShallowCopy;
	auto& trials = shallowCopy.trials;
	for (auto it = trials.begin(); it != trials.end(); ++it) {
		auto* trial = it->second;
		res[QString::fromStdString(trial->trialName)] = trial->trialID;
	}
	return res;
}

std::map<QString, int> getUsers(hmdbCommunication::IHMDBSession * session)
{
	std::map<QString, int> res;
	
	auto resp = session->authorization()->listUsers();
	auto usersList = hmdbServices::xmlWsdl::parseUsersList(resp);
	for (auto it = usersList.begin(); it != usersList.end(); ++it) {
		res[QString::fromStdString(it->login)] = it->id;
	}
	return res;
}


std::pair<QString, QString> getImgAndUserName(const core::Variant* v)
{
	auto unknown = QObject::tr("Unknown");
	std::string sourceFile;
	if (v->getMetadata("core/source", sourceFile)) {
		utils::Filesystem::Path stem = utils::Filesystem::Path(sourceFile).stem();
		auto img = QString::fromStdString(stem.stem().string());
		auto nameStr = stem.extension().string();
		nameStr.erase(0, 1);
		auto name = QString::fromStdString(nameStr);
		return std::make_pair(img.isEmpty() ? unknown : img, name.isEmpty() ? unknown : name);
	}
	return std::make_pair(unknown, unknown);
}


typedef std::map<int, std::map<int, hmdbServices::xmlWsdl::AnnotationStatus::Type>> Annotations;
Annotations getAnnotations(hmdbCommunication::IHMDBSession * session)
{
	Annotations res;
	auto resp = session->motionQueries()->listAnnotationsXML();
	auto annotations = hmdbServices::xmlWsdl::parseAnnotations(resp);
	for (auto& a : annotations) {
		res[a.trialID][a.userID] = a.status;
	}

	return res;
}



QString getStatusText(hmdbServices::xmlWsdl::AnnotationStatus::Type status)
{
	switch (status) {
		case hmdbServices::xmlWsdl::AnnotationStatus::Unspecified:		 return QObject::tr("Unspecified");
		case hmdbServices::xmlWsdl::AnnotationStatus::UnderConstruction: return QObject::tr("UnderConstruction");
		case hmdbServices::xmlWsdl::AnnotationStatus::ReadyForReview:	 return QObject::tr("ReadyForReview");
		case hmdbServices::xmlWsdl::AnnotationStatus::UnderReview:		 return QObject::tr("UnderReview");
		case hmdbServices::xmlWsdl::AnnotationStatus::Approved:			 return QObject::tr("Approved");
		case hmdbServices::xmlWsdl::AnnotationStatus::Rejected:			 return QObject::tr("Rejected");
		default: return QObject::tr("Unknown");
	}
	
}

struct AnnotationData
{
	std::map<QString, int> user2Id;
	std::map<QString, int> trial2Id;
	Annotations	annotations;
};

std::unique_ptr<AnnotationData> getComunnicationData()
{
	
	auto context = getShallowCopyContext();
	auto session = getSession(context);
	if (session) {
		auto data = std::unique_ptr<AnnotationData>(new AnnotationData);
		data->user2Id = getUsers(session);
		data->trial2Id = getTrials(context);
		data->annotations = getAnnotations(session);
		return data;
	}

	return std::unique_ptr<AnnotationData>();
}

void setAnnotationStatus(ImageTableRow &row, const AnnotationData& data)
{
	auto user = data.user2Id.at(row.userName);
	auto trial = data.trial2Id.at(row.imgName);
	auto tIt = data.annotations.find(trial);
	if (tIt != data.annotations.end()) {
		auto uIt = tIt->second.find(user);
		if (uIt != tIt->second.end()) {
			row.status = getStatusText(uIt->second);
		}
	}
}

void setAnnotationStatuses(std::vector<ImageTableRow>& rows)
{
	auto data = getComunnicationData();
	if (data) {
		for (auto& row : rows) {
			setAnnotationStatus(row, *data);
		}	
	}
}

void ImageTableModel::rebuildTable()
{
	auto objects = getLayersObjects();
	
	rows.clear();
	for (auto& object : objects) {
		dicom::LayersVectorConstPtr lv = object->get();
		auto synovitis = getSynovitisInfo(lv.get());
		auto imguser = getImgAndUserName(object.get());
		rows.push_back(ImageTableRow{ imguser.first, imguser.second, synovitis.first, synovitis.second, QString() });
	}

	setAnnotationStatuses(rows);
}

std::unique_ptr<ImageTableWorker> ImageTableModel::getWorker()
{
	auto objects = getLayersObjects();
	return std::unique_ptr<ImageTableWorker>(new ImageTableWorker(std::move(objects)));
}

void ImageTableModel::clearTable()
{
	beginRemoveRows(QModelIndex(), 0, rows.size());
	rows.clear(); 
	endRemoveRows();
}

void ImageTableModel::addRow(const ImageTableRow& r)
{
	beginInsertRows(QModelIndex(), rows.size(), rows.size());
	rows.push_back(r);
	endInsertRows();
}


ImageTableWorker::ImageTableWorker(core::ConstVariantsList&& o) :
	objects(o)
{

}

void ImageTableWorker::process()
{
	auto data = getComunnicationData();
	for (auto& object : objects) {
		dicom::LayersVectorConstPtr lv = object->get();
		auto synovitis = getSynovitisInfo(lv.get());
		auto imguser = getImgAndUserName(object.get());
		//rows.push_back(row{ imguser.first, imguser.second, synovitis.first, synovitis.second, QString() });
		auto row = ImageTableRow{ imguser.first, imguser.second, synovitis.first, synovitis.second, QString() };
		if (data) {
			setAnnotationStatus(row, *data);
		}
		rowProduced(row);
	}

	//setAnnotationStatuses(rows);
}
