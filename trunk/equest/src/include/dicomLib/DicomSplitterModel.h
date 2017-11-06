/********************************************************************
	created:	2015/07/16
	created:	23:17
	filename: 	DicomSplitterModel.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_DICOMIMPORTER__DicomSplitterModel_H__
#define HEADER_GUARD_DICOMIMPORTER__DicomSplitterModel_H__

#include <QtCore/QAbstractTableModel>
#include <QtCore/QString>
#include "DicomInternalStruct.h"
#include <vector>

namespace dicomImporter {
	const int COLS = 3;
	const int ROWS = 2;

	class DicomSplitterModel : public QAbstractTableModel
	{
		Q_OBJECT
	public:
		DicomSplitterModel(QObject *parent, std::vector<DicomInternalStructPtr>&& _data);
		int rowCount(const QModelIndex &parent = QModelIndex()) const;
		int columnCount(const QModelIndex &parent = QModelIndex()) const;
		QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
		bool setData(const QModelIndex & index, const QVariant & value, int role = Qt::EditRole);
		Qt::ItemFlags flags(const QModelIndex & index) const;
		const std::vector<DicomInternalStructPtr>& getSplits() { return dicomData;  }
	private:
		int countSessions() const;
		internalData::StudyPtr getSession(int i) const;
	signals:
		void editCompleted(const QString &);
	private:
		std::vector<DicomInternalStructPtr> dicomData;
	};
}
#endif
