/********************************************************************
	created:  2012/03/09
	created:  9:3:2012   11:31
	filename: IPatientCard.h
	author:   Mateusz Janiak

	purpose:
	*********************************************************************/
#ifndef HEADER_GUARD_HMDBCOMMUNICATION__IPATIENTCARD_H__
#define HEADER_GUARD_HMDBCOMMUNICATION__IPATIENTCARD_H__

#include <QtWidgets/QWidget>
#include <QtGui/QPixmap>
#include <hmdbserviceslib/ShallowCopy.h>

namespace hmdbCommunication
{
	//! Interfejs karty pacjenta
	class IPatientCard
	{
	public:

		//! Wirtualny destruktor
		virtual ~IPatientCard() {}

		//! \return Nazwa schematu karty pacjenta
		virtual const std::string name() = 0;
		//! \return Widget z kartą pacjenta
		virtual QWidget * widget() = 0;
		//! \param patient Pacjent którego należy sformatować w karcie pacjenta - dla nullptr należy zresetować kartę pacjenta
		//! \param subject Subject którego sesje będziemy czytać
		//! \param photo Zdjęcie pacjenta
		//! \param userData Dane użytkownika (na potrzeby notatek!!)
		virtual void setPatient(const hmdbServices::MedicalShallowCopy::Patient * patient = nullptr,
			const hmdbServices::MotionShallowCopy::Performer * subject = nullptr,
			const QPixmap & photo = QPixmap()) = 0;
	};
}

#endif	//	HEADER_GUARD_HMDBCOMMUNICATION__IPATIENTCARD_H__
