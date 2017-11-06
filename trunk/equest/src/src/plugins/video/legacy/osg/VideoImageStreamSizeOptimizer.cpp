#include "../VideoPCH.h"
#include <boost/foreach.hpp>
#include <core/Log.h>
#include <core/OsgWidgetUtils.h>
#include "VideoImageStreamSizeOptimizer.h"

#undef  min
#undef  max

VideoImageStreamSizeOptimizer::VideoImageStreamSizeOptimizer(osg::Uniform* imageSize /*= NULL*/) : 
imageSize( imageSize ? imageSize : new osg::Uniform() )
{
}

void VideoImageStreamSizeOptimizer::operator()( osg::Node* node, osg::NodeVisitor* nv )
{
    refreshSize();
    traverse(node,nv);
}

void VideoImageStreamSizeOptimizer::refreshSize()
{
    UTILS_ASSERT(image);
    int prevWidth = image->s();
    int prevHeight = image->t();
    // określenie maksymalnej szerokości
    float maxWidth = 0;
    for ( size_t i = 0; i < clients.size(); ++i ) {
        Client* client = clients[i];
        if ( client->isValid() ) {
            maxWidth = std::max(client->getDesiredImageSize().x(), maxWidth);
        }
    }
    
    // TODO: przenieść do metody wirtualnej?
    if ( video::osgPlugin::VideoImageStream* videoImageStream = dynamic_cast<video::osgPlugin::VideoImageStream*>(image.get()) ) {
        // aktualizacja wymiarów
        videoImageStream->setMaxWidth( int(ceil(maxWidth)) );
    }

    // aktualizacja rozmiaru
    if ( prevWidth != image->s() ) {
        imageSize->set( image->s(), image->t() );
        BOOST_FOREACH(Client* client, clients) {
            if ( client->isValid() ) {
                client->onImageResized(this, prevWidth, prevHeight);
            }
        }
    }
}

void VideoImageStreamSizeOptimizer::setImage( osg::Image* image )
{
    this->image = image;
    if ( image ) {
        imageSize->set( image->s(), image->t() );
    }
}
