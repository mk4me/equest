/********************************************************************
    created:  2012/03/19
    created:  19:3:2012   11:25
    filename: SourceManager.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___SOURCEMANAGER_H__
#define HEADER_GUARD___SOURCEMANAGER_H__

#include <corelib/ISourceManager.h>
#include <corelib/ISource.h>
#include <map>
#include <vector>

namespace core {

class SourceManager: public ISourceManager
{
public:
	//! Słownik usług.
	typedef std::map<UniqueID, plugin::ISourcePtr> SourcesMap;
	//! Sekwencja usług.
	typedef std::vector<plugin::ISourcePtr> SourcesList;

private:
	//! Słownik usług.
	SourcesMap sourcesMap; 
	//! Sekwencja usług.
	SourcesList sourcesList;

public:
	SourceManager();
	virtual ~SourceManager(void);

public:

	void update(double deltaTime);

	//! Metoda finalizująca wszystkie serwisy  wywoływana przed niszczeniem managera
	void finalizeSources();

	// IServiceManager
public:
	//! Rejestruje źródło. Źródła muszą mieć unikatowe ID!
	//! \param source Źródło do rejestracji.
	virtual void registerSource(const plugin::ISourcePtr & source);
	//! \return Liczba źródeł.
	virtual int getNumSources() const;
	//! \param idx Indeks źródła.
	//! \return Źródło o zadanym indeksie.
	virtual plugin::ISourcePtr getSource(int idx);
	//! \param id ID źródła do wyszukania.
	//! \return Odnalezione źródło bądź NULL.
	virtual plugin::ISourcePtr getSource(UniqueID id);
};

}

#endif	//	HEADER_GUARD___SOURCEMANAGER_H__
