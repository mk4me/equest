/********************************************************************
	created:	2012/02/22
	created:	22:2:2012   13:14
	filename: 	NewChartHelpers.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_NEW_CHART__NEWCHARTHELPERS_H__
#define HEADER_GUARD_NEW_CHART__NEWCHARTHELPERS_H__

#include <QtWidgets/QWidget>
#include <QtWidgets/QLabel>
#include <QtWidgets/QDoubleSpinBox>

class LabeledEditor : public QObject
{
	Q_OBJECT;
public:
	LabeledEditor(QWidget* parent, int userType);

public Q_SLOTS :
	void setValue(const QVariant&);
public:
	QVariant getValue();
	bool isEmitingSignals() const;
Q_SIGNALS:
	void valueChanged();
private:
	int userType;
	bool isEmiting;

public:
	static std::tuple<QWidget*, QLabel*, QWidget*, LabeledEditor*> create(const QString& labelText, const QVariant& defaultValue, QWidget* parent = nullptr);
	bool tryConnect(QWidget* parent);
};

//! Klasa tworzy widget z dwoma potomkami : Label i SpinBox
//! \version 0.9.1
class LabeledSpinbox
{
public:
    //! Tworzy etykietowany doubleSpinbox
    //! \param parent rodzic widgeta
    //! \param label tekst etykiety
    //! \param singleStep krok dla spinboxa
    //! \param minimum minimalna wartość spinboxa
    //! \param maximum maksymalna wartość spinboxa
    //! \return krotka z utworzonymi widgetami (kontener, etykieta, spinbox)
    static std::pair<QWidget*, QDoubleSpinBox*> create(const QString& label, double singleStep, double minimum, double maximum);

	//! Tworzy etykietowany doubleSpinbox
	//! \param parent rodzic widgeta
	//! \param label tekst etykiety
	//! \param singleStep krok dla spinboxa
	//! \param minimum minimalna wartość spinboxa
	//! \param maximum maksymalna wartość spinboxa
	//! \return krotka z utworzonymi widgetami (kontener, etykieta, spinbox)
	static std::pair<QWidget*, QSpinBox*> create(const QString& label, int singleStep, int minimum, int maximum);
};


#endif
