/********************************************************************
	created:	2014/07/13
	created:	11:47
	filename: 	OsgSerie.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_KINEMATIC__OsgSerie_H__
#define HEADER_GUARD_KINEMATIC__OsgSerie_H__


#include <corelib/IVisualizer.h>
#include "KinematicSerie.h"

class KinematicVisualizer;

//! Seria danych wizualizatora 3D dostarczająca węzeł osg
//! \version 0.9.1
class OsgSerie : public QObject, public KinematicSerie
{    
public:
    //! Konstuktor
    //! \param visualizer wizualizator, który stworzył serie danych
	OsgSerie(KinematicVisualizer * visualizer,
		const utils::TypeInfo & requestedType,
		const core::VariantConstPtr & data);

public:
    //! Ustawia nowa nazwę serii
    virtual void setName(const std::string & name);
    //! \return nazwa serii
    virtual const std::string getName() const;
    //! \return ustawione dane
    virtual const core::VariantConstPtr & getData() const;

	virtual void update();

	virtual const utils::TypeInfo & getRequestedDataType() const;
    //! \return macierz serii z transformacją, która nie została zmieniona przez manipulatory
    virtual osg::Matrix getInitialMatrix() const;

	
private:
    //! Wizualizator, który utworzył serie
	KinematicVisualizer * visualizer;
    //! wrapper przekazany serii
    core::VariantConstPtr data;
	//! Informacja o zadanym typie
	utils::TypeInfo requestedType;
    //! nazwa serii
    std::string name;
};
DEFINE_SMART_POINTERS(OsgSerie)


#endif
