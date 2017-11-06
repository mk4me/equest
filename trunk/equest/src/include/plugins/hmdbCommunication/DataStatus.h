/********************************************************************
    created:  2012/02/04
    created:  4:2:2012   11:24
    filename: DataStatus.h
    author:   Mateusz Janiak
    
    purpose:  Obiekty pozwalające określić status danych  - ich dostępnośc i użycie
*********************************************************************/
#ifndef HEADER_GUARD_HMDBCOMMUNICATION__DATASTATUS_H__
#define HEADER_GUARD_HMDBCOMMUNICATION__DATASTATUS_H__

namespace hmdbCommunication
{

//! Klasa dostarczający informacji o statusie poszczególnych elementów hierarchi danych oraz tych sztucznie stworzonych.
//! Przechowuje stan składowania danych: Lokalne, Zdalne, Częsciowo lokalne, Nieznany (problem z bazą, wymaganiami dla danych)
//! Przechowuje stan użycia danych: Załadowane, Niezaładowane, Częsciowo załadowane
class DataStatus
{
public:
	//! Status składowania danych
	enum DataStorage {
		UnknownStorage = 0x00, /* Nieznany status - problem na serwerze, brak lokalnie */
		Local = 0x01,           /* Dane całkowicie lokalne */
		Remote = 0x10,          /* Dane całkoowice zdalne */
		PartiallyLocal = 0x11   /* Dane częsciowo dostępne lokalnie*/
	};

	//! Status użycia danych w DM
	enum DataUsage {
		UnknownUsage = 0x00,    /* Nieznany status - status startowy przy wyznaczaniu finalnego statusu. Ostatecznie ten status nie może się pojawić dla danych */
		Loaded = 0x01,          /* Dane załadowane w całości do DM */
		Unloaded = 0x10,        /* Dane nie załadowane do DM */
		PartiallyLoaded = 0x11  /* Dane częsciowo załadowane do DM */
	};

	//! Status ważności danych
	enum DataValidity {
		UnknownValidity = 0x00,	/* Nieznany status - status startowy przy wyznaczaniu finalnego statusu. Ostatecznie ten status nie może się pojawić dla danych */
		Valid = 0x01,			/* Dane ważne - zsynchronizowane z bazą */
		Outdated = 0x10,        /* Dane nieważne - należy ściągnąć i załadowac ponownie */
		PartiallyValid = 0x11	/* Dane częsciowo zsynchronizowane z bazą */
	};

public:
	//! Status częsciowy dla wszystkich parametrów
	static const DataStatus PartialStatus;

public:

	//! Konstruktor kopiujący
	DataStatus(const DataStatus & status);

	//! \param storage Stan przechowywania danych
	//! \param usage Stan załadowania danych
	//! \param validity Stan ważności danych
	DataStatus(const DataStorage storage = UnknownStorage,
		const DataUsage usage = UnknownUsage,
		const DataValidity validity = UnknownValidity);

	//! \param storage Stan przechowywania danych
	explicit DataStatus(const DataStorage storage);
	//! \param usage Stan załadowania danych
	explicit DataStatus(const DataUsage usage);
	//! \param validity Stan ważności danych
	explicit DataStatus(const DataValidity validity);

    //! Wirtualny destruktor
	~DataStatus();

    //! \return Stan składowania danych
    const DataStorage storage() const;

    //! \return Stan użycia danych w DM
    const DataUsage usage() const;

	//! \return Stan ważności danych
	const DataValidity validity() const;

	//! \return Stan składowania danych
	void setStorage(const DataStorage storage);

	//! \return Stan użycia danych w DM
	void setUsage(const DataUsage usage);

	//! \return Stan ważności danych
	void setValidity(const DataValidity validity);

	//! Łączy logicznie statusy
	const DataStatus & merge(const DataStatus & status);
	//! Łączy logicznie statusy
	const DataStorage merge(const DataStorage storage);
	//! Łączy logicznie statusy
	const DataUsage merge(const DataUsage usage);
	//! Łączy logicznie statusy
	const DataValidity merge(const DataValidity validity);

	//! \param status Status ktory weryfikujemy wzgledem filtra
	//! \param fstatus Filtr dla statusu
	//! \return Prawda jeśli status pasuje do filtra
	static const bool filter(const DataStatus & status, const DataStatus & fstatus);

	//! Operatory konwersji
	/*
	operator DataStorage();
	operator DataUsage();
	operator DataValidity();
	*/

	//! Operator mniejszości
	bool operator<(const hmdbCommunication::DataStatus & b) const;

private:
	//! Status składowania danych
	DataStorage storage_;
	//! Status użycia danych w DM
	DataUsage usage_;
	//! Ważnośc danych
	DataValidity validity_;
};

}

//! Operator równości statusów
bool operator==(const hmdbCommunication::DataStatus & a, const hmdbCommunication::DataStatus & b);
//! Operator różności statusów
bool operator!=(const hmdbCommunication::DataStatus & a, const hmdbCommunication::DataStatus & b);
//! Operator mniejszości
bool operator<(const hmdbCommunication::DataStatus & a, const hmdbCommunication::DataStatus & b);

hmdbCommunication::DataStatus::DataStorage operator|(hmdbCommunication::DataStatus::DataStorage l, hmdbCommunication::DataStatus::DataStorage r);
hmdbCommunication::DataStatus::DataStorage & operator|=(hmdbCommunication::DataStatus::DataStorage & l, hmdbCommunication::DataStatus::DataStorage r);
hmdbCommunication::DataStatus::DataUsage operator|(hmdbCommunication::DataStatus::DataUsage l, hmdbCommunication::DataStatus::DataUsage r);
hmdbCommunication::DataStatus::DataUsage & operator|=(hmdbCommunication::DataStatus::DataUsage & l, hmdbCommunication::DataStatus::DataUsage r);
hmdbCommunication::DataStatus::DataValidity operator|(hmdbCommunication::DataStatus::DataValidity l, hmdbCommunication::DataStatus::DataValidity r);
hmdbCommunication::DataStatus::DataValidity & operator|=(hmdbCommunication::DataStatus::DataValidity & l, hmdbCommunication::DataStatus::DataValidity r);
hmdbCommunication::DataStatus operator|(const hmdbCommunication::DataStatus & l, const hmdbCommunication::DataStatus & r);
hmdbCommunication::DataStatus & operator|=(hmdbCommunication::DataStatus & l, const hmdbCommunication::DataStatus & r);
hmdbCommunication::DataStatus operator|(const hmdbCommunication::DataStatus & l, const hmdbCommunication::DataStatus::DataStorage & r);
hmdbCommunication::DataStatus & operator|=(hmdbCommunication::DataStatus & l, const hmdbCommunication::DataStatus::DataStorage & r);
hmdbCommunication::DataStatus operator|(const hmdbCommunication::DataStatus & l, const hmdbCommunication::DataStatus::DataUsage & r);
hmdbCommunication::DataStatus & operator|=(hmdbCommunication::DataStatus & l, const hmdbCommunication::DataStatus::DataUsage & r);
hmdbCommunication::DataStatus operator|(const hmdbCommunication::DataStatus & l, const hmdbCommunication::DataStatus::DataValidity & r);
hmdbCommunication::DataStatus & operator|=(hmdbCommunication::DataStatus & l, const hmdbCommunication::DataStatus::DataValidity & r);

#endif	//	HEADER_GUARD_HMDBCOMMUNICATION__DATASTATUS_H__
