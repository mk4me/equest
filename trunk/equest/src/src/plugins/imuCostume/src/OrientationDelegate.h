/********************************************************************
	created:  2014/12/30	10:38:16
	filename: OrientationDelegate.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_IMU__ORIENTATIONDELEGATE_H__
#define __HEADER_GUARD_IMU__ORIENTATIONDELEGATE_H__

#include <QtWidgets/QStyledItemDelegate>
#include <osg/Quat>

//! Klasa delegata obs³uguj¹cego wprowadzanie orientacji
class OrientationDelegate : public QStyledItemDelegate
{
	Q_OBJECT

public:
	//! \param parent Rodzic obiektu
	OrientationDelegate(QObject * parent = nullptr);
	//! Destruktor wurtualny
	virtual ~OrientationDelegate();
	//! \brief Tworzy edytor
	virtual QWidget * createEditor(QWidget * parent,
		const QStyleOptionViewItem & option,
		const QModelIndex & index) const override;

	//! \brief Ustawia dane edytora przed edycj¹ (z modelu)
	virtual void setEditorData(QWidget * editor,
		const QModelIndex & index) const override;

	//! \brief Ustawia dane modelu po zakoñczeniu edycji
	virtual void setModelData(QWidget * editor,
		QAbstractItemModel * model,
		const QModelIndex & index) const override;

	//! \brief Aktualizuje geometrie edytora
	virtual void updateEditorGeometry(QWidget *editor,
		const QStyleOptionViewItem &option,
		const QModelIndex &index) const override;

private slots:
	//! Slot dla akceptacji zmian
	void onAccept();
	//! Slot dla odrzucenia zmian
	void onReject();

private:
	//! Czy by³a zmiana
	mutable bool update;
};

#endif	// __HEADER_GUARD_IMU__ORIENTATIONDELEGATE_H__