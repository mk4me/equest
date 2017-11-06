/********************************************************************
    created:  2011/07/18
    created:  18:7:2011   14:05
    filename: ChannelWidget.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD_TIMELINE__CHANNELWIDGET_H__
#define HEADER_GUARD_TIMELINE__CHANNELWIDGET_H__

#include <utils/Debug.h>
#include <QtWidgets/QWidget>
#include <QtWidgets/QFrame>
#include <timelinelib/Model.h>

//! Klasa reprezentująca pojedynczy kanał w czasie - rysuje jego zakres na osi ze względu na parametry begin i end,
//! które stanowią znormalizowane wartości. Root zajmuje zawsze caly dostępny kanał, jego długość jest punktem 
//! odniesienia dla pozostałych kanałów. Zakres początku i końca to [0;1] gdzie początek musi być przed lub równo z końcem
class ChannelWidget : //public QWidget 
    public QFrame 
{

    Q_OBJECT;

private:

    //! Znormalizowana połowa szerokości dla aktualnego czasu
    double currentTimeHalfWidth;

    //! Kolor wypełnienia kanału
    QColor fillColor;

    //! Kolor tła
    QColor backgroundColor;

    //! Kolor markera czasu
    QColor markerColor;

    //! Left margin
    double leftMargin;

    //! Right margin
    double rightMargin;

    timeline::ControllerConstPtr controller;
    timeline::Model::TChannelConstPtr channel;

public:

    ChannelWidget(const timeline::ControllerConstPtr & controller, const timeline::Model::TChannelConstPtr & channel,
        QWidget* parent = 0, Qt::WindowFlags f = 0, double leftMargin = 0, double rightMargin = 0,
        double currentTimeHalfWidth = 0.03);


    //! \return Kolor wypełnienia kanału
    const QColor & getFillColor() const;

    //! \return Kolor tła
    const QColor & getBackgroundColor() const;

    //! \return Kolor znacznika czasu
    const QColor & getMarkerColor() const;

    //! \return Margines z lewej
    double getLeftMargin() const;

    //! \return Margines z prawej
    double getRightMargin() const;

public slots:

    //! \param leftMargin Lewy margines
    //! \param rightMargin Prawy margines
    void setMargins(double leftMargin, double rightMargin);

    //! \param margin Margines z lewej i prawej strony
    void setMargins(double margin);

    //! \param fillColor Kolor wypełniający kanał
    void setFillColor(const QColor & fillColor);

    //! \param backgorundColor Kolor tła
    void setBackgorundColor(const QColor & backgorundColor);

    //! \param backgorundColor Kolor tła
    void setMarkerColor(const QColor & markerColor);

protected:
    //! Reimplementacja metody rysującej z QWidget
    virtual void paintEvent(QPaintEvent * pEvent);

    virtual void drawSingleTag(QPainter & painter, const QRectF & rect, double normBegin, double normLength);

};

#endif  //  HEADER_GUARD_TIMELINE__CHANNELWIDGET_H__
