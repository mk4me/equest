/********************************************************************
	created:	2013/05/07
	created:	7:5:2013   13:19
	filename: 	HierarchyHelper.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_CORE__HIERARCHYHELPER_H__
#define HEADER_GUARD_CORE__HIERARCHYHELPER_H__

#include <QtCore/QString>
#include <corelib/Visualizer.h>
#include <corelib/IHierarchyItem.h>

namespace core {


//! klasa podpinana pod elementy drzewa, pozwala tworzyć i zasilac wizualizatory
class CORELIB_EXPORT HierarchyHelper
{
public:
    HierarchyHelper();
	virtual ~HierarchyHelper() {}

protected:
    //! Tworzy serie danych
    //! \param visualizer wizualizator, który będzie tworzył serie
    //! \param path ścieżka dla timeline
    //! \param series tutaj trafia stworzone serie
    virtual void createSeries(const VisualizerPtr & visualizer, const QString& path, std::vector<Visualizer::Serie*>& series) = 0;

public:
    //! \return utworzony obiekt wizualizatora
    virtual core::VisualizerPtr createVisualizer(core::IVisualizerManager* manager) = 0;
    //! zwraca utworzone serie danych
    //! \param visualizer wizualizator, który będzie tworzył serie
    //! \param path ścieżka dla timeline
    //! \param series tutaj trafia stworzone serie
    void getSeries(const VisualizerPtr & visualizer, const QString& path, std::vector<Visualizer::Serie*>& series);
    //! \return opis do np. menu kontekstowego -> 'multichart/create x from session"
    QString getText() const { return text; }
    //! ustawia opis do np. menu kontekstowego -> 'multichart/create x from session"
    //! \param val 
    void setText(QString val) { text = val; }
    //! \return typy wspierane przez helper
    virtual std::vector<utils::TypeInfo> getTypeInfos() const = 0;
    
    void setParent(const IHierarchyItemConstPtr val) { parent = val; }
    const IHierarchyItemConstWeakPtr getParent() const { return parent; }

private:
    //! utworzony przez helper wizualizator
    VisualizerWeakPtr createdVisualizer;
    //! opis do np. menu kontekstowego -> 'multichart/create x from session"
    QString text;
    //! 
    IHierarchyItemConstWeakPtr parent;
};
DEFINE_SMART_POINTERS(HierarchyHelper);


//! Najbardziej podstawowy helper dla drzewa danych
//! Tworzy domyślny wizualizator dla wrappera, bez dodatkowych operacji
class CORELIB_EXPORT WrappedItemHelper : public HierarchyHelper
{
public:
    //! Konstruktor wymaga podania wrappera z danymi
    //! \param wrapper 
    WrappedItemHelper(const VariantConstPtr & wrapper);
    virtual ~WrappedItemHelper() {}

public:
    //! zwraca utworzone serie danych
    //! \param visualizer wizualizator, który będzie tworzył serie
    //! \param path ścieżka dla timeline
    //! \param series tutaj trafia stworzone serie 
    virtual void createSeries(const VisualizerPtr & visualizer, const QString& path, std::vector<Visualizer::Serie*>& series);
    //! \return tworzy wizualizator o typie zgodnym z wrapperem (gdy w systemie jest zarejstrowanych wiecej wizualizatorów danego typu to tworzony jest przpadkowy)
    virtual VisualizerPtr createVisualizer(IVisualizerManager* manager);
    //! \return typ wspierany przez helper (typ wpisany do wrappera)
    std::vector<utils::TypeInfo> getTypeInfos() const;
    //! \return przypisany do helpera wrapper
	const VariantConstPtr getWrapper() const { return wrapper; }
protected:
    //! przypisany do helpera wrapper
	VariantConstPtr wrapper;
};
DEFINE_SMART_POINTERS(WrappedItemHelper);

//! Najbardziej podstawowy helper dla drzewa danych
//! Tworzy domyślny wizualizator dla wrappera, bez dodatkowych operacji
class CORELIB_EXPORT MetaTitlePatternWrappedItemHelper : public WrappedItemHelper
{
public:
	//! Konstruktor wymaga podania wrappera z danymi
	//! \param wrapper 
	MetaTitlePatternWrappedItemHelper(const VariantConstPtr & wrapper,
		const QString & pattern, const std::list<std::string> & metadata);
	virtual ~MetaTitlePatternWrappedItemHelper();

public:
	//! zwraca utworzone serie danych
	//! \param visualizer wizualizator, który będzie tworzył serie
	//! \param path ścieżka dla timeline
	//! \param series tutaj trafia stworzone serie 
	virtual void createSeries(const VisualizerPtr & visualizer, const QString& path, std::vector<Visualizer::Serie*>& series);	
private:
	const QString pattern;
	const std::list<std::string> metadata;
};
DEFINE_SMART_POINTERS(WrappedItemHelper);

}

#endif
