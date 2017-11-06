#include "DicomImporterPCH.h"
#include "dicomLib/DicomSplitterModel.h"

using namespace dicomImporter;
DicomSplitterModel::DicomSplitterModel(QObject *parent, std::vector<DicomInternalStructPtr>&& _data)
	:QAbstractTableModel(parent), dicomData(std::move(_data))
{
}

//-----------------------------------------------------------------
int DicomSplitterModel::rowCount(const QModelIndex & /*parent*/) const
{
	return countSessions();
}

//-----------------------------------------------------------------
int DicomSplitterModel::columnCount(const QModelIndex & /*parent*/) const
{
	return 2;
}

//-----------------------------------------------------------------
QVariant DicomSplitterModel::data(const QModelIndex &index, int role) const
{
	if (role == Qt::DisplayRole) {
		internalData::StudyPtr s = getSession(index.row());
		switch (index.column()) {
		case 0: return QString::fromStdString(s->studyDate);
		case 1: return s->studyNumber;
		}
		return QVariant();
	}
	return QVariant();
}

//-----------------------------------------------------------------
//! [quoting DicomSplitterModel_e]
bool DicomSplitterModel::setData(const QModelIndex & index, const QVariant & value, int role)
{
	if (role == Qt::EditRole) {
		internalData::StudyPtr session = getSession(index.row());
		//save value from editor to member m_gridData
		if (index.column() == 1) {
			int v = value.toInt();
			session->studyNumber = v;
			//emit editCompleted(value.toQString());
		}
		
		
	}
	return true;
}
//! [quoting DicomSplitterModel_e]

//-----------------------------------------------------------------
//! [quoting DicomSplitterModel_f]
Qt::ItemFlags DicomSplitterModel::flags(const QModelIndex &index) const
{
	return Qt::ItemIsEditable | QAbstractTableModel::flags(index);
}

int dicomImporter::DicomSplitterModel::countSessions() const
{
	int count = 0;
	for (auto& i : dicomData) {
		for (auto& p : i->patients) {
			count += p->sessions.size();
		}
	}
	return count;
}

dicomImporter::internalData::StudyPtr dicomImporter::DicomSplitterModel::getSession(int i) const
{
	int count = 0;
	for (auto& idata : dicomData) {
		for (auto& p : idata->patients) {
			int inCount = p->sessions.size();
			if (count + inCount > i) {
				return p->sessions[i - count];
			}
			count += inCount;
		}
	}
	throw std::runtime_error("Inner error");
}

//! [quoting DicomSplitterModel_f]
