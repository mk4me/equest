/********************************************************************
	created:  2016/07/27
	filename: CoreTimeStepEditorWidget.h
	author:	  Mateusz Janiak
	
	purpose:  Klasa realizuj¹ca edycjê d³ugoœci czasu na potrzeby interwa³ów
				Jej rozdzielczoœæ to mikrosekundy
*********************************************************************/
#ifndef __HEADER_GUARD_COREUI__CORETIMESTEPEDITORWIDGET_H__
#define __HEADER_GUARD_COREUI__CORETIMESTEPEDITORWIDGET_H__

#include <coreui/Export.h>
#include <coreui/CoreQuaternionValidator.h>
#include <QtWidgets/QWidget>
#include <QtCore/QPointer>
#include <osg/Quat>
#include <coreui/CoreQtMetaTypes.h>

namespace Ui
{
	class CoreTimeStepEditorWidget;
}

namespace coreUI
{
	//! Kalsa realizuj¹ca edycjê kroku w czasie z rozdzielczoœci¹ mikrosekund
	class COREUI_EXPORT CoreTimeStepEditorWidget : public QWidget
	{
		Q_OBJECT

	public:
		//! \param value Wartoœæ startowa kroku czasu
		//! \param parent
		//! \param f
		CoreTimeStepEditorWidget(quint64 value = 0, QWidget * parent = nullptr, Qt::WindowFlags f = 0);
		
		//! \param maxValue Maksymalna wartoœæ kroku czasu
		//! \param minValue Minimalna wartoœæ kroku czasu	
		//! \param value Wartoœæ startowa kroku czasu
		//! \param parent
		//! \param f
		CoreTimeStepEditorWidget(quint64 maxVal, quint64 minVal = 0, quint64 value = 0,
			QWidget * parent = nullptr, Qt::WindowFlags f = 0);

		//! Destruktor wirtualny
		virtual ~CoreTimeStepEditorWidget();

		//! \return Aktualna wartoœæ kroku czasu
		quint64 step() const;
		
		//! \param maxValue Maksymalna wartoœæ kroku czasu
		//! \param minValue Minimalna wartoœæ kroku czasu
		void setRange(quint64 maxVal, quint64 minVal = 0);

		//! \param maxValue Maksymalna wartoœæ kroku czasu
		void setMax(quint64 maxVal);

		//! \param minValue Minimalna wartoœæ kroku czasu
		void setMin(quint64 minVal);
		//! \param step Krok czasu w mikrosekundach
		//! \param who Kto taki krok posiada
		//! \param format Format zapisu tego kroku
		void registerPredefined(quint64 step, const QString & who, const QString & format);

	public slots:
		//! \param microseconds D³ugoœæ kroku
		void setStep(quint64 microseconds);

	private slots:

		//! Zmiana d³ugoœci kroku
		void stepChanged();
		//! Zmiana predefiniowanej wartoœci kroku
		//! \param idx Aktualny element
		void predefinedChanged(int idx);
		//! Zamiana formatu prezentacji d³ugoœci kroku
		//! \param idx Aktualny format
		void formatChanged(int idx);

	signals:

		//! \param value Aktualna wartoœæ kroku czasu
		void stepChanged(quint64 value);

	private:

		//! Aktualizacja formatu prezentacji d³ugoœci kroku
		void updateFormat();

	private:

		//! Ui
		Ui::CoreTimeStepEditorWidget * ui;
		//! Aktualny krok w mikrosekundach
		quint64 step_;		
		//! Minimalna d³ugoœæ kroku w mikrosekundach
		quint64 min_;
		//! Maksymalna d³ugoœæ kroku w mikrosekundach
		quint64 max_;
	};
}

#endif  // __HEADER_GUARD_COREUI__CORETIMESTEPEDITORWIDGET_H__