#include "PCH.h"
#include "SchemeDrawerContainer.h"

SchemeDrawerContainer::SchemeDrawerContainer() :
    nodes(new osg::Group()),
    wasInit(false)
{
}

osg::ref_ptr<osg::Node> SchemeDrawerContainer::getNode()
{
    return nodes;
}

//void SchemeDrawerContainer::init( VisualizationSchemeConstPtr scheme )
//{
//    OsgSchemeDrawer::init(scheme);
//    wasInit = true;
//    BOOST_FOREACH(OsgSchemeDrawerPtr drawer, drawers) {
//        if (!drawer->isInitialized()) {
//            drawer->init(scheme);
//        }
//        nodes->addChild(drawer->getNode());
//    }
//}

void SchemeDrawerContainer::addDrawer( IOsgSchemeDrawerPtr drawer )
{
    UTILS_ASSERT(!wasInit);
    drawers.insert(drawer);
}

void SchemeDrawerContainer::removeDrawer( IOsgSchemeDrawerPtr drawer )
{
    UTILS_ASSERT(false);
    // TODO jeśli byl init to usun z kolekcji i noda
    //      jeśli nie to tylko z kolekcji
    drawers.erase(drawer);
}

void SchemeDrawerContainer::deinit()
{
    BOOST_FOREACH(IOsgSchemeDrawerPtr drawer, drawers) {
        drawer->deinit();
    }
}

void SchemeDrawerContainer::draw()
{
    BOOST_FOREACH(IOsgSchemeDrawerPtr drawer, drawers) {
        drawer->draw();
    }
}

void SchemeDrawerContainer::update()
{
    BOOST_FOREACH(OsgSchemeDrawerPtr drawer, drawers) {
        drawer->update();
    }
}

SchemeDrawerContainer::const_range SchemeDrawerContainer::getDrawers() const
{
    return boost::make_iterator_range(drawers.cbegin(), drawers.cend());
}

SchemeDrawerContainer::range SchemeDrawerContainer::getDrawers()
{
    return boost::make_iterator_range(drawers.begin(), drawers.end());
}


