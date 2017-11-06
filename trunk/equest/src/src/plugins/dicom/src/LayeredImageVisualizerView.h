/********************************************************************
	created:	2013/10/17
	created:	17:10:2013   10:12
	filename: 	LayeredImageVisualizerView.h
	author:		Wojciech Kniec

	purpose:
	*********************************************************************/

#ifndef HEADER_GUARD_DICOM__LAYEREDIMAGEVISUALIZERVIEW_H__
#define HEADER_GUARD_DICOM__LAYEREDIMAGEVISUALIZERVIEW_H__

#include <QtWidgets/QWidget>
#include <QtCore/QItemSelection>
#include <coreui/GraphicSceneWithStateMachine.h>
#include "LayeredModelView.h"
#include "LayeredStateMachine.h"
#include <coreui/CoreAction.h>
#include "AnnotationsDelegate.h"
#include "IDicomService.h"
#include <hmdbserviceslib/Entity.h>
#include "corelib/IVisualizer.h"

class QGraphicsScene;

namespace Ui { class LayeredImageVisualizer; }

class QGraphicsPixmapItem;

namespace dicom {
	class LayeredImageVisualizer;

	//! Widok wizualizatora obs³uguj¹cego obrazy z warstwami
	class LayeredImageVisualizerView : public QWidget
	{
		Q_OBJECT

	public:

		typedef hmdbServices::xmlWsdl::Annotation AnnotationStatus;

	public:
		//! Standardowy konstruktor
		LayeredImageVisualizerView(LayeredImageVisualizer* model, QWidget* parent = 0, Qt::WindowFlags f = 0);
		virtual ~LayeredImageVisualizerView() {}

	public:
		void setAnnotationStatus(hmdbServices::xmlWsdl::AnnotationStatus::Type status);
		const AnnotationStatus annotationStatus(bool refresh = true) const;

	public Q_SLOTS:
		//! odœwie¿enie ca³ego wizualizatora, np. gdy zmieni³a siê aktywna seria
		void refresh();
		//! \param enable Czy button do usuwania ma byc aktywny - dezaktywuje dla poziomu krwii i poziomu schorzenia
		void setDeletionButtonEnabled(const bool enable);

	private Q_SLOTS:
		void undo();
		void redo();
		void crop();
		void normalState();
		void moveState();

		void boneState();
		void skinState();
		void tendonState();
		void jointState();
		void inflamatoryState();
		void noiseState();

		void editSelectedSerie();

		void changeSelection(const QModelIndex &);
		void selectionChanged(const QModelIndex &);

		void acceptAnnotation();
		void rejectAnnotation();
		void requestAnnotationVerification();

	private:

		const bool getComment(const QString & title, const QString & content, QString & comment);

		void setActionsEnabled(const bool enable);

		void refreshChat(const AnnotationStatus & as);


		const AnnotationStatus getAnnotationStatus(const std::string& user, int trialId, bool refresh = true) const;

		const bool verifySerie();

	private:
		//! ui z designera
		Ui::LayeredImageVisualizer* ui;
		//! model wizualizatora
		LayeredImageVisualizer* model;
		QGraphicsView* lastView;
		AdnotationsDelegate * adnotationDelegate0;
		AdnotationsDelegate * adnotationDelegate1;
		coreUI::CoreAction* acceptAction;
		coreUI::CoreAction* rejectAction;
		coreUI::CoreAction* toVerifyAction;

		static hmdbServices::DateTime lastUpdate;
		static std::map<int, std::map<int, AnnotationStatus>> annotations;
		static hmdbServices::xmlWsdl::UsersList usersList;
	};
}
#endif
