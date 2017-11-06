/********************************************************************
	created:	2011/10/18
	created:	18:10:2011   8:55
	filename: 	ConfigurationDialog.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_HMM__CONFIGURATIONDIALOG_H__
#define HEADER_GUARD_HMM__CONFIGURATIONDIALOG_H__

#include <QtWidgets/QDialog>
#include <QtCore/QFileInfo>
#include <QtWidgets/QHBoxLayout>
#include "ConfigurationPainter.h"
#include <plugins/hmmlib/Export.h>

namespace Ui {
    class FilterConfiguratorWidget;
}

//! Widget z konfiguratorem graficznym
class HMMLIB_EXPORT ConfigurationWidget : public QWidget//, public Ui::FilterConfiguratorWidget
{
    Q_OBJECT;
public:
    ConfigurationWidget(QWidget* parent);

public:
    //! ustawia opis konfiguratora
    //! \param text ustawiany tekst
    void setText(const QString& text);
    //! wczytuje pliki z rozmieszczeniem obszarów konfiguratora
    //! \param frontXml plik z obszarami dla przodu postaci
    //! \param backXml plik z obszarami dla tylu postaci
    //! \param names mapa [nazwa elementu - > nazwa do wyświetlenia]
    void loadConfigurations(const QString& frontXml, const QString& backXml, const  std::map<QString, std::pair<QString, QString>>& names);
    //! Ustawia widoczność elementów w konfiguratorze
    //! \param visibles mapa [nazwa -> czy element ma być widoczny]
    void setVisibles(const std::map<QString, bool>& visibles);

Q_SIGNALS:
    //! kliknięto w element
    //! \param name nazwa elementu
    //! \param  
    void itemSelected(const QString& name, bool);
    //! Element został najechany myszką
    //! \param name nazwa elementu
    //! \param selected 
    void elementHovered(const QString& name, bool selected);

private Q_SLOTS:
    //! element został najechany przez myszkę
    //! \param name nazwa elementu
    //! \param selected
    void onElementHovered(const QString& name, bool selected);
    //! kliknięto w element
    //! \param name nazwa elementu
    //! \param selected 
    void onItemSelected(const QString& name, bool selected);

private:   
    //! wczytuje wszystkie pliki z listy i dodaje je do paintera (pozycja 0,0)
    //! \param painter tam trafia wczytywane obrazki
    //! \param files pliki do wczytania
    void loadPictures(ConfigurationPainter& painter, const QStringList &files );
    //! Wczytuje pojedynczy obrazek i dodaje go do paintera
    //! \param painter tam trafi wczytywany obrazek
    //! \param name nazwa pliku do wczytania
    //! \param x pozycja x
    //! \param y pozycja y
    //! \param alwaysVisible czy obrazek ma być widoczny zawsze
    void loadPicture(ConfigurationPainter& painter, const QString& name, int x, int y, bool alwaysVisible = false);
    //! laduje plik xml z zapisanymi obszarami w postaci markerów lub obrazków
    //! \param painter konfigurowany painter, tam trafia wczytane obszary
    //! \param filename ścieżka do pliku
    void loadXml(ConfigurationPainter& painter, const QString& filename);
    //! Tworzy pojedynczy marker i dodaje go do paintera
    //! \param painter tam trafi tworzony marker
    //! \param name nazwa markera
    //! \param x pozycja x
    //! \param y pozycja y
    void createMarker(ConfigurationPainter &painter, const QString& name, int x, int y);
    //! pokazuje przód postaci
    void showFront();
    //! pokazuje tył postaci
    void showBack();
    //! Ustawia obrazek z tłem
    //! \param painter modyfikowany painter
    //! \param name nazwa widoku
    //! \param pixmap obrazek tła
    void setBackground(ConfigurationPainter& painter, const QString& name, QImageConstPtr pixmap);

private:
    //! painter, który rysuje przód podstaci
    ConfigurationPainter painterFront;
    //! painter, który rysuje tył podstaci
    ConfigurationPainter painterBack;
    //! aktualny painter
    ConfigurationPainter* currentPainter;
    //! layout ze scrollem (jeśli widget nie mieści się w wyznaczonym miejscu)
    QHBoxLayout scrollLayout;
    //! czy konfigurator aktualnie pokazuje przód postaci
    bool isFront;
    Ui::FilterConfiguratorWidget* ui;
};

#endif
