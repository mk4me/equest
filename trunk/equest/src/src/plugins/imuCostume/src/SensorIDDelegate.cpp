#include "SensorIDDelegate.h"

SensorIDDelegate::SensorIDDelegate(QObject * parent)
	: QStyledItemDelegate(parent)
{

}

SensorIDDelegate::~SensorIDDelegate()
{

}

void SensorIDDelegate::setSensors(const imuCostume::Costume::SensorIDsSet & sensors)
{
	sensors_ = sensors;
}

QString SensorIDDelegate::defaultText()
{
	return tr("Select sensor...");
}

int SensorIDDelegate::defaultValue()
{
	return -1;
}

QWidget * SensorIDDelegate::createEditor(QWidget * parent,
	const QStyleOptionViewItem & option,
	const QModelIndex & index) const
{
	std::unique_ptr<QComboBox> editor(new QComboBox(parent));

	auto locSensors(sensors_);

	/*

	auto s = index.model()->rowCount();
	for (unsigned int i = 0; i < s; ++i)
	{
	if (i != index.row())
	{
	auto sensorID = index.model()->data(index.model()->index(i, index.column())).toInt();
	locSensors.erase(sensorID);
	}
	}

	*/

	editor->addItem(defaultText(), QVariant(defaultValue()));
	const QStandardItemModel* model = qobject_cast<const QStandardItemModel*>(editor->model());
	QStandardItem* item = model->item(0);

	item->setFlags(item->flags() & ~(Qt::ItemIsSelectable | Qt::ItemIsEnabled));

	if (locSensors.empty() == false)
	{
		editor->insertSeparator(1);

		for (const auto & j : locSensors)
		{
			editor->addItem(QString("%1").arg(j), QVariant(j));
		}
	}

	return editor.release();
}

void SensorIDDelegate::setEditorData(QWidget * editor, const QModelIndex & index) const
{
	QComboBox * e = qobject_cast<QComboBox*>(editor);
	e->setCurrentText(index.model()->data(index).toString());
}

void SensorIDDelegate::setModelData(QWidget * editor, QAbstractItemModel * model,
	const QModelIndex & index) const
{
	QComboBox * e = qobject_cast<QComboBox*>(editor);
	model->setData(index, e->currentText());
	model->setData(index, e->currentData(), Qt::UserRole);

	for (int i = 0; i < model->rowCount(); ++i)
	{
		auto idx = model->index(i, index.column());
		if (i != index.row() && e->currentData().toInt() == model->data(idx).toInt()){
			model->setData(idx, defaultText());
			model->setData(idx, QVariant(defaultValue()), Qt::UserRole);
			emit sensorIDReset(i);
			break;
		}
	}
}

void SensorIDDelegate::updateEditorGeometry(QWidget * editor,
	const QStyleOptionViewItem & option,
	const QModelIndex & index) const
{
	editor->setGeometry(option.rect);
}