/********************************************************************
	created:  2010/06/30
	created:  30:6:2010   15:04
	filename: tm.h
	author:	  Piotr Gwiazdowski
	
	purpose:  
*********************************************************************/
#ifndef __HEADER_GUARD__TM_H__
#define __HEADER_GUARD__TM_H__

////////////////////////////////////////////////////////////////////////////////
namespace timeline {
////////////////////////////////////////////////////////////////////////////////

class Controller;

//!
class TimelineManager
{
private:
  //! Instancja.
  static TimelineManager * instance;

private:
	//! Prywatny konstruktor, tylko dla metod konstruujących.
  TimelineManager() {}
	//! Prywatny destruktor, tylko dla metod konstruujących.
  virtual ~TimelineManager() {}

public:
  //! Tworzy instancję. Jeżeli istnieje, raporuje błąd.
  //! \return Instancja typu.
  static TimelineManager* createInstance();
  //! \return Instancja typu.
  static TimelineManager* getInstance();
  //! Niszczy singleton.
  static void destroyInstance();

  //! \return Kontroler timeline.
  Controller* getController();
};


////////////////////////////////////////////////////////////////////////////////
} // namespace timeline
////////////////////////////////////////////////////////////////////////////////


#endif  // __HEADER_GUARD__TM_H__
