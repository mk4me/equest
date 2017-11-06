/********************************************************************
	created:	2013/06/05
	created:	5:6:2013   9:17
	filename: 	ConfiguredFilterCommand.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_HMM__CONFIGUREDFILTERCOMMAND_H__
#define HEADER_GUARD_HMM__CONFIGUREDFILTERCOMMAND_H__

#include <QtWidgets/QTreeWidget>
#include <QtWidgets/QTreeWidgetItem>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QCheckBox>

#include <functional>
#include <type_traits>

#include <utils/SmartPtr.h>

#include <plugins/c3d/C3DChannels.h>
#include <plugins/c3d/C3DCollections.h>
#include <plugins/subject/ISubjectService.h>

#include <corelib/IFilterCommand.h>
#include <corelib/IHierarchyItem.h>
#include <plugins/hmmlib/ConfigurationDialog.h>
#include <corelib/VariantsCollection.h>
#include "Measurements.h"
#include "SubjectFilterCommand.h"

//! mapa [element konfiguracji (np. elektroda) -> wpis z odpowiadającą nazwą + ew. nazwą obrazka]
typedef std::map<QString, std::pair<QString,QString> > NamesDictionary;

//! klasa, dzięki której możliwe jest korzystanie z sygnałow i slotów w klasie szablonowej.
//! obsługuje sloty w klasach Vector3DFilterCommand i Vector3DFilterCommand2
class __Helper : public QObject
{
    Q_OBJECT;
public:
    //! konstruktor, pobiera funkcję, która obsługuje zmianę widgeta
    //! \param function odpowiada sygnaturze checkBoxChanged
    __Helper(std::function<void (const QString&, int)> function);

    public slots:
        //! slot, które przekaże informację o zmianie checkBoxa do ownera
        void checkBoxChanged ( int state );
        //! slot, które przekaże informację o zaznaczeniu elementu do ownera
        void onItemSelected(const QString& name, bool selected);
        //! slot, które przekaże informację o najechaniu elementu do ownera
        void onElementHovered(const QString& name, bool selected);

public:
    //! ustawia mapę z elementami konfiguratora
    //! \param val mapa z elementami konfiguratora oraz ich identyfikatorami
    void setNamesDictionary(const NamesDictionary& val) { namesDictionary = val; }
    //! \return mapa z elementami konfiguratora
    const NamesDictionary& getNamesDictionary() const { return namesDictionary; }

private:
    //! funkcja, która obsługuje zmianę widgeta
    std::function<void (const QString&, int)> function;
    //! mapa z elementami konfiguratora
    NamesDictionary namesDictionary;
};

//! klasa która wykrzystuje metody buildera drzewa danych, do stworzenia przefiltrowanej gałęzi, dodatkowo dostarcza konfigurator
class BuilderConfiguredFilterCommand : public SimpleFilterCommand
{

public:
    //! konstruktor
    //! \param function funktor tworzący gałąź
    //! \param namesDictionary mapa z elementami konfiguratora
    //! \param frontXml informacje, o rozmieszczeniu elementów z przodu pacjenta
    //! \param backXml informacje, o rozmieszczeniu elementów z tyłu pacjenta
    //! \param rootIcon ikona gałęzi
    //! \param elementIcon ikona dla liści gałęzi
    BuilderConfiguredFilterCommand( const NamesDictionary& namesDictionary, utils::TypeInfo typeInfo,
        const QString& frontXml, const QString& backXml, const QString& name, const QIcon& icon, const QIcon& rootIcon = QIcon(), const QIcon& elementIcon = QIcon()) :
          SimpleFilterCommand(typeInfo, name, icon),
		  helper(std::bind(&BuilderConfiguredFilterCommand::checkBoxChanged, this, std::placeholders::_1, std::placeholders::_2)),
          frontXml(frontXml),
          backXml(backXml),
          configurationWidget(nullptr),
          parentWidget(nullptr)
      {
        helper.setNamesDictionary(namesDictionary);
      }

      //! metoda, tkóra jest związana z helperem, zmienił się stan elementu
      //! \param box nazwa zmienionego elementu
      //! \param state aktywny / nieaktywny
      void checkBoxChanged (const QString& box, int state )
      {
          std::string name = box.toStdString();
          activeElements[name] = (bool)state;
      }

//private:
//    //! tworzy mapę z elementami konfiguratora
//    //! \param collection kolekcja, która będzie konfigurowana
//    void createNameDictionary(const CollectionConstPtr & collection)
//    {
//        int count = collection->getNumChannels();
//        for (int i = 0; i < count; ++i) {
//            auto channel = collection->getChannel(i);
//            std::string name = channel->getName();
//            auto it = activeElements.find(name);
//            if (it == activeElements.end()) {
//                activeElements[name] = true;
//            }
//        }
//    }

public:
    //! wywołane po zakończegu konfiguracji
    //! \param result rezultat, oznacza, czy zmiany mają wejść w życie
    virtual void configurationStop( ConfigurationResult result )
    {
        if (result == Cancel) {
            activeElements = tempNameDictionary;
        }
    }
    //! \return widget konfiguratora lub nullptr jesli nie został dostarczony
    virtual QWidget* getConfigurationWidget()
    {
        if (!configurationWidget) {
            parentWidget = new QWidget(nullptr);
            QLayout* h = new QHBoxLayout();
            h->setMargin(0);
            h->setContentsMargins(0, 0, 0, 0);
            configurationWidget = new ConfigurationWidget(nullptr);
            QWidget* left = new QWidget();
            QWidget* right = new QWidget();
            left->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
            right->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
            h->addWidget(left);
            h->addWidget(configurationWidget);
            h->addWidget(right);
            parentWidget->setLayout(h);
            configurationWidget->loadConfigurations(frontXml, backXml, helper.getNamesDictionary());
            QObject::connect(configurationWidget, SIGNAL(itemSelected(const QString&, bool)), &helper, SLOT(onItemSelected(const QString&, bool)));
            QObject::connect(configurationWidget, SIGNAL(elementHovered(const QString&, bool)), &helper, SLOT(onElementHovered(const QString&, bool)));
        }
        return parentWidget;
    }
    //! przywraca domyślne ustawienia konfiguratora
    virtual void reset()
    {
        for (auto it = activeElements.begin(); it != activeElements.end(); ++it) {
            it->second = true;
        }
        if (configurationWidget) {
            configurationStart();
            configurationWidget->repaint();
        }
    }
    //! konfigurator został wywołany
    virtual void configurationStart()
    {
        tempNameDictionary = activeElements;
        std::map<QString, bool> visibles;
        const NamesDictionary& names = helper.getNamesDictionary();
        for (auto elementIT = activeElements.begin(); elementIT != activeElements.end(); ++elementIT) {
            for (auto nameIT = names.cbegin(); nameIT != names.cend(); ++nameIT) {
                if (nameIT->second.first.toStdString() == elementIT->first) {
                    visibles[nameIT->first] = elementIT->second;
                }
            }
        }
        configurationWidget->setVisibles(visibles);
    }


protected:
    virtual bool checkItem( core::IHierarchyItemConstPtr item ) 
    {
        if (SimpleFilterCommand::checkItem(item)) {
            auto name = item->getName().toStdString();
            auto entry = activeElements.find(name);
            if (entry != activeElements.end() && !entry->second) {
                return false;
            }

            return true;
        }

        return false;
    }

protected:
    //! mapa aktywnych elemnetów konfiguratora
    std::map<std::string, bool> activeElements;
    //! pomocnicze, przy zatwierdzaniu lub odrzucaniu zmian konfiguratora
    std::map<std::string, bool> tempNameDictionary;
    //PluginSubject::DataFilterPtr simpleTypeFilter;
    //! pomocniczy obiekt, dzięki któremu można zgrać szablony z sygnałami qt
    __Helper helper;
    //! ścieżka do pliku z rozmieszczeniem elemnetów
    QString frontXml;
    //! ścieżka do pliku z rozmieszczeniem elemnetów
    QString backXml;
    //! konfigurator
    ConfigurationWidget* configurationWidget;
    //! rodzic
    QWidget* parentWidget;
};



#endif