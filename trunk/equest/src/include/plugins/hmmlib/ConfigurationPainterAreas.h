/********************************************************************
	created:	2012/06/05
	created:	5:6:2012   8:57
	filename: 	ConfigurationPainterAreas.h
	author:		Wojciech Kniec

	purpose:
*********************************************************************/

#ifndef HEADER_GUARD_HMM__CONFIGURATIONPAINTERAREAS_H__
#define HEADER_GUARD_HMM__CONFIGURATIONPAINTERAREAS_H__

#include <QtCore/QString>
#include <QtGui/QPainter>
#include <utils/SmartPtr.h>
#include <plugins/hmmlib/Export.h>

//! okresla obszar na konfiguratorze graficznym
class HMMLIB_EXPORT IArea
{
public:
    IArea();
    virtual ~IArea() {}

public:
    //! \return pozycja Y rysowanego obszaru
    virtual int getY() const = 0;
    //! \param val ustawia pozycję Y rysowanego obszaru
    virtual void setY(int val) = 0;
    //! \return pozycja X rysowanego obszaru
    virtual int getX() const = 0;
    //! \param val ustawia pozycję X rysowanego obszaru
    virtual void setX(int val) = 0;
    //! \return szerokość obszaru
    virtual int getWidth() const = 0;
    //! \return wysokość obszaru
    virtual int getHeight() const = 0;
    //! \return nazwa przypisana do obszaru
    virtual const QString& getName() const = 0;
    //! pozwala odrysować się obiektowi
    //! \param painter obiekt paintera
    //! \param selected czy obiekt jest zaznaczony
    virtual void draw(QPainter& painter, bool selected) = 0;
    //! czy podany punkt znajduje się wewnątrz obszaru
    //! \param x współrzędna x
    //! \param y współrzędna y
    virtual bool isInside(int x, int y) const;
    //! czy obszar jest aktywny (normalnie - czy kliknięto w niego myszka)
    bool isActive() const { return active; }
    //! ustawia aktywność obszaru (norm. po kliknieciu myszka)
    //! \param val
    void setActive(bool val) { active = val; }
    //! \return skala obszaru
    float getScale() const { return scale; }
    //! ustawia skalę rysowanego obszaru, nie może być równa 0
    //! \param val nowa skala
    void setScale(float val) { scale = val; }

private:
    //! czy obszar jest aktywny (normalnie - czy kliknięto w niego myszka)
    bool active;
    //! skala obszaru
    float scale;
};
typedef utils::shared_ptr<IArea> IAreaPtr;
typedef utils::shared_ptr<const IArea> IAreaConstPtr;

//! zwykly punkt na konfiguratorze
//! rysowany jako polprzezroczyste kolo
class SingleMarker : public IArea
{
public:
    //! Konstruktor
    //! \param name nazwa punktu
    //! \param posX pozycja x
    //! \param posY pozycja y
    SingleMarker(const QString& name, int posX, int posY);
    //! Konstruktor
    //! \param name nazwa punktu
    //! \param posX pozycja x
    //! \param posY pozycja y
    //! \param r promień rysowanego kola
    SingleMarker(const QString& name, int posX, int posY, int r);

public:
    //! \return pozycja Y rysowanego markera (środek - promień)
    virtual int getY() const { return static_cast<int>(getScale() * ( posY - r)); }
    //! ustawia pozycję Y rysowanego markera
    //! \param val nowa pozycja
    virtual void setY(int val) { posY = val + r; }
    //! \return pozycja Y rysowanego markera
    virtual int getX() const { return static_cast<int>(getScale() * (posX - r)); }
    //! ustawia pozycję X rysowanego markera  (środek - promień)
    //! \param val nowa pozycja
    virtual void setX(int val) { posX = val + r; }
    //! \return szerokość rysowanego markera (2 x promień)
    virtual int getWidth() const { return static_cast<int>(getScale() * (2 * r)); }
    //! \return wysokość rysowanego markera (2 x promień)
    virtual int getHeight() const { return static_cast<int>(getScale() * (2 * r)); }
    //! \return nazwa markera
    virtual const QString& getName() const { return name; }
    //! pozwala odrysować się obiektowi
    //! \param painter obiekt paintera
    //! \param selected czy marker jest zaznaczony przez myszkę
    virtual void draw(QPainter& painter, bool selected);

private:
    //! Inicjalizacja markera
    //! \param name nazwa markera
    //! \param posX współrzędna X środka markera
    //! \param posY współrzędna Y środka markera
    //! \param r promień markera
    void init (const QString& name, int posX, int posY, int r);

private:
    //! pozycja x
    int posX;
    //! pozycja y
    int posY;
    //! promień odrysowywanego kola
    int r;
    //! nazwa obiektu
    QString name;
};
typedef utils::shared_ptr<SingleMarker> SingleMarkerPtr;
typedef utils::shared_ptr<const SingleMarker> SingleMarkerConstPtr;

typedef utils::shared_ptr<QImage> QImagePtr;
typedef utils::shared_ptr<const QImage> QImageConstPtr;

//! reprezentuje obrazek na konfiguratorze
class SinglePicture : public IArea
{
public:
    //! Konstruktor
    //! \param name nazwa obrazka do wczytania
    //! \param posX pozycja x
    //! \param posY pozycja y
    //! \param alwaysVisible czy obrazek ma być zawsze widoczny
    SinglePicture(const QString& name, int x = 0, int y = 0, bool alwaysVisible = false);
    //! Konstruktor
    //! \param name nazwa obszaru
    //! \param pixmap wyświetlany obrazek
    //! \param x pozycja x
    //! \param y pozycja y
    //! \param alwaysVisible czy obrazek ma być zawsze widoczny
    SinglePicture(const QString& name, const QImagePtr& pixmap, int x = 0, int y = 0, bool alwaysVisible = false);

public:
    //! \return współrzędna Y rysowanego obrazka
    virtual int getY() const { return getScale() * (y); }
    //! ustawia współrzędną Y rysowanego obrazka
    //! \param val nowa współrzędna
    virtual void setY(int val) { y = val; }
    //! \return współrzędna X rysowanego obrazka
    virtual int getX() const { return getScale() * (x); }
    //! ustawia współrzędną Y rysowanego obrazka
    //! \param val nowa współrzędna
    virtual void setX(int val) { x = val; }
    //! \return szerokość obrazka
    virtual int getWidth() const { return getScale() * (pixmap->width()); }
    //! \return wysokość obrazka
    virtual int getHeight() const { return getScale() * (pixmap->height()); }
    //! \return nazwa wyświetlana wraz z obrazkiem
    virtual const QString& getName() const { return name; }
    //! pozwala odrysować się obiektowi
    //! \param painter obiekt paintera
    //! \param selected czy marker jest zaznaczony przez myszkę
    virtual void draw(QPainter& painter, bool selected);
    //! czy podany punkt znajduje się wewnątrz obszaru
    //! \param x współrzędna x
    //! \param y współrzędna y
    virtual bool isInside(int x, int y) const;

public:
    //! \return wyświetlany obrazek
    QImageConstPtr getPixmap() const { return pixmap; }

public:
    //! ustawia nowa wartość przezroczystości obrazka
    //! \param pixmap obrazek, którego alpha się zmienia
    //! \param alpha nowa wartość przezroczystości
    static void setPixmapAlpha(QImage& pixmap, unsigned int alpha);
    
private:
    //! nazwa obszaru
    QString name;
    //! wyświetlany obrazek
    QImagePtr pixmap;
    //! kopia obrazka ze zmieniona alpha (wyświetlane, gdy zaznacza się obszar)
    QImagePtr alphaPixmap;
    //! pozycja x
    int x;
    //! pozycja y
    int y;
    //! czy obrazek ma być zawsze widoczny
    bool alwaysVisible;
};
typedef utils::shared_ptr<SinglePicture> SinglePicturePtr;
typedef utils::shared_ptr<const SinglePicture> SinglePictureConstPtr;

#endif
