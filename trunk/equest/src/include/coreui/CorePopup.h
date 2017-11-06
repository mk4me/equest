/********************************************************************
	created:	2014/01/16
	created:	16:1:2014   23:45
	filename: 	CorePopup.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_COREUI__COREPOPUP_H__
#define HEADER_GUARD_COREUI__COREPOPUP_H__

namespace coreUI {

#include <coreui/Export.h>

class COREUI_EXPORT CorePopup 
{
public:
    static void showMessage(const QString& title, const QString& message, int ms, const QRect& rect);
    static void showMessage(const QString& title, const QString& message, int ms = 3000);
};

}
#endif
