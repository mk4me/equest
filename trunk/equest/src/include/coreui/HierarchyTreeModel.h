/********************************************************************
	created:	2013/04/17
	created:	17:4:2013   10:13
	filename: 	HierarchyTreeModel.h
	author:		Wojciech Kniec

	purpose:
	*********************************************************************/

#ifndef HEADER_GUARD_COREUI__HierarchyTreeModel_H__
#define HEADER_GUARD_COREUI__HierarchyTreeModel_H__

#include <QtCore/QAbstractItemModel>
#include <QtCore/QModelIndex>
#include <QtCore/QVariant>
#include <corelib/HierarchyHelper.h>
#include <corelib/IHierarchyItem.h>
#include <corelib/IHierarchyDataItem.h>
#include <corelib/IDataManager.h>
#include <corelib/IDataHierarchyManagerReader.h>
#include <coreui/Export.h>
#include <corelib/IFilterCommand.h>

namespace coreUI {
	//! Klasa jest poœrednikiem pomiêdzy hierarchiczn¹ struktur¹ opart¹ na IHierarchyItem, a klasami widoku Qt
	class COREUI_EXPORT HierarchyTreeModel : public QAbstractItemModel
	{
		Q_OBJECT
	public:
		HierarchyTreeModel(QObject* parent = nullptr);
		virtual ~HierarchyTreeModel();

	public:
		QVariant data(const QModelIndex &index, int role) const;
		Qt::ItemFlags flags(const QModelIndex &index) const;
		QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
		QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
		QModelIndex parent(const QModelIndex &index) const;
		int rowCount(const QModelIndex &parent = QModelIndex()) const;
		int columnCount(const QModelIndex &parent = QModelIndex()) const;

		void addRootItem(core::IHierarchyItemConstPtr root);
		void removeRootItem(core::IHierarchyItemConstPtr root);
		void updateItem(core::IHierarchyItemConstPtr item);
		void clear();
		int getNumChildren() const;
		core::IHierarchyItemConstPtr getChild(int idx);

		core::IHierarchyItemConstPtr internalSmart(const QModelIndex& idx) const;

		void applyChange(const core::IDataHierarchyManagerReader::Change& change);
		void applyChanges(const core::IDataHierarchyManagerReader::ChangeList & changes);

		void setFilter(const core::IFilterCommandPtr& filter);

		void rebuildFilteredRoots();

	private:
		QModelIndex createSmartIndex(int row, int col, core::IHierarchyItemConstPtr ptr) const;
		void clearSmartIndex(core::IHierarchyItemConstPtr ptr);
		bool hasChild(core::IHierarchyItemConstPtr parent, core::IHierarchyItemConstPtr child) const;

	private:
		mutable std::map<const void*, core::IHierarchyItemConstWeakPtr> raw2Smart;
		//core::IHierarchyItemPtr rootItem;
		std::vector<core::IHierarchyItemConstPtr> roots;

		std::vector<core::IHierarchyItemConstPtr> rootOrigins;
		core::IFilterCommandPtr currentFilter;
	};
}

#endif
