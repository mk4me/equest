#include "MdeBridge.h"
#include "corelib/IHierarchyItem.h"
#include "corelib/IDataManager.h"
#include "plugins/hmmlib/TreeItemHelper.h"
#include "corelib/HierarchyDataItem.h"
#include "corelib/Variant.h"
#include "corelib/IDataHierarchyManager.h"
#include "objectwrapperlib/ObjectWrapper.h"
#include "coreui/CoreVisualizerWidget.h"
#include "plugins/python/python/PythonPluginUtils.h"
#include "coreui/CoreDockWidget.h"
#include "corelib/ISourceManager.h"
#include "plugins/fileSource/IFileSource.h"
#include "boost/python/class.hpp"
#include "boost/python/suite/indexing/vector_indexing_suite.hpp"
#include "coreui/CoreDockWidgetManager.h"
#include "QtWidgets/QApplication"
#include "corelib/PluginCommon.h"

namespace py = boost::python;




python::PythonVectorChannel python::MdeBridge::getVectorChannel(const std::string& sessionDesc, const std::string& dataDesc)
{
	return getDataChannel<PythonVectorChannel>(sessionDesc, dataDesc);
}

python::PythonScalarChannel python::MdeBridge::getScalarChannel(const std::string& sessionDesc, const std::string& dataDesc)
{
	return getDataChannel<PythonScalarChannel>(sessionDesc, dataDesc);
}

void python::MdeBridge::addVectorChannel(const PythonVectorChannel& channel)
{
	if (channel.getFrequency() <= 0.0f) {
		throw std::runtime_error("Invalid channel frequency");
	} else if (channel.getData().empty()) {
		throw std::runtime_error("Channel was not initialized with data");
	}
	auto c = PythonVectorChannel::convert(channel);

	auto wrapper = core::Variant::wrap(utils::const_pointer_cast<c3dlib::VectorChannelReaderInterface>(c));
	core::HierarchyHelperPtr helper = core::HierarchyHelperPtr(new NewVector3ItemHelper(wrapper));
	std::string name;

	auto adf = c->feature<dataaccessor::DescriptorFeature>();

	if (adf != nullptr) {
		name = adf->name();
	}

	if (name.empty()) {
		name = wrapper->data()->getClassName();
	}
	core::IHierarchyItemPtr dataItem = core::HierarchyItemPtr(new core::HierarchyDataItem(wrapper, QIcon(), QString::fromStdString(name), QString(), helper));
	auto hierarchyTransaction = getHierarchyManager()->transaction();
	hierarchyTransaction->addRoot(dataItem);
}

//TODO - otemplateowac
void python::MdeBridge::addScalarChannel(const PythonScalarChannel& channel)
{
	if (channel.getFrequency() <= 0.0f) {
		throw std::runtime_error("Invalid channel frequency");
	} else if (channel.getData().empty()) {
		throw std::runtime_error("Channel was not initialized with data");
	}
	auto c = PythonScalarChannel::convert(channel);

	auto wrapper = core::Variant::wrap(utils::const_pointer_cast<c3dlib::ScalarChannelReaderInterface>(c));
	core::HierarchyHelperPtr helper = core::HierarchyHelperPtr(new NewChartItemHelper(wrapper));
	std::string name;

	auto adf = c->feature<dataaccessor::DescriptorFeature>();

	if (adf != nullptr) {
		name = adf->name();
	}

	if (name.empty()) {
		name = wrapper->data()->getClassName();
	}
	core::IHierarchyItemPtr dataItem = core::HierarchyItemPtr(new core::HierarchyDataItem(wrapper, QIcon(), QString::fromStdString(name), QString(), helper));
	auto hierarchyTransaction = getHierarchyManager()->transaction();
	hierarchyTransaction->addRoot(dataItem);
}
python::DataList python::MdeBridge::listLoadedVectors()
{
	python::DataList dataList;
	return listLoadedData<c3dlib::VectorChannelReaderInterface>();
}

python::DataList python::MdeBridge::listLoadedScalars()
{
	return listLoadedData<c3dlib::ScalarChannelReaderInterface>();
}

python::PythonVectorChannel python::MdeBridge::createVectorChannel()
{
	return PythonVectorChannel();
}

python::PythonScalarChannel python::MdeBridge::createScalarChannel()
{
	return PythonScalarChannel();
}

void python::MdeBridge::addFile(const std::string& file)
{
	auto source = core::querySource<fileSource::IFileSource>(getSourceManager());
	if (source != nullptr) {
		source->addFile(file);
	}
	else {
		throw std::runtime_error("Unable to find FileSource plugin");
	}
}

void python::MdeBridge::close(int errorCode /*= 0*/)
{
	QApplication::exit(errorCode);
	exit(errorCode);
}

void python::MdeBridge::log(const std::string& txt)
{
	pluginLog(txt);
}

static void gather(const core::IHierarchyItemConstPtr& item, std::vector<python::Helper>& ret, core::IVisualizerManager* manager)
{
	core::IHierarchyDataItemConstPtr dataItem = utils::dynamic_pointer_cast<const core::IHierarchyDataItem>(item);
	if (dataItem) {
		std::list<core::HierarchyHelperPtr> helpers = dataItem->getHelpers();
		for (auto& h : helpers) {
			ret.push_back(python::Helper(h, manager));
		}
	}
	for (int count = item->getNumChildren() - 1; count >= 0; --count) {

		gather(item->getChild(count), ret, manager);
	}
};

std::vector<python::Helper> python::MdeBridge::getHelpers()
{
	std::vector<python::Helper> ret;
	core::IDataHierarchyManager* hierarchyManager = getHierarchyManager();
	if (hierarchyManager != nullptr) {
		auto t = hierarchyManager->transaction();
		std::for_each(t->hierarchyBegin(), t->hierarchyEnd(),
			[&](const core::IHierarchyItemConstPtr& item) { gather(item, ret, getVisualizerManager()); });
	}
	else {
		throw std::runtime_error("Unable to find source");
	}

	return ret;
}


std::string python::Helper::name()
{
	auto h = utils::const_pointer_cast<core::HierarchyHelper>(helper.lock());
	auto item = h->getParent().lock();
	std::string res = item->getName().toStdString();
	while (item = item->getParent()) {
		res = item->getName().toStdString() + "/" + res;
	}

	auto desc = h->getText();
	if (desc != "Create Visualizer") {
		res += "/" + h->getText().toStdString();
	}
	return res;
}

void python::Helper::createVisualizer()
{
	auto h = utils::const_pointer_cast<core::HierarchyHelper>(helper.lock());
	if (h) {
		auto visualizer = h->createVisualizer(manager);
		visualizer->getOrCreateWidget();

		auto visWidget = new coreUI::CoreVisualizerWidget(visualizer);
		//visWidget->show();

		auto dockVisWidget = coreUI::CoreDockWidget::embeddWidget(visWidget, "test", Qt::AllDockWidgetAreas, false);
		//todo
		//registerVisualizerContext(getContextEventFilter(), qobject_cast<coreUI::CoreTitleBar*>(dockVisWidget->titleBarWidget()), qobject_cast<coreUI::CoreVisualizerWidget*>(dockVisWidget->widget()), visualizer);

		std::vector<core::Visualizer::Serie*> series;
		h->getSeries(visualizer, "test/t", series);
		dockVisWidget->setMinimumSize((std::max)(280, dockVisWidget->minimumWidth()), (std::max)(280, dockVisWidget->minimumHeight()));

		coreUI::CoreDockWidgetManager* manager = nullptr; 
		QWidgetList wlist = QApplication::topLevelWidgets();
		for (auto& w : wlist) {
			manager = w->findChild<coreUI::CoreDockWidgetManager*>();
			if (manager) {
				break;
			}
		}

		if (manager) {
			manager->autoAddDockWidget(dockVisWidget, QString("Test"));
		} else {
			dockVisWidget->show();
		}
	}
	else {
		throw std::runtime_error("Helper no longer exists");
	}
}
