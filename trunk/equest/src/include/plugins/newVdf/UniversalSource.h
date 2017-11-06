/********************************************************************
	created:	2013/10/02
	created:	2:10:2013   10:50
	filename: 	UniversalSource.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_NEWVDF__UNIVERSALSOURCE_H__
#define HEADER_GUARD_NEWVDF__UNIVERSALSOURCE_H__

#include <QtWidgets/QTreeView>
#include <corelib/HierarchyItem.h>
#include <corelib/HierarchyDataItem.h>
#include <plugins/newVdf/TypeFilter.h>
#include <coreui/HierarchyTreeModel.h>
#include <plugins/newVdf/UniversalSource.h>
#include <plugins/newVdf/UniversalOutputPin.h>
#include <dflib/Node.h>
#include <dflib/IDFNode.h>
#include <plugins/newVdf/INodeConfiguration.h>

namespace vdf {

    template<class T>
    class UniversalSource : public df::SourceNode, public df::IDFSource, public vdf::INodeConfiguration, public vdf::INodeValidation, public vdf::INodeHierarchyObserver
    {
    public:
        UniversalSource ()
        {
            tree = new QTreeView();
            filter = utils::make_shared<TypeFilter<T>>();
            model.setFilter(filter);
            tree->setModel(&model);

            tree->setHeaderHidden(true);
            outPinA = new UniversalOutputPinT<T>(this);
            addOutputPin(outPinA);
            used = false;
        }

        virtual ~UniversalSource() { delete tree; }
    public:
        virtual void reset()
        {
            used = false;
        }
        virtual const bool empty() const
        {
			return used;// || !channel;
        }
        virtual void produce()
        {
            auto item = tryGetSelectedItem();
            auto dataItem = utils::dynamic_pointer_cast<const core::IHierarchyDataItem>(item);
            if (dataItem && dataItem->getData()) {
                outPinA->setWrapper(dataItem->getData());
            }
			used = true;
        }


        virtual QWidget* getConfigurationWidget() { return tree; }
        virtual bool isNodeValid()
        {
			return tryGetSelectedItem() != core::IHierarchyItemConstPtr();
        }
        virtual QString getErrorMessage()
        {
            return isNodeValid() ? QString() : QString("Source is not set");
        }

		virtual void refresh(core::IDataHierarchyManager* hm, const core::IDataHierarchyManager::ChangeList & changes)
        {
            if (changes.empty()) {
				auto t = hm->transaction();
                model.clear();
                model.setFilter(filter);
                for (auto it = t->hierarchyBegin(); it != t->hierarchyEnd(); ++it) {
                    model.addRootItem(*it);
                }
            } else {
                model.applyChanges(changes);
            }
        }

    private:

        core::IHierarchyItemConstPtr tryGetSelectedItem()
        {
            QModelIndexList list = tree->selectionModel()->selectedIndexes();
            if (!list.empty()) {
                QModelIndex idx = list.front();
                return model.internalSmart(idx);
            }
            return core::IHierarchyItemConstPtr();
        }


    private:
        UniversalOutputPin * outPinA;
        QTreeView* tree;
        UniversalOutputPin::ConstPtr channel;
        bool used;
        QIcon rootIcon;
        QIcon leafIcon;
        coreUI::HierarchyTreeModel model;
        utils::shared_ptr<TypeFilter<T>> filter;
    };

}
#endif
