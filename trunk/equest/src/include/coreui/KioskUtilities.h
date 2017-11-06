/********************************************************************
	created:  2016/03/15 10:02
	filename: KioskUtilities.h
	author:	  Wojciech Knieæ
	
	purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___KIOSKUTILITIES_H__
#define HEADER_GUARD___KIOSKUTILITIES_H__

#include <functional>
#include <coreui/Export.h>

namespace coreUI {

//! Klasa zawiera w sobie metody u³atwiaj¹ce uruchomienie trybu "Kiosk".
//! Aplikacja taka powinna zablokowaæ mo¿liwoœæ wyjœcia z niej za pomoc¹ standardowych przycisków
//! oraz kombinacji klawiszy (ALT + TAB i podobnych)
class COREUI_EXPORT KioskUtilities
{
public:
	using additionalOperations = std::function < void(void) > ;
	//! Blokuje kombinacje klawiszy, które mog¹ wyjœæ z aplikacji
	static void blockSpecialKeyCombinations();
	//! Odblokowuje kombinacje klawiszy, które mog¹ wyjœæ z aplikacji
	static void unblockSpecialKeyCombinations();
	//! Uruchamia tryb kiosk
	//! \param f dodatkowe operacje, które zostan¹ przeprowadzone przy uruchomieniu trybu
	static void startKioskMode(additionalOperations f = [](){});
	//! Uruchamia tryb kiosk jeœli ustawiony jest parametr aplikacji --kiosk 
	//! \param f dodatkowe operacje, które zostan¹ przeprowadzone przy uruchomieniu trybu
	static void startKioskModeIfParamIsSet(additionalOperations f = [](){});
	//! Koñczy tryb kiosk
	//! \param f dodatkowe operacje, które zostan¹ przeprowadzone przy zakoñczeniu trybu
	static void stopKioskMode(additionalOperations f = [](){});
	//! Koñczy tryb kiosk, nie rzuci wyj¹tku przy b³êdzie
	//! \param f dodatkowe operacje, które zostan¹ przeprowadzone przy zakoñczeniu trybu
	static void safeStopKioskMode(additionalOperations f = [](){});
};

class COREUI_EXPORT ScopedKiosk {
public:
	ScopedKiosk(KioskUtilities::additionalOperations startOp = [](){}, KioskUtilities::additionalOperations stopOp = [](){}, bool onlyIfParam = true);
	~ScopedKiosk();
private:
	KioskUtilities::additionalOperations stopOperation;
};
}
#endif