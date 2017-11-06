/********************************************************************
	created:	2012/02/22
	created:	22:2:2012   10:30
	filename: 	NewChartLegend.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_NEW_CHART__NEWCHARTLEGEND_H__
#define HEADER_GUARD_NEW_CHART__NEWCHARTLEGEND_H__

#include <QtWidgets/QWidget>
#include <qwt_legend.h>
#include <qwt_legend_data.h>

class QCheckBox;
class QPushButton;
class QwtPlotItem;

//! Klasa zarządzająca legendą New Charta
//! \version 0.9.1
class NewChartLegend : public QwtLegend
{
    Q_OBJECT;
public:
    //! konstruktor
    //! \param parent  
    explicit NewChartLegend( QWidget *parent = nullptr ) : QwtLegend(parent) {}
    //! destruktor
    virtual ~NewChartLegend() {}

protected:
    //! Tworzy widget legendy, wywoływane przez qwt
    //! \param & standardowe qwt dane nt. legendy
    virtual QWidget *createWidget( const QwtLegendData & ) const;

signals:
    //! zmienił się checkbox z widocznością serii
    //! \param item element legendy zawierający checkbox
    //! \param active seria powinna stac się widoczna/ukryta
    void checkboxChanged(const QVariant& info, bool active);

public slots:
    //! Aktualizacja legendy
    //virtual void updateLegend( const QwtPlotItem *, const QList<QwtLegendData> & );
    //virtual void updateLegend( const QVariant &, const QList<QwtLegendData> & );
    //! Aktualizacja pojedynczego elementu legendy
    //! \param w element klasy NewChartLegendItem
    //! \param data zmienione dane elementu
    virtual void updateWidget( QWidget* w, const QwtLegendData& data );

private slots:
    //! zmienił się checkbox z widocznością serii
    //! \param checked seria ma stac się widoczna/ukryta
    void onCheck(bool checked);
};


#endif
