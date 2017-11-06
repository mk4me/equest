#include "CorePCH.h"
#include "LanguageChangeFilter.h"
#include <QtCore/QCoreApplication>
#include <QtCore/QEvent>

using namespace core;

LanguageChangeFilter::LanguageChangeFilter() : languageChanged(false)
{
	//muszê zainstalowac ten filtr zeby aplikacja nie wysylala tego eventu,
	//bo przeladowujemy wiecej translatorow i nie chcemy na kazdy event tlumaczy wszystkiego od nowa
	qApp->installEventFilter(this);
}

LanguageChangeFilter::~LanguageChangeFilter()
{
	//teraz moge wylaczyc filtr
	qApp->removeEventFilter(this);

	if(languageChanged == true){
		//i sami emitujemy zmianê jesli trzeba
		QEvent ev(QEvent::LanguageChange);
		QCoreApplication::sendEvent(qApp, &ev);
	}
}

bool LanguageChangeFilter::eventFilter(QObject * object, QEvent * event)
{
	if(event->type() == QEvent::LanguageChange){
		languageChanged = true;
		return true;
	}

	return QObject::eventFilter(object, event);
}