/********************************************************************
	created:  2010/11/12
	created:  12:11:2010   16:29
	filename: StreamOsgWidget.h
	author:	  Piotr Gwiazdowski
	
	purpose:  
*********************************************************************/
#ifndef __HEADER_GUARD__StreamOsgWidget_H__
#define __HEADER_GUARD__StreamOsgWidget_H__

#include <string>
#include <osg/ref_ptr>
#include <osgWidget/Widget>
#include <vidlib/PixelFormat.h>

////////////////////////////////////////////////////////////////////////////////
namespace video {
////////////////////////////////////////////////////////////////////////////////

/**
 *	Widget dostowany do wymagań strumieni video. Obsługuje niestandardowe
 *  formaty pikseli.
 */
class StreamOsgWidget : public osgWidget::Widget
{
    //! Program używany w przypadku konwersji yuv w karcie graficznej.
    osg::ref_ptr<osg::Program> yuvProgram;
    //! Shader używany dla texture_rectów.
    osg::ref_ptr<osg::Shader> yuvTextureRectShader;
    //! Shader używany dla texture2D.
    osg::ref_ptr<osg::Shader> yuvTexture2DShader;
    //! Format pikseli.
    vidlib::VIDLIB_PixelFormat format;

public: 
    //! \param name Nazwa widgetu.
    StreamOsgWidget(const std::string& name);
    //! 
    virtual ~StreamOsgWidget();

public:
    //! \return
    vidlib::VIDLIB_PixelFormat getPixelFormat() const;
    //! \param format
    void setPixelFormat(vidlib::VIDLIB_PixelFormat format);
    //! \return Czy używany jest texture_rectangle
    bool isUsingTextureRectangle() const;
    //! \return
    osg::Shader* getYuvTextureRectShader() const;
    //! \param yuvTextureRectShader
    void setYuvTextureRectShader(osg::Shader* yuvTextureRectShader);
    //! \return
    osg::Shader* getYuvTexture2DShader() const;
    //! \param yuvTexture2DShader
    void setYuvTexture2DShader(osg::Shader* yuvTexture2DShader);
    //!
    void refreshShaders();

private:
    //! \param useTextureRect
    //! \param format
    void refreshShaders(bool useTextureRect, vidlib::VIDLIB_PixelFormat format);
};

////////////////////////////////////////////////////////////////////////////////
} // namespace video
////////////////////////////////////////////////////////////////////////////////




#endif  // __HEADER_GUARD__StreamOsgWidget_H__
