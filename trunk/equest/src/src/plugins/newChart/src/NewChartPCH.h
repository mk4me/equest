/********************************************************************
	created:	2011/11/12
	created:	12:11:2011   15:09
	filename: 	ChartPCH.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_NEWCHART__CHARTPCH_H__
#define HEADER_GUARD_NEWCHART__CHARTPCH_H__

#include <limits>

#define QT_NO_QT_INCLUDE_WARN
#pragma warning( disable : 4250 )
#include <qwt_plot_canvas.h>
#include <qwt_scale_draw.h>
#include <qwt_scale_widget.h>
#include <qwt_plot_layout.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_panner.h>
#include <qwt_plot_magnifier.h>
#include <qwt_legend.h>
#include <qwt_plot_marker.h>
#include <qwt_scale_map.h>
#include <qwt_plot.h>
#include <qwt_text.h>
#include <qwt_series_data.h>
//#include <qwt_plot_grid.h>
//#include <qwt_plot_zoomer.h>
//#include <qwt_picker_machine.h>
//#include <qwt_curve_fitter.h>
//#include <qwt_plot_directpainter.h>
//#include <qwt_legend_label.h>
#pragma warning( default : 4250 )
#undef QT_NO_QT_INCLUDE_WARN

#include <QtCore/QObject>
#include <QtCore/QEvent>
#include <QtWidgets/QAction>
#include <QtWidgets/QWidget>
#include <QtGui/QCloseEvent>
#include <utils/SmartPtr.h>

#include <corelib/IPlugin.h>
#include <loglib/Exceptions.h>

#endif
