#include "CommunicationPCH.h"
#include "NotesWidget.h"
#include <utils/Debug.h>

#include <QtWidgets/QFormLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QTableWidgetItem>
#include <QtWidgets/QMessageBox>


NoteDialog::NoteDialog(QWidget * parent) : QDialog(parent),
	titleEdit(new QLineEdit()), textEdit(new QTextEdit()),
	acceptButton(new QPushButton()), cancelButton(new QPushButton(tr("Cancel")))
{
	connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
	connect(acceptButton, SIGNAL(clicked()), this, SLOT(buttonAccepted()));

	QFormLayout * formLayout = new QFormLayout();

	formLayout->addRow(tr("Title"), titleEdit);
	formLayout->addRow(tr("Text"), textEdit);

	QHBoxLayout * hBoxLayout = new QHBoxLayout();
	hBoxLayout->addWidget(acceptButton);
	hBoxLayout->addItem(new QSpacerItem(0,0, QSizePolicy::Expanding, QSizePolicy::Fixed));
	hBoxLayout->addWidget(cancelButton);

	QVBoxLayout * vBoxLayout = new QVBoxLayout();

	vBoxLayout->addLayout(formLayout);
	vBoxLayout->addLayout(hBoxLayout);

	this->setLayout(vBoxLayout);
}

void NoteDialog::buttonAccepted()
{
	emit acceptRequest();
}

NoteDialog::~NoteDialog()
{

}

NotesWidget::NotesWidget(QWidget * parent) : QFrame(parent), currentPatientID(-1)
{
	setupUi(this);
	notesTable->horizontalHeader()->setStyleSheet(QString::fromUtf8("QHeaderView::section:horizontal {\n"
		"	background: rgb(41,41,41);\n"
		"	color: white;\n"
		"	padding: 2px;\n"
		"	border: 2px solid black;\n"
		"	border-top: none;\n"
		"	border-right: none;\n"
		"}"));
	
	removeNoteButton->setEnabled(false);
	editNoteButton->setEnabled(false);
}

NotesWidget::~NotesWidget()
{

}

void NotesWidget::setCurrentPatient(int patientID)
{
	currentPatientID = patientID;

	reloadPatientNotes();
}

void NotesWidget::reloadPatientNotes()
{
	clearNotesList();
	fillNotesList();

	for(int i = 0; i < notesTable->columnCount(); ++i){
		notesTable->resizeColumnToContents(i);
	}
}

void NotesWidget::addNote(int patientID, const QDateTime & created, const QString & title, const QString & text)
{
	UTILS_ASSERT(patientID >= 0,"Bledny identyfikator pacjenta");

	utils::shared_ptr<NoteData> noteData(new NoteData());
	noteData->title = title;
	noteData->text = text;
	noteData->globalID = generateNoteUniqueID();
	noteData->patientID = patientID;
	noteData->localID = patientNotes[patientID].size();
	noteData->created = created;

	notes[noteData->globalID] = noteData;

	patientNotes[patientID][noteData->localID] = noteData;

	if(currentPatientID == patientID){
		//dodać dane do listy(tabeli)
		bool sorting = notesTable->isSortingEnabled();
		notesTable->setSortingEnabled(false);
		int row = notesTable->rowCount();
		notesTable->insertRow(row);
		//ID
		notesTable->setItem(row, 0, new QTableWidgetItem(QString::number(noteData->localID)));
		//Title
		notesTable->setItem(row, 1, new QTableWidgetItem(noteData->title));
		//Created
		notesTable->setItem(row, 1, new QTableWidgetItem(noteData->created.toString("dd.MM.yyyy")));
		//Modified - empty
		notesTable->setItem(row, 1, new QTableWidgetItem());
		notesTable->setSortingEnabled(sorting);
	}
}

void NotesWidget::addNote(const QDateTime & created, const QString & title, const QString & text)
{
	utils::shared_ptr<NoteData> noteData(new NoteData());
	noteData->title = title;
	noteData->text = text;
	noteData->globalID = generateNoteUniqueID();
	noteData->patientID = currentPatientID;
	noteData->localID = patientNotes[currentPatientID].size() + 1;
	noteData->created = created;

	notes[noteData->globalID] = noteData;

	patientNotes[currentPatientID][noteData->localID] = noteData;

	bool sorting = notesTable->isSortingEnabled();
	notesTable->setSortingEnabled(false);
	int row = notesTable->rowCount();
	notesTable->insertRow(row);
	//ID
	notesTable->setItem(row, 0, new QTableWidgetItem(QString::number(noteData->globalID)));
	//Title
	notesTable->setItem(row, 1, new QTableWidgetItem(noteData->title));
	//Created
	notesTable->setItem(row, 2, new QTableWidgetItem(noteData->created.toString("dd.MM.yyyy")));
	//Modified - empty
	notesTable->setItem(row, 3, new QTableWidgetItem());
	notesTable->setSortingEnabled(sorting);
}

void NotesWidget::removeNote()
{
	auto localID = currentNote->localID;
	notes.erase(currentNote->globalID);

	auto& pNotes = patientNotes[currentPatientID];
	// ta operacja podmnienia currentNote na nullptr!
	notesTable->clearSelection();
	bool sorting = notesTable->isSortingEnabled();
	notesTable->setSortingEnabled(false);
	notesTable->removeRow(notesTable->currentRow());

	for(unsigned int i = localID; i < pNotes.size() - 1; ++i){
		pNotes[i] = pNotes[i+1];
		pNotes[i]->localID = i;

		//przebuduj indeksy pozostałych
		notesTable->item(notesTable->visualRow(i), 0)->setText(QString::number(i));
	}

	pNotes.erase(--pNotes.end());

	notesTable->setSortingEnabled(sorting);

	if(notesTable->rowCount() > 0){
		notesTable->selectRow(0);
	}
}

void NotesWidget::addNoteDialog()
{
	NoteDialog dialog;

	dialog.setWindowTitle(tr("New note"));
	dialog.acceptButton->setText(tr("Add note"));

	connect(&dialog, SIGNAL(acceptRequest()), this, SLOT(noteDialogConfirm()));

	int ret = dialog.exec();
	if(ret == QDialog::Accepted){
		addNote(QDateTime::currentDateTime(), dialog.titleEdit->text(), dialog.textEdit->toPlainText());
	}
}

void NotesWidget::editNoteDialog()
{
	NoteDialog dialog;

	dialog.setWindowTitle(tr("Edit note"));
	dialog.acceptButton->setText(tr("Update note"));

	connect(&dialog, SIGNAL(acceptRequest()), this, SLOT(noteDialogConfirm()));

	int ret = dialog.exec();

	if(ret == QDialog::Accepted){
		updateNote(QDateTime::currentDateTime(), dialog.titleEdit->text(), dialog.textEdit->toPlainText());
	}
}

void NotesWidget::noteDialogConfirm()
{
	NoteDialog * dialog = qobject_cast<NoteDialog*>(sender());

	if(dialog->titleEdit->text().isEmpty() == true || dialog->textEdit->toPlainText().isEmpty() == true){
		QMessageBox message(this);
		message.setWindowTitle(tr("Note validation"));
		message.setText(tr("All note fields must be filled. Please fill them and try again."));
		message.setIcon(QMessageBox::Warning);
		message.setStandardButtons(QMessageBox::Ok);
		message.setDefaultButton(QMessageBox::Ok);
		message.show();
	}else{

		dialog->accept();
	}
}

void NotesWidget::loadNote()
{
	if(currentNote == nullptr){
		notePlaceholder->clear();
	}else{
		notePlaceholder->setText(currentNote->text);
	}
}

void NotesWidget::clearNote()
{
	notePlaceholder->clear();
}

void NotesWidget::fillNotesList()
{
	auto it = patientNotes.find(currentPatientID);

	bool sorting = notesTable->isSortingEnabled();
	notesTable->setSortingEnabled(false);

	if(it == patientNotes.end() || it->second.empty() == true){
		//tworzymy puste wiersze

		// TODO - w jakim celu byly tworzone te wiersze?
		/*int emptyRows = 2;
		notesTable->setRowCount(emptyRows);

		for(int row = 0; row < emptyRows; ++row){
			notesTable->setItem(row, 0, new QTableWidgetItem());
			notesTable->setItem(row, 1, new QTableWidgetItem());
			notesTable->setItem(row, 2, new QTableWidgetItem());
			notesTable->setItem(row, 3, new QTableWidgetItem());
		}*/
	}else{
		unsigned int rows = (std::max)(it->second.size(), static_cast<PatientNotes::size_type>(2));
		notesTable->setRowCount(rows);
		unsigned int row = 0;
		for( ; row < it->second.size(); ++row){
			notesTable->setItem(row, 0, new QTableWidgetItem(QString::number(it->second[row]->globalID)));
			notesTable->setItem(row, 1, new QTableWidgetItem(it->second[row]->title));
			notesTable->setItem(row, 2, new QTableWidgetItem(it->second[row]->created.toString("dd.MM.yyyy")));
			notesTable->setItem(row, 3, new QTableWidgetItem());
		}

		for( ; row < rows; ++row){
			notesTable->setItem(row, 0, new QTableWidgetItem());
			notesTable->setItem(row, 1, new QTableWidgetItem());
			notesTable->setItem(row, 2, new QTableWidgetItem());
			notesTable->setItem(row, 3, new QTableWidgetItem());
		}
	}

	notesTable->setSortingEnabled(sorting);
	notesTable->resizeColumnsToContents();
}

void NotesWidget::clearNotesList()
{
	notesTable->clearSelection();
	notesTable->clearContents();
}

void NotesWidget::updateNote(const QDateTime & modified, const QString & title, const QString & text)
{
	currentNote->modified = modified;
	currentNote->title = title;
	currentNote->text = text;

	//odśwież listę danych (tabelę) - aktualny wpis
	bool sorting = notesTable->isSortingEnabled();
	notesTable->setSortingEnabled(false);

	int row = notesTable->currentRow();
	notesTable->item(row, 1)->setText(title);
	notesTable->item(row, 3)->setText(modified.toString("dd.MM.yyyy"));

	notesTable->setSortingEnabled(sorting);
	//odśwież treść notatki
	loadNote();
}

void NotesWidget::onCurrentNoteChange()
{

	if(patientNotes.find(currentPatientID) == patientNotes.end()){
		return;
	}

	int noteID = -1;
	auto idText = notesTable->item(notesTable->currentRow(), 0)->text();

	if(idText.isEmpty() == false){
		noteID = idText.toInt();
		if(notes.find(noteID) == notes.end()) {
			noteID = -1;
		}
	}

	if(noteID < 0){
		currentNote.reset();
		clearNote();
		removeNoteButton->setEnabled(false);
		editNoteButton->setEnabled(false);
	}else{
		currentNote = notes[noteID];
		loadNote();
		removeNoteButton->setEnabled(true);
		editNoteButton->setEnabled(true);
	}
}

int NotesWidget::generateNoteUniqueID()
{
	return notes.size();
}
