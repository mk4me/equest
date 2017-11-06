/********************************************************************
	created:	2013/10/12
	created:	12:10:2013   23:31
	filename: 	LayeredImageVisualizer.h
	author:		Wojciech Kniec

	purpose:
	*********************************************************************/

#ifndef HEADER_GUARD_DICOM__LAYEREDIMAGEVISUALIZER_H__
#define HEADER_GUARD_DICOM__LAYEREDIMAGEVISUALIZER_H__

#include <QtCore/QObject>
#include <QtWidgets/QWidget>
#include <corelib/IVisualizer.h>
#include <corelib/AbstractSerie.h>
#include <plugins/dicom/ILayeredImage.h>
#include "LayeredSerie.h"
#include <plugins/dicom/Annotations.h>
#include <hmdbserviceslib/Entity.h>

class QLabel;
class QScrollArea;
class QPushButton;
class QSlider;

class RemoveLayerCommand;

namespace dicom {
	class LayeredImageVisualizerView;
	class LayeredSerie;
	class AddLayerCommand;
	//! Wizualizator Dicom
	class LayeredImageVisualizer : public QObject, public plugin::IVisualizer
	{
		Q_OBJECT;
		UNIQUE_ID("{4CBA33A4-F0EA-4607-9CB1-C1816697D1A1}");
		CLASS_DESCRIPTION("Layered Image Visualizer", "Layered Image Visualizer");

		friend class LayeredImageVisualizerView;
		friend class LayeredSerie;
		friend class AddLayerCommand;
		friend class RemoveLayerCommand;

	public:
		LayeredImageVisualizer();
		virtual ~LayeredImageVisualizer();

	public:

		virtual bool eventFilter(QObject * watched, QEvent * event);

		//! \return pusty obiekt wizualizatora
		virtual IVisualizer* create() const;
		//! Tworzy obiekt typu NewChartSerie
		//! \param data Variant z interfejsem do odczytu akcesora
		//! \param name nazwa tworzonej serii danych
		//! \return utworzona seria
		plugin::IVisualizer::ISerie *createSerie(const utils::TypeInfo & requestedType, const core::VariantConstPtr& data);
		// \return metoda nie jest obs³ugiwana, nullptr
		plugin::IVisualizer::ISerie *createSerie(const plugin::IVisualizer::ISerie * serie);

		virtual ISerie* createSerie(const ISerie* serie, const utils::TypeInfo & requestedType, const core::VariantConstPtr & data);

		//! Usuwa serie z wizualizatora
		//! \param serie seria do usuniêcia, musi nale¿eæ do wizualizatora i musi byæ przez niego stworzona
		virtual void removeSerie(plugin::IVisualizer::ISerie *serie);
		
		static hmdbCommunication::IHMDBShallowCopyContext * remoteShallowContext(const plugin::IVisualizer::ISerie * serie);

		virtual void setActiveSerie(plugin::IVisualizer::ISerie * serie);
		virtual const plugin::IVisualizer::ISerie * getActiveSerie() const;
		virtual plugin::IVisualizer::ISerie * getActiveSerie();
		//! Tworzy g³ówny widget wizualizatora
		//! \param manager Manager z akcjami do flexi bara
		//! \return utworzony widget
		virtual QWidget* createWidget();
		//! \return ikona wizualizatora
		virtual QIcon* createIcon();
		//! \return makymalna liczba serii, -1 bo nie ma limitu
		virtual int getMaxDataSeries() const;
		//! Zwraca info o wspieranych typach (tylko ScalarChannelReaderInterface)
		//! \param info zwracana struktura z opisami
		virtual void getSupportedTypes(utils::TypeInfoList & supportedTypes) const;
		//! Metoda wywo³ywana cyklicznie pozwala odœwie¿yæ stan wizualizatora
		//! \param deltaTime czas od ostatniego wywo³ania metody
		virtual void update(double deltaTime);
		//! Zrzut wizualizatora do pixmapy
		virtual QPixmap takeScreenshot() const;

		int getNumSeries() const;
		int getCurrentSerieNo() const;

		std::string getUserName() const;

		void editSerie(int tagIdx, int idx);
		void removeLayer(int tagIdx, int idx);
		void selectLayer(int tagIdx, int idx);

		const bool userIsReviewer() const;

		const bool verifyCompletness() const;

		const bool verifyImflamatoryLevel() const;

		const bool verifyBloodLevel() const;

		const bool verifyJointType() const;

		const bool verifyFingerType() const;

		public Q_SLOTS:
		void setPrevSerie();
		void setNextSerie();
		void trySetSerie(int val);
		void saveSerie();
		void uploadSerie();
		void removeSelectedLayers();

	Q_SIGNALS:
		void serieChanged();
		void changeSelection(const QModelIndex&);

	private:

		void setStatus(const hmdbServices::xmlWsdl::AnnotationStatus::Type type,
			const QString & comment = QString());

		const std::string getCurrentLayerUserName() const;
		const int currnetTrialID() const;
		const bool trySave();

		bool correctIndex(int i) const;
		std::pair<std::string, int> selectedLayer() const;

		void gatherCommunicationInfo(const plugin::IVisualizer::ISerie*);

	private:
		LayeredImageVisualizerView* mainWidget;

		std::vector<LayeredSerie*> series;
		int currentSerie;
		std::string currentLayerUser_;
		int currentTrialID;

		bool isReviewer;
		std::string userName;
	};
	DEFINE_SMART_POINTERS(LayeredImageVisualizer);
}
#endif
