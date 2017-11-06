/********************************************************************
    created:  2011/07/19
    created:  19:7:2011   14:18
    filename: View.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD_TIMELINE__VIEW_H__
#define HEADER_GUARD_TIMELINE__VIEW_H__

#include <timelinelib/Types.h>
#include <timelinelib/State.h>
#include <utils/ObserverPattern.h>
#include <utils/Debug.h>

namespace timeline{

class View : public utils::Observer<State>
{
public:

    //! \return Kontroler z którym współpracuje widok
    const ControllerPtr & getController()
    {
        return controller;
    }

    //! \return Kontroler z którym współpracuje widok
    const ControllerConstPtr & getController() const
    {
        return constController;
    }

    //! \param controller Kontroler z którym współpracuje widok
    void setController(const ControllerPtr & controller)
    {
        UTILS_ASSERT((controller != nullptr), "Błędny kontroler");
        this->constController = this->controller = controller;
    }

private:
    //! Kontroler z któym współpracuje widok
    ControllerPtr controller;
    //! Kontroler z któym współpracuje widok
    ControllerConstPtr constController;
};

}

#endif  //  HEADER_GUARD_TIMELINE__VIEW_H__
