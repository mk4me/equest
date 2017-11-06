#include "CommunicationPCH.h"
#include "PersonalDataWidget.h"

PersonalDataWidget::PersonalDataWidget(QWidget * parent) : QFrame(parent)
{
	setupUi(this);
	reset();
}

PersonalDataWidget::~PersonalDataWidget()
{

}

void PersonalDataWidget::setPhoto(const QPixmap & photo)
{
	if(photo.isNull() == false){
		photoPlaceholder->clear();
		photoPlaceholder->setPixmap(photo.scaled(photoPlaceholder->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
	}else{
		photoPlaceholder->setPixmap(QPixmap());
		photoPlaceholder->setText(tr("No photo"));
	}
}

void PersonalDataWidget::setName(const QString & name)
{
	namePlaceholder->setText(name);
}

void PersonalDataWidget::setSurname(const QString & surname)
{
	surnamePlaceholder->setText(surname);
}

void PersonalDataWidget::setGender(const QString & gender)
{
	genderPlaceholder->setText(gender);
}

void PersonalDataWidget::setBirthdate(const QString & birthdate)
{
	birthdatePlaceholder->setText(birthdate);
}

void PersonalDataWidget::reset()
{
	namePlaceholder->clear();
	surnamePlaceholder->clear();
	genderPlaceholder->clear();
	birthdatePlaceholder->clear();
	setPhoto(QPixmap());
}
