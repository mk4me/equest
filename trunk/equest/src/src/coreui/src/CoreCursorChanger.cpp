#include "CoreUiPCH.h"
#include <coreui/CoreCursorChanger.h>
#include <QtWidgets/QApplication>

using namespace coreUI;

CoreCursorChanger::CoreCursorChanger(const Qt::CursorShape cursorShape) : restored(false)
{
	QApplication::setOverrideCursor(cursorShape);
}

CoreCursorChanger::CoreCursorChanger(const QCursor & cursor) : restored(false)
{
	QApplication::setOverrideCursor(cursor);
}

CoreCursorChanger::~CoreCursorChanger()
{
	restore();
}

void CoreCursorChanger::restore()
{
	if (restored == false){
		QApplication::restoreOverrideCursor();
		restored = true;
	}
}