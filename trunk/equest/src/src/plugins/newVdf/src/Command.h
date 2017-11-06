/********************************************************************
	created:	2012/12/26
	created:	26:12:2012   22:37
	filename: 	Command.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_NEWVDF__COMMAND_H__
#define HEADER_GUARD_NEWVDF__COMMAND_H__

#include <utils/SmartPtr.h>
#include <QtCore/QObject>
#include <QtCore/QPointF>
#include <utils/ICommand.h>

#include "SceneBuilder.h"
#include "SceneModel.h"

namespace vdf {


class IVisualItem;

//! Dodaje element (node) na scenê.
class AddToSceneCommand : public utils::ICommand
{
public:
	//! Konstruktor
	//! \param scene  Model sceny vdf, do którego dodajemy element
	//! \param node Dodawany element
	//! \param scenePos Pozycja na scenie, gdzie zostanie wstawiony element
	AddToSceneCommand(SceneModelPtr scene, const SceneBuilder::VisualNodeWithPins& node, const QPointF& scenePos);
public:
	//! Wstawia elment na scenê wraz z pinami, które przechowuje.
	virtual void doIt();
	//! Usuwa element wraz z pinami ze sceny
	virtual void undoIt();
	//! Nazwa polecenia
	virtual QString name();
private:
	//! Model sceny vdf, do którego dodajemy element
	SceneModelPtr sceneModel;
	//! Dodawany element
	SceneBuilder::VisualNodeWithPins nodeWithPins;
	//! Pozycja na scenie, gdzie zostanie wstawiony element
	QPointF position;
};

//! Polecenie obs³uguje dodanie po³¹czenia pomiêdzy pinami.
class AddConnectionCommand : public utils::ICommand
{
public:
	//! Konstruktor
	//! \param scene Model sceny vdf, do którego dodajemy element
	//! \param p1 Pin wyjœciowy
	//! \param p2 Pin wejœciowy
	AddConnectionCommand(SceneModelPtr scene, IVisualOutputPinPtr p1, IVisualInputPinPtr p2);
public:
	//! Dodaje po³¹czenie pomiêdzy pinami
	virtual void doIt();
	//! Usuwa po³¹czenie
	virtual void undoIt();
	//! Nazwa polecenia
	virtual QString name() { return QString(typeid(this).name()); }
private:
	//! Model sceny vdf, do którego dodajemy element
	SceneModelPtr sceneModel;
	//! Pin wyjœciowy
	IVisualOutputPinPtr outputPin;
	//! Pin wejœciowy
	IVisualInputPinPtr inputPin;
	//! Utworzone po³¹czenie
    IVisualConnectionPtr connection;
};

////! Polecenie przesuwa elementy na scenie
//class MoveCommand : public utils::ICommand
//{
//public:
//	//! Konstruktor
//	//! \param item Element graficzny sceny, które zostanie przesuniêty 
//	//! \param newP Nowa pozycja elementu 
//	//! \param oldP Stara pozycja elementu
//	MoveCommand(QGraphicsItem* item, const QPointF& newP, const QPointF& oldP);
//
//public:
//	//! Przesuwa element
//	virtual void doIt();
//	//! Cofa przesuniêcie
//	virtual void undoIt();
//	//! Nazwa polecenia 
//	virtual QString name() { return QString(typeid(this).name()); }
//
//private:
//	//! Element graficzny sceny, które zostanie przesuniêty 
//	QGraphicsItem* item;
//	//! Nowa pozycja elementu graficznego
//	QPointF newPosition;
//	//! Stara pozycja elementu graficznego
//	QPointF oldPosition;
//};

////! Polecenie agreguje inne polecenia
//class MultiCommand : public utils::ICommand
//{
//public:
//	//! Konstruktor
//	//! \param c Wektor z poleceniami do wykonania
//	MultiCommand(const std::vector<utils::ICommandPtr>& c);
//public:
//	//! Kolejno wykonuje dostarczone polecenia
//	virtual void doIt();
//	//! Cofa polecenia w odwrotnej kolejnoœci do wykonania
//	virtual void undoIt();
//	//! Nazwa polecenia 
//	virtual QString name() { return QString(typeid(this).name()); }
//
//private:
//	//! Dostarczone polecenia do wykonania
//	std::vector<utils::ICommandPtr> commands;
//};

//! Polecenie usuwa po³¹czenie pomiêdzy pinami
class RemoveConnectionCommand : public utils::ICommand
{
public:
    //! Konstruktor
    //! \param scene Model sceny vdf 
    //! \param toRemove Po³¹czenie do usuniêcia
    RemoveConnectionCommand(SceneModelPtr scene, IVisualConnectionPtr toRemove);

public:
	//! Usuwa po³¹czenie
    virtual void doIt();
	//! Ponownie wstawia po³¹czenie na scenê
    virtual void undoIt();
	//! Nazwa polecenia
    virtual QString name() { return QString(typeid(this).name()); }

private:
	//! Po³¹czenie do usuniêcia
    IVisualConnectionPtr item;
	//! Model sceny vdf
    SceneModelPtr sceneModel;
};

//! Polecenie usuwa element (node) ze sceny vdf
class RemoveNodeCommand : public utils::ICommand
{
public:
	//! Konstruktor
	//! \param scene Model sceny vdf 
	//! \param toRemove Element do usuniêcia
	RemoveNodeCommand(SceneModelPtr scene, IVisualNodePtr toRemove);
public:
	//! Usuwa wêze³ ze sceny, razem z pinami i dowi¹zanymi do nich po³¹czeniami
	virtual void doIt();
	//! Wstawia z powrotem usuniêty wêze³, przywraca po³¹czenia
	virtual void undoIt();
	//! Nazwa polecenia
	virtual QString name() { return QString(typeid(this).name()); }

private:
	//! Usuwa po³¹czenie dla pinu wejœciowego (mo¿liwe jedno)
	//! \param pin Pin, z którego usuwamy
	void removeConnectionFromPin( IVisualInputPinPtr pin );
    //! Usuwa po³¹czenia dl pinu wyjœciowego (mo¿liwe wiele)
    //! \param pin Pin, z którego usuwamy
    void removeConnectionFromPin( IVisualOutputPinPtr pin );
    //! Usuwa po³¹czenie ze sceny
    //! \param connection po³¹czenie do usuniêcia
    void removeConnection( IVisualConnectionPtr connection );

private:
	//! Usuwany element
	IVisualNodePtr item;
	//! Model sceny vdf
	SceneModelPtr sceneModel;
	//! Wszystkie usuniête przy okazji po³¹czenia
	std::list<IVisualConnectionPtr> removedConnections;
	//! Wszystkie usuniête przy okazji piny
	std::list<IVisualPinPtr> removedPins;
};

//! Usuwa ca³¹ grupê elementów graficznych, wykorzustuje inne polecenia do realizacji tego celu
class RemoveSelectedCommand : public utils::ICommand
{
public:
	//! Konstruktor
	//! \param scene Model sceny vdf 
	//! \param selectedItems Elementy graficzne, których reprezentacja logiczna ma zostaæ usuniêta z modelu
	RemoveSelectedCommand(SceneModelPtr scene, const QList<QGraphicsItem*> selectedItems);
public:
	//! Usuwa zaznaczone elementy
	virtual void doIt();
	//! Przywraza usuniête elementy
	virtual void undoIt();
	//! Nazwa polecenia
	virtual QString name() { return QString(typeid(this).name()); }
private:
	//! Elementy graficzne, których reprezentacja logiczna ma zostaæ usuniêta z modelu
    QList<QGraphicsItem*> items;
	//! Model sceny vdf
    SceneModelPtr sceneModel;
	//! Polecenia usuwaj¹ce elementy ze sceny
    std::vector<utils::ICommandPtr> commands;
};
}

#endif
