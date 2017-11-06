#include "HierarchyTreeModelTest.h"
#include <corelib/IHierarchyItem.h>
#include <corelib/HierarchyItem.h>
#include <corelib/HierarchyDataItem.h>
#include "coreui/HierarchyTreeModel.h"
#include <QtWidgets/QApplication>
#include <QtWidgets/QTreeView>
#include "utils/PtrPolicyStd.h"
#include "utils/ClonePolicies.h"

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(HierarchyTreeModelTest);


DEFINE_WRAPPER(int, utils::PtrPolicyStd, utils::ClonePolicyCopyConstructor);



void HierarchyTreeModelTest::setUp()
{
}

void HierarchyTreeModelTest::testModel()
{
    coreUI::HierarchyTreeModel model;
    core::IHierarchyItemPtr rootItem1 = createSimpleTree();
    core::IHierarchyItemPtr rootItem2 = createSimpleTree();
    core::IHierarchyItemPtr rootItem3 = createSimpleTree();

    model.addRootItem(rootItem1);
    model.addRootItem(rootItem2);

    CPPUNIT_ASSERT(model.getNumChildren() == 2);
    CPPUNIT_ASSERT(!rootItem1->getParent());
    CPPUNIT_ASSERT_THROW(model.addRootItem(rootItem1), std::runtime_error);
    CPPUNIT_ASSERT_THROW(model.removeRootItem(rootItem3), std::runtime_error);
    model.addRootItem(rootItem3);
    CPPUNIT_ASSERT(model.getNumChildren() == 3);
    CPPUNIT_ASSERT(model.getChild(0) == rootItem1);
    CPPUNIT_ASSERT(model.getChild(2) == rootItem3);
    model.removeRootItem(rootItem3);
    CPPUNIT_ASSERT(model.getNumChildren() == 2);

    
	core::IDataHierarchyManagerReader::Change change;
    change.item = rootItem3;
	change.modification = core::IDataHierarchyManagerReader::ADD_ITEM;
    model.applyChange(change);
    CPPUNIT_ASSERT(model.getNumChildren() == 3);
    CPPUNIT_ASSERT(model.getChild(2) == rootItem3);

    core::IDataHierarchyManagerReader::Change change1;
    core::IDataHierarchyManagerReader::Change change2;
    core::IDataHierarchyManagerReader::Change change3;

    change1.item = createSimpleTree();
	change1.modification = core::IDataHierarchyManagerReader::ADD_ITEM;

    change2.item = rootItem2;
	change2.modification = core::IDataHierarchyManagerReader::UPDATE_ITEM;

    change3.item = rootItem1;
	change3.modification = core::IDataHierarchyManagerReader::REMOVE_ITEM;
    
	core::IDataHierarchyManagerReader::ChangeList l;
    l.push_back(change1);
    l.push_back(change2);
    l.push_back(change3);

    model.applyChanges(l);
    CPPUNIT_ASSERT(model.getNumChildren() == 3);
    CPPUNIT_ASSERT(model.getChild(2) == change1.item);

    CPPUNIT_ASSERT_THROW(model.applyChanges(l), std::runtime_error);

    CPPUNIT_ASSERT(model.getNumChildren() == 3);
    CPPUNIT_ASSERT(model.getChild(2) == change1.item);

    CPPUNIT_ASSERT(model.data( model.index(0,0), Qt::DisplayRole ) == QString("ROOT") );
}

void HierarchyTreeModelTest::testModelView()
{
    core::IHierarchyItemPtr rootItem1 = createSimpleTree();
    core::IHierarchyItemPtr rootItem2 = createSimpleTree();
    core::IHierarchyItemPtr rootItem3 = createSimpleTree();
    coreUI::HierarchyTreeModel model;
    model.addRootItem(rootItem1);
    model.addRootItem(rootItem2);
    model.addRootItem(rootItem3);
    QTreeView view;
    view.setModel(&model);
    QModelIndex root = view.rootIndex();
    CPPUNIT_ASSERT(model.rowCount(root) == 3);
}

core::IHierarchyItemPtr HierarchyTreeModelTest::createSimpleTree()
{
    core::IHierarchyItemPtr rootItem = utils::make_shared<core::HierarchyItem>("ROOT", "ROOT_DESC", QIcon());
    core::IHierarchyItemPtr child1 = utils::make_shared<core::HierarchyItem>("CHILD1", "CHILD1_DESC", QIcon());
    rootItem->appendChild(child1);

	core::VariantPtr wrapper11 = core::Variant::create<int>();
    wrapper11->set(utils::make_shared<int>(11));
    core::HierarchyHelperPtr helper = utils::make_shared<core::WrappedItemHelper>(wrapper11);
    core::HierarchyDataItemPtr child11 = utils::make_shared<core::HierarchyDataItem>(QIcon(), "CHILD11", "CHILD11_DESC", helper);
    child1->appendChild(child11);

	core::VariantPtr wrapper = core::Variant::create<int>();
    wrapper->set(utils::make_shared<int>(2));
    core::HierarchyDataItemPtr child2 = utils::make_shared<core::HierarchyDataItem>(wrapper, QIcon(), "CHILD2", "CHILD2_DESC");
    rootItem->appendChild(child2);

    return rootItem;
}

void HierarchyTreeModelTest::tearDown()
{
}


