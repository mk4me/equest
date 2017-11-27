/********************************************************************
    created:  2017/10/24 19:36
    filename: EquestAnnotationVisualizer.h
    author:   Wojciech Knieæ
    
    purpose: 
*********************************************************************/
#ifndef HEADER_GUARD_PLUGIN_EQUEST__EquestAnnotationVisualizer_h__
#define HEADER_GUARD_PLUGIN_EQUEST__EquestAnnotationVisualizer_h__

#include <string>
#include <corelib/IVisualizer.h>
#include "ui_toolbar.h"
#include "ui_AnnotationArea.h"
#include "EquestAnnotation.h"
#include "plugins/hmdbCommunication/IHMDBShallowCopyContext.h"

class QTableWidget;
class QDoubleSpinBox;
class QToolButton;
class QTimer;

namespace equest {
	//! Wizualizator .
	class EquestAnnotationVisualizer : public QObject, public plugin::IVisualizer
	{
		Q_OBJECT;
		UNIQUE_ID("{4A445E6E-D808-4D03-A2CB-CE83BB8E9264}");
		CLASS_DESCRIPTION("Equest annotation Visualizer", "Equest annotation Visualizer");

	private:

		class AnnotationSerie : public plugin::IVisualizer::ISerie
		{
		public:
			AnnotationSerie(EquestAnnotationVisualizer * visualizer);
			virtual ~AnnotationSerie();

			virtual void setName(const std::string & name);

			virtual const std::string getName() const;

			virtual void setData(const utils::TypeInfo & requestedType, const core::VariantConstPtr & data);

			virtual const core::VariantConstPtr & getData() const;

			virtual const utils::TypeInfo & getRequestedDataType() const;

			virtual void update();


			//! Czas zawiera siê miêdzy 0 a getLength()
			//! \param time Aktualny, lokalny czas kana³u w sekundach
			virtual void setTime(double time);

			void addAnnotation();
			void invalidateTable();
			QDoubleSpinBox* createTableSpinbox(int row, int column, double value);
			QToolButton* createTableButton(int row, int column);

			EquestAnnotationsConstPtr getAnnotations() const;
			EquestAnnotationsPtr getAnnotations();
			void invalidateCell(int row, int col, double value);
			void invalidateCell(int row, int col);
			core::VariantConstPtr getRelatedC3D();
			EquestAnnotationVisualizer * visualizer;
			Ui::AnnotationArea ui;
			std::string name;
			core::VariantConstPtr data;
			utils::TypeInfo requestedType;
			//double currentTime;
		};

		friend class AnnotationSerie;

	private:
		QWidget* widget;
		Ui::Toolbar ui;
		AnnotationSerie* currentSerie;
		double currentTime;
		QTimer* timer; 

	public:
		//!
		EquestAnnotationVisualizer();
		//!
		virtual ~EquestAnnotationVisualizer();

	public Q_SLOTS:
		void moveTimeline();
		void cellDoubleClicked(int row, int column);
		void cellChanged(int row, int column);
		void cellValueChanged(double value);
		void addAnnotation();
		void save();
		void upload();

	private Q_SLOTS:
		void updateTime();

	public:
		//! \see IVisualizer::create
		virtual plugin::IVisualizer* create() const;
		//! \see IVisualizer::getSlotInfo
		virtual void getSupportedTypes(utils::TypeInfoList & supportedTypes) const;
		//! Nic nie robi.
		//! \see IVisualizer::update
		virtual void update(double deltaTime);
		//! \see IVisualizer::createWidget
		virtual QWidget* createWidget();
		//! \see IVisualizer::createIcon
		virtual QIcon* createIcon();
		virtual QPixmap takeScreenshot() const;

		virtual int getMaxDataSeries() const;

		//! \return Seria danych która mo¿na ustawiac - nazwa i dane, nie zarz¹dza ta seria danych - czasem jej zycia, my zwalniamy jej zasoby!!
		virtual plugin::IVisualizer::ISerie* createSerie(const utils::TypeInfo & requestedType, const core::VariantConstPtr & data);

		virtual plugin::IVisualizer::ISerie* createSerie(const plugin::IVisualizer::ISerie * serie);

		virtual plugin::IVisualizer::ISerie* createSerie(const plugin::IVisualizer::ISerie * serie, const utils::TypeInfo & requestedType, const core::VariantConstPtr & data);

		//! \param serie Seria danych do usuniêcia, nie powinien usuwaæ tej serii! Zarz¹dzamy ni¹ my!!
		virtual void removeSerie(plugin::IVisualizer::ISerie* serie);

		virtual void setActiveSerie(plugin::IVisualizer::ISerie * serie);

		virtual const plugin::IVisualizer::ISerie * getActiveSerie() const;

		virtual plugin::IVisualizer::ISerie * getActiveSerie();


		hmdbCommunication::IHMDBShallowCopyContext* sourceContextForData(const core::VariantConstPtr data);

	};
}
#endif