/********************************************************************
	created:  2014/12/29	10:43:42
	filename: CoreEulerAnglesEditorWidget.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_COREUI__COREEULERANGLESEDITORWIDGET_H__
#define __HEADER_GUARD_COREUI__COREEULERANGLESEDITORWIDGET_H__

#include <coreui/Export.h>
#include <QtWidgets/QWidget>
#include <osg/Vec3d>
#include <osg/Quat>
#include <kinematicUtils/Types.h>
#include <coreui/CoreQtMetaTypes.h>

namespace Ui
{
	class CoreEulerAnglesEditorWidget;
}

namespace coreUI
{
	class COREUI_EXPORT CoreEulerAnglesEditorWidget : public QWidget
	{
		Q_OBJECT

	public:

		CoreEulerAnglesEditorWidget(QWidget * parent = nullptr);

		virtual ~CoreEulerAnglesEditorWidget();

		osg::Quat value() const;		

	public slots:

		void setValue(const osg::Quat & orientation);

		void setAngleRepresentation(const bool radians);

		void setConvention(const int axisOrder);

	private slots:

		void updateValue();

		void changeAngleRepresentation(const int idx);

		void changeConvention(const int idx);

	signals:

		void valueChanged(const osg::Quat & orientation);

	private:

		void updateValue(const osg::Vec3d & rotAngles);

		void updateAngleRepresentation(const bool radians);

		void updateConvention(const kinematicUtils::AxisOrder::Type axisOrder);

	private:

		bool anglesRadiansRepresentation;

		kinematicUtils::AxisOrder::Type anglesConvention;

		Ui::CoreEulerAnglesEditorWidget * ui;

		osg::Quat orientation;
		osg::Vec3d values;
	};
}

#endif	// __HEADER_GUARD_COREUI__COREEULERANGLESEDITORWIDGET_H__