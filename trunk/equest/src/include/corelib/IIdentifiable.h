/********************************************************************
	created:  2010/09/28
	created:  28:9:2010   1:22
	filename: IIdentifiable.h
	author:	  Piotr Gwiazdowski
	
	purpose:  
*********************************************************************/
#ifndef __HEADER_GUARD__IIDENTIFIABLE_H__
#define __HEADER_GUARD__IIDENTIFIABLE_H__

#include <corelib/BaseDataTypes.h>

namespace plugin {

//! Interfejs reprezentujący obiekt możliwy do identyfikacji.
class IIdentifiable
{
public:
	//! Destruktor wirtualny
	virtual ~IIdentifiable() {}
    //! \return Unikalny identyfikator obiektu
    virtual const core::UniqueID ID() const = 0;
};

//! Interfejs dostarczający opisu
class IDescription
{
public:
	//! Destruktor wirtualny
	virtual ~IDescription() {}
	//! \return Nazwa
	virtual const std::string name() const = 0;
	//! \return Nazwa skrócona
	virtual const std::string shortName() const = 0;
	//! \return Opis
	virtual const std::string  description() const = 0;
};

class Description : public IDescription
{
public:
	//! Konstruktor
	//! \param name Nazwa obiektu
	//! \param shortName Krótka nazwa obiektu
	//! \param description Opis obiektu
	Description(const std::string & name = std::string(),
		const std::string & shortName = std::string(),
		const std::string & description = std::string())
		: name_(name), shortName_(shortName),
		description_(description)
	{

	}

	//! \param name Nazwa obiektu
	void setName(const std::string & name) { name_ = name; }
	//! \param shortName Krótka nazwa obiektu
	void setShortName(const std::string & shortName) { shortName_ = shortName; }
	//! \param description Opis obiektu
	void setDescription(const std::string & description) { description_ = description; }

	//! Destruktor wirtualny
	virtual ~Description() {}
	//! \return Nazwa
	virtual const std::string name() const override { return name_; }
	//! \return Nazwa skrócona
	virtual const std::string shortName() const override { return shortName_; }
	//! \return Opis
	virtual const std::string  description() const override { return description_; }

private:
	//! Nazwa obiektu
	std::string name_;
	//! Krótka nazwa obiektu
	std::string shortName_;
	//! Opis obiektu
	std::string description_;
};

//! Interfejs dostarczający opisu dostawcy produktu
class IVendorDescription : public IDescription
{
public:
	//! Destruktor wirtualny
	virtual ~IVendorDescription() {}
	//! \return Kontakt
	virtual const std::string contact() const = 0;	
};

class VendorDescription : public IVendorDescription
{
public:
	//! Konstruktor
	//! \param name Nazwa dostawcy
	//! \param shortName Krótka nazwa dostawcy
	//! \param description Opis dostawcy
	//! \param contact Jak skontaktować się z dostawcą
	VendorDescription(const std::string & name = std::string(),
		const std::string & shortName = std::string(),
		const std::string & description = std::string(),
		const std::string & contact = std::string())
		: desc(name, shortName, description),
		contact_(contact)
	{

	}

	//! Destruktor wirtualny
	virtual ~VendorDescription() {}
	//! \return Nazwa
	virtual const std::string name() const override { return desc.name(); }
	//! \return Nazwa skrócona
	virtual const std::string shortName() const override { return desc.shortName(); }
	//! \return Opis
	virtual const std::string  description() const override { return desc.description(); }
	//! \return Kontakt
	virtual const std::string contact() const override { return contact_; }

	//! \param name Nazwa dostawcy
	void setName(const std::string & name) { desc.setName(name); }
	//! \param shortName Krótka nazwa dostawcy
	void setShortName(const std::string & shortName) { desc.setShortName(shortName); }
	//! \param description Opis dostawcy
	void setDescription(const std::string & description) { desc.setDescription(description); }
	//! \param contact Jak skontaktować się z dostawcą
	void setContact(const std::string & contact) { contact_ = contact; }

private:
	//! Opis dostawcy
	Description desc;
	//! Kontakt z dostawcą
	std::string contact_;
};

//! Interfejs podstawowych elemntów aplikacji
class ICoreElement : public IIdentifiable, public IDescription
{
public:
	//! Destruktor wirtualny
	virtual ~ICoreElement() {}
	//! \return Instancja danego elementu
	virtual ICoreElement * create() const = 0;
};

}

//! Makro ułatwiające "wstrzykiwanie" metod interfejsu IIdentifable do klas implementujących go
#define UNIQUE_ID(uuidStr)												\
 public:                                                                \
    inline static const core::UniqueID getClassID()						\
    {                                                                   \
		static const auto cid = core::UID::GenerateUniqueID(uuidStr);	\
		return cid;														\
    }                                                                   \
                                                                        \
    virtual const core::UniqueID ID() const									\
    {                                                                   \
        return getClassID();                                            \
    }                                                                   \
private:

//! Makro ułatwiające "wstrzykiwanie" metod interfejsu IIdentifable do klas implementujących go
#define CLASS_DESCRIPTION(name_, description_)						\
 public:                                                            \
	inline static const std::string getClassName()                  \
	{                                                               \
		return std::string(name_);									\
	}                                                               \
																	\
	inline static const std::string getClassDescription()           \
	{                                                               \
		return std::string(description_);							\
	}																\
																	\
	virtual const std::string name() const							\
	{                                                               \
		return getClassName();                                      \
	}                                                               \
																	\
	virtual const std::string shortName() const						\
	{                                                               \
		return getClassName();                                      \
	}                                                               \
																	\
	virtual const std::string description() const					\
	{                                                               \
		return getClassDescription();                               \
	}                                                               \
private:

//! Makro ułatwiające "wstrzykiwanie" metod interfejsu IIdentifable do klas implementujących go
#define CLASS_DESCRIPTION_EXT(name, shortName, description)			\
	public:															\
	inline static const std::string getClassName()					\
	{																\
		return std::string(name);									\
	}																\
																	\
	inline static const std::string getClassDescription()           \
	{																\
		return std::string(description);							\
	}																\
																	\
	virtual const std::string name() const							\
	{																\
		return getClassName();										\
	}																\
																	\
	virtual const std::string shortName() const						\
	{																\
		return std::string(shortName);								\
	}																\
																	\
	virtual const std::string  description() const					\
	{																\
		return getClassDescription();								\
	}																\
	private:

#endif  // __HEADER_GUARD__IIDENTIFIABLE_H__
