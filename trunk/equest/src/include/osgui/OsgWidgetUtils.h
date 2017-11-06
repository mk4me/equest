/********************************************************************
	created:  2010/12/22
	created:  22:12:2010   17:01
	filename: OsgWidgetUtils.h
	author:	  Piotr Gwiazdowski
	
	purpose:  
*********************************************************************/
#ifndef HEADER_GUARD__OSGWIDGETUTILS_H__
#define HEADER_GUARD__OSGWIDGETUTILS_H__

#include <osg/StateSet>
#include <osg/Image>
#include <osg/TextureRectangle>
#include <osg/Texture>
#include <osgWidget/Widget>


////////////////////////////////////////////////////////////////////////////////
namespace osgui {
////////////////////////////////////////////////////////////////////////////////

osg::Texture* getTexture(osgWidget::Widget* widget);
osg::Texture* getTexture(osg::StateSet* state);
const osg::Texture* getTexture(const osgWidget::Widget* widget);
const osg::Texture* getTexture(const osg::StateSet* state);

osg::TextureRectangle* getTextureRect(osgWidget::Widget* widget);
osg::TextureRectangle* getTextureRect(osg::StateSet* state);
const osg::TextureRectangle* getTextureRect(const osgWidget::Widget* widget);
const osg::TextureRectangle* getTextureRect(const osg::StateSet* state);

osg::Image* getImage(osgWidget::Widget* widget);
osg::Image* getImage(osg::StateSet* state);
osg::Image* getImage(osg::Texture* texture);
const osg::Image* getImage(const osgWidget::Widget* widget);
const osg::Image* getImage(const osg::StateSet* state);
const osg::Image* getImage(const osg::Texture* texture);

void correctTexCoords(osgWidget::Widget* widget);
void correctTexCoords(osgWidget::Widget* widget, osg::Image* image);

void flipTexCoordsVertically(osgWidget::Widget* widget);

////////////////////////////////////////////////////////////////////////////////
} // namespace osgui
////////////////////////////////////////////////////////////////////////////////


#endif  // HEADER_GUARD__OSGWIDGETUTILS_H__
