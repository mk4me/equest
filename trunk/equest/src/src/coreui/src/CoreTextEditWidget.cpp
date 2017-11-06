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

#include "CoreUiPCH.h"
#include <coreui/CoreTextEditWidget.h>
#include <utils/Debug.h>
#include <corelib/PluginCommon.h>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtGui/QClipboard>
#include <QtWidgets/QColorDialog>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QFontComboBox>
#include <QtCore/QFile>
#include <QtWidgets/QFileDialog>
#include <QtCore/QFileInfo>
#include <QtGui/QFontDatabase>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtPrintSupport/QPrintDialog>
#include <QtPrintSupport/QPrinter>
#include <QtCore/QTextCodec>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QLabel>
#include <QtGui/QTextCursor>
#include <QtGui/QTextDocumentWriter>
#include <QtGui/QTextList>
#include <QtCore/QtDebug>
#include <QtCore/QMimeData>
#include <QtGui/QCloseEvent>
#include <QtWidgets/QMessageBox>
#include <QtPrintSupport/QPrintPreviewDialog>
#include <QtCore/QFile>
#include <QtGui/QTextFrame>
#include <QtCore/QUrl>
#include <QtCore/QBuffer>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QScrollBar>
#include <QtWidgets/QLayout>
#include <corelib/PluginCommon.h>

const QString rsrcPath = ":/coreUI/icons/textedit";

using namespace coreUI;

CoreTextEditWidget::CoreTextEditWidget(QWidget *parent, int pageWidth):
    QMainWindow(parent),
    pageWidth(pageWidth)
{
    int topMargin = 64;

    textEdit = new QTextEdit();
    auto left = new QWidget();
    auto right = new QWidget();
    auto cont = new QWidget();
    cont->setLayout(new QHBoxLayout());
    cont->layout()->addWidget(left);
    cont->layout()->addWidget(textEdit);
    cont->layout()->addWidget(right);
    cont->layout()->setSpacing(0);
    cont->layout()->setContentsMargins(0,0,0,0);

    auto top = new QWidget();
    top->setMinimumHeight(topMargin);
    auto bottom = new QWidget();
    bottom->setMinimumHeight(topMargin);
    QWidget* widget = new QWidget();
    widget->setLayout(new QVBoxLayout());
    widget->layout()->addWidget(top);
    widget->layout()->addWidget(cont);
    widget->layout()->addWidget(bottom);
    widget->layout()->setSpacing(0);
    widget->layout()->setContentsMargins(0,0,0,0);

    textEdit->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    textEdit->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    
    textEdit->setMinimumWidth(pageWidth);
    textEdit->setMaximumWidth(pageWidth);
    textEdit->setMinimumHeight(pageWidth / 0.7);


    scroll = new QScrollArea();
    //scroll->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    scroll->setWidget(widget);
    scroll->setWidgetResizable(true);
    scroll->setStyleSheet(QString::fromUtf8("background-color: rgb(220, 220, 220)"));
    textEdit->setStyleSheet(QString::fromUtf8("background-color: white"));
    connect(textEdit, SIGNAL(currentCharFormatChanged(QTextCharFormat)), this, SLOT(currentCharFormatChanged(QTextCharFormat)));
    connect(textEdit, SIGNAL(cursorPositionChanged()), this, SLOT(cursorPositionChanged()));
    setCentralWidget(scroll);
    textEdit->setFocus();
    setCurrentFileName(QString());

    createEditActions();
    createFileActions();
    createTextActions();

    fontChanged(textEdit->font());
    colorChanged(textEdit->textColor());
    alignmentChanged(textEdit->alignment());

    connect(textEdit->document(), SIGNAL(modificationChanged(bool)), this, SLOT(setWindowModified(bool)));

    setWindowModified(textEdit->document()->isModified());



#ifndef QT_NO_CLIPBOARD
    connect(QApplication::clipboard(), SIGNAL(dataChanged()), this, SLOT(clipboardDataChanged()));
#endif

}

void CoreTextEditWidget::createFileActions()
{
    UTILS_ASSERT(textEdit);
    QAction *a;

    QIcon newIcon = QIcon::fromTheme("document-new", QIcon(rsrcPath + "/filenew.png"));
    actionNew = a = new QAction( newIcon, tr("&New"), this);
    a->setPriority(QAction::LowPriority);
    a->setShortcut(QKeySequence::New);
    connect(a, SIGNAL(triggered()), this, SLOT(fileNew()));
    fileList.push_back(a);

    actionOpen = a = new QAction(QIcon::fromTheme("document-open", QIcon(rsrcPath + "/fileopen.png")),
                    tr("&Open..."), this);
    a->setShortcut(QKeySequence::Open);
    connect(a, SIGNAL(triggered()), this, SLOT(fileOpen()));
    fileList.push_back(a);

    actionSave = a = new QAction(QIcon::fromTheme("document-save", QIcon(rsrcPath + "/filesave.png")), tr("&Save"), this);
    a->setShortcut(QKeySequence::Save);
    connect(a, SIGNAL(triggered()), this, SLOT(fileSave()));
    connect(textEdit->document(), SIGNAL(modificationChanged(bool)), actionSave, SLOT(setEnabled(bool)));
    a->setEnabled(textEdit->document()->isModified());
    fileList.push_back(a);

    actionSaveAs = a = new QAction(QIcon(rsrcPath + "/filesave.png"), tr("Save &As..."), this);
    a->setPriority(QAction::LowPriority);
    connect(a, SIGNAL(triggered()), this, SLOT(fileSaveAs()));
    fileList.push_back(a);

#ifndef QT_NO_PRINTER
    actionPrint = a = new QAction(QIcon::fromTheme("document-print", QIcon(rsrcPath + "/fileprint.png")),
                    tr("&Print..."), this);
    a->setPriority(QAction::LowPriority);
    a->setShortcut(QKeySequence::Print);
    connect(a, SIGNAL(triggered()), this, SLOT(filePrint()));
    fileList.push_back(a);

    actionPrintPreview = a = new QAction(QIcon::fromTheme("fileprint", QIcon(rsrcPath + "/fileprint.png")),
                    tr("Print Preview..."), this);
    connect(a, SIGNAL(triggered()), this, SLOT(filePrintPreview()));
    fileList.push_back(a);

    actionExportPdf = a = new QAction(QIcon::fromTheme("exportpdf", QIcon(rsrcPath + "/exportpdf.png")),
    tr("&Export PDF..."), this);
    a->setPriority(QAction::LowPriority);
    a->setShortcut(Qt::CTRL + Qt::Key_D);
    connect(a, SIGNAL(triggered()), this, SLOT(filePrintPdf()));
    fileList.push_back(a);
#endif

   /* a = new QAction(tr("&Quit"), this);
    a->setShortcut(Qt::CTRL + Qt::Key_Q);
    connect(a, SIGNAL(triggered()), this, SLOT(close()));
    fileList.push_back(a);*/
}

void CoreTextEditWidget::createEditActions()
{
    QAction *a;
    a = actionUndo = new QAction(QIcon::fromTheme("edit-undo", QIcon(rsrcPath + "/editundo.png")), tr("&Undo"), this);

    a->setShortcut(QKeySequence::Undo);
    editList.push_back(a);

    a = actionRedo = new QAction(QIcon::fromTheme("edit-redo", QIcon(rsrcPath + "/editredo.png")), tr("&Redo"), this);
    a->setPriority(QAction::LowPriority);
    a->setShortcut(QKeySequence::Redo);
    editList.push_back(a);

    a = actionCut = new QAction(QIcon::fromTheme("edit-cut", QIcon(rsrcPath + "/editcut.png")), tr("Cu&t"), this);
    a->setPriority(QAction::LowPriority);
    a->setShortcut(QKeySequence::Cut);
    editList.push_back(a);

    a = actionCopy = new QAction(QIcon::fromTheme("edit-copy", QIcon(rsrcPath + "/editcopy.png")), tr("&Copy"), this);
    a->setPriority(QAction::LowPriority);
    a->setShortcut(QKeySequence::Copy);
    editList.push_back(a);

    a = actionPaste = new QAction(QIcon::fromTheme("edit-paste", QIcon(rsrcPath + "/editpaste.png")), tr("&Paste"), this);
    a->setPriority(QAction::LowPriority);
    a->setShortcut(QKeySequence::Paste);
    editList.push_back(a);

#ifndef QT_NO_CLIPBOARD
    if (const QMimeData *md = QApplication::clipboard()->mimeData()) {
        actionPaste->setEnabled(md->hasText());
    }
#endif

    actionUndo->setEnabled(textEdit->document()->isUndoAvailable());
    actionRedo->setEnabled(textEdit->document()->isRedoAvailable());

    connect(actionUndo, SIGNAL(triggered()), textEdit, SLOT(undo()));
    connect(actionRedo, SIGNAL(triggered()), textEdit, SLOT(redo()));

    actionCut->setEnabled(false);
    actionCopy->setEnabled(false);

    connect(actionCut, SIGNAL(triggered()), textEdit, SLOT(cut()));
    connect(actionCopy, SIGNAL(triggered()), textEdit, SLOT(copy()));
    connect(actionPaste, SIGNAL(triggered()), textEdit, SLOT(paste()));

    connect(textEdit, SIGNAL(copyAvailable(bool)), actionCut, SLOT(setEnabled(bool)));
    connect(textEdit, SIGNAL(copyAvailable(bool)), actionCopy, SLOT(setEnabled(bool)));

    connect(textEdit->document(), SIGNAL(undoAvailable(bool)), actionUndo, SLOT(setEnabled(bool)));
    connect(textEdit->document(), SIGNAL(redoAvailable(bool)), actionRedo, SLOT(setEnabled(bool)));
}

void CoreTextEditWidget::createTextActions()
{
    actionTextBold = new QAction(QIcon::fromTheme("format-text-bold", QIcon(rsrcPath + "/textbold.png")), tr("&Bold"), this);
    actionTextBold->setShortcut(Qt::CTRL + Qt::Key_B);
    actionTextBold->setPriority(QAction::LowPriority);
	QFont bold;
    bold.setBold(true);
    actionTextBold->setFont(bold);
    connect(actionTextBold, SIGNAL(triggered()), this, SLOT(textBold()));
    actionTextBold->setCheckable(true);
    textList.push_back(actionTextBold);

    actionTextItalic = new QAction(QIcon::fromTheme("format-text-italic", QIcon(rsrcPath + "/textitalic.png")), tr("&Italic"), this);
    actionTextItalic->setPriority(QAction::LowPriority);
    actionTextItalic->setShortcut(Qt::CTRL + Qt::Key_I);
    QFont italic;
    italic.setItalic(true);
    actionTextItalic->setFont(italic);
    connect(actionTextItalic, SIGNAL(triggered()), this, SLOT(textItalic()));
    actionTextItalic->setCheckable(true);
    textList.push_back(actionTextItalic);

    actionTextUnderline = new QAction(QIcon::fromTheme("format-text-underline", QIcon(rsrcPath + "/textunder.png")), tr("&Underline"), this);
    actionTextUnderline->setShortcut(Qt::CTRL + Qt::Key_U);
    actionTextUnderline->setPriority(QAction::LowPriority);
    QFont underline;
    underline.setUnderline(true);
    actionTextUnderline->setFont(underline);
    connect(actionTextUnderline, SIGNAL(triggered()), this, SLOT(textUnderline()));
    actionTextUnderline->setCheckable(true);
    textList.push_back(actionTextUnderline);

    QActionGroup *grp = new QActionGroup(this);
    connect(grp, SIGNAL(triggered(QAction*)), this, SLOT(textAlign(QAction*)));

    // Make sure the alignLeft  is always left of the alignRight
    if (QApplication::isLeftToRight()) {
        actionAlignLeft = new QAction(QIcon::fromTheme("format-justify-left", QIcon(rsrcPath + "/textleft.png")),
                                      tr("&Left"), grp);
        actionAlignCenter = new QAction(QIcon::fromTheme("format-justify-center", QIcon(rsrcPath + "/textcenter.png")), tr("C&enter"), grp);
        actionAlignRight = new QAction(QIcon::fromTheme("format-justify-right", QIcon(rsrcPath + "/textright.png")), tr("&Right"), grp);
    } else {
        actionAlignRight = new QAction(QIcon::fromTheme("format-justify-right", QIcon(rsrcPath + "/textright.png")), tr("&Right"), grp);
        actionAlignCenter = new QAction(QIcon::fromTheme("format-justify-center", QIcon(rsrcPath + "/textcenter.png")), tr("C&enter"), grp);
        actionAlignLeft = new QAction(QIcon::fromTheme("format-justify-left", QIcon(rsrcPath + "/textleft.png")), tr("&Left"), grp);
    }
    actionAlignJustify = new QAction(QIcon::fromTheme("format-justify-fill", QIcon(rsrcPath + "/textjustify.png")), tr("&Justify"), grp);

    actionAlignLeft->setShortcut(Qt::CTRL + Qt::Key_L);
    actionAlignLeft->setCheckable(true);
    actionAlignLeft->setPriority(QAction::LowPriority);
    actionAlignCenter->setShortcut(Qt::CTRL + Qt::Key_E);
    actionAlignCenter->setCheckable(true);
    actionAlignCenter->setPriority(QAction::LowPriority);
    actionAlignRight->setShortcut(Qt::CTRL + Qt::Key_R);
    actionAlignRight->setCheckable(true);
    actionAlignRight->setPriority(QAction::LowPriority);
    actionAlignJustify->setShortcut(Qt::CTRL + Qt::Key_J);
    actionAlignJustify->setCheckable(true);
    actionAlignJustify->setPriority(QAction::LowPriority);

    //textList.push_back(grp);
    textList.push_back(actionAlignCenter);
    textList.push_back(actionAlignJustify);
    textList.push_back(actionAlignLeft);
    textList.push_back(actionAlignRight);

    QPixmap pix(16, 16);
    pix.fill(Qt::black);
    actionTextColor = new QAction(pix, tr("&Color..."), this);
    connect(actionTextColor, SIGNAL(triggered()), this, SLOT(textColor()));
    textList.push_back(actionTextColor);

    comboStyle = new QComboBox();

    comboStyle->addItem("Standard");
    comboStyle->addItem("Bullet textList (Disc)");
    comboStyle->addItem("Bullet textList (Circle)");
    comboStyle->addItem("Bullet textList (Square)");
    comboStyle->addItem("Ordered textList (Decimal)");
    comboStyle->addItem("Ordered textList (Alpha lower)");
    comboStyle->addItem("Ordered textList (Alpha upper)");
    comboStyle->addItem("Ordered textList (Roman lower)");
    comboStyle->addItem("Ordered textList (Roman upper)");
    connect(comboStyle, SIGNAL(activated(int)), this, SLOT(textStyle(int)));

    textList.push_back(comboStyle);

    comboFont = new QFontComboBox();
    connect(comboFont, SIGNAL(activated(QString)), this, SLOT(textFamily(QString)));
    textList.push_back(comboFont);

    comboSize = new QComboBox();
    comboSize->setObjectName("comboSize");
    comboSize->setEditable(true);

    QFontDatabase db;
    for(int size : db.standardSizes())
        comboSize->addItem(QString::number(size));

    connect(comboSize, SIGNAL(activated(QString)),
            this, SLOT(textSize(QString)));
    comboSize->setCurrentIndex(comboSize->findText(QString::number(QApplication::font().pointSize())));

    textList.push_back(comboSize);
}

bool CoreTextEditWidget::load(const QString &f)
{
    if (!QFile::exists(f))
        return false;
    QFile file(f);
    if (!file.open(QFile::ReadOnly))
        return false;

    QByteArray data = file.readAll();
    QTextCodec *codec = Qt::codecForHtml(data);
    QString str = codec->toUnicode(data);
    if (Qt::mightBeRichText(str)) {
        textEdit->setHtml(str);
    } else {
        str = QString::fromLocal8Bit(data);
        textEdit->setPlainText(str);
    }

    setCurrentFileName(f);
    return true;
}

bool CoreTextEditWidget::maybeSave()
{
    if (!textEdit->document()->isModified())
        return true;
    if (fileName.startsWith(QLatin1String(":/")))
        return true;
    QMessageBox::StandardButton ret;
    ret = QMessageBox::warning(this, tr("Application"),
                               tr("The document has been modified.\n"
                                  "Do you want to save your changes?"),
                               QMessageBox::Save | QMessageBox::Discard
                               | QMessageBox::Cancel);
    if (ret == QMessageBox::Save)
        return fileSave();
    else if (ret == QMessageBox::Cancel)
        return false;
    return true;
}

void CoreTextEditWidget::setCurrentFileName(const QString &fileName)
{
    this->fileName = fileName;
    textEdit->document()->setModified(false);

    QString shownName;
    if (fileName.isEmpty())
        shownName = "untitled.txt";
    else
        shownName = QFileInfo(fileName).fileName();

    setWindowTitle(tr("%1[*] - %2").arg(shownName).arg(tr("Rich Text")));
    setWindowModified(false);
}

void CoreTextEditWidget::fileNew()
{
    if (maybeSave()) {
        textEdit->clear();
        setCurrentFileName(QString());
    }
}

void CoreTextEditWidget::fileOpen()
{
    QString fn = QFileDialog::getOpenFileName(this, tr("Open File..."),
                                              QString(), tr("HTML-Files (*.htm *.html);;All Files (*)"));
    if (!fn.isEmpty())
        load(fn);
}

bool CoreTextEditWidget::fileSave()
{
    if (fileName.isEmpty())
        return fileSaveAs();
    QTextDocument* savedDocument = textEdit->document();
    if(fileName.endsWith(".odt")) {
        savedDocument = textEdit->document();
        //QTextCursor cursor(savedDocument);
        //QTextFrame *root = savedDocument->rootFrame();
        processFrame(savedDocument->rootFrame(), savedDocument, toOdt);

    } else if (fileName.endsWith(".htm") || fileName.endsWith(".html")) {
        savedDocument = textEdit->document();
        processFrame(savedDocument->rootFrame(), savedDocument, toHtml);
    }
    QTextDocumentWriter writer(fileName);
    bool success = writer.write(savedDocument);
    if (success)
        textEdit->document()->setModified(false);
    return success;
}

bool CoreTextEditWidget::fileSaveAs()
{
    QString fn = QFileDialog::getSaveFileName(this, tr("Save as..."),
                                              QString(), tr("ODF files (*.odt);;HTML-Files (*.htm *.html);;All Files (*)"));
    if (fn.isEmpty())
        return false;
    if (! (fn.endsWith(".odt", Qt::CaseInsensitive) || fn.endsWith(".htm", Qt::CaseInsensitive) || fn.endsWith(".html", Qt::CaseInsensitive)) )
        fn += ".odt"; // default
    setCurrentFileName(fn);
    return fileSave();
}

void CoreTextEditWidget::filePrint()
{
#ifndef QT_NO_PRINTER
    QPrinter printer(QPrinter::HighResolution);
    printer.setOrientation(QPrinter::Landscape);
    printer.setPageMargins(15, 15, 15, 15, QPrinter::Millimeter);
    QPrintDialog *dlg = new QPrintDialog(&printer, this);
    if (textEdit->textCursor().hasSelection())
        dlg->addEnabledOption(QAbstractPrintDialog::PrintSelection);
    dlg->setWindowTitle(tr("Print Document"));
    if (dlg->exec() == QDialog::Accepted) {
        textEdit->print(&printer);
    }
    delete dlg;
#endif
}

void CoreTextEditWidget::filePrintPreview()
{
#ifndef QT_NO_PRINTER
    QPrinter printer(QPrinter::HighResolution);
    printer.setOrientation(QPrinter::Landscape);
    printer.setPageMargins(15, 15, 15, 15, QPrinter::Millimeter);
    QPrintPreviewDialog preview(&printer, this);
    connect(&preview, SIGNAL(paintRequested(QPrinter*)), SLOT(printPreview(QPrinter*)));
    preview.exec();
#endif
}

void CoreTextEditWidget::printPreview(QPrinter *printer)
{
#ifdef QT_NO_PRINTER
    Q_UNUSED(printer);
#else
    textEdit->print(printer);
#endif
}


void CoreTextEditWidget::filePrintPdf()
{
#ifndef QT_NO_PRINTER
//! [0]
    QString fileName = QFileDialog::getSaveFileName(this, "Export PDF",
                                                    QString(), "*.pdf");
    if (!fileName.isEmpty()) {
        if (QFileInfo(fileName).suffix().isEmpty())
            fileName.append(".pdf");
        QPrinter printer(QPrinter::HighResolution);
        printer.setPageSize(QPrinter::A3);
        printer.setOutputFormat(QPrinter::PdfFormat);
        printer.setOutputFileName(fileName);
        textEdit->document()->print(&printer);
    }
//! [0]
#endif
}

void CoreTextEditWidget::textBold()
{
    QTextCharFormat fmt;
    fmt.setFontWeight(actionTextBold->isChecked() ? QFont::Bold : QFont::Normal);
    mergeFormatOnWordOrSelection(fmt);
}

void CoreTextEditWidget::textUnderline()
{
    QTextCharFormat fmt;
    fmt.setFontUnderline(actionTextUnderline->isChecked());
    mergeFormatOnWordOrSelection(fmt);
}

void CoreTextEditWidget::textItalic()
{
    QTextCharFormat fmt;
    fmt.setFontItalic(actionTextItalic->isChecked());
    mergeFormatOnWordOrSelection(fmt);
}

void CoreTextEditWidget::textFamily(const QString &f)
{
    QTextCharFormat fmt;
    fmt.setFontFamily(f);
    mergeFormatOnWordOrSelection(fmt);
}

void CoreTextEditWidget::textSize(const QString &p)
{
    qreal pointSize = p.toFloat();
    if (p.toFloat() > 0) {
        QTextCharFormat fmt;
        fmt.setFontPointSize(pointSize);
        mergeFormatOnWordOrSelection(fmt);
    }
}

void CoreTextEditWidget::textStyle(int styleIndex)
{
    QTextCursor cursor = textEdit->textCursor();

    if (styleIndex != 0) {
        QTextListFormat::Style style = QTextListFormat::ListDisc;

        switch (styleIndex) {
            default:
            case 1:
                style = QTextListFormat::ListDisc;
                break;
            case 2:
                style = QTextListFormat::ListCircle;
                break;
            case 3:
                style = QTextListFormat::ListSquare;
                break;
            case 4:
                style = QTextListFormat::ListDecimal;
                break;
            case 5:
                style = QTextListFormat::ListLowerAlpha;
                break;
            case 6:
                style = QTextListFormat::ListUpperAlpha;
                break;
            case 7:
                style = QTextListFormat::ListLowerRoman;
                break;
            case 8:
                style = QTextListFormat::ListUpperRoman;
                break;
        }

        cursor.beginEditBlock();

        QTextBlockFormat blockFmt = cursor.blockFormat();

        QTextListFormat listFmt;

        if (cursor.currentList()) {
            listFmt = cursor.currentList()->format();
        } else {
            listFmt.setIndent(blockFmt.indent() + 1);
            blockFmt.setIndent(0);
            cursor.setBlockFormat(blockFmt);
        }

        listFmt.setStyle(style);

        cursor.createList(listFmt);

        cursor.endEditBlock();
    } else {
        // ####
        QTextBlockFormat bfmt;
        bfmt.setObjectIndex(-1);
        cursor.mergeBlockFormat(bfmt);
    }
}

void CoreTextEditWidget::textColor()
{
    QColor col = QColorDialog::getColor(textEdit->textColor(), this);
    if (!col.isValid())
        return;
    QTextCharFormat fmt;
    fmt.setForeground(col);
    mergeFormatOnWordOrSelection(fmt);
    colorChanged(col);
}

void CoreTextEditWidget::textAlign(QAction *a)
{
    if (a == actionAlignLeft)
        textEdit->setAlignment(Qt::AlignLeft | Qt::AlignAbsolute);
    else if (a == actionAlignCenter)
        textEdit->setAlignment(Qt::AlignHCenter);
    else if (a == actionAlignRight)
        textEdit->setAlignment(Qt::AlignRight | Qt::AlignAbsolute);
    else if (a == actionAlignJustify)
        textEdit->setAlignment(Qt::AlignJustify);
}

void CoreTextEditWidget::currentCharFormatChanged(const QTextCharFormat &format)
{
    fontChanged(format.font());
    colorChanged(format.foreground().color());
}

void CoreTextEditWidget::cursorPositionChanged()
{
    alignmentChanged(textEdit->alignment());
    textEdit->setMinimumHeight((std::max)((int)textEdit->document()->size().height(), textEdit->minimumHeight()));
    auto pos = textEdit->mapTo(scroll, textEdit->cursorRect().center());
    scroll->ensureVisible(pos.x(), pos.y() + scroll->verticalScrollBar()->value());
}

void CoreTextEditWidget::clipboardDataChanged()
{
#ifndef QT_NO_CLIPBOARD
    if (const QMimeData *md = QApplication::clipboard()->mimeData())
        actionPaste->setEnabled(md->hasText());
#endif
}

void CoreTextEditWidget::about()
{
    QMessageBox::about(this, tr("About"), tr("This example demonstrates Qt's "
        "rich text editing facilities in action, providing an example "
        "document for you to experiment with."));
}

void CoreTextEditWidget::mergeFormatOnWordOrSelection(const QTextCharFormat &format)
{
    QTextCursor cursor = textEdit->textCursor();
    if (!cursor.hasSelection())
        cursor.select(QTextCursor::WordUnderCursor);
    cursor.mergeCharFormat(format);
    textEdit->mergeCurrentCharFormat(format);
}

void CoreTextEditWidget::fontChanged(const QFont &f)
{
    comboFont->setCurrentIndex(comboFont->findText(QFontInfo(f).family()));
    comboSize->setCurrentIndex(comboSize->findText(QString::number(f.pointSize())));
    actionTextBold->setChecked(f.bold());
    actionTextItalic->setChecked(f.italic());
    actionTextUnderline->setChecked(f.underline());
}

void CoreTextEditWidget::colorChanged(const QColor &c)
{
    QPixmap pix(16, 16);
    pix.fill(c);
    actionTextColor->setIcon(pix);
}

void CoreTextEditWidget::alignmentChanged(Qt::Alignment a)
{
    if (a & Qt::AlignLeft) {
        actionAlignLeft->setChecked(true);
    } else if (a & Qt::AlignHCenter) {
        actionAlignCenter->setChecked(true);
    } else if (a & Qt::AlignRight) {
        actionAlignRight->setChecked(true);
    } else if (a & Qt::AlignJustify) {
        actionAlignJustify->setChecked(true);
    }
}

void CoreTextEditWidget::setHtml( const QString& html )
{
    textEdit->clear();
    textEdit->ensureCursorVisible();
    textEdit->setDisabled(false);
    textEdit->setReadOnly(false);
    textEdit->setFocus();
    setCurrentFileName(QString());
    textEdit->setHtml(html);
    changeLinksToBase64(textEdit->document()->rootFrame(), textEdit->document());
}

void CoreTextEditWidget::processFrame( QTextFrame * frame, QTextDocument* document, SaveMode saveMode )
{
    for (auto rootIt = frame->begin(); !(rootIt.atEnd()); ++rootIt) {
        QTextBlock block = rootIt.currentBlock();
        for (auto it = block.begin(); !(it.atEnd()); ++it) {
            QTextFragment fragment = it.fragment();
            if (fragment.isValid()) {
                QTextImageFormat newImageFormat = fragment.charFormat().toImageFormat();

                if (newImageFormat.isValid()) {
                    QString name = newImageFormat.name();

                    //QString prefix = (saveMode == toHtml) ? : "data:im";//age/png;base64,";
                    QString res;
                    if (saveMode == toOdt) {
                        if (name.startsWith("data:im")) { ;
                        res = base64ToResource(document, name.right(name.length() -  name.indexOf(",")));
                        }
                    } else if (name.startsWith("created_image", Qt::CaseInsensitive)) {
                        res = QString("data:image/png;base64,")  + resourceToBase64(document, name);
                    }

                    if (res.length()) {
                        QTextCursor c = textEdit->textCursor();
                        c.setPosition(fragment.position());
                        c.setPosition(fragment.position() + fragment.length(), QTextCursor::KeepAnchor);
                        newImageFormat.setName(res);
                        c.setCharFormat(newImageFormat);
                    }
                }

            }
        }
    }

    auto frames = frame->childFrames();
    for (auto it = frames.begin(); it != frames.end(); ++it){
        processFrame(*it, document, saveMode);
    }

}

QString CoreTextEditWidget::base64ToResource(QTextDocument* document, const QString& encrypted ) const
{
    QByteArray ba = QByteArray::fromBase64(encrypted.toLatin1());
    QImage image;
    image.loadFromData(ba);
    static int counter = 0;
    QString name = QString("created_image%1").arg(counter++);
    document->addResource(QTextDocument::ImageResource, QUrl(name.toLatin1()), image);

    return name;
}

QString CoreTextEditWidget::resourceToBase64( QTextDocument* document, const QString& resource ) const
{
    QImage img = qvariant_cast<QImage>(document->resource(QTextDocument::ImageResource, QUrl(resource.toLatin1())));

    QBuffer buffer;
    img.save(&buffer, "PNG");
    return buffer.buffer().toBase64();
}

void CoreTextEditWidget::changeLinksToBase64( QTextFrame * root, QTextDocument* document )
{
    for (auto rootIt = root->begin(); !(rootIt.atEnd()); ++rootIt) {
        QTextBlock block = rootIt.currentBlock();
        for (auto it = block.begin(); !(it.atEnd()); ++it) {
            QTextFragment fragment = it.fragment();
            if (fragment.isValid()) {
                QTextImageFormat newImageFormat = fragment.charFormat().toImageFormat();

                if (newImageFormat.isValid()) {
                    QString name = newImageFormat.name();

                    if (!name.startsWith("data:im") && !name.startsWith("created_image")) {
                        QString res;
                        bool loaded = tryChangeLinkToBase64(res, name);
                        if (loaded) {
                            QTextCursor c = textEdit->textCursor();
                            c.setPosition(fragment.position());
                            c.setPosition(fragment.position() + fragment.length(), QTextCursor::KeepAnchor);
                            newImageFormat.setName(QString("data:image/png;base64,") + res);
                            c.setCharFormat(newImageFormat);
                        }
                    }


                }

            }
        }
    }

    auto frames = root->childFrames();
    for (auto it = frames.begin(); it != frames.end(); ++it){
        changeLinksToBase64(*it, document);
    }
}

bool CoreTextEditWidget::tryChangeLinkToBase64( QString& result, const QString& link ) const
{
	auto templateDir = plugin::getResourcePath("templates\\");
    
    QString imgPath(QString::fromStdString((plugin::getResourcePath("templates") / link.toStdString()).string()));
    if (QFile::exists(imgPath)) {
        QPixmap file(imgPath);
        QLabel* l = new QLabel;
        l->setPixmap(file);
        l->show();
        QBuffer buffer;
        file.save(&buffer, "PNG");
        result = buffer.buffer().toBase64();
        return true;
    } else {
        return false;
    }
} 
