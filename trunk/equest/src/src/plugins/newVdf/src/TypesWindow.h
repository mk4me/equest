/********************************************************************
	created:	2012/12/17
	created:	17:12:2012   8:56
	filename: 	TypesWindow.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_NEWVDF__TYPESWINDOW_H__
#define HEADER_GUARD_NEWVDF__TYPESWINDOW_H__


#include <QtWidgets/QWidget>
#include <utils/ObserverPattern.h>
#include <utils/CommandStack.h>
#include <plugins/newVdf/IDataProcessorManager.h>
#include <plugins/newVdf/IDataSinkManager.h>
#include <plugins/newVdf/IDataSourceManager.h>
#include "TypesModel.h"

namespace Ui { class TypesWindow; } 

namespace vdf {

class NewVdfWidget;


//! Widget z elementami do wstawienia na scenê
class TypesWindow : public QWidget
{
    Q_OBJECT;
public:
    //! Konstruktor
    //! \param stack Stos komend (umo¿liwia m.in. undo)
    //! \param canvas Model ze stylami elementów
    //! \param sceneModel Scena, na któr¹ trafi¹ elementy 
    //! \param parent 
    //! \param f 
    explicit TypesWindow(TypesModelPtr model, QWidget* parent = 0, Qt::WindowFlags f = 0);
	virtual ~TypesWindow() {}

//protected:
//    //! Wywo³ywane, gdy pojawi¹ siê nowe elementy przetwarzaj¹ce
//    //! \param pm 
//    void update(const IDataProcessorManager* pm);
//    //! Wywo³ywane, gdy pojawi¹ siê nowe Ÿród³a
//    //! \param sm 
//    void update(const IDataSourceManager* sm);
//    //! Wywo³ywane, gdy pojawi¹ siê nowe sinki
//    //! \param sm 
//    void update(const IDataSinkManager* sm);

private Q_SLOTS:
    //! Dodanie nowego elementu do odpwiedniego widgeta
    //! \param entry nazwa/identyfikator elementu
    //! \param icon ikona elementu
    //! \param type typ elementu, warunkuje, do którego widgeta trafi wpis
    void addEntry(const QString& entry, const QIcon& icon, IVisualItem::Type type);

private:
    //! ui tworzone w designerze
    Ui::TypesWindow* ui;
	//TypesModelPtr model;
};

}

#endif
