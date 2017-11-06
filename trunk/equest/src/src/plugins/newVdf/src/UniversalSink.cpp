#include "NewVdfPCH.h"
#include <plugins/newVdf/UniversalSink.h>
#include <corelib/HierarchyHelper.h>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLineEdit>
#include <corelib/HierarchyItem.h>
#include <plugins/hmmlib/TreeItemHelper.h>
#include <c3dlib/C3DTypes.h>
#include <QtWidgets/QLabel>

using namespace vdf;

void UniversalSink::_UniversalSink()
{
	static int count = 0;
	runNo = 0;
	inPinA = new UniversalInputPinBase(this);
	addInputPin(inPinA);
	widget = new QWidget();
	QLayout* layout = new QHBoxLayout();
	widget->setLayout(layout);

	QLabel* label = new QLabel("Set name:");
	line = new QLineEdit();
	defaultName.sprintf("Sink: %04d", count++);
	line->setText(defaultName);
	layout->addWidget(label);
	layout->addWidget(line);
}

void UniversalSink::consume()
{
	core::VariantConstPtr wrapper = inPinA->getWrapper();

	core::IHierarchyItemPtr dataItem;

	if (wrapper->data()->isSupported(typeid(core::ConstVariantsList))) {
		dataItem = core::HierarchyItemPtr(new core::HierarchyItem("Motion Analysis Results", QString()));

		utils::shared_ptr<const core::ConstVariantsList> data;
		data = wrapper->get();

		for (auto it = data->begin(); it != data->end(); ++it){
			core::HierarchyHelperPtr helper;

			if ((*it)->data()->isSupported(typeid(c3dlib::VectorChannelReaderInterface))) {
				helper = core::HierarchyHelperPtr(new NewVector3ItemHelper(*it));
			} else {
				helper = core::HierarchyHelperPtr(new core::WrappedItemHelper(*it));
			}

			std::string name = (*it)->data()->getClassName();
			(*it)->getMetadata("core/label", name);

			core::IHierarchyItemPtr di =
				core::HierarchyItemPtr(new core::HierarchyDataItem(*it, QIcon(), QString::fromStdString(name), QString(), helper));

			dataItem->appendChild(di);
		}
	}
	else {
		core::HierarchyHelperPtr helper;

		if (wrapper->data()->isSupported(typeid(c3dlib::VectorChannelReaderInterface))) {
			helper = core::HierarchyHelperPtr(new NewVector3ItemHelper(wrapper));
		}
		else {
			helper = core::HierarchyHelperPtr(new core::WrappedItemHelper(wrapper));
		}

		dataItem = core::HierarchyItemPtr(new core::HierarchyDataItem(wrapper, QIcon(), QString::fromStdString(wrapper->data()->getClassName()), QString(), helper));
	}

	emit itemConsumed(dataItem);

	this->dataItem = dataItem;
}

void UniversalSink::reset()
{
	//runNo = hmm->getProcessedBranchCount();
}

UniversalSink::UniversalSink()
{
	_UniversalSink();
}

QWidget* UniversalSink::getConfigurationWidget()
{
	return widget;
}