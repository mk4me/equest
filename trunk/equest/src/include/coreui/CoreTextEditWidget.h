/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the demonstration applications of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

/********************************************************************
    created:  2013/01/30
    created:  30:1:2013   12:25
    filename: CoreTextEditWidget.h
    author:   Wojciech Knieć
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___CORETEXTEDITWIDGET_H__
#define HEADER_GUARD___CORETEXTEDITWIDGET_H__

#include <coreui/Export.h>
#include <QtWidgets/QMainWindow>
#include <QtCore/QMap>
#include <QtCore/QPointer>
#include <QtGui/QTextDocument>
#include <QtPrintSupport/QPrinter>

QT_FORWARD_DECLARE_CLASS(QAction)
QT_FORWARD_DECLARE_CLASS(QComboBox)
QT_FORWARD_DECLARE_CLASS(QFontComboBox)
QT_FORWARD_DECLARE_CLASS(QTextEdit)
QT_FORWARD_DECLARE_CLASS(QTextCharFormat)
QT_FORWARD_DECLARE_CLASS(QMenu)
QT_FORWARD_DECLARE_CLASS(QScrollArea)

namespace coreUI {

//TODO
//Czy to musi być QMainWIndow?

//! Kontrolka zawiera edytor tekstu rich format
class COREUI_EXPORT CoreTextEditWidget : public QMainWindow
{
    Q_OBJECT
    //! możliwe formaty zapisu
    enum SaveMode {
        toHtml,
        toOdt
    };

public:
    //! konstruktor
    //! \param parent 
    CoreTextEditWidget(QWidget *parent = 0, int pageWidth = 1000);

public slots:
    //! zasila kontrolkę w kod html
    //! \param html kod, który ma zostać wyświetlony 
    void setHtml(const QString& html);


public:
    //! \return akcje związane z zapisem / odczytem plików
    const QList<QObject*>& getFileActions() const { return fileList; }
    //! \return akcje związane z edycją tekstu
    const QList<QObject*>& getEditActions() const { return editList; }
    //! \return akcje związane z operacjami na tekście
    const QList<QObject*>& getTextActions() const { return textList; }

private:
    //! wywoływane po zmianie czcionki
    //! \param f nowa czcionka
    void fontChanged(const QFont &f);
    //! wywoływane po zmianie koloru
    //! \param c nowy kolor tesktu
    void colorChanged(const QColor &c);
    //! wywoływane po zmianie wyrównania
    //! \param a nowe wyrównanie zaznaczonego tekstu
    void alignmentChanged(Qt::Alignment a);
    //! łączy stary format z nowym (np. bold + italic)
    //! \param format nowy format
    void mergeFormatOnWordOrSelection(const QTextCharFormat &format);
    //! próba zapisu
    //! \return true lub false, gdy użytkownik zrezygnował z zapisu
    bool maybeSave();
    //! wczytuje plik
    //! \param f ścieżka pliku do wczytania
    bool load(const QString &f);
    //! ustawia nazwę aktualnego pliku
    //! \param fileName nazwa aktualnie edytowanego pliku
    void setCurrentFileName(const QString &fileName);
    //! twórz akcje związane z zapisem / odczytem plików
    void createFileActions();
    //! twórz akcje związane z edycją tekstu
    void createEditActions();
    //! twórz akcje związane z operacjami na tekście
    void createTextActions();

private slots:
    //! twórz nowy plik
    void fileNew();
    //! otwórz nowy plik
    void fileOpen();
    //! zapisz plik
    //! \return true lub false, gdy użytkownik zrezygnował
    bool fileSave();
    //! zapisz plik jako..
    //! \return true lub false, gdy użytkownik zrezygnował
    bool fileSaveAs();
    //! drukuj
    void filePrint();
    //! pogląd wydruku
    void filePrintPreview();
    //! twórz plik pdf
    void filePrintPdf();
    //! pogrubienie tekstu
    void textBold();
    //! podkreślenie tekstu
    void textUnderline();
    //! kursywa tekstu
    void textItalic();
    //! zmieniła się czcionka
    //! \param f nowa czcionka
    void textFamily(const QString &f);
    //! zmienił się rozmiar czcionki
    //! \param p nowy rozmiar czcionki
    void textSize(const QString &p);
    //! zmienił się styl
    //! \param styleIndex 
    void textStyle(int styleIndex);
    //! zmienił się kolor tekstu
    void textColor();
    //! wyrównaj tekst
    //! \param a 
    void textAlign(QAction *a);
    //! zmienił się format
    //! \param format 
    void currentCharFormatChanged(const QTextCharFormat &format);
    //! zmieniła się pozycja kursora
    void cursorPositionChanged();
    //! zmieniła się zawartość schowka
    void clipboardDataChanged();
    //! 
    void about();
    //! pokaż podgląd wydruku
    //! \param * 
    void printPreview(QPrinter *);

private:
    //! metoda tworzy zasób dokumentu na podstawie kodu base 64
    //! \param document dokument, do którego trafi nowy zasób
    //! \param encrypted obrazek zakodowany jako base 64
    //! \return ścieżka do zasobu
    QString base64ToResource(QTextDocument* document, const QString& encrypted) const;
    //! metoda tworzy obrazek w kodzie base64 na podstawie zasobu dokumentu
    //! \param document dokument zawierający zasób
    //! \param resource ścieżka do zasobu
    //! \return obrazek w kodzie base64
    QString resourceToBase64(QTextDocument* document, const QString& resource) const;
    //! metoda zamienia odnośnik (html) na obrazek w kodzie base64
    //! \param result obrazek w kodziebase64
    //! \param link link do obrazka (html)
    //! \return true, jeśli się udało, false, w przeciwnym razie
    bool tryChangeLinkToBase64(QString& result, const QString& link) const;
    //! metoda przechodzi przez tekst i przygotowuje go do zapisu
    //! \param root ramka z zapisywanym tekstem
    //! \param savedDocument zapisywany dokument
    //! \param saveMode tryb zapisu, na tej podstawie tworzone są zasoby, lub obrazki w base64
    void processFrame( QTextFrame * root, QTextDocument* savedDocument, SaveMode saveMode );
    //! zamienia wszystkie odnośniki na obrazki w kodzie base64
    //! \param root ramka ze zmienianym tekstem
    //! \param savedDocument zmieniany dokument
    void changeLinksToBase64( QTextFrame * root, QTextDocument* document);


private: 
    //! lista akcji związanych z edycją tekstu
    QList<QObject*> editList;
    //! lista akcji związanych z zapisem / odczytem plików
    QList<QObject*> fileList;
    //! lista akcji związanych z operacjami na tekście
    QList<QObject*> textList;

    //! akcja, która trafi do flexi widgeta
    QAction* actionSave;
    //! akcja, która trafi do flexi widgeta
    QAction* actionTextBold;
    //! akcja, która trafi do flexi widgeta
    QAction* actionTextUnderline;
    //! akcja, która trafi do flexi widgeta
    QAction* actionTextItalic;
    //! akcja, która trafi do flexi widgeta
    QAction* actionTextColor;
    //! akcja, która trafi do flexi widgeta
    QAction* actionAlignLeft;
    //! akcja, która trafi do flexi widgeta
    QAction* actionAlignCenter;
    //! akcja, która trafi do flexi widgeta
    QAction* actionAlignRight;
    //! akcja, która trafi do flexi widgeta
    QAction* actionAlignJustify;
    //! akcja, która trafi do flexi widgeta
    QAction* actionUndo;
    //! akcja, która trafi do flexi widgeta
    QAction* actionRedo;
    //! akcja, która trafi do flexi widgeta
    QAction* actionCut;
    //! akcja, która trafi do flexi widgeta
    QAction* actionCopy;
    //! akcja, która trafi do flexi widgeta
    QAction* actionPaste;
    //! akcja, która trafi do flexi widgeta
    QAction* actionNew;
    //! akcja, która trafi do flexi widgeta
    QAction* actionOpen;
    //! akcja, która trafi do flexi widgeta
    QAction* actionSaveAs;
    //! akcja, która trafi do flexi widgeta
    QAction* actionPrint;
    //! akcja, która trafi do flexi widgeta
    QAction* actionPrintPreview;
    //! akcja, która trafi do flexi widgeta
    QAction* actionExportPdf;

    //! kombo ze stylami, które trafi do flexi widgeta
    QComboBox *comboStyle;
    //! kombo z czcionkami, które trafi do flexi widgeta
    QFontComboBox *comboFont;
    //! kombo z rozmiarami, które trafi do flexi widgeta
    QComboBox *comboSize;

    //! aktualna nazwa edytowanego pliku
    QString fileName;
    //! kontrolka z edytowanym tekstem
    QTextEdit *textEdit;
    //! kontrolka opakowująca tekst i marginesy wokoło niego
    QScrollArea* scroll;
    //! szerokość wyświetlanej strony
    int pageWidth;
};

}

#endif	//	HEADER_GUARD___CORETEXTEDITWIDGET_H__
