/********************************************************************
	created:  2011/01/07
	created:  7:1:2011   16:26
	filename: DisableableRenderer.h
	author:	  Piotr Gwiazdowski
	
	purpose:  
*********************************************************************/
#ifndef __HEADER_GUARD__DISABLEABLERENDERER_H__
#define __HEADER_GUARD__DISABLEABLERENDERER_H__

#include <osgViewer/Renderer>

////////////////////////////////////////////////////////////////////////////////
namespace core {
////////////////////////////////////////////////////////////////////////////////

class DisableableRenderer : public osgViewer::Renderer
{
private:
    bool renderEnabled;

public:
    //! \param camera
    DisableableRenderer( osg::Camera* camera ) :
    osgViewer::Renderer(camera), renderEnabled(true)
    {
    }

    //! \return Czy renderowanie jest dostępne?
    bool isRenderingEnabled() const
    { 
        return renderEnabled;
    }
    //! \param renderEnabled Czy renderowanie jest dostępne?
    void setRenderingEnabled(bool renderEnabled) 
    { 
        this->renderEnabled = renderEnabled; 
    }

    //! \param ctx
    virtual void operator ()( osg::GraphicsContext* ctx );
};

////////////////////////////////////////////////////////////////////////////////
} // namespace core
////////////////////////////////////////////////////////////////////////////////


#endif  // __HEADER_GUARD__DISABLEABLERENDERER_H__
