/********************************************************************
	created:	2013/10/30
	created:	30:10:2013   20:07
	filename: 	WheelGraphicsView.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_COREUI__WHEELGRAPHICSVIEW_H__
#define HEADER_GUARD_COREUI__WHEELGRAPHICSVIEW_H__


#include <QtWidgets/QGraphicsView> 
#include <coreui/Export.h>

class QWheelEvent;
class QGraphicsScene;

namespace coreUI {
    
//! Klasa obrazuj¹c¹ scenê GraphicsView. Dodaje obs³ugê zoomu (na scroll myszki)
class COREUI_EXPORT WheelGraphicsView : public QGraphicsView
{
    Q_OBJECT;
public:
    //! Konstruktor
    //! \param scene 
    WheelGraphicsView(QGraphicsScene* scene, Qt::KeyboardModifier modifier = Qt::NoModifier);

    //! \return aktualnie ustawiony modyfikator (np. dodatkowy CTRL potrzebny do zoomowania)
    Qt::KeyboardModifier getModifier() const { return modifier; }
    //! ustawia modyfikator (np. dodatkowy CTRL potrzebny do zoomowania) 
    void setModifier(Qt::KeyboardModifier val) { modifier = val; }

protected:
#ifndef QT_NO_WHEELEVENT
    //! Obs³uga scrolla myszy, zmienia skalê widoku
    //! \param event 
    virtual void wheelEvent(QWheelEvent *event);
#endif

private:
    //! aktualna skala widoku sceny
    qreal viewScale;
    //! Modyfikator, czyli np. dodatkowy CTRL potrzebny, aby zoomowac widok
    Qt::KeyboardModifier modifier;
};

}

#endif
