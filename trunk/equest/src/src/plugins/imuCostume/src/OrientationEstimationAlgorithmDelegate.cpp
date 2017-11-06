#include "OrientationEstimationAlgorithmDelegate.h"
#include <corelib/BaseDataTypes.h>

Q_DECLARE_METATYPE(IMU::IIMUOrientationEstimationAlgorithmConstPtr);

OrientationEstimationAlgorithmDelegate::OrientationEstimationAlgorithmDelegate(QObject * parent)
	: QStyledItemDelegate(parent)
{

}

OrientationEstimationAlgorithmDelegate::~OrientationEstimationAlgorithmDelegate()
{

}

void OrientationEstimationAlgorithmDelegate::setOrientationEstimationAlgorithms(const IMU::IIMUDataSource::OrientationEstimationAlgorithms & algorithms)
{
	algorithms_ = algorithms;
}

const IMU::IIMUDataSource::OrientationEstimationAlgorithms & OrientationEstimationAlgorithmDelegate::orientationEstimationAlgorithms() const
{
	return algorithms_;
}

QString OrientationEstimationAlgorithmDelegate::defaultText()
{
	return tr("Select algorithm...");
}

IMU::IIMUOrientationEstimationAlgorithmConstPtr OrientationEstimationAlgorithmDelegate::defaultValue()
{
	return IMU::IIMUOrientationEstimationAlgorithmConstPtr();
}

QWidget * OrientationEstimationAlgorithmDelegate::createEditor(QWidget * parent, const QStyleOptionViewItem & option, const QModelIndex & index) const
{
	std::unique_ptr<QComboBox> editor(new QComboBox(parent));

	editor->addItem(defaultText(), QVariant::fromValue(defaultValue()));
	const QStandardItemModel* model = qobject_cast<const QStandardItemModel*>(editor->model());
	QStandardItem* item = model->item(0);

	item->setFlags(item->flags() & ~(Qt::ItemIsSelectable | Qt::ItemIsEnabled));

	if (algorithms_.empty() == false)
	{
		editor->insertSeparator(1);

		unsigned int counter = 1;

		for (const auto & j : algorithms_)
		{
			editor->addItem(QString("%1").arg(QString::fromStdString(j.second->name())), QVariant::fromValue(j.second));
			editor->setItemData(counter++, QString("ID: %1").arg(QString::fromStdString(boost::lexical_cast<std::string>(j.first))), Qt::ToolTipRole);
		}
	}

	return editor.release();
}

void OrientationEstimationAlgorithmDelegate::setEditorData(QWidget * editor, const QModelIndex & index) const
{
	QComboBox * e = qobject_cast<QComboBox*>(editor);
	e->setCurrentText(index.model()->data(index).toString());
}

void OrientationEstimationAlgorithmDelegate::setModelData(QWidget * editor, QAbstractItemModel * model, const QModelIndex & index) const
{
	QComboBox * e = qobject_cast<QComboBox*>(editor);
	model->setData(index, e->currentText());
	model->setData(index, e->currentData(), Qt::UserRole);
}

void OrientationEstimationAlgorithmDelegate::updateEditorGeometry(QWidget * editor, const QStyleOptionViewItem & option, const QModelIndex & index) const
{
	editor->setGeometry(option.rect);
}