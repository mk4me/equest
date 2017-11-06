#include "CommunicationPCH.h"
#include "DisordersDataWidget.h"
#include <qthtmlgenlib/DocumentGenerator.h>
#include <qthtmlgenlib/TableGenerator.h>

DisordersDataWidget::DisordersDataWidget(QWidget * parent) : QTextEdit(parent)
{
	initDisorders();
	setReadOnly(true);
}

DisordersDataWidget::~DisordersDataWidget()
{

}

void DisordersDataWidget::setAutoUpdate(bool autoUpdate)
{
	autoUpdate_ = autoUpdate;
	tryUpdateContent();
}

void DisordersDataWidget::setDisordersCount(int count)
{
	disordersTable.content.setRows(count + 1);
	initDisordersAtributes();

	clearDisorders();
}

void DisordersDataWidget::clearDisorders()
{
	bool tmpAutoUpdate = autoUpdate_;
	setAutoUpdate(false);
	auto last = disordersTable.content.rows() - 1;
	for(int i = 0; i < last; ++i){
		setDisorder(i, QString(), QString(), QString());
	}

	setAutoUpdate(tmpAutoUpdate);
}

void DisordersDataWidget::setDisorder(int idx, const QString & name, const QString & diagnosisDate, const QString & focus)
{
	int row = idx + 1;
	disordersTable.content.setCell(row, 0, QString::number(row));
	disordersTable.content.setCell(row, 1, name);
	disordersTable.content.setCell(row, 2, diagnosisDate);
	disordersTable.content.setCell(row, 3, focus);

	tryChangeContent();
}

void DisordersDataWidget::initDisorders()
{
	initDisordersContent();
	initDisordersHeaderStructure();
	initDisordersAtributes();
	initDisordersStyles();
}

void DisordersDataWidget::initDisordersHeaderStructure()
{
	disordersTable.headersStructure.headerRows = 1;
	disordersTable.headersStructure.headerColumns = 1;
}

void DisordersDataWidget::initDisordersContent()
{
	//wymiary
	disordersTable.content.setDimensions(3, 4);

	//zawartość tabeli z danymi ogólnymi
	//! wiersz nagłówkowy - pierwsza komórka pusta - łącznik z nagłówkiem pionowym
	disordersTable.content.setCell(0, 1, tr("Disorder"));
	disordersTable.content.setCell(0, 2, tr("Diagnosis date"));
	disordersTable.content.setCell(0, 3, tr("Focus"));
	//! kolumna nagłówkowa - pierwsza komórka pusta - łącznik z nagłówkiem poziomym
	disordersTable.content.setCell(1, 0, QString::number(1));
	disordersTable.content.setCell(2, 0, QString::number(2));	
}

void DisordersDataWidget::initDisordersStyles()
{
	disordersTable.styles.tableAttributes.setCellPadding(4);
	disordersTable.styles.tableAttributes.setCellSpacing(0);
	disordersTable.styles.tableAttributes.setWidth(100, htmlGen::WRelative);
	disordersTable.styles.tableAttributes.setHeight(100, htmlGen::WRelative);
	disordersTable.styles.tableAttributes.setBorder(1);

	disordersTable.styles.tableStyle_ = "border-color: rgb(0, 0, 0);";
	disordersTable.styles.headerRowStyle_[htmlGen::Single] = "color: white; background: rgb(41, 41, 41);";

	//wiersz nagłówkowy
	disordersTable.styles.headersStyles[htmlGen::Single][htmlGen::Middle] = "color: white; border-style: none; border-color: white;";
	// kolumna nagłówkowa - musze dodać kolor, nie da się kolumnom przypisywać właściwości
	disordersTable.styles.headersStyles[htmlGen::Middle][htmlGen::Single] = "color: white; border-style: none; border-color: white; background: rgb(41, 41, 41);";
	// ostatni element kolumny nagłówkowej - tylko kolor
	disordersTable.styles.headersStyles[htmlGen::Last][htmlGen::Single] = "color: white; background: rgb(41, 41, 41);";

	//style contentu - tutaj chcemy uzyskać efekt kratownicy bez ramki
	disordersTable.styles.contentStyles[htmlGen::First][htmlGen::First] = "border-style: none; border-color: rgb(41, 41, 41);";
	disordersTable.styles.contentStyles[htmlGen::Last][htmlGen::First] = "border-style: none; border-color: rgb(41, 41, 41);";
	disordersTable.styles.contentStyles[htmlGen::Last][htmlGen::Last] = "border-style: none; border-color: rgb(41, 41, 41);";
}

void DisordersDataWidget::initDisordersAtributes()
{
	disordersTable.cellsAttributes.setDimensions(disordersTable.content.rows(), disordersTable.content.columns());

	htmlGen::HtmlCellAttributes attributes;
	attributes.setHAlign(htmlGen::HACenter);
	attributes.setVAlign(htmlGen::VAMiddle);

	//centrujemy dane
	for(int i = 0; i < disordersTable.cellsAttributes.rows(); ++i){
		for(int j = 0; j < disordersTable.cellsAttributes.columns(); ++j){
			disordersTable.cellsAttributes.setCell(i, j, attributes);
		}
	}

	attributes.setHAlign(htmlGen::HALeft);

	//kolumnę z opisem wyrównujemy do lewej
	for(int i = 1; i < disordersTable.cellsAttributes.rows(); ++i){
		disordersTable.cellsAttributes.setCell(i, 1, attributes);
	}

	disordersTable.cellsAttributes.cell(0, 0).setWidth(5, htmlGen::WRelative);
	disordersTable.cellsAttributes.cell(0, 2).setWidth(20, htmlGen::WRelative);
	disordersTable.cellsAttributes.cell(0, 3).setWidth(10, htmlGen::WRelative);
}

void DisordersDataWidget::updateContent()
{
	QString html;

	htmlGen::DocumentGenerator::openDocument(html);
	htmlGen::DocumentGenerator::openBody(html);

	QString disTable;

	htmlGen::HtmlDataTableGenerator::generateHtmlTable(disTable, disordersTable.content, disordersTable.headersStructure,
		disordersTable.cellsAttributes, disordersTable.styles);

	html += disTable;

	htmlGen::DocumentGenerator::closeBody(html);
	htmlGen::DocumentGenerator::closeDocument(html);

	setHtml(html);
}

void DisordersDataWidget::tryUpdateContent()
{
	if(autoUpdate_ == true && changed_ == true){
		updateContent();
		changed_ = false;
	}
}

void DisordersDataWidget::tryChangeContent()
{
	if(autoUpdate_ == true){
		updateContent();
		changed_ = false;
	}else{
		changed_ = true;
	}
}
