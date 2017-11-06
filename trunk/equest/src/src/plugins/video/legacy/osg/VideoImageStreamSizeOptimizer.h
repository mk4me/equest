/********************************************************************
	created:  2010/07/08
	created:  8:7:2010   13:33
	filename: StreamsViewOSGWidget.h
	author:	  Piotr Gwiazdowski
	
	purpose:  
*********************************************************************/
#ifndef __HEADER_GUARD__STREAMSVIEWOSGWIDGET_H__
#define __HEADER_GUARD__STREAMSVIEWOSGWIDGET_H__

#include <vector>
#include <osg/NodeCallback>
#include <osg/Referenced>
#include <osg/Image>
#include "VideoImageStream.h"

class VideoImageStreamSizeOptimizer : public osg::NodeCallback
{
public:
    class Client : public osg::Referenced
    {
    public:
        virtual ~Client() {}
        //! \return Rozmiar obrazka.
        virtual osg::Vec2 getDesiredImageSize() = 0;
        //! Reakcja na zmianę rozmiaru obrazka.
        virtual void onImageResized(VideoImageStreamSizeOptimizer* optimizer, int prevS, int prevT) = 0;
        //! \return Czy można jeszcze korzystać z tego callbacka?
        virtual bool isValid() = 0;
    };

private:
    //! Lista kontrolowanych widgetów.
    std::vector<osg::ref_ptr<Client> > clients;
    //! Kontrolowany strumień.
    osg::ref_ptr<osg::Image> image;
    //! Zmienna którą można podpiąć do shaderów; zawsze będzie miała aktualny rozmiar.
    osg::ref_ptr<osg::Uniform> imageSize;

public:
    VideoImageStreamSizeOptimizer( osg::Uniform* imageSize = NULL);

public:
    //! \return Widgety.
    std::vector<osg::ref_ptr<Client> >& getClients()
    { 
        return clients;
    }
    //! \return Widgety.
    const std::vector<osg::ref_ptr<Client> >& getClients() const
    { 
        return clients;
    }
    //! \return
    osg::Uniform* getImageSize()
    { 
        return imageSize;
    }
    //! \return
    osg::Image* getImage()
    { 
        return image;
    }
    //!
    void setImage(osg::Image* image);

    //! Odświeża rozmiar strumienia.
    void refreshSize();

// osg::NodeCallback
public:
    //! Wywołuje refreshSize.
    virtual void operator()(osg::Node* node, osg::NodeVisitor* nv);
};

#endif  // __HEADER_GUARD__STREAMSVIEWOSGWIDGET_H__
