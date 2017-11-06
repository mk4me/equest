#include "PCH.h"
#include <osgui/OsgWidgetUtils.h>
#include <utils/Debug.h>

////////////////////////////////////////////////////////////////////////////////
namespace osgui {
////////////////////////////////////////////////////////////////////////////////

osg::Texture* getTexture(osgWidget::Widget* widget)
{
    if (widget) {
        return getTexture(widget->getStateSet());
    }
    return NULL;
}

osg::Texture* getTexture(osg::StateSet* state)
{
    if (state) {
        return dynamic_cast<osg::Texture*>(state->getTextureAttribute(0, osg::StateAttribute::TEXTURE));
    }
    return NULL;
}

const osg::Texture* getTexture(const osgWidget::Widget* widget)
{
    if (widget) {
        return getTexture(widget->getStateSet());
    }
    return NULL;
}

const osg::Texture* getTexture(const osg::StateSet* state)
{
    if (state) {
        return dynamic_cast<const osg::Texture*>(state->getTextureAttribute(0, osg::StateAttribute::TEXTURE));
    }
    return NULL;
}

osg::TextureRectangle* getTextureRect(osg::StateSet* state)
{
    return dynamic_cast<osg::TextureRectangle*>(getTexture(state));
}

const osg::TextureRectangle* getTextureRect(const osg::StateSet* state)
{
    return dynamic_cast<const osg::TextureRectangle*>(getTexture(state));
}

osg::TextureRectangle* getTextureRect(osgWidget::Widget* widget)
{
    return dynamic_cast<osg::TextureRectangle*>(getTexture(widget));
}

const osg::TextureRectangle* getTextureRect(const osgWidget::Widget* widget)
{
    return dynamic_cast<const osg::TextureRectangle*>(getTexture(widget));
}

osg::Image* getImage( osgWidget::Widget* widget )
{
    if ( widget ) {
        return getImage( getTexture(widget) );
    }
    return NULL;
}

osg::Image* getImage( osg::StateSet* state )
{
    if ( state ) {
        return getImage( getTexture(state) );
    }
    return NULL;
}

osg::Image* getImage( osg::Texture* texture )
{
    if ( texture ) {
        return texture->getImage(0);
    }
    return NULL;
}

const osg::Image* getImage( const osgWidget::Widget* widget )
{
    if ( widget ) {
        return getImage( getTexture(widget) );
    }
    return NULL;
}

const osg::Image* getImage( const osg::StateSet* state )
{
    if ( state ) {
        return getImage( getTexture(state) );
    }
    return NULL;
}

const osg::Image* getImage( const osg::Texture* texture )
{
    if ( texture ) {
        return texture->getImage(0);
    }
    return NULL;
}


void correctTexCoords(osgWidget::Widget* widget)
{
    correctTexCoords( widget, getImage(widget) );
}

void correctTexCoords(osgWidget::Widget* widget, osg::Image* image)
{
    switch ( image->getOrigin() ) {
    case osg::Image::TOP_LEFT:
        flipTexCoordsVertically(widget);
        break;
    case osg::Image::BOTTOM_LEFT:
        break;
    default:
        UTILS_ASSERT(false, "Not implemented.");
        break;
    }
}

void flipTexCoordsVertically(osgWidget::Widget* widget)
{
    // czy trzeba zrobić flipa?
    osgWidget::TexCoord temp = widget->getTexCoord(osgWidget::Widget::UPPER_LEFT);
    widget->setTexCoord(widget->getTexCoord(osgWidget::Widget::LOWER_LEFT), osgWidget::Widget::UPPER_LEFT);
    widget->setTexCoord(temp, osgWidget::Widget::LOWER_LEFT);
    temp = widget->getTexCoord(osgWidget::Widget::UPPER_RIGHT);
    widget->setTexCoord(widget->getTexCoord(osgWidget::Widget::LOWER_RIGHT), osgWidget::Widget::UPPER_RIGHT);
    widget->setTexCoord(temp, osgWidget::Widget::LOWER_RIGHT);
}

////////////////////////////////////////////////////////////////////////////////
} // namespace osgui
////////////////////////////////////////////////////////////////////////////////
