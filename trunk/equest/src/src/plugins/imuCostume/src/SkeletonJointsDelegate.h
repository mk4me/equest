/********************************************************************
	created:  2015/03/09	20:45:18
	filename: SkeletonJointsDelegate.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD___SKELETONJOINTSDELEGATE_H__
#define __HEADER_GUARD___SKELETONJOINTSDELEGATE_H__

#include <QtWidgets/QStyledItemDelegate>
#include <QtWidgets/QComboBox>
#include <QtGui/QStandardItemModel>
#include <kinematiclib/LinearizedSkeleton.h>

//! Delegat obs³uguj¹cy wybór jointa szkieletu
class SkeletonJointsDelegate : public QStyledItemDelegate
{
	Q_OBJECT

public:

	SkeletonJointsDelegate(const bool oneToOne = true,
		QObject * parent = nullptr);

	virtual ~SkeletonJointsDelegate();
	//! \param root Korzeñ drzewa szkiletu jointów
	void setJoints(const kinematic::LinearizedSkeleton::Mapping & joints);
	//! \return Domyœlny tekst wyboru jointa
	static QString defaultText();
	//! \return Domyslna wartoœæ
	static kinematic::LinearizedSkeleton::NodeIDX defaultValue();

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

	//! \return Czy mamy mapowanie jeden do jeden
	bool oneToOne() const;

public slots:
	//! \param oneToOne Czy mamy mapowanie jeden joint do jednego sensora
	void setOneToOne(const bool oneToOne);

private:
	kinematic::LinearizedSkeleton::Mapping joints;
	bool oneToOne_;
};

#endif	// __HEADER_GUARD___SKELETONJOINTSDELEGATE_H__