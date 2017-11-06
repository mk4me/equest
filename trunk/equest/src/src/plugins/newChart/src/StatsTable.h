/********************************************************************
	created:	2011/12/06
	created:	6:12:2011   10:34
	filename: 	StatsTable.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_NEW_CHART__STATSTABLE_H__
#define HEADER_GUARD_NEW_CHART__STATSTABLE_H__

#include "ui_statsTable.h"
#include <boost/range.hpp>
#include <plugins/c3d/C3DChannels.h>

#include <QtWidgets/QItemDelegate>

//! klasa powstała, aby wymusic wielkość wpisów, dzięki temu można łatwo obliczyć potrzebna wysokość
class ItemDelegate : public QItemDelegate
{
public:
    //! Konstruktor wymagający podania rozmiaru
    //! \param width szerokość 
    //! \param height wysokość
    ItemDelegate(int width, int height) : width(width), height(height) {}
    //! metoda wykorzystywana przez Qt przy ustalaniu wielkości elementów
    QSize sizeHint ( const QStyleOptionViewItem & option, const QModelIndex & index ) const
    {
        return QSize(width, height);
    }

    //! 
    int getWidth() const { return width; }
    //! 
    //! \param val 
    void setWidth(int val) { width = val; }
    //! 
    int getHeight() const { return height; }
    //! 
    //! \param val 
    void setHeight(int val) { height = val; }

private:
    //! szerokość
    int width;
    //! wysokość
    int height;
};

//! wyświetla statystyki wykresow w postaci pogrupowanego drzewa wpisów.
//! Widget normalnie jest zwiniety, rozwijanie nastepuje po nacisnieciu przycisku.
class StatsTable : public QWidget, private Ui::StatsTable
{
    Q_OBJECT;
public:
	typedef std::multimap<c3dlib::ScalarChannelReaderInterfaceConstPtr, QTreeWidgetItem*> statsMultimap;
    typedef boost::iterator_range<statsMultimap::iterator> range;
    typedef boost::iterator_range<statsMultimap::const_iterator> const_range;	

public:
    StatsTable(QWidget* parent = nullptr, Qt::WindowFlags f = 0);
	virtual ~StatsTable() {}

public slots:
    //! Dodanie wpisu ze statystykami
    //! \param group nazwa grupy (np. strona lewa, prawa ...), jeśli grupa nie istnieje, to zostanie stworzona, jeśli "" to grupa jest pominięta.
    //! \param name nazwa wpisu 
    //! \param stats obiekt ze statystykami
    //! \param backgroundColor kolor tła dla dodawanego elementu
    //! \return utworzony element drzwa
QTreeWidgetItem* addEntry(const QString& group, const QString& name, c3dlib::ScalarChannelReaderInterfaceConstPtr channel, const QColor& backgroundColor = Qt::white);
    //! próbuje pobrać wpis ze statystykami
    //! \param group nazwa grupy (np. strona lewa, prawa ...)
    //! \param name nazwa wpisu 
    //! \return element drzewa z wpisem lub nullptr
    QTreeWidgetItem* tryGetEntry(const QString& group, const QString& name);
    //! Pobiera wszystkie wpisy związane z konkretynymi statystykami
    //! \param stats statystyki, dla których maja być pobrane wpisy
    //! \return obiekt boost::range z wpisami
	range getEntries(c3dlib::ScalarChannelReaderInterfaceConstPtr channel);    
    //! \return wszytskie grupy wpisów (np. strona lewa, prawa ...)
    QStringList getGroups() const;
    //! dany element staje się aktywny (rozwiniety, pozostałe na jego poziome są zwijane)
    //! \param treeItem obiekt, który ma stac się aktywny
    void setActive(QTreeWidgetItem* treeItem);
    //! dany element staje się aktywny (rozwiniety, pozostałe na jego poziome są zwijane)
    //! \param group nazwa grupy (np. strona lewa, prawa ...)
    //! \param name nazwa wpisu 
    void setActive(const QString& group, const QString& name);
    //! czysci / resetuje obiekt
    void clear();
    //! oblicza na nowo wymagana wysokość
    void recalculateHeight();
    
private:
    //! wysokość wiersza
    int rowHeight;
    //! mapa statystyki -> wpis z drzewie ze statystykami
	statsMultimap stats2TreeItems;
    ItemDelegate treeItemDelegate;
};
typedef utils::shared_ptr<StatsTable> StatsTablePtr;
typedef utils::shared_ptr<const StatsTable> StatsTableConstPtr;


#endif
