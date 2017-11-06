/********************************************************************
    created:  2012/11/19
    created:  19:11:2012   13:52
    filename: ModelCanvasScene.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___MODELCANVASSCENE_H__
#define HEADER_GUARD___MODELCANVASSCENE_H__

#include <QtGui/QGraphicsScene>

class ModelCanvasScene : private QGraphicsScene
{
	Q_OBJECT

public:
	ModelCanvasScene(QObject * parent = 0);
	ModelCanvasScene(const QRectF & sceneRect, QObject * parent = 0);
	ModelCanvasScene(qreal x, qreal y, qreal width, qreal height, QObject * parent = 0);

	void addNode()

};

#endif	//	HEADER_GUARD___MODELCANVASSCENE_H__
