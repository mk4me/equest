/********************************************************************
	created:	2013/12/19
	created:	19:12:2013   19:15
	filename: 	EquestPerspective.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_EQUEST__EquestPerspective_H__
#define HEADER_GUARD_EQUEST__EquestPerspective_H__

#include <plugins/hmdbCommunication/IHierarchyPerspective.h>
#include <corelib/HierarchyHelper.h>
#include <corelib/Visualizer.h>
#include <corelib/HierarchyDataItem.h>
#include "plugins/equest/Export.h"
#include <plugins/c3d/C3DCollections.h>

namespace equest {

std::string getUserName(const core::VariantConstPtr data);
std::string getUserName(const void * data);
std::string getUserNameFromMetadata(core::VariantConstPtr variant);

//! Klasa pomocnicza przy tworzeniu wizualizatorów
class PLUGIN_EQUEST_EXPORT EquestHelper : public core::WrappedItemHelper
{
public:
    EquestHelper(const core::VariantConstPtr & wrapper, const core::ConstVariantsList& layers,
        const std::string& xmlFilename, const std::string& trialName);
    virtual void createSeries( const core::VisualizerPtr & visualizer, const QString& path, std::vector<core::Visualizer::Serie*>& series );

public:
	const std::string& getTrialName() { return trialName;  }
private:
    core::ConstVariantsList layers;
    std::string xmlFilename; 
    std::string trialName;
};
DEFINE_SMART_POINTERS(EquestHelper);

//! Klasa pomocnicza przy tworzeniu wizualizatorów
class PLUGIN_EQUEST_EXPORT EquestMultiHelper : public core::HierarchyHelper
{
public:
	//EquestMultiHelper(const std::vector<EquestHelperPtr>& helpers);
	EquestMultiHelper(core::IHierarchyItemWeakPtr sessionItem);

protected:
	virtual void createSeries(const core::VisualizerPtr & visualizer, const QString& path, std::vector<core::Visualizer::Serie*>& series);

	std::vector<EquestHelperPtr> getHelpers(core::IHierarchyItem* item) const;

public:
	virtual core::VisualizerPtr createVisualizer(core::IVisualizerManager* manager);
	virtual std::vector<utils::TypeInfo> getTypeInfos() const;

private:
	//std::vector<EquestHelperPtr> helpers;
	core::IHierarchyItemWeakPtr sessionItem;
};
DEFINE_SMART_POINTERS(EquestMultiHelper);

//! Klasa realizuj¹ca perspektywe danych
class PLUGIN_EQUEST_EXPORT EquestPerspective : public hmdbCommunication::IHierarchyPerspective
{
public:
	virtual ~EquestPerspective() {}

    virtual core::IHierarchyItemPtr getPerspective( PluginSubject::SubjectPtr subject );

    core::HierarchyDataItemPtr tryGetHierarchyItem(const std::string& filename) const;
private:
	bool hasValidData(PluginSubject::SubjectPtr subject);
	core::IHierarchyItemPtr createTree(const QString& rootItemName, const PluginSubject::SubjectConstPtr& subject);//! Tworzy ga³¹Ÿ dla markerów
	//! \param motion próba pomiarowa
	//! \param rootName nazwa dla rodzica tworzonych elementów
	//! \param rootIcon ikona rodzica
	//! \param itemIcon ikona dla liœci
	core::IHierarchyItemPtr createMarkersBranch(const PluginSubject::MotionConstPtr & motion, const QString& rootName, const QIcon& rootIcon, const QIcon& itemIcon, const std::string &);
	//! Tworzy ga³¹Ÿ dla stawów
	//! \param motion próba pomiarowa
	//! \param rootName nazwa dla rodzica tworzonych elementów
	//! \param rootIcon ikona rodzica
	//! \param itemIcon ikona dla liœci
	//core::IHierarchyItemPtr createJointsBranch(const PluginSubject::MotionConstPtr & motion, const QString& rootName, const QIcon& rootIcon, const QIcon& itemIcon, const std::string &);
	//! Tworzy ga³¹Ÿ dla nagrañ video
	//! \param motion próba pomiarowa
	//! \param rootName nazwa dla rodzica tworzonych elementów
	//! \param rootIcon ikona rodzica
	//! \param itemIcon ikona dla liœci
	core::IHierarchyItemPtr createVideoBranch(const PluginSubject::MotionConstPtr & motion, const QString& rootName, const QIcon& rootIcon, const QIcon& itemIcon, const std::string &);
	core::IHierarchyItemPtr createAnnotationsBranch(const PluginSubject::MotionConstPtr & motion, const QString& rootName, const QIcon& rootIcon, const QIcon& itemIcon, const std::string &userName, const std::string& label);

	

	//! Tworzy ga³¹Ÿ dla danych grf
	//! \param motion próba pomiarowa
	//! \param rootName nazwa dla rodzica tworzonych elementów
	//! \param rootIcon ikona rodzica
	//! \param itemIcon ikona dla liœci
	core::IHierarchyItemPtr createGRFBranch(const PluginSubject::MotionConstPtr & motion, const QString& rootName, const QIcon& rootIcon, const QIcon& itemIcon, const std::string &);
	//! Tworzy ga³¹Ÿ dla danych emg
	//! \param motion próba pomiarowa
	//! \param rootName nazwa dla rodzica tworzonych elementów
	//! \param rootIcon ikona rodzica
	//! \param itemIcon ikona dla liœci
	core::IHierarchyItemPtr createEMGBranch(const PluginSubject::MotionConstPtr & motion, const QString& rootName, const QIcon& rootIcon, const QIcon& itemIconconst, const std::string & conf);
	//! Tworzy ga³¹Ÿ dla 
	//! \param motion próba pomiarowa
	//! \param rootName nazwa dla rodzica tworzonych elementów
	//! \param rootIcon ikona rodzica
	//! \param itemIcon ikona dla liœci
	core::IHierarchyItemPtr createForcesBranch(const PluginSubject::MotionConstPtr & motion, const QString& rootName, const QIcon& rootIcon, const QIcon& itemIcon, const std::string &);
	//! Tworzy ga³¹Ÿ dla momentów
	//! \param motion próba pomiarowa
	//! \param rootName nazwa dla rodzica tworzonych elementów
	//! \param rootIcon ikona rodzica
	//! \param itemIcon ikona dla liœci
	core::IHierarchyItemPtr createMomentsBranch(const PluginSubject::MotionConstPtr & motion, const QString& rootName, const QIcon& rootIcon, const QIcon& itemIcon, const std::string &);
	//! Tworzy ga³¹Ÿ dla mocy
	//! \param motion próba pomiarowa
	//! \param rootName nazwa dla rodzica tworzonych elementów
	//! \param rootIcon ikona rodzica
	//! \param itemIcon ikona dla liœci
	core::IHierarchyItemPtr createPowersBranch(const PluginSubject::MotionConstPtr & motion, const QString& rootName, const QIcon& rootIcon, const QIcon& itemIcon, const std::string &);

	//! \return ikona dla ga³êzi z markerami
	const QIcon& getMarkersIcon() { static QIcon icon(QString::fromUtf8(":/mde/icons/markerSmall.png")); return icon; }
	//! \return ikona dla ga³êzi ze stawami
	const QIcon& getJointsIcon() { static QIcon icon(QString::fromUtf8(":/mde/icons/jointSmall.png"));  return icon; }
	//! \return ikona dla ga³êzi z nagraniami
	const QIcon& getVideoIcon() { static QIcon icon(QString::fromUtf8(":/mde/icons/videoSmall.png"));  return icon; }
	//! \return ikona dla ga³êzi z danymi grf
	const QIcon& getGRFIcon() { static QIcon icon(QString::fromUtf8(":/mde/icons/grfSmall.png"));    return icon; }
	//! \return ikona dla ga³êzi z danymi emg
	const QIcon& getEMGIcon() { static QIcon icon(QString::fromUtf8(":/mde/icons/emg1Small.png"));   return icon; }
	//! \return ikona dla ga³êzi z si³ami
	const QIcon& getForcesIcon() { static QIcon icon(QString::fromUtf8(":/mde/icons/forcesSmall.png")); return icon; }
	//! \return ikona dla ga³êzi z momentami
	const QIcon& getMomentsIcon() { static QIcon icon(QString::fromUtf8(":/mde/icons/momentSmall.png")); return icon; }
	//! \return ikona dla ga³êzi z mocami
	const QIcon& getPowersIcon() { static QIcon icon(QString::fromUtf8(":/mde/icons/powerSmall.png"));  return icon; }

	//! \return ikona dla g³ównej ga³êzi z markerami
	const QIcon& getRootMarkersIcon() { static QIcon icon(QString::fromUtf8(":/mde/icons/markerSmall.png")); return icon; }
	//! \return ikona dla g³ównej ga³êzi ze stawami
	const QIcon& getRootJointsIcon() { static QIcon icon(QString::fromUtf8(":/mde/icons/jointSmall.png"));  return icon; }
	//! \return ikona dla g³ównej ga³êzi z nagraniami
	const QIcon& getRootVideoIcon() { static QIcon icon(QString::fromUtf8(":/mde/icons/videoSmall.png"));  return icon; }
	//! \return ikona dla g³ównej ga³êzi z danymi grf
	const QIcon& getRootGRFIcon() { static QIcon icon(QString::fromUtf8(":/mde/icons/grfSmall.png"));    return icon; }
	//! \return ikona dla g³ównej ga³êzi z danymi emg
	const QIcon& getRootEMGIcon() { static QIcon icon(QString::fromUtf8(":/mde/icons/emg1Small.png"));   return icon; }
	//! \return ikona dla g³ównej ga³êzi z si³ami
	const QIcon& getRootForcesIcon() { static QIcon icon(QString::fromUtf8(":/mde/icons/forcesSmall.png")); return icon; }
	//! \return ikona dla g³ównej ga³êzi z momentami
	const QIcon& getRootMomentsIcon() { static QIcon icon(QString::fromUtf8(":/mde/icons/momentSmall.png")); return icon; }
	//! \return ikona dla g³ównej ga³êzi z mocami
	const QIcon& getRootPowersIcon() { static QIcon icon(QString::fromUtf8(":/mde/icons/powerSmall.png"));  return icon; }
	//! tworzy element drzewa z dowolnej kolekcji kana³ów danych
	//! \param motion próba pomiarowa
	//! \param rootName nazwa dla rodzica tworzonych elementów
	//! \param rootIcon ikona rodzica
	//! \param itemIcon ikona dla liœci
	template <class Channel, class Collection>
	core::IHierarchyItemPtr createTBranch(const PluginSubject::MotionConstPtr & motion, const QString& rootName, const QIcon& rootIcon, const QIcon& itemIcon);
	//! dodanie do drzewa elementów z podpiêtymi danymi w postaci wektorów
	//! \param motion próba pomiarowa
	//! \param collection kolekcja zawieraj¹ca wektory
	//! \param name nazwa tworzonego elementu
	//! \param childIcon ikona dla liœci
	//! \param parentItem rodzic dla tworzonych elementów
	//! \param createContainerItem czy stworzyæ element pomiêdzy rodzicem a liœæmi
	template <class Channel, class CollectionPtr>
	void tryAddVectorToTree(const PluginSubject::MotionConstPtr & motion, const CollectionPtr & collection, const std::string& name, const QIcon& childIcon, core::IHierarchyItemPtr parentItem, bool createContainerItem = true);

	c3dlib::EventsCollectionConstPtr getEvents(const PluginSubject::MotionConstPtr & motion);

	core::HierarchyHelperPtr allTFromSession(const std::string& channelName, PluginSubject::SessionConstPtr s, int channelNo);
	core::HierarchyHelperPtr createNormalized(core::VariantConstPtr wrapper, PluginSubject::MotionConstPtr motion, c3dlib::C3DParser::IEvent::Context context);
	core::HierarchyHelperPtr createNormalizedFromAll(const std::string& channelName, PluginSubject::SessionConstPtr s, c3dlib::C3DParser::IEvent::Context context);
	QString createDescription(PluginSubject::MotionConstPtr motion);
private:
    std::map<std::string, core::HierarchyDataItemWeakPtr> name2hierarchy;
};
DEFINE_SMART_POINTERS(EquestPerspective);


class EquestAnnotationItemHelper : public core::WrappedItemHelper
{
public:
	EquestAnnotationItemHelper(const core::VariantConstPtr& wrapper, PluginSubject::MotionConstPtr motion, const std::string& label);

public:
	//! Tworzy wizualizator newChart
	//virtual core::VisualizerPtr createVisualizer(core::IVisualizerManager* manager);
	//! zwraca utworzone serie danych
	//! \param visualizer wizualizator, który bêdzie tworzy³ serie
	//! \param path œcie¿ka dla timeline
	//! \param series tutaj trafia stworzone serie 
	virtual void createSeries(const core::VisualizerPtr & visualizer, const QString& path, std::vector<core::Visualizer::Serie*>& series);
	//! \return typ ScalarChannelReaderInterface
	//std::vector<utils::TypeInfo> getTypeInfos() const;

protected:
	PluginSubject::MotionConstPtr motion;
	std::string label;
};
DEFINE_SMART_POINTERS(EquestAnnotationItemHelper);
}
#endif
