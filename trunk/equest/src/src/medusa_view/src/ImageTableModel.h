/********************************************************************
	created:	2015/05/11
	created:	14:22
	filename: 	ImageTableModel.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_MEDUSA__ImageTableModel_H__
#define HEADER_GUARD_MEDUSA__ImageTableModel_H__

#include <QAbstractTableModel>
#include <corelib/Variant.h>

struct ImageTableRow
{
	QString imgName;
	QString userName;
	int numberOfSynovitis;
	QString regions;
	QString status;
};

class ImageTableWorker;
class ImageTableModel : public QAbstractTableModel {
	Q_OBJECT;
public:

	ImageTableModel(QObject *parent = 0);
	virtual ~ImageTableModel() {}
	
public:
	virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
	virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;
	virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
	virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

public:
	void rebuildTable();
	void clearTable();
	std::unique_ptr<ImageTableWorker> getWorker();

public Q_SLOTS:
	void addRow(const ImageTableRow& r);

private:
	core::ConstVariantsList getLayersObjects();

private:
	std::vector<ImageTableRow> rows;
};
DEFINE_SMART_POINTERS(ImageTableModel);

class ImageTableWorker : public QObject
{
	Q_OBJECT;
public:
	ImageTableWorker(core::ConstVariantsList&& o);
public Q_SLOTS:
	void process();

Q_SIGNALS:
	void rowProduced(const ImageTableRow& r);

private:
	core::ConstVariantsList objects;
};


#endif // HEADER_GUARD 
