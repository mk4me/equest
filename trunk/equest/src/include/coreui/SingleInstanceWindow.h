/********************************************************************
    created:  2014/02/07
    created:  7:2:2014   10:24
    filename: SingleInstanceWindow.h
    author:   Mateusz Janiak
    
    purpose:  Klasa gwarantuj¹ca uruchomienie jednej instancji danego widoku
		aplikacji na lokalnym komputerze
*********************************************************************/
#ifndef HEADER_GUARD_COREUI__SINGLEINSTANCEWINDOW_H__
#define HEADER_GUARD_COREUI__SINGLEINSTANCEWINDOW_H__

#include <coreui/Export.h>
#include <string>
#include <utils/SmartPtr.h>

namespace coreUI
{
	class COREUI_EXPORT SingleInstanceWindow
	{
	private:
		//! Prywatna implementacja funkcjonalnoœci
		class SingleInstanceWindowImpl;

	protected:
		//! \param viewUniqueID Unikalny identyfikator widoku aplikacji
		SingleInstanceWindow(const std::string & viewUniqueID);

	public:
		//! Destruktor wirtualny
		virtual ~SingleInstanceWindow();

		//! \return Czy to jedyna instancja tego widoku
		const bool isSingle() const;

	private:
		//! Obiekt realizuj¹cy funkcjonalnoœæ
		utils::shared_ptr<SingleInstanceWindowImpl> impl;
	};
}

#endif	//	HEADER_GUARD_COREUI__SINGLEINSTANCEWINDOW_H__
