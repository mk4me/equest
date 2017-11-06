/********************************************************************
	created:	2011/10/12
	created:	12:10:2011   12:28
	filename: 	TreeBuilder.h
	author:		Wojciech Kniec

	purpose:
*********************************************************************/

#ifndef HEADER_GUARD_NEWCOMMUNICATION__TREEBUILDER_H__
#define HEADER_GUARD_NEWCOMMUNICATION__TREEBUILDER_H__

#include <plugins/subject/Types.h>
#include <plugins/subject/SubjectDataFilters.h>
#include <corelib/IHierarchyItem.h>
#include <QtGui/QIcon>
#include <corelib/HierarchyHelper.h>
#include <plugins/hmdbCommunication/IHierarchyPerspective.h>
#include <plugins/c3d/C3DCollections.h>


class MotionPerspective : public hmdbCommunication::IHierarchyPerspective
{
public:
	virtual ~MotionPerspective() {}

    virtual core::IHierarchyItemPtr getPerspective( PluginSubject::SubjectPtr subject );
private:
    bool hasValidData( PluginSubject::SubjectPtr subject );

};

template <typename Collection>
void push_not_null(Collection& vec, const typename Collection::value_type& t)
{
	if (t) {
		vec.push_back(t);
	}
}

class TreeBuilder
{
public:  
    //! Tworzy drzewo na podstawie dostarczony sesji
    //! \param rootItemName nazwa korzenia
    //! \param sessions dostarczone sesje
    //! \param dataFilter obiekt filtruj¹cy sesje
    //! \return stworzony element
    static core::IHierarchyItemPtr  createTree(const QString& rootItemName, const core::ConstVariantsList& sessions, const SubjectHierarchyFilterPtr & dataFilter);

    static core::IHierarchyItemPtr createTree(const QString& rootItemName, const core::ConstVariantsList& sessions);

    static core::IHierarchyItemPtr createTree(const QString& rootItemName, const PluginSubject::SubjectConstPtr& subject);
    static core::IHierarchyItemPtr createTree(const QString& rootItemName, const PluginSubject::SubjectConstPtr& subject, const SubjectHierarchyFilterPtr & dataFilter);
    //! Tworzy ga³¹Ÿ dla markerów
    //! \param motion próba pomiarowa
    //! \param rootName nazwa dla rodzica tworzonych elementów
    //! \param rootIcon ikona rodzica
    //! \param itemIcon ikona dla liœci
    static core::IHierarchyItemPtr createMarkersBranch( const PluginSubject::MotionConstPtr & motion, const QString& rootName, const QIcon& rootIcon, const QIcon& itemIcon, const std::string & );
    //! Tworzy ga³¹Ÿ dla stawów
    //! \param motion próba pomiarowa
    //! \param rootName nazwa dla rodzica tworzonych elementów
    //! \param rootIcon ikona rodzica
    //! \param itemIcon ikona dla liœci
    static core::IHierarchyItemPtr createJointsBranch( const PluginSubject::MotionConstPtr & motion, const QString& rootName, const QIcon& rootIcon, const QIcon& itemIcon, const std::string & );
    //! Tworzy ga³¹Ÿ dla nagrañ video
    //! \param motion próba pomiarowa
    //! \param rootName nazwa dla rodzica tworzonych elementów
    //! \param rootIcon ikona rodzica
    //! \param itemIcon ikona dla liœci
    static core::IHierarchyItemPtr createVideoBranch( const PluginSubject::MotionConstPtr & motion, const QString& rootName, const QIcon& rootIcon, const QIcon& itemIcon, const std::string & );
    //! Tworzy ga³¹Ÿ dla danych grf
    //! \param motion próba pomiarowa
    //! \param rootName nazwa dla rodzica tworzonych elementów
    //! \param rootIcon ikona rodzica
    //! \param itemIcon ikona dla liœci
    static core::IHierarchyItemPtr createGRFBranch( const PluginSubject::MotionConstPtr & motion, const QString& rootName, const QIcon& rootIcon, const QIcon& itemIcon, const std::string & );
    //! Tworzy ga³¹Ÿ dla danych emg
    //! \param motion próba pomiarowa
    //! \param rootName nazwa dla rodzica tworzonych elementów
    //! \param rootIcon ikona rodzica
    //! \param itemIcon ikona dla liœci
    static core::IHierarchyItemPtr createEMGBranch( const PluginSubject::MotionConstPtr & motion, const QString& rootName, const QIcon& rootIcon, const QIcon& itemIconconst, const std::string & conf );
    //! Tworzy ga³¹Ÿ dla 
    //! \param motion próba pomiarowa
    //! \param rootName nazwa dla rodzica tworzonych elementów
    //! \param rootIcon ikona rodzica
    //! \param itemIcon ikona dla liœci
    static core::IHierarchyItemPtr createForcesBranch( const PluginSubject::MotionConstPtr & motion, const QString& rootName, const QIcon& rootIcon, const QIcon& itemIcon, const std::string & );
    //! Tworzy ga³¹Ÿ dla momentów
    //! \param motion próba pomiarowa
    //! \param rootName nazwa dla rodzica tworzonych elementów
    //! \param rootIcon ikona rodzica
    //! \param itemIcon ikona dla liœci
    static core::IHierarchyItemPtr createMomentsBranch( const PluginSubject::MotionConstPtr & motion, const QString& rootName, const QIcon& rootIcon, const QIcon& itemIcon, const std::string & );
    //! Tworzy ga³¹Ÿ dla mocy
    //! \param motion próba pomiarowa
    //! \param rootName nazwa dla rodzica tworzonych elementów
    //! \param rootIcon ikona rodzica
    //! \param itemIcon ikona dla liœci
    static core::IHierarchyItemPtr createPowersBranch( const PluginSubject::MotionConstPtr & motion, const QString& rootName, const QIcon& rootIcon, const QIcon& itemIcon, const std::string & );

    //! \return ikona dla ga³êzi z markerami
    static const QIcon& getMarkersIcon( )      { static QIcon icon(QString::fromUtf8(":/mde/icons/markerSmall.png")); return icon; }
    //! \return ikona dla ga³êzi ze stawami
    static const QIcon& getJointsIcon( )       { static QIcon icon(QString::fromUtf8(":/mde/icons/jointSmall.png"));  return icon; }
    //! \return ikona dla ga³êzi z nagraniami
    static const QIcon& getVideoIcon( )        { static QIcon icon(QString::fromUtf8(":/mde/icons/videoSmall.png"));  return icon; }
    //! \return ikona dla ga³êzi z danymi grf
    static const QIcon& getGRFIcon( )          { static QIcon icon(QString::fromUtf8(":/mde/icons/grfSmall.png"));    return icon; }
    //! \return ikona dla ga³êzi z danymi emg
    static const QIcon& getEMGIcon( )          { static QIcon icon(QString::fromUtf8(":/mde/icons/emg1Small.png"));   return icon; }
    //! \return ikona dla ga³êzi z si³ami
    static const QIcon& getForcesIcon( )       { static QIcon icon(QString::fromUtf8(":/mde/icons/forcesSmall.png")); return icon; }
    //! \return ikona dla ga³êzi z momentami
    static const QIcon& getMomentsIcon( )      { static QIcon icon(QString::fromUtf8(":/mde/icons/momentSmall.png")); return icon; }
    //! \return ikona dla ga³êzi z mocami
    static const QIcon& getPowersIcon( )       { static QIcon icon(QString::fromUtf8(":/mde/icons/powerSmall.png"));  return icon; }

    //! \return ikona dla g³ównej ga³êzi z markerami
    static const QIcon& getRootMarkersIcon( )  { static QIcon icon(QString::fromUtf8(":/mde/icons/markerSmall.png")); return icon; }
    //! \return ikona dla g³ównej ga³êzi ze stawami
    static const QIcon& getRootJointsIcon( )   { static QIcon icon(QString::fromUtf8(":/mde/icons/jointSmall.png"));  return icon; }
    //! \return ikona dla g³ównej ga³êzi z nagraniami
    static const QIcon& getRootVideoIcon( )    { static QIcon icon(QString::fromUtf8(":/mde/icons/videoSmall.png"));  return icon; }
    //! \return ikona dla g³ównej ga³êzi z danymi grf
    static const QIcon& getRootGRFIcon( )      { static QIcon icon(QString::fromUtf8(":/mde/icons/grfSmall.png"));    return icon; }
    //! \return ikona dla g³ównej ga³êzi z danymi emg
    static const QIcon& getRootEMGIcon( )      { static QIcon icon(QString::fromUtf8(":/mde/icons/emg1Small.png"));   return icon; }
    //! \return ikona dla g³ównej ga³êzi z si³ami
    static const QIcon& getRootForcesIcon( )   { static QIcon icon(QString::fromUtf8(":/mde/icons/forcesSmall.png")); return icon; }
    //! \return ikona dla g³ównej ga³êzi z momentami
    static const QIcon& getRootMomentsIcon( )  { static QIcon icon(QString::fromUtf8(":/mde/icons/momentSmall.png")); return icon; }
    //! \return ikona dla g³ównej ga³êzi z mocami
    static const QIcon& getRootPowersIcon( )   { static QIcon icon(QString::fromUtf8(":/mde/icons/powerSmall.png"));  return icon; }


private:
    //! tworzy element drzewa z dowolnej kolekcji kana³ów danych
    //! \param motion próba pomiarowa
    //! \param rootName nazwa dla rodzica tworzonych elementów
    //! \param rootIcon ikona rodzica
    //! \param itemIcon ikona dla liœci
    template <class Channel, class Collection>
    static core::IHierarchyItemPtr createTBranch( const PluginSubject::MotionConstPtr & motion, const QString& rootName, const QIcon& rootIcon, const QIcon& itemIcon );
    //! dodanie do drzewa elementów z podpiêtymi danymi w postaci wektorów
    //! \param motion próba pomiarowa
    //! \param collection kolekcja zawieraj¹ca wektory
    //! \param name nazwa tworzonego elementu
    //! \param childIcon ikona dla liœci
    //! \param parentItem rodzic dla tworzonych elementów
    //! \param createContainerItem czy stworzyæ element pomiêdzy rodzicem a liœæmi
    template <class Channel, class CollectionPtr>
    static void tryAddVectorToTree(const PluginSubject::MotionConstPtr & motion, const CollectionPtr & collection, const std::string& name, const QIcon& childIcon,  core::IHierarchyItemPtr parentItem, bool createContainerItem = true );

	static c3dlib::EventsCollectionConstPtr getEvents(const PluginSubject::MotionConstPtr & motion);

    static core::HierarchyHelperPtr allTFromSession( const std::string& channelName, PluginSubject::SessionConstPtr s, int channelNo );
    static core::HierarchyHelperPtr createNormalized( core::VariantConstPtr wrapper, PluginSubject::MotionConstPtr motion, c3dlib::C3DParser::IEvent::Context context );
    static core::HierarchyHelperPtr createNormalizedFromAll( const std::string& channelName, PluginSubject::SessionConstPtr s, c3dlib::C3DParser::IEvent::Context context );
    static QString createDescription( PluginSubject::MotionConstPtr motion);
};


#endif
