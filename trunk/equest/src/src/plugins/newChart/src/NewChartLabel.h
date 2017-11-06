/********************************************************************
	created:	2012/05/28
	created:	28:5:2012   21:10
	filename: 	NewChartLabel.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_NEW_CHART__NEWCHARTLABEL_H__
#define HEADER_GUARD_NEW_CHART__NEWCHARTLABEL_H__

//#include <plugins/c3d/C3DChannels.h>
#include "NewChartDot.h"

class NewChartLabel;
class NewChartVerticalsConnection : public QwtPlotItem
{
public:
	//! styl połączeń między wskazywanymi punktami a etykietą
	enum ConnectionStyle
	{
		Simple,       //!< proste strzałki
		Horizontal,   //!< strzałki łamane, najpierw poziome
		Vertical      //!< strzałki łamane, najpierw pionowe
	};

public:
	NewChartVerticalsConnection(ConnectionStyle connectionStyle, NewChartLabel* label);
	virtual ~NewChartVerticalsConnection() {}
	virtual void draw(QPainter *painter,
					  const QwtScaleMap &xMap, const QwtScaleMap &yMap,
					  const QRectF &canvasRect) const;

	//! Emulacja rtti obsługiwana przez Qwt
	virtual int rtti() const { return QwtPlotItem::Rtti_PlotUserItem; }

	NewChartDotConstPtr getPoint1() const { return point1; }
	void setPoint1(NewChartDotConstPtr val) { point1 = val; }
	NewChartDotConstPtr getPoint2() const { return point2; }
	void setPoint2(NewChartDotConstPtr val) { point2 = val; }
	ConnectionStyle getConnectionStyle() const { return connectionStyle; }
	void setConnectionStyle(ConnectionStyle val) { connectionStyle = val; }

private:
	//! Rysowanie połączenia
	//! \param painter obiekt paintera
	//! \param box obszar zajmowany przez etykietę
	//! \param transformedTo już przetransformowany punkt, który ma być przyłączony
	//! \param style styl łączenia (prosty, horyzontalny, ...)
	//! \param arrowOutside czy strzałki mają być do wewnątrz, czy na zewnątrz
	void drawConnection(QPainter* painter, const QRect& box, const QPoint& transformedTo, ConnectionStyle style, bool arrowOutside = false) const;
	//! Rysowanie strzałki
	//! \param painter obiekt paintera
	//! \param transformedFrom już przetransformowany punkt, z którego wychodzi strzałka
	//! \param transformedTo już przetransformowany punkt, do którego prowadzi strzałka
	//! \param outside czy strzałki mają być do wewnątrz, czy na zewnątrz
	void drawArrow(QPainter* painter, const QPoint& transformedFrom, const QPoint& transformedTo, bool outside = false) const;

private:
	ConnectionStyle connectionStyle;
	//! pierwszy podpięty punkt
	NewChartDotConstPtr point1;
	//! drugi podpięty punkt (może być nullptr)
	NewChartDotConstPtr point2;
	NewChartLabel* label;
};
typedef NewChartVerticalsConnection* NewChartVerticalsConnectionPtr;
typedef const NewChartVerticalsConnection* NewChartVerticalsConnectionConstPtr;

//! Klasa reprezentuje etykietę na wykresie
class NewChartLabel : public QwtPlotItem
{
public:
    //! Konstruktor
    //! \param text tekst etykiety
    //! \param shift domyślne przesunięcie (między wsk. punktem i etykietą)
    //! \param size rozmiar etykiety
    NewChartLabel(const QString& text = QString(""), const QPoint& shift = QPoint(20, 20), const QPoint& size = QPoint(80, 35), bool closeable = true);

public:
    //! Odrysowanie etykiety
    //! \param painter obiekt paintera
    //! \param xMap transformacja X
    //! \param yMap transformacja Y
    //! \param canvasRect obszar do rysowania
    virtual void draw( QPainter *painter, const QwtScaleMap &xMap, const QwtScaleMap &yMap, const QRectF &canvasRect ) const;

	virtual QRectF boundingRect() const;

	virtual void setVisible(bool visible);

	QRect getTextRect(QPainter * painter, const QPoint &transformed1) const;

	//! Sprawdza, czy punkt znajduje się wewnątrz etykiety
    //! \param transformedPoint przetransformowany punkt
    //! \param curve krzywa, wykorzystuje się jej transformację
    bool isInsideLabel(const QPoint& transformedPoint, const QwtPlotCurve* curve) const;
	//! Sprawdza, czy punkt znajduje się nad 'x'
	//! \param transformedPoint przetransformowany punkt
	bool isInsideClose(const QPoint& transformedPoint) const;
    //! \return tekst etykiety
    const QString& getText() const { return text; }
    //! ustawia tekst etykiety
    //! \param val nowy tekst
    void setText(const QString& val) { text = val; }
    //! \return przeusniecie między wskazywanym punktem, a etykietą
    const QPoint& getShift() const { return shift; }
    //! ustawia przeusniecie między wskazywanym punktem, a etykietą
    //! \param val nowe przesunięcie
    void setShift(const QPoint& val) { shift = val; }
    //! \return pierwszy punkt podpięty pod etykietę
    NewChartDotConstPtr getPoint1() const { return points->getPoint1(); }
    //! \return przetransformowana już na podstawie krzywej pozycja pierwszego punktu
    QPoint getPoint1Transformed(const QwtPlotCurve* curve) const;
    //! ustawia pierwszy punkt podpięty pod etykietę
    //! \param val ustawiany punkt
    void setPoint1(NewChartDotConstPtr val) { points->setPoint1(val); }
    //! \return drugi punkt podpięty pod etykietę (może być nullptr)
	NewChartDotConstPtr getPoint2() const { return points->getPoint2(); }
    //! \return przetransformowana już na podstawie krzywej pozycja drugiego punktu
    QPoint getPoint2Transformed(const QwtPlotCurve* curve) const;
    //! ustawia drugi punkt podpięty pod etykietę 
    //! \param val drugi punkt (może być nullptr)
    void setPoint2(NewChartDotConstPtr val) { points->setPoint2(val); }
    //! \return pędzel, który jest używany do rysowania etykiety
    const QBrush& getBrush() const { return brush; }
    //! ustawia pędzel
    //! \param val pędzel, który będzie używany do rysowania etykiety
    void setBrush(const QBrush& val) { brush = val; }
    //! \return pióro, który jest używane do rysowania etykiety
    const QPen& getPen() const { return pen; }
    //! ustawia pióro
    //! \param val pióro, który będzie używane do rysowania etykiety
    void setPen(const QPen& val) { pen = val; }
    //! Emulacja rtti wykorzystywana przez Qwt
    virtual int rtti() const { return QwtPlotItem::Rtti_PlotUserItem; }
    //! łączy punkt z etykietą
    //! \param dot podłączany punkt
    //! \param style styl połączenia
	void connectDot(NewChartDotConstPtr dot, NewChartVerticalsConnection::ConnectionStyle style = NewChartVerticalsConnection::Simple);
    //! łączy dwa punkty z etykietą
    //! \param point1 pierwszy podłączany punkt
    //! \param point2 drugi podłączany punkt
    //! \param style styl połączenia
	void connectDots(NewChartDotConstPtr point1, NewChartDotConstPtr point2, NewChartVerticalsConnection::ConnectionStyle style);
    //! tworzy a nastepnie łączy dwa punkty z etykietą
    //! \param point1 pierwszy podłączany punkt
    //! \param point2 drugi podłączany punkt
    //! \param style styl połączenia
	void connectDots(const QPointF& point1, const QPointF& point2, NewChartVerticalsConnection::ConnectionStyle style);
	QRect getBoxRect(QPainter * painter, QPoint &transformed1) const;
	QRect getBoxRect(const QRect& textRect) const;
	NewChartVerticalsConnectionPtr getConnectionsItem() const { return points; }

private:
	void drawX(QPainter * painter, const QRect& r) const;

private:
    //! tekst etykiety
    QString text;
    //! rozmiar etykiety
    QPoint size;
    //! przesunięcie między wskazywanym punktem a etykietą
    QPoint shift;
    //! pióro 
    QPen pen;
    //! pędzel 
    QBrush brush;
   
	NewChartVerticalsConnectionPtr points;
	mutable QRectF boundingR;
	bool closeable;
};
typedef NewChartLabel* NewChartLabelPtr;
typedef const NewChartLabel* NewChartLabelConstPtr;




#endif
