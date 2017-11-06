/********************************************************************
	created:	2012/06/05
	created:	5:6:2012   14:28
	filename: 	MultiserieColorStrategy.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_HMM__MULTISERIECOLORSTRATEGY_H__
#define HEADER_GUARD_HMM__MULTISERIECOLORSTRATEGY_H__

#include <QtGui/QColor>
#include <plugins/newChart/INewChartSerie.h>
//! interfejs realizujący strategie do zmiany kolorów multiserii
class IMultiserieColorStrategy
{
public:
    virtual ~IMultiserieColorStrategy() {}

public:
    //! Na podstawie serii i wrappera dobierany jest kolor
    //! \return kolor dla dajnej serii
    virtual QColor getColor(INewChartSerie* s, core::VariantConstPtr ow) const = 0;
};
typedef utils::shared_ptr<IMultiserieColorStrategy> IMultiserieColorStrategyPtr;
typedef utils::shared_ptr<const IMultiserieColorStrategy> IMultiserieColorStrategyConstPtr;

//! klasa dostarcza losowych kolorów dla serii
class RandomMultiserieColorStrategy : public IMultiserieColorStrategy
{
public:
    //! \return losowy kolor
    QColor getColor(INewChartSerie* s, core::VariantConstPtr w) const
    {
        return QColor(rand() % 256, rand() % 256, rand() % 256);
    }
};

//! wykorzystywany jest kolor przypisany do serii
class CopyColorMultiserieColorStrategy : public IMultiserieColorStrategy
{
public:
    //! \return przypisany do serii kolor
    QColor getColor(INewChartSerie* s, core::VariantConstPtr w) const
    {
        return s->getColor();
    }
};

//! podawane są dwa kolory, strategia dostarcza losowy kolor interpolowany pomiędzy nimi
class RandomBetweenMultiserieColorStrategy : public IMultiserieColorStrategy
{
public:
    //! Konstruktor
    //! \param c1 lewy zakres interpolacji
    //! \param c2 prawy zakres interpolacji
    RandomBetweenMultiserieColorStrategy(QColor c1, QColor c2) : c1(c1), c2(c2) {}

    //! \return interpolowany kolor
    QColor getColor(INewChartSerie* s, core::VariantConstPtr w) const
    {
        float r = static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX);
        return QColor(
            lerp(c1.red(), c2.red(), r),
            lerp(c1.green(), c2.green(), r),
            lerp(c1.blue(), c2.blue(), r)
            );
    }

private:
    //! interpolacja liniowa
    //! \param from lewy zakres interpolacji
    //! \param to prawy zakres interpolacji
    //! \param r współczynnik interpolacji [0 - 1]
    int lerp(int from, int to, float r) const
    {
        return static_cast<int>(from * (1.0f - r) + to * r);
    }
    QColor c1, c2;
};

//! kazdy wrapper dostaje kolor przypisany z zewnątrz
class ColorMapMultiserieStrategy : public IMultiserieColorStrategy
{
public:
    //! Konstruktor
    //! \param colorMap mapa przechowuje kolory dla konkretnych widgetów
    ColorMapMultiserieStrategy(const std::map<core::VariantConstPtr, QColor>& colorMap) : colorMap(colorMap) {}
    //! \return przypisany kolor
    virtual QColor getColor(INewChartSerie* s, core::VariantConstPtr w) const 
    {
        auto it = colorMap.find(w);
        if (it != colorMap.end()) {
            return it->second;
        }

        return QColor();
    }

private:
    //! mapa przechowuje kolory dla konkretnych widgetów
    std::map<core::VariantConstPtr, QColor> colorMap;
};


#endif
