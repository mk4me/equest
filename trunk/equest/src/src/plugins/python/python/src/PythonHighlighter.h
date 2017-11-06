/********************************************************************
	created:	2012/12/04
	created:	4:12:2012   19:07
	filename: 	PythonHighlighter.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_PYTHON__PYTHONHIGHLIGHTER_H__
#define HEADER_GUARD_PYTHON__PYTHONHIGHLIGHTER_H__

#include <QtGui/QSyntaxHighlighter>

/** Prosta klasa podœwietlaj¹ca skrypty pythona*/
class PythonHighlighter : public QSyntaxHighlighter
{
    Q_OBJECT;
public:
    PythonHighlighter(QTextDocument *parent = 0);
	virtual ~PythonHighlighter() {}

protected:
    void highlightBlock(const QString &text);

private:
    struct HighlightingRule
    {
        QRegExp pattern;
        QTextCharFormat format;
    };
    QVector<HighlightingRule> highlightingRules;

    struct MultilineRule
    {   
        QRegExp startExpression;
        QRegExp endExpression;
        QTextCharFormat format;
    };
    QVector<MultilineRule> multilineRules;

    QTextCharFormat keywordFormat;
    QTextCharFormat commentFormat;
    QTextCharFormat quotationFormat;
    QTextCharFormat functionFormat;
};

#endif
