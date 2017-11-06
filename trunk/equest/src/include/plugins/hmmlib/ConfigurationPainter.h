/********************************************************************
	created:	2011/10/18
	created:	18:10:2011   8:58
	filename: 	ConfigurationPainter.h
	author:		Wojciech Kniec

	purpose:
*********************************************************************/

#ifndef HEADER_GUARD_HMM__CONFIGURATIONPAINTER_H__
#define HEADER_GUARD_HMM__CONFIGURATIONPAINTER_H__

#include <QtCore/QFileInfo>
#include <map>
#include <list>
#include <QtWidgets/QWidget>
#include <QtGui/QPen>
#include <QtGui/QPainter>
#include "ConfigurationPainterAreas.h"
#include <plugins/hmmlib/Export.h>

//! klasa odpowiada za wyświetlenie graficznego konfiguratora
class HMMLIB_EXPORT ConfigurationPainter : public QWidget
{
    Q_OBJECT;
public:
    ConfigurationPainter(QWidget* parent);

public:
    //! lista rysowanych obszarów
    typedef std::list<IAreaPtr> AreasList;
    //! 
    typedef std::map<QString, std::pair<QString, QString>> NamesDictionary;
public:
    //! \return obrazek z tłem
    QImageConstPtr getBackground() const { return background; }
    //! \return nazwa paintera
    const QString& getName() const { return name; }
    //! ustawia obrazek z tłem
    //! \param name nazwa obrazka
    //! \param val pixmapa z obrazkiem
    void setBackground(const QString& name, const QImageConstPtr & val) { this->name = name; background = val; }
    //! dodaje nowy obszar do paintera (marker lub obrazek)
    //! \param data dodawany obszar
    void addArea(const IAreaPtr & data);
    //! usuwa obszar (marker lub obrazek) z paintera
    //! \param name nazwa obszaru
    void removeArea(const QString& name);
    //! \return niemodyfikowalny iterator wskazujący początek kolekcji obszarów
    AreasList::const_iterator begin() const { return areas.cbegin(); }
    //! \return niemodyfikowalny iterator wskazujący koniec kolekcji obszarów
    AreasList::const_iterator end() const { return areas.cend(); }
    //! \return iterator wskazujący początek kolekcji obszarów
    AreasList::iterator begin() { return areas.begin(); }
    //! \return iterator wskazujący koniec kolekcji obszarów
    AreasList::iterator end() { return areas.end(); }
    //! jeśli obszar o danej nazwie jest w painterze, to staje się lub przestaje być aktywny
    //! \param name nazwa elementu
    //! \param selected czy ma być lub przestać być aktywny
    void trySetActive( const QString& name, bool selected );
    //! Iloczyn zbiorów nazw dostarczonych przez konfigurator i nazw obsługiwanych (nazw dostępnych obszarów)
    //! \param names zbior do porownania (brany jest pod uwage tylko pierwszy element wpisu mapy)
    void intersectNames( const NamesDictionary& names );
    //! \return globalna skala dla paintera
    float getScale() const { return scale; }
    //! ustawia globalna skale dla paintera
    //! \param val nowa skala
    void setScale(float val);

Q_SIGNALS:
    //! obszar został najechany myszką
    //! \param name nazwa obszaru
    //! \param selected czy został lub przestał być zaznaczony
    void elementHovered(const QString& name, bool selected);
    //! obszar został kliknięty myszką
    //! \param name nazwa obszaru
    //! \param selected czy stał się lub przestał być aktywny
    void elementSelected(const QString& name, bool selected);

protected:
    //! obsługa ruchu myszki
    //! \param event
    virtual void mouseMoveEvent ( QMouseEvent * event );
    //! obsługa naciśnięcia przycisku myszki
    //! \param event
    virtual void mousePressEvent ( QMouseEvent * event );
    //! obsługa zwolnienia przycisku myszki
    //! \param event
    virtual void mouseReleaseEvent ( QMouseEvent * event );
    //! odrysowanie konfiguratora
    //! \param *
    virtual void paintEvent(QPaintEvent *);
    //! filtruje zdarzenia związane z ruchem myszki
    //! \param obj
    //! \param event
    bool eventFilter(QObject *obj, QEvent *event);

private:
    //! pobiera arene na podstawie współrzędnych (w przypadku obrazków uwzględniana jest tez alpha)
    //! \param x współrzędna x
    //! \param y współrzędna y
    //! \return obszar wystepujący w punkcie lub null jeśli takiego nie ma. Jeśli wystepuje kilka takich obszarów to zwracany jest najbliższy
    IAreaPtr getArea(int x, int y);

private:
    //! tło paintera
    QImageConstPtr background;
    //! nazwa paintera
    QString name;
    //! lista obszarów w painterze
    AreasList areas;
    //! element aktualnie najechany myszka
    IAreaPtr currentArea;
    //! globalna skala paintera
    float scale;
};


#endif
