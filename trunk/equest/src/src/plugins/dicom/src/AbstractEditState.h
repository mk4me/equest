/********************************************************************
	created:	2015/10/06
	created:	13:28
	filename: 	AbstractEditState.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_DICOM__AbstractEditState_H__
#define HEADER_GUARD_DICOM__AbstractEditState_H__


#include <coreui/AbstractState.h>
#include "PointsLayer.h"
#include <utils/ICommand.h>
#include <coreui/MoveCommand.h>
namespace dicom {

    //! Polecenie dodaje punkt do obszaru
    class AddPointCommand : public utils::ICommand
    {
    public:
        //! Konstruktor
        //! \param item Element graficzny sceny, które zostanie przesuniêty 
        //! \param newP Nowa pozycja elementu 
        //! \param oldP Stara pozycja elementu
        AddPointCommand(PointsLayerPtr layer, const QPointF& newP, bool bestFit);

    public:
        //! Przesuwa element
        virtual void doIt();

        //! Cofa przesuniêcie
        virtual void undoIt();
        //! Nazwa polecenia 
        virtual QString name();
        int getBestIdx();
    private:
        //! Modyfikowany obszar
        PointsLayerPtr layer;
        //! Nowododany punkt
        QPointF newP;
        //! indeks punktu do usuniêcia
        int idx;
        /// usuniêty punkt, który mo¿e zostaæ przywrócony. Przywracaj¹c nie mo¿na stworzyæ nowej instancji
        /// gdy¿ mog³oby to spowodowaæ b³êdy np. przy MoveCommand
        std::unique_ptr<QGraphicsItem> removedPoint;
		//! 
        bool bestFit;
    };

    //! Polecenie usuwa punkt z obszaru
    class RemovePointCommand : public utils::ICommand
    {
    public:
        //! Konstruktor
        //! \param item Element graficzny sceny, które zostanie przesuniêty 
        //! \param newP Nowa pozycja elementu 
        //! \param oldP Stara pozycja elementu
        RemovePointCommand(PointsLayerPtr layer, int idx);

    public:
        //! Przesuwa element
        virtual void doIt();

        //! Cofa przesuniêcie
        virtual void undoIt();
        //! Nazwa polecenia 
        virtual QString name();

    private:
        //! Modyfikowany obszar
        PointsLayerPtr layer;
        //! indeks punktu do usuniêcia
        int idx;
        /// usuniêty punkt, który mo¿e zostaæ przywrócony. Przywracaj¹c nie mo¿na stworzyæ nowej instancji
        /// gdy¿ mog³oby to spowodowaæ b³êdy np. przy MoveCommand
        std::unique_ptr<QGraphicsItem> removedPoint;
    };

	//! Polecenie obs³uguj¹ce przesuniêcie z jednego punktu do drugiego
    class MovePathCommand : public coreUI::MoveCommand 
    {
    public:
        MovePathCommand(PointsLayerPtr layer, QGraphicsItem* item, const QPointF& newP, const QPointF& oldP);

          virtual void doIt();

          virtual void undoIt();

    private:
        PointsLayerPtr layer;
    };


class LayeredStateMachine;

//! Stan po którym dziedzicz¹ stany edycyjne - PointsState i EditState.
//! Agreguje ich czêœci wspólne, obs³ugê myszki, menu
class AbstractEditState : public QObject, public coreUI::AbstractState
{
    Q_OBJECT
public:
	AbstractEditState(LayeredStateMachine* machine);
	virtual ~AbstractEditState() {}
    friend class AddLayerCommand;
public:
    virtual bool keyReleaseEvent( QKeyEvent *event );
	virtual bool mouseMoveEvent(QGraphicsSceneMouseEvent* e);
    virtual bool focusOutEvent(QFocusEvent * event);
	virtual void begin(coreUI::AbstractStateConstPtr lastState);
	virtual void end();
protected:
	QGraphicsItem* extractItem(QGraphicsSceneMouseEvent* e);

protected:
    std::vector<std::pair<QGraphicsItem*, QPointF>> positionsToCheck;
    PointsLayerPtr layer;
    LayeredStateMachine* machine;
    bool possibleMove;
    utils::weak_ptr<QMenu> rightClickMenu;
	QGraphicsLineItem line[2];
};
DEFINE_SMART_POINTERS(AbstractEditState);

}
#endif
