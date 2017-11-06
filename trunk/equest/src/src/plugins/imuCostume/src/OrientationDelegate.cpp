#include "PCH.h"
#include "OrientationDelegate.h"
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QVBoxLayout>
#include <coreui/CoreRotationWidgetEditor.h>

OrientationDelegate::OrientationDelegate(QObject * parent)
	: QStyledItemDelegate(parent), update(false)
{

}

OrientationDelegate::~OrientationDelegate()
{

}

QWidget * OrientationDelegate::createEditor(QWidget * parent,
	const QStyleOptionViewItem & option,
	const QModelIndex & index) const
{
	auto d = new QDialog(parent);
	auto e = new coreUI::CoreRotationWidgetEditor(d);

	auto model = index.model();
	auto row = index.row();

	auto j = model->data(model->index(row, 0)).toString();
	auto s = model->data(model->index(row, 1)).toString();
	
	d->setModal(true);
	d->setAttribute(Qt::WA_DeleteOnClose);
	d->setWindowTitle(tr("[Joint: %1] : [Sensor: %2] -> local orientation").arg(s).arg(j));
	auto bb = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, d);

	auto l = new QVBoxLayout;
	l->addWidget(e);
	l->addWidget(bb);
	d->setLayout(l);

	connect(bb, SIGNAL(accepted()), this, SLOT(onAccept()));
	connect(bb, SIGNAL(rejected()), this, SLOT(onReject()));

	connect(bb, SIGNAL(accepted()), d, SLOT(accept()));
	connect(bb, SIGNAL(rejected()), d, SLOT(reject()));

	update = false;

	return d;
}

void OrientationDelegate::setEditorData(QWidget * editor,
	const QModelIndex & index) const
{
	auto r = editor->findChild<coreUI::CoreRotationWidgetEditor*>();
	r->setValue(index.data(Qt::UserRole).value<osg::Quat>());
}

void OrientationDelegate::setModelData(QWidget * editor,
	QAbstractItemModel * model,
	const QModelIndex & index) const
{
	if(update == true){
		auto r = editor->findChild<coreUI::CoreRotationWidgetEditor*>();
		auto value = r->value();
		model->setData(index, QVariant::fromValue(value), Qt::UserRole);
		model->setData(index, QString("[(%1, %2, %3), %4]").arg(value[0]).arg(value[1]).arg(value[2]).arg(value[3]));		
	}
}

void OrientationDelegate::updateEditorGeometry(QWidget *editor,
	const QStyleOptionViewItem &option,
	const QModelIndex &index) const
{
	//CELOWO PUSTE - ZAWSZE NA ŒRODKU PARENTA!!
}

void OrientationDelegate::onAccept()
{
	update = true;	
}

void OrientationDelegate::onReject()
{	
	update = false;
}