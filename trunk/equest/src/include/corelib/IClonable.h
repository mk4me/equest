/********************************************************************
	created:	2011/05/12
	created:	12:5:2011   9:12
	filename: 	IClonable.h
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_CORE__ICLONABLE_H__
#define HEADER_GUARD_CORE__ICLONABLE_H__

namespace core {

//! Wzorzec interfejsu zapewniającego klonowanie obiektu.
//! Właśność metod virtualnych pozwala klasom pochodnym od klasy dziedziczącej ten interfejs zwracać wskaźniki na własny typ

template<class T>
class IClonable
{
public:
    //! \return Klon obiektu
    virtual T* createClone() const = 0;

    virtual ~IClonable() {}
};

}

#endif
