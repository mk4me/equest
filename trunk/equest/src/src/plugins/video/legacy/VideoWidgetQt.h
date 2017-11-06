/********************************************************************
	created:  2010/10/06
	created:  6:10:2010   15:36
	filename: VideoWidgetQt.h
	author:	  Piotr Gwiazdowski
	
	purpose:  
*********************************************************************/
#ifndef __HEADER_GUARD__VIDEOWIDGETQT_H__
#define __HEADER_GUARD__VIDEOWIDGETQT_H__

#include <vector>
#include <QtGui/QWidget>
#include <QtGui/QBitmap>
#include "ui_VideoWidgetQt.h"
#include <plugins/video/core/VideoStream.h>


class VideoWidgetQt : public QWidget, public Ui::VideoWidgetQt
{
private:
    //! Strumienie video.
    std::vector<video::VideoStream*> streams;
    //! Bitmapki na grafikę ze strumeni.
    std::vector<QImage*> images;
    //! Bufory na dane ze srumieni.
    std::vector<video::Picture> buffers;
    //!
    QGraphicsView* views[4];

public:
    //! 
    VideoWidgetQt();
    //! 
    virtual ~VideoWidgetQt();

public:
    //! 
    //! \param files
    void init( const std::vector<std::string> &files );
    virtual void resizeEvent(QResizeEvent *event);
};


#endif  // __HEADER_GUARD__VIDEOWIDGETQT_H__
