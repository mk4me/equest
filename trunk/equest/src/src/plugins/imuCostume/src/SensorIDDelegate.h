/********************************************************************
	created:  2015/03/09	21:06:53
	filename: SensorIDDelegate.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD___SENSORIDDELEGATE_H__
#define __HEADER_GUARD___SENSORIDDELEGATE_H__

#include <QtWidgets/QStyledItemDelegate>
#include <QtWidgets/QComboBox>
#include <QtGui/QStandardItemModel>
#include <imucostumelib/ImuCostume.h>

//! Delegat obs³uguj¹cy wybór identyfikatora sensora
class SensorIDDelegate : public QStyledItemDelegate
{
	Q_OBJECT

public:

	SensorIDDelegate(QObject * parent = nullptr);

	virtual ~SensorIDDelegate();
	//! \param sensors Pula sensorów z której mo¿na wybieraæ identyfikatory
	void setSensors(const imuCostume::Costume::SensorIDsSet & sensors);
	//! \return Domyslny tekst dla wyboru identyfikatora sensora
	static QString defaultText();
	//! \return Domyslna wartoœæ dla wyboru identyfikatora sensora
	static int defaultValue();

	virtual QWidget * createEditor(QWidget * parent,
		const QStyleOptionViewItem & option,
		const QModelIndex & index) const override;

	virtual void setEditorData(QWidget * editor,
		const QModelIndex & index) const override;

	virtual void setModelData(QWidget * editor,
		QAbstractItemModel * model,
		const QModelIndex & index) const override;

	virtual void updateEditorGeometry(QWidget * editor,
		const QStyleOptionViewItem & option,
		const QModelIndex & index) const override;

signals:

	void sensorIDReset(int row) const;

private:
	imuCostume::Costume::SensorIDsSet sensors_;
};

#endif	// __HEADER_GUARD___SENSORIDDELEGATE_H__