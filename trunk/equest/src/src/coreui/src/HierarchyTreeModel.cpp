#include "CoreUiPCH.h"
#include <coreui/HierarchyTreeModel.h>
#include <QtWidgets/QMenu>
#include <corelib/IHierarchyDataItem.h>
#include <corelib/IHierarchyItem.h>
#include <corelib/HierarchyItem.h>
#include <corelib/AbstractFilterCommand.h>

#ifdef _DEBUG
#include <utils/modeltest.h>
#endif

using namespace coreUI;

HierarchyTreeModel::HierarchyTreeModel(QObject *parent) :
QAbstractItemModel(parent),
currentFilter(new core::NullFilter())
{
#ifdef _DEBUG
	new ModelTest(this, this);
#endif
}

HierarchyTreeModel::~HierarchyTreeModel()
{
}

int HierarchyTreeModel::columnCount(const QModelIndex &parent) const
{
	return 1;
}

QVariant HierarchyTreeModel::data(const QModelIndex &index, int role) const
{
	if (!index.isValid()) {
		return QVariant();
	}

	core::IHierarchyItemConstPtr item = internalSmart(index);
	if (role == Qt::DisplayRole) {
		return item->getName();
	}

	if (role == Qt::DecorationRole) {
		return item->getIcon();
	}

	return QVariant();
}

Qt::ItemFlags HierarchyTreeModel::flags(const QModelIndex &index) const
{
	if (!index.isValid()) {
		return 0;
	}

	return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

QVariant HierarchyTreeModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (role == Qt::DisplayRole)
	{
		if (orientation == Qt::Horizontal && section == 0) {
			return QString(tr("User area"));
		}
	}
	return QVariant();
}

QModelIndex HierarchyTreeModel::index(int row, int column, const QModelIndex &parent)
const
{
	if (!hasIndex(row, column, parent)) {
		return QModelIndex();
	}

	core::IHierarchyItemConstPtr childItem;
	if (!parent.isValid()) {
		childItem = roots[row];
	}
	else {
		core::IHierarchyItemConstPtr parentItem = internalSmart(parent);
		childItem = parentItem->getChild(row);
	}

	if (childItem) {
		return createSmartIndex(row, column, childItem);
	}
	else {
		return QModelIndex();
	}
}

QModelIndex HierarchyTreeModel::parent(const QModelIndex &index) const
{
	if (!index.isValid()) {
		return QModelIndex();
	}

	core::IHierarchyItemConstPtr childItem = internalSmart(index);

	if (!childItem) {
		return QModelIndex();
	}

	core::IHierarchyItemConstPtr parentItem = childItem->getParent();

	if (!parentItem) {
		return QModelIndex();
	}
	int row = parentItem->getIndex();
	for (int i = roots.size() - 1; i >= 0; --i) {
		if (parentItem == roots[i]) {
			row = i;
			break;
		}
	}
	return createSmartIndex(row, 0, parentItem);
}

int HierarchyTreeModel::rowCount(const QModelIndex &parent) const
{
	core::IHierarchyItemConstPtr parentItem;
	if (parent.column() > 0) {
		UTILS_ASSERT(false);
		return 0;
	}

	if (!parent.isValid()) {
		return getNumChildren();
	}

	parentItem = internalSmart(parent);
	return parentItem->getNumChildren();
}

void HierarchyTreeModel::addRootItem(core::IHierarchyItemConstPtr root)
{
	auto it = std::find(rootOrigins.begin(), rootOrigins.end(), root);
	if (it != rootOrigins.end()) {
		throw loglib::runtime_error("Root item is already present in tree model");
	}
	else {
		//Q_EMIT layoutAboutToBeChanged();
		roots.push_back(currentFilter->getFilteredTree(root));
		rootOrigins.push_back(root);
		//Q_EMIT layoutChanged();
		beginResetModel();
		endResetModel();
	}
}

QModelIndex HierarchyTreeModel::createSmartIndex(int row, int col, core::IHierarchyItemConstPtr ptr) const
{
	raw2Smart[ptr.get()] = ptr;
	return createIndex(row, col, const_cast<core::IHierarchyItem*>(ptr.get()));
}

void HierarchyTreeModel::clearSmartIndex(core::IHierarchyItemConstPtr ptr)
{
	raw2Smart.erase(ptr.get());
	for (auto i = 0; i < ptr->getNumChildren(); ++i){
		clearSmartIndex(ptr->getChild(i));
	}
}

core::IHierarchyItemConstPtr HierarchyTreeModel::internalSmart(const QModelIndex& idx) const
{
	if (idx.isValid()) {
		auto raw = idx.internalPointer();
		auto it = raw2Smart.find(const_cast<void*>(raw));
		if (it != raw2Smart.end()) {
			return it->second.lock();
		}

		UTILS_ASSERT(false);
	}
	return core::IHierarchyItemConstPtr();
}

void HierarchyTreeModel::updateItem(core::IHierarchyItemConstPtr item)
{
	// TODO : zrobic to wlasciwie
	rebuildFilteredRoots();
	//Q_EMIT layoutChanged();
	beginResetModel();
	endResetModel();
}

void HierarchyTreeModel::removeRootItem(core::IHierarchyItemConstPtr root)
{
	auto it = std::find(rootOrigins.begin(), rootOrigins.end(), root);
	if (it != rootOrigins.end()) {
		clearSmartIndex(root);
		rootOrigins.erase(it);
		rebuildFilteredRoots();
	}
	else {
		throw loglib::runtime_error("Unable to remove root item from hierarchy tree model");
	}
}

bool HierarchyTreeModel::hasChild(core::IHierarchyItemConstPtr parent, core::IHierarchyItemConstPtr child) const
{
	auto count = parent->getNumChildren();
	for (auto i = 0; i < count; ++i) {
		if (parent->getChild(i) == child) {
			return true;
		}
	}

	return false;
}

void coreUI::HierarchyTreeModel::clear()
{
	roots.clear();
	rootOrigins.clear();
	currentFilter = utils::make_shared<core::NullFilter>();
	beginResetModel();
	endResetModel();
}

void coreUI::HierarchyTreeModel::applyChange(const core::IDataHierarchyManagerReader::Change& change)
{
	switch (change.modification) {
	case core::IDataHierarchyManagerReader::ADD_ITEM:
		addRootItem(change.item);
		break;
	case  core::IDataHierarchyManagerReader::REMOVE_ITEM:
		removeRootItem(change.item);
		break;
	case core::IDataHierarchyManagerReader::UPDATE_ITEM:
		updateItem(change.item);
		break;
	default:
		UTILS_ASSERT(false);
	}
}

void coreUI::HierarchyTreeModel::applyChanges(const core::IDataHierarchyManagerReader::ChangeList & changes)
{
	for (auto it = changes.begin(); it != changes.end(); ++it) {
		applyChange(*it);
	}
}

int coreUI::HierarchyTreeModel::getNumChildren() const
{
	return roots.size();
}

core::IHierarchyItemConstPtr coreUI::HierarchyTreeModel::getChild(int idx)
{
	return rootOrigins[idx];
}

void coreUI::HierarchyTreeModel::setFilter(const core::IFilterCommandPtr& filter)
{
	this->currentFilter = filter ? filter : utils::make_shared<core::NullFilter>();
	rebuildFilteredRoots();
}

void coreUI::HierarchyTreeModel::rebuildFilteredRoots()
{
	roots.clear();
	for (auto it = rootOrigins.begin(); it != rootOrigins.end(); ++it) {
		roots.push_back(currentFilter->getFilteredTree(*it));
	}

	beginResetModel();
	endResetModel();
}