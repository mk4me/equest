/********************************************************************
	created:  2010/12/15
	created:  15:12:2010   10:50
	filename: OsgSceneDump.h
	author:	  Piotr Gwiazdowski

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD__OSGSCENEDUMP_H__
#define __HEADER_GUARD__OSGSCENEDUMP_H__

#include <string>
#include <sstream>
#include <osg/NodeVisitor>
#include <osgViewer/Viewer>
#include <osgGA/GUIEventHandler>

class QTreeWidget;
class QTreeWidgetItem;

////////////////////////////////////////////////////////////////////////////////
namespace osgui {
////////////////////////////////////////////////////////////////////////////////

class OsgSceneDumpString : public osg::NodeVisitor
{
private:
    //! Poziom zagnieżdżenia.
    std::string identation;
    //! Strumień wyjściowy.
    std::ostringstream stream;

public:
    std::string getString();
    std::ostringstream& getStream();

    virtual void apply(osg::Group& group);
    virtual void apply(osg::Node& node);
    virtual void apply(osg::Geode& geode);
    virtual void apply(osg::Drawable& drawable);
    virtual void apply(osg::Geometry& geometry);

private:
    void log(osg::Object& object, osg::StateSet* stateset);
    void increaseIdent();
    void decreaseIdent();
};

//------------------------------------------------------------------------------

class OsgSceneDumpFileHandler: public osgGA::GUIEventHandler
{
private:
    std::string filename;
    osg::observer_ptr<osgViewer::Viewer> viewer;

public:
    OsgSceneDumpFileHandler(const std::string& filename) : filename(filename) {}

    virtual bool handle(
        const osgGA::GUIEventAdapter& gea,
        osgGA::GUIActionAdapter&      gaa,
        osg::Object*                  obj,
        osg::NodeVisitor*             nv
        );
};


//------------------------------------------------------------------------------

class OsgSceneDumpQtTree : public osg::NodeVisitor
{
private:
    //! Bieżący rodzic.
    QTreeWidgetItem* currentParent;
    //! Widget.
    QTreeWidget* widget;

public:
    OsgSceneDumpQtTree(QTreeWidget* widget, QTreeWidgetItem* root = NULL);

    virtual void apply(osg::Switch& group);
    virtual void apply(osg::Group& group);
    virtual void apply(osg::Node& node);
    virtual void apply(osg::Geode& geode);
    virtual void apply(osg::Drawable& drawable);
    virtual void apply(osg::Geometry& geometry);

private:
    QTreeWidgetItem* createItem(osg::Object& object, osg::StateSet* stateset);
};

////////////////////////////////////////////////////////////////////////////////
} // namespace osgui
////////////////////////////////////////////////////////////////////////////////


#endif  // __HEADER_GUARD__OSGSCENEDUMP_H__
