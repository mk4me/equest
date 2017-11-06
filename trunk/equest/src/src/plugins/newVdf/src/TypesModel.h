/********************************************************************
	created:  2016/02/19 14:09
	filename: TypesModel.h
	author:	  Wojciech Knieæ
	
	purpose:  
*********************************************************************/
#ifndef HEADER_GUARD_PLUGIN_NEWVDF__TYPESMODEL_H__
#define HEADER_GUARD_PLUGIN_NEWVDF__TYPESMODEL_H__

#include <QtWidgets/QWidget>
#include <utils/ObserverPattern.h>
#include <utils/CommandStack.h>
#include <plugins/newVdf/IDataProcessorManager.h>
#include <plugins/newVdf/IDataSinkManager.h>
#include <plugins/newVdf/IDataSourceManager.h>

#include "Command.h"
#include "CanvasStyleEditor.h"
#include "SceneModel.h"
#include "plugins/newVdf/IVisualItem.h"
#include <QtGui/QIcon>
#include <QtCore/QString>


namespace vdf {

class NewVdfWidget;


//! Widget z elementami do wstawienia na scenê
class TypesModel : public QObject, public utils::Observer<vdf::IDataProcessorManager>, public utils::Observer<vdf::IDataSinkManager>, public utils::Observer<vdf::IDataSourceManager>
{
    Q_OBJECT;
public:
    //! Konstruktor
    //! \param stack Stos komend (umo¿liwia m.in. undo)
    //! \param canvas Model ze stylami elementów
    //! \param sceneModel Scena, na któr¹ trafi¹ elementy 
    //! \param parent 
    //! \param f 
    explicit TypesModel(utils::ICommandStackPtr stack, CanvasStyleEditorPtr canvas, SceneModelPtr sceneModel, QObject* parent = nullptr);
	virtual ~TypesModel() {}

public:
    // TODO : zmienic idedtyfikacje z name na uid

    //! Tworzy element na podstawie nazwy i umieszcza go na scenie, komenda trafia na stos wywo³añ.
    //! \param name nazwa elementu, który zostanie utworzony
    //! \param scenePos pozycja na scenie, gdzie trafi obiekt
    void insert(const QString& name, const QPointF& scenePos);

    //! Tworzy element, który jest gotowy aby go wstawiæ na scenê
    //! \param entry nazwa/identyfikator elementu
    SceneBuilder::VisualNodeWithPins createItemByEntry(const QString& entry);

    core::UniqueID getId(const QString& name) const;
        
protected:
    //! Wywo³ywane, gdy pojawi¹ siê nowe elementy przetwarzaj¹ce
    //! \param pm 
    void update(const IDataProcessorManager* pm);
    //! Wywo³ywane, gdy pojawi¹ siê nowe Ÿród³a
    //! \param sm 
    void update(const IDataSourceManager* sm);
    //! Wywo³ywane, gdy pojawi¹ siê nowe sinki
    //! \param sm 
    void update(const IDataSinkManager* sm);
	
private:
    //! Model ze stylami elmentów
    CanvasStyleEditorPtr canvas;
    //! Stos komend (umo¿liwia m.in. undo)
	utils::ICommandStackPtr commmandStack;
    //! Scena, na któr¹ trafi¹ elementy
    SceneModelPtr sceneModel;
    //! nazwa/identyfikator elementu -> prototyp elementu
    std::map<QString, IWorkflowItemBasePtr> name2node;

Q_SIGNALS:
	void entryAdded(const QString&, const QIcon&, IVisualItem::Type);
};
DEFINE_SMART_POINTERS(TypesModel);
}

#endif
