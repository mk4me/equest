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

//! Klasa pomocnicza przy tworzeniu wizualizator�w
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

//! Klasa pomocnicza przy tworzeniu wizualizator�w
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

//! Klasa realizuj�ca perspektywe danych
class PLUGIN_EQUEST_EXPORT EquestPerspective : public hmdbCommunication::IHierarchyPerspective
{
public:
	virtual ~EquestPerspective() {}

    virtual core::IHierarchyItemPtr getPerspective( PluginSubject::SubjectPtr subject );

    core::HierarchyDataItemPtr tryGetHierarchyItem(const std::string& filename) const;
private:
	bool hasValidData(PluginSubject::SubjectPtr subject);
	core::IHierarchyItemPtr createTree(const QString& rootItemName, const PluginSubject::SubjectConstPtr& subject);//! Tworzy ga��� dla marker�w
	//! \param motion pr�ba pomiarowa
	//! \param rootName nazwa dla rodzica tworzonych element�w
	//! \param rootIcon ikona rodzica
	//! \param itemIcon ikona dla li�ci
	core::IHierarchyItemPtr createMarkersBranch(const PluginSubject::MotionConstPtr & motion, const QString& rootName, const QIcon& rootIcon, const QIcon& itemIcon, const std::string &);
	//! Tworzy ga��� dla staw�w
	//! \param motion pr�ba pomiarowa
	//! \param rootName nazwa dla rodzica tworzonych element�w
	//! \param rootIcon ikona rodzica
	//! \param itemIcon ikona dla li�ci
	//core::IHierarchyItemPtr createJointsBranch(const PluginSubject::MotionConstPtr & motion, const QString& rootName, const QIcon& rootIcon, const QIcon& itemIcon, const std::string &);
	//! Tworzy ga��� dla nagra� video
	//! \param motion pr�ba pomiarowa
	//! \param rootName nazwa dla rodzica tworzonych element�w
	//! \param rootIcon ikona rodzica
	//! \param itemIcon ikona dla li�ci
	core::IHierarchyItemPtr createVideoBranch(const PluginSubject::MotionConstPtr & motion, const QString& rootName, const QIcon& rootIcon, const QIcon& itemIcon, const std::string &);
	core::IHierarchyItemPtr createAnnotationsBranch(const PluginSubject::MotionConstPtr & motion, const QString& rootName, const QIcon& rootIcon, const QIcon& itemIcon, const std::string &userName, const std::string& label);

	

	//! Tworzy ga��� dla danych grf
	//! \param motion pr�ba pomiarowa
	//! \param rootName nazwa dla rodzica tworzonych element�w
	//! \param rootIcon ikona rodzica
	//! \param itemIcon ikona dla li�ci
	core::IHierarchyItemPtr createGRFBranch(const PluginSubject::MotionConstPtr & motion, const QString& rootName, const QIcon& rootIcon, const QIcon& itemIcon, const std::string &);
	//! Tworzy ga��� dla danych emg
	//! \param motion pr�ba pomiarowa
	//! \param rootName nazwa dla rodzica tworzonych element�w
	//! \param rootIcon ikona rodzica
	//! \param itemIcon ikona dla li�ci
	core::IHierarchyItemPtr createEMGBranch(const PluginSubject::MotionConstPtr & motion, const QString& rootName, const QIcon& rootIcon, const QIcon& itemIconconst, const std::string & conf);
	//! Tworzy ga��� dla 
	//! \param motion pr�ba pomiarowa
	//! \param rootName nazwa dla rodzica tworzonych element�w
	//! \param rootIcon ikona rodzica
	//! \param itemIcon ikona dla li�ci
	core::IHierarchyItemPtr createForcesBranch(const PluginSubject::MotionConstPtr & motion, const QString& rootName, const QIcon& rootIcon, const QIcon& itemIcon, const std::string &);
	//! Tworzy ga��� dla moment�w
	//! \param motion pr�ba pomiarowa
	//! \param rootName nazwa dla rodzica tworzonych element�w
	//! \param rootIcon ikona rodzica
	//! \param itemIcon ikona dla li�ci
	core::IHierarchyItemPtr createMomentsBranch(const PluginSubject::MotionConstPtr & motion, const QString& rootName, const QIcon& rootIcon, const QIcon& itemIcon, const std::string &);
	//! Tworzy ga��� dla mocy
	//! \param motion pr�ba pomiarowa
	//! \param rootName nazwa dla rodzica tworzonych element�w
	//! \param rootIcon ikona rodzica
	//! \param itemIcon ikona dla li�ci
	core::IHierarchyItemPtr createPowersBranch(const PluginSubject::MotionConstPtr & motion, const QString& rootName, const QIcon& rootIcon, const QIcon& itemIcon, const std::string &);

	//! \return ikona dla ga��zi z markerami
	const QIcon& getMarkersIcon() { static QIcon icon(QString::fromUtf8(":/mde/icons/markerSmall.png")); return icon; }
	//! \return ikona dla ga��zi ze stawami
	const QIcon& getJointsIcon() { static QIcon icon(QString::fromUtf8(":/mde/icons/jointSmall.png"));  return icon; }
	//! \return ikona dla ga��zi z nagraniami
	const QIcon& getVideoIcon() { static QIcon icon(QString::fromUtf8(":/mde/icons/videoSmall.png"));  return icon; }
	//! \return ikona dla ga��zi z danymi grf
	const QIcon& getGRFIcon() { static QIcon icon(QString::fromUtf8(":/mde/icons/grfSmall.png"));    return icon; }
	//! \return ikona dla ga��zi z danymi emg
	const QIcon& getEMGIcon() { static QIcon icon(QString::fromUtf8(":/mde/icons/emg1Small.png"));   return icon; }
	//! \return ikona dla ga��zi z si�ami
	const QIcon& getForcesIcon() { static QIcon icon(QString::fromUtf8(":/mde/icons/forcesSmall.png")); return icon; }
	//! \return ikona dla ga��zi z momentami
	const QIcon& getMomentsIcon() { static QIcon icon(QString::fromUtf8(":/mde/icons/momentSmall.png")); return icon; }
	//! \return ikona dla ga��zi z mocami
	const QIcon& getPowersIcon() { static QIcon icon(QString::fromUtf8(":/mde/icons/powerSmall.png"));  return icon; }

	//! \return ikona dla g��wnej ga��zi z markerami
	const QIcon& getRootMarkersIcon() { static QIcon icon(QString::fromUtf8(":/mde/icons/markerSmall.png")); return icon; }
	//! \return ikona dla g��wnej ga��zi ze stawami
	const QIcon& getRootJointsIcon() { static QIcon icon(QString::fromUtf8(":/mde/icons/jointSmall.png"));  return icon; }
	//! \return ikona dla g��wnej ga��zi z nagraniami
	const QIcon& getRootVideoIcon() { static QIcon icon(QString::fromUtf8(":/mde/icons/videoSmall.png"));  return icon; }
	//! \return ikona dla g��wnej ga��zi z danymi grf
	const QIcon& getRootGRFIcon() { static QIcon icon(QString::fromUtf8(":/mde/icons/grfSmall.png"));    return icon; }
	//! \return ikona dla g��wnej ga��zi z danymi emg
	const QIcon& getRootEMGIcon() { static QIcon icon(QString::fromUtf8(":/mde/icons/emg1Small.png"));   return icon; }
	//! \return ikona dla g��wnej ga��zi z si�ami
	const QIcon& getRootForcesIcon() { static QIcon icon(QString::fromUtf8(":/mde/icons/forcesSmall.png")); return icon; }
	//! \return ikona dla g��wnej ga��zi z momentami
	const QIcon& getRootMomentsIcon() { static QIcon icon(QString::fromUtf8(":/mde/icons/momentSmall.png")); return icon; }
	//! \return ikona dla g��wnej ga��zi z mocami
	const QIcon& getRootPowersIcon() { static QIcon icon(QString::fromUtf8(":/mde/icons/powerSmall.png"));  return icon; }
	//! tworzy element drzewa z dowolnej kolekcji kana��w danych
	//! \param motion pr�ba pomiarowa
	//! \param rootName nazwa dla rodzica tworzonych element�w
	//! \param rootIcon ikona rodzica
	//! \param itemIcon ikona dla li�ci
	template <class Channel, class Collection>
	core::IHierarchyItemPtr createTBranch(const PluginSubject::MotionConstPtr & motion, const QString& rootName, const QIcon& rootIcon, const QIcon& itemIcon);
	//! dodanie do drzewa element�w z podpi�tymi danymi w postaci wektor�w
	//! \param motion pr�ba pomiarowa
	//! \param collection kolekcja zawieraj�ca wektory
	//! \param name nazwa tworzonego elementu
	//! \param childIcon ikona dla li�ci
	//! \param parentItem rodzic dla tworzonych element�w
	//! \param createContainerItem czy stworzy� element pomi�dzy rodzicem a li��mi
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
	//! \param visualizer wizualizator, kt�ry b�dzie tworzy� serie
	//! \param path �cie�ka dla timeline
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
