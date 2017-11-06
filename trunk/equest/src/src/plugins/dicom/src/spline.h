/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * Qwt Widget Library
 * Copyright (C) 1997   Josef Wilgen
 * Copyright (C) 2002   Uwe Rathmann
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the Qwt License, Version 1.0
 *****************************************************************************/
#ifndef HEADER_GUARD_DICOM__SPLINE_H__
#define HEADER_GUARD_DICOM__SPLINE_H__



#include "qwt_global.h"
#include <qpolygon.h>
#include <qrect.h>
#include <qwt_spline.h>

#include "qwt_global.h"
#include <qpolygon.h>
#include <qvector.h>


// Plik zawiera klase, ktora tworzy krzywe, tez te zamkniete
// W calosci oparty o Qwt
// Klasa SSpline jest skopiowana z wczesniejszych rewizji qwt (QwtSpline), jest to rozwiazanie tymczasowe



class SSpline
{
public:
	//! Spline type
	enum SplineType
	{
		//! A natural spline
		Natural,

		//! A periodic spline
		Periodic
	};

	SSpline();
	SSpline(const SSpline &);

	~SSpline();

	SSpline &operator=(const SSpline &);

	void setSplineType(SplineType);
	SplineType splineType() const;

	bool setPoints(const QPolygonF& points);
	QPolygonF points() const;

	void reset();

	bool isValid() const;
	double value(double x) const;

	const QVector<double> &coefficientsA() const;
	const QVector<double> &coefficientsB() const;
	const QVector<double> &coefficientsC() const;

protected:
	bool buildNaturalSpline(const QPolygonF &);
	bool buildPeriodicSpline(const QPolygonF &);

private:
	class PrivateData;
	PrivateData *d_data;
};

//! Jest to nieco przerobiona klasa QwtSplineCurveFitter. Dodano do niej wsparcie dla zamkniêtych polygonów.
class SplineCurveFitter
{
public:

    SplineCurveFitter();
    virtual ~SplineCurveFitter();

    void setSpline( const SSpline& );
    const SSpline &spline() const;
    SSpline &spline();

    void setSplineSize( int size );
    int splineSize() const;

    virtual QPolygonF fitCurve( const QPolygonF & ) const;

private:
    QPolygonF fitParametric( const QPolygonF & , bool) const;

    class PrivateData;
    PrivateData *d_data;
};

#endif
