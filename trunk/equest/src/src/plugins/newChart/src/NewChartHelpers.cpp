#include "NewChartPCH.h"
#include <QtWidgets/QHBoxLayout>
#include "NewChartHelpers.h"
#include "QItemEditorFactory"


template <class Spin, typename T>
std::pair<QWidget*, Spin*> createSB(const QString& label, T singleStep, T minimum, T maximum)
{
	std::pair<QWidget*, Spin*> ret;
	ret.first = new QWidget();
	auto layout = new QHBoxLayout();
	layout->setContentsMargins(3, 0, 3, 0);
	layout->setSpacing(3);
	ret.first->setLayout(layout);
	QLabel* l = new QLabel(label);
	layout->addWidget(l);
	Spin* spin = new Spin();

	spin->setMaximum(maximum);
	spin->setMinimum(minimum);
	spin->setSingleStep(singleStep);
	spin->setMaximumWidth(90);
	layout->addWidget(spin);
	l->setBuddy(spin);
	ret.second = spin;
	return ret;
}

std::pair<QWidget*, QDoubleSpinBox*> LabeledSpinbox::create(const QString& label, double singleStep, double minimum, double maximum)
{
	return createSB<QDoubleSpinBox, double>(label, singleStep, minimum, maximum);
}

std::pair<QWidget*, QSpinBox*> LabeledSpinbox::create(const QString& label, int singleStep, int minimum, int maximum)
{
	return createSB<QSpinBox, int>(label, singleStep, minimum, maximum);
}

LabeledEditor::LabeledEditor(QWidget* parent, int userType) : QObject(parent), userType(userType)
{
	isEmiting = tryConnect(parent);
}

void LabeledEditor::setValue(const QVariant& val)
{
	UTILS_ASSERT(val.type() == userType);
	QByteArray name = QItemEditorFactory::defaultFactory()->valuePropertyName(val.type());
	parent()->setProperty(name, val);
}

QVariant LabeledEditor::getValue()
{
	QByteArray name = QItemEditorFactory::defaultFactory()->valuePropertyName(userType);
	return parent()->property(name);
}


bool LabeledEditor::isEmitingSignals() const
{
	return isEmiting;
}

std::tuple<QWidget*, QLabel*, QWidget*, LabeledEditor*> LabeledEditor::create(const QString& labelText, const QVariant& defaultValue, QWidget* parent)
{
	QWidget* mainWidget = new QWidget(parent);
	auto layout = new QHBoxLayout();
	layout->setContentsMargins(3, 0, 3, 0);
	layout->setSpacing(3);
	mainWidget->setLayout(layout);
	QLabel* label = new QLabel(labelText, mainWidget);
	QWidget* widget = QItemEditorFactory::defaultFactory()->createEditor(defaultValue.type(), mainWidget);
	layout->addWidget(label);
	layout->addWidget(widget);
	LabeledEditor* editor = new LabeledEditor(widget, defaultValue.type());
	editor->setValue(defaultValue);
	return std::make_tuple(mainWidget, label, widget, editor);
}

bool LabeledEditor::tryConnect(QWidget* parent)
{
	// TODO : niestety nie udalo mi sie znalezc uniwersalnej metody, ktora umozliwialaby laczenie odpowiednich sygnalow
	// to rozwiazanie dziala, choc jest koszmarne
	if(connect(parent, SIGNAL(valueChanged(const QString &)), this, SIGNAL(valueChanged()))) {
		return true;
	}
	if (connect(parent, SIGNAL(currentTextChanged(const QString &)), this, SIGNAL(valueChanged()))) {
		return true;
	}
	if (connect(parent, SIGNAL(dateTimeChanged(const QDateTime &)), this, SIGNAL(valueChanged()))) {
		return true;
	}
	if (connect(parent, SIGNAL(textChanged(const QString & )), this, SIGNAL(valueChanged()))) {
		return true;
	}
	return false;
}
