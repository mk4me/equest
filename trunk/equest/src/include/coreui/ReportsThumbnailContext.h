/********************************************************************
	created:	2013/06/03
	created:	3:6:2013   12:08
	filename: 	ReportsThumbnailContext.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_COREUI__REPORTSTHUMBNAILCONTEXT_H__
#define HEADER_GUARD_COREUI__REPORTSTHUMBNAILCONTEXT_H__

class QPlainTextEdit;
class QComboBox;
class QTabWidget;

#include <QtCore/QObject>
#include <coreui/IAppUsageContext.h>
#include <coreui/Export.h>

namespace coreUI {
//! kontekst dla zak³adki z miniaturkami obrazów do raportu
class COREUI_EXPORT ReportsThumbnailContext : public QObject, public coreUI::IAppUsageContext
{
    Q_OBJECT;
public:
    //! konstruktor
    //! \param flexiTabWidget flexi widget kontekstu
    //! \param hmm g³ówne okno aplikacji
    ReportsThumbnailContext(QTabWidget * flexiTabWidget, QWidget* reportsTab);
public:
    //! wywo³ywane, po aktywowaniu siê kontekstu, do flexi bara trafi¹ akcje zwi¹zane z tworzeniem raportów
    //! \param contextWidget wiget, który wywo³a³ kontekst
    virtual void activateContext(QWidget * contextWidget);
    //! wywo³ywane, gdy kontekst przestaje byæ aktywny
    //! z flexibara znikaj¹ akcje zwi¹zane z tworzeniem raportów
    virtual void deactivateContext(QWidget * nextContextWidget, bool refresh);
    //! rejestracja widgeta z miniaturkami raportów
    virtual void onRegisterContextWidget(QWidget * contextWidget);
    //! wyrejstrowanie widgeta z miniaturkami raportów
    virtual void onUnregisterContextWidget(QWidget * contextWidget);

public:
    //! Utworzenie raportu na podstawie szablonu i stylu, z do³¹czonymi obrazami
    static QString createRaport(const QWidget* thumbs, const QString& projectName, const QString& templateReport = QString(), const QString& css = QString());

Q_SIGNALS:
    //! Sygna³ generowany, gdy u¿ytkownik klikn¹³ przycisk 'utwórz raport'
    //! \param str Wygenerowany raport (w HTML)
    void reportCreated(const QString& str);

private:
    //! flexi widget
    QTabWidget * flexiTabWidget;
    //! sekcja obs³ugiwanego flexi widgeta
    QWidget* flexiSection;
    //! identyfikator grupy dla miniatur raportów
    int groupID;
    //! nazwa tworzonego raportu
    QPlainTextEdit* projectName;
    //! kombo box, z szablonami raportów
    QComboBox* projectTemplate;
    //! kombo box ze stylami raportów
    QComboBox* cssCombo;
    //! miejsce, gdzie trafiaja miniaturki
    QWidget* reportsTab;
};
DEFINE_SMART_POINTERS(ReportsThumbnailContext);
}
#endif