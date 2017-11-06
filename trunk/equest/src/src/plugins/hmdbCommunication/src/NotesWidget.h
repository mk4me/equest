/********************************************************************
    created:  2012/03/12
    created:  12:3:2012   17:49
    filename: NotesWidget.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD_HMDBCOMMUNICATION__NOTESWIDGET_H__
#define HEADER_GUARD_HMDBCOMMUNICATION__NOTESWIDGET_H__

#include "ui_NotesWidget.h"
#include <QtWidgets/QFrame>
#include <QtWidgets/QDialog>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTextEdit>
#include <QtCore/QDateTime>
#include <utils/SmartPtr.h>

class NotesWidget : public QFrame, private Ui::NotesWidget
{
	Q_OBJECT

public:

	NotesWidget(QWidget * parent = nullptr);
	virtual ~NotesWidget();

public slots:

	//! \param patientID Identyfikator aktualnego pacjenta
	void setCurrentPatient(int patientID);

	//! \param patientID Identyfikator pacjenta któemu dodajemy notatkę
	//! \param created Czas utworzenia notatki
	//! \param title Tytuł notatki
	//! \param text Treść notatki
	void addNote(int patientID, const QDateTime & created, const QString & title, const QString & text);
	//! \param created Czas utworzenia notatki
	//! \param title Tytuł notatki
	//! \param text Treść notatki
	void addNote(const QDateTime & created, const QString & title, const QString & text);

	//! Usuwa aktualną notatkę
	void removeNote();

	//! Wywołuje dialog dodawania notatki
	void addNoteDialog();
	//! Wywołuje dialog edycji notatki
	void editNoteDialog();

	//! Czyści zawartość pola notatki
	void clearNote();

	//! Wypełnia listę notatek (tabelę)
	void fillNotesList();
	//! Czyści liste notatek (tabelę)
	void clearNotesList();

private slots:

	//! Ładuje text aktualnej notatki
	void loadNote();
	//! \param created Czas utworzenia notatki
	//! \param title Tytuł notatki
	//! \param text Treść notatki
	void updateNote(const QDateTime & modified, const QString & title, const QString & text);
	//! Przeładowywuje notatki po zmianie użytkownika
	void reloadPatientNotes();

	//! Reakcja na zmianę notatki
	void onCurrentNoteChange();
	//! Weryfikuje dane dialogu edycji i nowej notatki
	void noteDialogConfirm();

private:
	//! \return Unikalne ID notatki
	int generateNoteUniqueID();

private:
	//! Struktura opisująca notatkę
	struct NoteData {
		//! Tytuł
		QString title;
		//! Treść
		QString text;
		//! Lokalne ID w obrębie pacjenta
		int localID;
		//! Globalne ID
		int globalID;
		//! ID pacjenta
		int patientID;
		//! Czas utworzenia
		QDateTime created;
		//! Czas modyfikacji
		QDateTime modified;
	};

	//! Agregat indeksowanych notatek
	typedef std::map<int, utils::shared_ptr<NoteData>> Notes;
	//! Notatki wg pacjentów
	typedef std::map<int, Notes> PatientNotes;

private:
	//! ID Aktualnego pacjenta
	int currentPatientID;
	//! Aktualna notatka
	utils::shared_ptr<NoteData> currentNote;
	//! Notatki
	Notes notes;
	//! Notatki wg pacjentów
	PatientNotes patientNotes;
};

class NoteDialog : public QDialog
{
	Q_OBJECT

public:

	NoteDialog(QWidget * parent = nullptr);
	virtual ~NoteDialog();

	QLineEdit * titleEdit;
	QTextEdit * textEdit;

	QPushButton * acceptButton;
	QPushButton * cancelButton;

signals:

	void acceptRequest();

private slots:

	void buttonAccepted();
};

#endif	//	HEADER_GUARD_HMDBCOMMUNICATION__NOTESWIDGET_H__
