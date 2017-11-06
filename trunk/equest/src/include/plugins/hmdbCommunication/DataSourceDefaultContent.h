/********************************************************************
	created:  2014/08/11	12:56:59
	filename: DataSourceDefaultContent.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_HMDBCOMMUNICATION__DATASOURCEDEFAULTCONTENT_H__
#define __HEADER_GUARD_HMDBCOMMUNICATION__DATASOURCEDEFAULTCONTENT_H__

#include <plugins/hmdbCommunication/Export.h>
#include <plugins/hmdbCommunication/IDataSourceContent.h>

namespace hmdbCommunication
{
	class HMDBCOMMUNICATION_EXPORT DataSourceDefaultContent : public IDataSourceContent
	{
	public:
		DataSourceDefaultContent();
		~DataSourceDefaultContent();

		virtual const QString name() const;
		
		virtual const bool fillContent(QTreeWidgetItem * item, const ShallowCopy & shallowCopy) const;		
		virtual const bool headers(const QTreeWidgetItem * item, QStringList & headers) const;		
		virtual const int statusField(const QTreeWidgetItem * item) const;		
		virtual const int downloadField(const QTreeWidgetItem * item) const;		
		virtual void release();
	};
}

#endif	// __HEADER_GUARD_HMDBCOMMUNICATION__DATASOURCEDEFAULTCONTENT_H__