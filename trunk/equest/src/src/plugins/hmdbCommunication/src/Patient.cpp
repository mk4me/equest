#include "CommunicationPCH.h"
#include "Patient.h"

using namespace hmdbCommunication;

Patient::Patient(PluginSubject::SubjectID id, const std::string & name, const std::string & surname, const std::string & birthday,
    Gender gender, const utils::shared_ptr<const QPixmap> & photo, const std::vector<Disorder> & disorders)
    :id(id), name(name), surname(surname), birthday(birthday), gender(gender), photo(photo), disorders(disorders)
{

}

Patient::~Patient()
{

}

PluginSubject::SubjectID Patient::getID() const
{
    return id;
}

const std::string & Patient::getName() const
{
    return name;
}

const std::string & Patient::getSurname() const
{
    return surname;
}

const std::string & Patient::getBirthday() const
{
    return birthday;
}

IPatient::Gender Patient::getGender() const
{
    return gender;
}

const utils::shared_ptr<const QPixmap> & Patient::getPhoto() const
{
    return photo;
}

unsigned int Patient::disordersCount() const
{
    return disorders.size();
}

const Disorder & Patient::disorder(unsigned int idx) const
{
    return disorders[idx];
}

void Patient::setName(const std::string & name)
{
	this->name = name;
}

void Patient::setSurname(const std::string & surname)
{
	this->surname = surname;
}

void Patient::setBirthday(const std::string & birthday)
{
	this->birthday = birthday;
}

void Patient::setGender(Gender gender)
{
	this->gender = gender;
}

void Patient::setPhoto(const utils::shared_ptr<const QPixmap> & photo)
{
	this->photo = photo;
}

IPatient::Gender Patient::decodeGender(char g)
{
	char gUpper = toupper(g);
	IPatient::Gender ret = IPatient::NotSpecifiedGender;
	switch(gUpper){
	case 'M':
		ret = IPatient::Male;
		break;
	case 'F':
	case 'W':
		ret = IPatient::Female;
		break;
	}

	return ret;
}
