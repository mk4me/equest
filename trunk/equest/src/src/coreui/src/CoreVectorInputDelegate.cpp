#include "CoreUiPCH.h"
#include <coreui/CoreVectorInputDelegate.h>

using namespace coreUI;

CoreVectorInputDelegate::CoreVectorInputDelegate(QObject * parent)
	: QStyledItemDelegate(parent)
{

}

CoreVectorInputDelegate::~CoreVectorInputDelegate()
{

}

void CoreVectorInputDelegate::commitAndCloseEditor()
{
	QWidget * editor = qobject_cast<QWidget*>(sender());
	emit commitData(editor);
	emit closeEditor(editor);
}

CoreVectorEditor::CoreVectorEditor(QWidget * parent) : QWidget(parent)
{
	setAutoFillBackground(true);
}

CoreVectorEditor::~CoreVectorEditor()
{

}

void CoreVectorEditor::onValueChanged()
{
	QWidget * w = static_cast<QWidget*>(sender());
	auto idx = indexOfValueEditor(w);
	customActionsOnValueChange(idx);
	emit valueChanged(idx);
}

QString CoreVectorEditor::vectorPattern(const unsigned int size, const QChar valueSeparator)
{
	QString ret;

	if (size < 2){
		return ret;
	}

	ret += "(";

	unsigned int i = 0;

	for ( ; i < size - 1; ++i)
	{
		ret += QString("%%1").arg(i) + valueSeparator + " ";
	}

	ret += QString("%%1)").arg(i);

	return ret;
}

QString CoreVectorEditor::quaternionPattern(const QChar valueSeparator)
{
	QString ret("[(");	

	unsigned int i = 0;

	for (; i < 2; ++i)
	{
		ret += QString("%%1").arg(i) + valueSeparator + " ";
	}

	ret += QString("%%1)").arg(i++) + valueSeparator + " ";
	ret += QString("%%1]").arg(i);

	return ret;
}