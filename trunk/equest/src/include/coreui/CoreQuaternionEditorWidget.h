/********************************************************************
	created:  2014/12/29	14:48:16
	filename: CoreQuaternionEditorWidget.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_COREUI__COREQUATERNIONEDITORWIDGET_H__
#define __HEADER_GUARD_COREUI__COREQUATERNIONEDITORWIDGET_H__

#include <coreui/Export.h>
#include <coreui/CoreQuaternionValidator.h>
#include <QtWidgets/QWidget>
#include <QtCore/QPointer>
#include <osg/Quat>
#include <coreui/CoreQtMetaTypes.h>

namespace Ui
{
	class CoreQuaternionEditorWidget;
}

namespace coreUI
{
	class COREUI_EXPORT CoreQuaternionEditorWidget : public QWidget
	{
		Q_OBJECT

	public:

		CoreQuaternionEditorWidget(QWidget * parent = nullptr);

		virtual ~CoreQuaternionEditorWidget();

		osg::Quat value() const;

		void setValidator(const CoreQuaternionValidator * validator);

		const CoreQuaternionValidator * validator() const;

		void setRange(const double minVal, const double maxVal);

	public slots:

		void setValue(const osg::Quat & value);

	private slots:

		void updateValue();

	signals:

		void valueChanged(const osg::Quat & value);

	private:

		void updateValue(const osg::Quat & value);

	private:

		Ui::CoreQuaternionEditorWidget * ui;
		osg::Quat value_;
		QPointer<CoreQuaternionValidator> validator_;
	};
}

#endif	// __HEADER_GUARD_COREUI__COREQUATERNIONEDITORWIDGET_H__