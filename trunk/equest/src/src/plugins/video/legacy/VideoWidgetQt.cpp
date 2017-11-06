#include "VideoPCH.h"
#include <stdint.h>
#include "VideoWidgetQt.h"


VideoWidgetQt::VideoWidgetQt()
{
    setupUi(this);
    views[0] = view0;
    views[1] = view1;
    views[2] = view2;
    views[3] = view3;
}

VideoWidgetQt::~VideoWidgetQt()
{
    for (size_t i = 0; i < streams.size(); ++i) {
        delete streams[i];
        delete images[i];
        buffers[i].free();
    }
}

void VideoWidgetQt::init( const std::vector<std::string> &files )
{
    for (size_t i = 0; i < files.size(); ++i) {
        video::VideoStream* stream = NULL;
        try {
            stream = video::VideoStream::create(video::VideoStream::FFmpeg, files[i]);
        } catch ( video::VideoError& error ) {
            
        }
        // TODO
        // może przekroczyć zakres
        if ( stream ) {
            video::Picture picture = video::Picture::create( stream->getWidth(), stream->getHeight(), video::PixelFormatRGB24 );
            stream->setTime(0);
            stream->getFrame(picture);

            QImage* image = new QImage(picture.data, picture.width, picture.height, QImage::Format_RGB888);

            QGraphicsScene* scene = new QGraphicsScene();
            views[streams.size()]->setScene(scene);
            QGraphicsPixmapItem* item = new QGraphicsPixmapItem(QPixmap::fromImage(*image));
            scene->addItem(item);
            views[streams.size()]->fitInView(item, Qt::KeepAspectRatio);


            //QPixmap* pixmap = new QPixmap(picture.width, picture.height, "rgb", );
            //pixmap->loadFromData()
            streams.push_back(stream);
            buffers.push_back(picture);
            images.push_back(image);
            //images.push_back(bitmap);



        }
    }
}

void VideoWidgetQt::resizeEvent( QResizeEvent *event )
{
    QWidget::resizeEvent(event);
    for (int i = 0; i < 4; ++i) {
        if ( views[i]->items().size() ) {
            views[i]->fitInView(  views[i]->items().first(), Qt::KeepAspectRatio );
        }
    }
}
