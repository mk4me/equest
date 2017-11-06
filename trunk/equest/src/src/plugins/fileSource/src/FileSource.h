/********************************************************************
	created:	2013/08/13
	created:	13:8:2013   11:49
	filename: 	FileSource.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_FILESOURCE__FILESOURCE_H__
#define HEADER_GUARD_FILESOURCE__FILESOURCE_H__

#include <corelib/ISource.h>
#include <utils/Filesystem.h>
#include <plugins/newChart/Wrappers.h>
#include <plugins/fileSource/IFileSource.h>

class FileSource : public plugin::ISource, public fileSource::IFileSource
{
    UNIQUE_ID("{D9DBFE39-29BC-42C9-92AA-691B88635892}")
    CLASS_DESCRIPTION("File Source", "File Source")
public:
    FileSource();
	virtual ~FileSource() {}


public:
    virtual void init( core::IDataManager * memoryDM, core::IStreamDataManager * streamDM, core::IFileDataManager * fileDM,
		core::IDataHierarchyManager * hierarchyDM);

    virtual bool lateInit();

    virtual void finalize();

    virtual void update( double deltaTime );

    virtual QWidget* getWidget();

    virtual QWidget* getControlWidget();

    virtual QWidget* getSettingsWidget();

    virtual void getOfferedTypes( utils::TypeInfoList & offeredTypes ) const;
    void addFile( const utils::Filesystem::Path& path );

	void loadAsfAmc();

	void addChartStream();

	void fillStream();

private:
    core::IDataManager* memoryDM;
	core::IDataHierarchyManager * hierarchyDM;
    core::IFileDataManager* fileDM;
    core::IStreamDataManager* streamDM;
    std::vector<ScalarStreamPtr> streams;
};
DEFINE_SMART_POINTERS(FileSource);

#endif
