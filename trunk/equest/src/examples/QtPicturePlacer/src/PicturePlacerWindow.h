/********************************************************************
	created:	2011/10/16
	created:	16:10:2011   17:06
	filename: 	PicturePlacerWindow.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_PICTURE_PLACER__PICTUREPLACERWINDOW_H__
#define HEADER_GUARD_PICTURE_PLACER__PICTUREPLACERWINDOW_H__

#include <list>
#include <QtCore/QString>
#include <QtGui/QMouseEvent>
#include <QtGui/QWidget>
#include <QtGui/QPainter>
#include <QtGui/QMainWindow>
#include <QtGui/QHBoxLayout>
#include <boost/smart_ptr.hpp>
#include <boost/foreach.hpp>
#include "ui_PicturePlacerWindow.h"

typedef boost::shared_ptr<QPixmap> QPixmapPtr;
typedef boost::shared_ptr<const QPixmap> QPixmapConstPtr;

class IArea
{
public:
    virtual int getY() const = 0;
    virtual void setY(int val) = 0;
    virtual int getX() const = 0;
    virtual void setX(int val) = 0;
    virtual int getWidth() const = 0;
    virtual int getHeight() const = 0;
    virtual const QString& getName() const = 0;
    virtual void draw(QPainter& painter) = 0;
};
typedef boost::shared_ptr<IArea> IAreaPtr;
typedef boost::shared_ptr<const IArea> IAreaConstPtr;

class SingleMarker : public IArea
{
public:
     SingleMarker(const QString& name, int posX, int posY)
     {
         init(name, posX, posY, 5);
     }

     SingleMarker(const QString& name, int posX, int posY, int r)
     {
         init(name, posX, posY, r);
     }

public:
    virtual int getY() const { return posY - r; }
    virtual void setY(int val) { posY = val + r; }
    virtual int getX() const { return posX - r; }
    virtual void setX(int val) { posX = val + r; }
    virtual int getWidth() const { return 2 * r; }
    virtual int getHeight() const { return 2 * r; }
    virtual const QString& getName() const { return name; }
    void setName(const QString& name) { this->name = name; }
    virtual void draw(QPainter& painter) 
    {
        QPen pen(QColor(100,255,10));
        pen.setWidth(3);
        painter.setPen(pen);
        painter.drawRect(getX(), getY(), getWidth(), getHeight());
    }

private:
    void init (const QString& name, int posX, int posY, int r)
    {
        this->name = name;
        this->posX = posX;
        this->posY = posY;
        this->r = r;
    }

private:
    int posX, posY;
    int r;
    QString name;
};
typedef boost::shared_ptr<SingleMarker> SingleMarkerPtr;
typedef boost::shared_ptr<const SingleMarker> SingleMarkerConstPtr;

class SinglePicture : public IArea
{
public:
    SinglePicture(const QString& name) :
        pixmap(new QPixmap(name)),
        name(name),
        x(0), y(0)
    {}

    SinglePicture(const QString& name, int x, int y) :
        pixmap(new QPixmap(name)),
        name(name),
        x(x), y(y)
    {}

    SinglePicture(const QString& name, const QPixmapPtr& pixmap) :
        pixmap(pixmap),
        name(name),
        x(0), y(0)
    {}

    SinglePicture(const QString& name, const QPixmapPtr& pixmap, int x, int y) :
        pixmap(pixmap),
        name(name),
        x(x), y(y)
    {}

public:
    virtual int getY() const { return y; }
    virtual void setY(int val) { y = val; }
    virtual int getX() const { return x; }
    virtual void setX(int val) { x = val; }
    virtual int getWidth() const { return pixmap->width(); }
    virtual int getHeight() const { return pixmap->height(); }
    virtual const QString& getName() const { return name; }
    virtual void draw(QPainter& painter)
    {
        painter.drawPixmap(x, y, pixmap->width(), pixmap->height(), *pixmap);
    }

public:
    QPixmapConstPtr getPixmap() const { return pixmap; }
    
    
private:
    QString name;
    QPixmapPtr pixmap;
    
    int x, y;
    
};
typedef boost::shared_ptr<SinglePicture> SinglePicturePtr;
typedef boost::shared_ptr<const SinglePicture> SinglePictureConstPtr;


class Painter : public QWidget
{
    Q_OBJECT;
public:
    Painter(QWidget* parent) :
      QWidget(parent),
      pressedX(0),
      pressedY(0),
      markerCounter(0),
      state(Normal)
      {}

public:
    typedef std::list<IAreaPtr> AreasList;
    enum State 
    {
        Normal,
        Markers
    };

public:
    QPixmapConstPtr getBackground() const { return background; }
    const QString& getName() const { return name; }
    void setBackground(const QString& name, QPixmapConstPtr val) { this->name = name; background = val; }
    void addArea(IAreaPtr data);
    void removeArea(const QString& name);
    AreasList::const_iterator getBegin() const { return areas.cbegin(); }
    AreasList::const_iterator getEnd() const { return areas.cend(); }
    Painter::State getState() const { return state; }
    void setState(Painter::State val) { state = val; }

    IAreaPtr getActiveArea() const { return activeArea; }
    void setActiveArea(IAreaPtr val) { activeArea = val; repaint(rect()); }

signals:
    void markerAdded(SingleMarkerPtr);

protected:
    virtual void mouseMoveEvent ( QMouseEvent * event );
    virtual void mousePressEvent ( QMouseEvent * event );
    virtual void mouseReleaseEvent ( QMouseEvent * event );
    virtual void paintEvent(QPaintEvent *);

private:
    IAreaPtr getArea(int x, int y);
    

private:
    QPixmapConstPtr background;
    QString name;
    AreasList areas;
    IAreaPtr currentArea;
    int pressedX;
    int pressedY;
    int markerCounter;
    State state;
    IAreaPtr activeArea;
};



class PicturePlacerWindow : public QMainWindow, public Ui::PicturePlacerWindow
{
    Q_OBJECT;
public:
    
public:
    PicturePlacerWindow();

public:
    void setBackground(const QString& name, QPixmapConstPtr pixmap);

public slots:
    void showAbout();
    void loadBackground();
    void loadPictures();
    void saveProject();
    void loadProject();
    void power();
    void undo();
    void redo();
    void setNormal();
    void setMarkers();
    void markerAdded(SingleMarkerPtr marker);
    void markerNameChanged(QListWidgetItem * current);
    void currentItemChanged(QListWidgetItem * current, QListWidgetItem * previous);
private:    
    void loadPictures(const QStringList &files );

    void loadPicture( const QString& name, int x, int y);

    void saveXml(const QString& filename, const Painter& painter );
    void loadXml(const QString& filename);

    void addMarker( SingleMarkerPtr marker );

private:
    Painter painter;
    QHBoxLayout scrollLayout;    
    std::map<QListWidgetItem*, IAreaPtr> item2Area;
};

#endif 
