/********************************************************************
	created:  2014/09/24	09:58:51
	filename: WrapperT.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_SQLITEUTILS__WRAPPERT_H__
#define __HEADER_GUARD_SQLITEUTILS__WRAPPERT_H__

#include <threadingUtils/SmartPtr.h>

namespace sqliteUtils
{
	template<typename T, typename D = std::default_delete<T>>
	class UniqueWrapperT : public threadingUtils::unique_ptr<T, D>
	{
	public:
		//! \param wrapper Obs�ugiwany uchwyt
		UniqueWrapperT(UniqueWrapperT & wrapper) : threadingUtils::unique_ptr<T, D>(wrapper)
		{

		}

		//! \param wrapper Obs�ugiwany uchwyt
		UniqueWrapperT(UniqueWrapperT&& wrapper) : threadingUtils::unique_ptr<T, D>(std::move(wrapper))
		{

		}

		//! \param elem Obs�ugiwany uchwyt
		//! \param del Obiekt zwalniaj�cy zasoby uchwuty
		explicit UniqueWrapperT(T * elem, const D & del = D()) : threadingUtils::unique_ptr<T, D>(elem, del)
		{

		}

		//! \param elem Obs�ugiwany uchwyt
		//! \param del Obiekt zwalniaj�cy zasoby uchwuty
		explicit UniqueWrapperT(T * elem, D && del) : threadingUtils::unique_ptr<T, D>(elem, std::move(del))
		{

		}

		//! Destruktor
		~UniqueWrapperT()
		{

		}

		//! \return Obs�ugiwany uchwyt bazy danych
		inline operator T*() const
		{
			return this->threadingUtils::unique_ptr<T, D>::get();
		}
	};

	template<typename T>
	class SharedWrapperT : public threadingUtils::shared_ptr<T>
	{
	public:
		//! \param wrapper Obs�ugiwany uchwyt
		SharedWrapperT(SharedWrapperT & wrapper) : threadingUtils::shared_ptr<T>(wrapper)
		{

		}

		//! \param wrapper Obs�ugiwany uchwyt
		SharedWrapperT(SharedWrapperT&& wrapper) : threadingUtils::shared_ptr<T>(std::move(wrapper))
		{

		}

		//! \param elem Obs�ugiwany uchwyt
		//! \param del Obiekt zwalniaj�cy zasoby uchwuty
		template<typename D>
		explicit SharedWrapperT(T * elem, const D & del = D()) : threadingUtils::shared_ptr<T>(elem, del)
		{

		}

		//! \param elem Obs�ugiwany uchwyt
		//! \param del Obiekt zwalniaj�cy zasoby uchwuty
		template<typename D>
		explicit SharedWrapperT(T * elem, D && del) : threadingUtils::shared_ptr<T>(elem, std::move(del))
		{

		}

		//! Destruktor
		~SharedWrapperT()
		{

		}

		//! \return Obs�ugiwany uchwyt bazy danych
		inline operator T*() const
		{
			return this->threadingUtils::shared_ptr<T>::get();
		}
	};
}

#endif	// __HEADER_GUARD_SQLITEUTILS__WRAPPERT_H__
