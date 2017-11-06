/********************************************************************
	created:  2015/03/09	20:38:17
	filename: Vector3DDelegate.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD___VECTOR3DDELEGATE_H__
#define __HEADER_GUARD___VECTOR3DDELEGATE_H__

#include <QtWidgets/QStyledItemDelegate>

class Vector3DDelegate : public QStyledItemDelegate
{
public:
	Vector3DDelegate(QObject * parent = nullptr) : QStyledItemDelegate(parent)
	{

	}

	virtual ~Vector3DDelegate()
	{

	}

	virtual QWidget * createEditor(QWidget * parent, const QStyleOptionViewItem & option, const QModelIndex & index) const override
	{
		//QWidget * retEditor = nullptr;
		//auto row = index.row();
		//auto data = index.model()->data(index.model()->index(row, JointIDX), Qt::UserRole);
		//if (data.canConvert<kinematic::JointConstPtr>() == true){
		//	auto joint = data.value<kinematic::JointConstPtr>();
		//	if (joint != nullptr){
		//		if (joint->children.size() == 1){
		//			std::unique_ptr<QDoubleSpinBox> editor(new QDoubleSpinBox(parent));
		//			editor->setMinimum(0);
		//			editor->setMaximum(joint->children.front()->value.position.length());
		//			const auto val = index.model()->data(index, Qt::UserRole).value<osg::Vec3d>();
		//			editor->setValue(val.length());
		//			return editor.release();
		//		}
		//		else{
		//			//TODO - custom editor for local position: x, y, z
		//		}
		//	}
		//}

		//return retEditor;
		return nullptr;
	}

	virtual void setEditorData(QWidget * editor, const QModelIndex & index) const override
	{
		//auto row = index.row();
		//auto data = index.model()->data(index.model()->index(row, JointIDX), Qt::UserRole);
		//if (data.canConvert<kinematic::JointConstPtr>() == true){
		//	auto joint = data.value<kinematic::JointConstPtr>();
		//	if (joint != nullptr){
		//		if (joint->children.size() == 1){
		//			QDoubleSpinBox * e = qobject_cast<QDoubleSpinBox*>(editor);
		//			auto direction = joint->children.front()->value.position;
		//			e->setValue(direction.length());
		//		}
		//		else{
		//			//TODO - custom editor for local position: x, y, z
		//		}
		//	}
		//}
	}

	virtual void setModelData(QWidget * editor, QAbstractItemModel * model, const QModelIndex & index) const override
	{
		//auto row = index.row();
		//auto data = index.model()->data(index.model()->index(row, JointIDX), Qt::UserRole);
		//if (data.canConvert<kinematic::JointConstPtr>() == true){
		//	auto joint = data.value<kinematic::JointConstPtr>();
		//	if (joint != nullptr){
		//		if (joint->children.size() == 1){
		//			QDoubleSpinBox * e = qobject_cast<QDoubleSpinBox*>(editor);
		//			osg::Vec3d direction = joint->children.front()->value.position;
		//			direction.normalize();
		//			direction *= e->value();
		//			model->setData(index, QVariant::fromValue(direction), Qt::UserRole);
		//			model->setData(index, coreUI::CoreVectorEditor::vectorToString(direction, 3, coreUI::CoreVectorEditor::vectorPattern(3)));
		//		}
		//		else{
		//			//TODO - custom editor for local position: x, y, z
		//		}
		//	}
		//}
	}

	virtual void updateEditorGeometry(QWidget * editor, const QStyleOptionViewItem & option, const QModelIndex & index) const override
	{
		//editor->setGeometry(option.rect);
	}

private:

	//const int column;
};

#endif	// __HEADER_GUARD___VECTOR3DDELEGATE_H__