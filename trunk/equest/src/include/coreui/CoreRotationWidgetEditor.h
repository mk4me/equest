/********************************************************************
	created:  2014/12/29	18:06:20
	filename: CoreRotationWidgetEditor.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_COREUI__COREROTATIONWIDGETEDITOR_H__
#define __HEADER_GUARD_COREUI__COREROTATIONWIDGETEDITOR_H__

#include <coreui/Export.h>
#include <QtWidgets/QWidget>
#include <osg/Quat>
#include <coreui/CoreQtMetaTypes.h>

class QLabel;

namespace coreUI
{
	class CoreEulerAnglesEditorWidget;
	class CoreQuaternionEditorWidget;
	class CoreUnitQuaternionValidator;

	class COREUI_EXPORT CoreRotationWidgetEditor : public QWidget
	{
		Q_OBJECT

	public:

		CoreRotationWidgetEditor(QWidget * parent = nullptr);

		virtual ~CoreRotationWidgetEditor();

		osg::Quat value() const;

	public slots:

		void setValue(const osg::Quat & orientation);

		void setDataInterpretation(const bool orientation);
	
	private slots:

		void onValueChanged(const osg::Quat & orientation);

	signals:

		void valueChanged(const osg::Quat & orientation);

	private:
		bool dataIsOrientation;
		QLabel * anglesNamelabel;
		CoreEulerAnglesEditorWidget * eulerAngles;
		CoreQuaternionEditorWidget * quaternion;
		CoreUnitQuaternionValidator * validator;
	};
}

#endif	// __HEADER_GUARD_COREUI__COREROTATIONWIDGETEDITOR_H__