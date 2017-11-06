/********************************************************************
    created:  2013/01/31
    created:  31:1:2013   9:14
    filename: CoreSplitableDockWidgetT.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___CORESPLITABLEDOCKWIDGETT_H__
#define HEADER_GUARD___CORESPLITABLEDOCKWIDGETT_H__

#include <coreui/CoreSplitableDockWidget.h>

namespace coreUI {

	template<class WidgetClonePolicy>
	class CoreSplitableDockWidgetT : public CoreSplitableDockWidget
	{
	public:
		explicit CoreSplitableDockWidgetT(const QString &title, QWidget *parent = 0, Qt::WindowFlags flags = 0)
			: CoreSplitableDockWidget(title, parent, flags)
		{

		}

		explicit CoreSplitableDockWidgetT(QWidget *parent = nullptr, Qt::WindowFlags flags = 0)
			: CoreSplitableDockWidget(parent, flags)
		{

		}

		virtual ~CoreSplitableDockWidgetT()
		{

		}

	private:

		virtual CoreSplitableDockWidget * clone() const
		{
			CoreSplitableDockWidgetT * newDockWidget = new CoreSplitableDockWidgetT(windowTitle(), nullptr, windowFlags());

			QWidget * currentWidget = widget();
			if(currentWidget != nullptr){
				QWidget * newWidget = WidgetClonePolicy::cloneWidget(currentWidget);
				UTILS_ASSERT(newWidget);
				newDockWidget->setWidget(newWidget);
			}

			return newDockWidget;			
		}
	};

	template<class Widget>
	class WidgetClonePolicyCopyConstructor {

		static_assert((std::is_base_of<QWidget, Widget>::value), "Widget class should inherit from QWidget");

	public:
		static QWidget * cloneWidget(QWidget * widget)
		{
			Widget * w = qobject_cast<Widget*>(widget);
			if(w == nullptr){
				return nullptr;
			}

			return new Widget(*w);
		}
	};

}

#endif	//	HEADER_GUARD___CORESPLITABLEDOCKWIDGETT_H__
