/********************************************************************
	created:  2014/07/25	10:20:55
	filename: DataViewWidget.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_HMDBCOMMUNICATION__DATAVIEWWIDGET_H__
#define __HEADER_GUARD_HMDBCOMMUNICATION__DATAVIEWWIDGET_H__

#include <plugins/hmdbCommunication/Export.h>
#include <QtWidgets/QWidget>
#include <plugins/hmdbCommunication/ShallowCopy.h>
#include <plugins/hmdbCommunication/IHMDBRemoteContext.h>
#include <plugins/hmdbCommunication/IHMDBShallowCopyContext.h>
#include <plugins/hmdbCommunication/IHMDBStatusManager.h>
#include <coreui/CoreCursorChanger.h>
#include <plugins/hmdbCommunication/DataStatus.h>
#include <plugins/hmdbCommunication/Types.h>
#include <corelib/IThreadPool.h>

class QTreeWidgetItem;

namespace Ui
{
	class DataViewWidget;
}

namespace hmdbCommunication
{
	class IHMDBShallowCopyContext;
	class IDataSourcePerspective;	
	class IDataSourceContent;
	class ShallowCopyFilter;
}

Q_DECLARE_OPAQUE_POINTER(hmdbCommunication::IDataSourcePerspective*);
Q_DECLARE_OPAQUE_POINTER(hmdbCommunication::IDataSourceContent*);
Q_DECLARE_OPAQUE_POINTER(hmdbCommunication::ShallowCopyFilter*);
Q_DECLARE_METATYPE(hmdbServices::ID);
Q_DECLARE_METATYPE(hmdbCommunication::DataType);

class HMDBCOMMUNICATION_EXPORT DataViewWidget : public QWidget
{
	Q_OBJECT

public:

	class IOperation : public threadingUtils::INormalizedProgress, public threadingUtils::IAbortable
	{
	public:
		virtual ~IOperation() {}
		virtual QString name() const = 0;
	};

	DEFINE_SMART_POINTERS(IOperation);

public:

	DataViewWidget(hmdbCommunication::IHMDBShallowCopyContextPtr shallowCopyContext,
		QWidget * parent = nullptr, Qt::WindowFlags f = 0);

	virtual ~DataViewWidget();

	IOperation * operation();

	hmdbCommunication::ShallowCopyConstPtr currentShallowCopy() const;
	hmdbCommunication::ShallowCopyConstPtr completeShallowCopy() const;

public slots:

	void selectItem(char type, int id);

	void setPerspective(hmdbCommunication::IDataSourcePerspective * perspective);
	void setContent(hmdbCommunication::IDataSourceContent * content);
	void setFilter(hmdbCommunication::ShallowCopyFilter * filter);

	void rebuildPerspective();
	void refreshContent();	
	void onRefreshStatus();
	void onTrySynchronize();
	void onForceSynchronize();

private slots:

	void onLoad();
	void onLoadAll();
	void onUnload();
	void onUnloadAll();
	void onRebuild();
	void onDownload();
	void onDownloadAll();
	void onForceDownload();
	void onForceDownloadAll();
	void onTreeCurrentItemChange(QTreeWidgetItem* current, QTreeWidgetItem* previous);
	void setRemoteOperationsEnabled(const bool enable);
	void setLocalOperationsEnabled(const bool enable);
	void onSynchronizeFinished();
	void onDownloadFinished();
	void initializeShallowCopy();
	void onContextMenu(QPoint position);
	void onUpdateModifiedTrials();

signals:

	void currentItemChanged(char currentType, int currentId, char previousType, int previousId);
	void operationAboutToStart();
	void operationFinished();
	void shallowCopyChanged();
	void modifiedTrialsPresent();

private:
	static void extractItem(const QTreeWidgetItem * item, int & id, char & type);

	void doShallowCopyFiltering();
	void rawFilterShallowCopy();
	void filterShallowCopy();
	void setShallowCopy(const hmdbCommunication::ShallowCopyConstPtr shallowCopy);
	void extractLocalShallowCopy();
	void noShallowCopyInStorage();
	void noShallowCopyInStorageAndLocalOnly();
	void extractLocalShallowCopyAndTryUpdate();

	void refreshCurrentContent(bool perspectiveFirst);	

	hmdbCommunication::DataStatus refrshItemContent(QTreeWidgetItem * item,
		const bool perspectiveFirst);

	hmdbCommunication::DataStatus refreshDataStatus(QTreeWidgetItem * item,
		const bool perspectiveFirst);
	void refreshDataStatus(const bool perspectiveFirst);	
	bool tryRebuildDataStatus();

	QIcon statusIcon(const hmdbCommunication::DataStatus dataStatus);

	void onSynchronize();
	

	void initializeActions();
	void setDefaultPerspectiveHeaders();

	void synchronize(hmdbCommunication::IHMDBShallowCopyRemoteContext::SynchronizeOperationPtr sOp,
		utils::shared_ptr<coreUI::CoreCursorChanger> cursorChanger);
	void download(hmdbCommunication::IHMDBRemoteContext::OperationPtr dOp,
		utils::shared_ptr<coreUI::CoreCursorChanger> cursorChanger);

	void setupDownload(const hmdbCommunication::StorageFileNames & files);

	
	hmdbCommunication::StorageFileNames filterModifiedTrialFiles(hmdbCommunication::IncrementalBranchShallowCopyConstPtr incShallowCopy);
	bool isTrialLocal(const hmdbServices::IncrementalBranchShallowCopy::Trial& trial,
		hmdbCommunication::IHMDBStatusManager::TransactionConstPtr transaction);

private:
	core::Thread remoteOperationThread;
	core::Thread filteringThread;
	hmdbCommunication::ShallowCopyConstPtr currentShallowCopy_;
	hmdbCommunication::ShallowCopyConstPtr completeShallowCopy_;
	hmdbCommunication::IDataSourcePerspective * perspective_;
	hmdbCommunication::IDataSourceContent * content_;
	hmdbCommunication::ShallowCopyFilter * filter_;
	hmdbCommunication::IHMDBShallowCopyContextPtr shallowCopyContext_;	
	Ui::DataViewWidget * ui;
	IOperationPtr operation_;
	std::map<hmdbCommunication::DataStatus, QIcon> icons;

	hmdbCommunication::StorageFileNames modifiedTrialsFiles;
};


#endif	// __HEADER_GUARD_HMDBCOMMUNICATION__DATAVIEWWIDGET_H__