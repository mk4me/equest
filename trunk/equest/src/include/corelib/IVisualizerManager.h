/********************************************************************
    created:  2011/04/22
    created:  22:4:2011   11:50
    filename: IVisualizerManager.h
    author:	  Piotr Gwiazdowski
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD_CORE__IVISUALIZERMANAGER_H__
#define HEADER_GUARD_CORE__IVISUALIZERMANAGER_H__

#include <list>
#include <corelib/BaseDataTypes.h>
#include <utils/TypeInfo.h>
#include <corelib/Visualizer.h>

namespace core 
{
	class Visualizer;

    class IVisualizerManager
    {
		friend class Visualizer;
	public:
		//! Typ agregatu prototyp�w
		typedef std::list<VisualizerConstPtr> VisualizerPrototypes;
		//! Typ agreguj�cy aktualnie �ywe instancje wizualizator�w
		typedef std::list<const Visualizer*> VisualizerInstances;

		//! Typ operacji rejestrowania, wyrejestrowywania wizualizator�w
		enum VisuzalizerOperation {
			Creation,	//! Rejestracja wizualizatora
			Destruction	//! Wyrejestrowywanie wizualizatora
		};

		class IObserver
		{
		public:
			virtual void update(VisuzalizerOperation modification, Visualizer * visualizer);
		};

    public:

		virtual ~IVisualizerManager() {}

		//! \param prototypes [out] Wszystkie prototypy wizualizator�w
		virtual void visualizerPrototypes(VisualizerPrototypes & prototypes) = 0;
		//! \param id ID �r�d�a do wyszukania.
		//! \return Odnalezione �r�d�o b�d� NULL.
		virtual VisualizerConstPtr getVisualizerPrototype(UniqueID id) = 0;
		//! \param type Typ dla kt�rego poszukujeemy wizualizator�w
		//! \param prototypes [out] Prototypy wizualizator�w potrafi�cych obs�u�y� zadany typ
		virtual void getVisualizerPrototypes(const utils::TypeInfo & type, VisualizerPrototypes & prototypes, bool exact) = 0;
		//! \param visInstances [out] Obiekt do ktorego zapiszemy aktualnie zyjace instancje wizualizatorow
		virtual void getVisualizersInstances(VisualizerInstances & visInstances) const = 0;
		//! \param observer Obserwator tworzonych wizualziator�w w��czony do obserwacji
		virtual void registerObserver(IObserver * observer) = 0;
		//! \param observer Obserwator tworzonych wizualziator�w wy�anczany z obserwacji
		virtual void unregisterObserver(IObserver * observer) = 0;

	private:

		virtual void registerVisualizer(Visualizer* visualizer) = 0;
		virtual void unregisterVisualizer(Visualizer* visualizer) = 0;
    };
} // namespace core

#endif  // HEADER_GUARD_CORE__IVISUALIZERMANAGER_H__
