#include "PythonPCH.h"
#include "PythonHighlighter.h"
#include <objectwrapperlib/ObjectWrapper.h>
#include <QtWidgets/QLabel>

const char * const keywords[] = {
    "and", "as", "assert", "break", "class",
    "continue", "def", "del", "elif", "else",
    "except", "exec", "finally", "for", "from",
    "global", "if", "import", "in", "is",
    "lambda", "not", "or", "pass", "print", "raise",
    "return", "try", "while", "with", "yield", "True",
    "False", "None"
};


PythonHighlighter::PythonHighlighter( QTextDocument *parent /*= 0*/ )
    : QSyntaxHighlighter(parent)
{
    HighlightingRule rule;

    keywordFormat.setForeground(Qt::darkBlue);
    keywordFormat.setFontWeight(QFont::Bold);

    int count = sizeof(keywords) / sizeof(keywords[0]);
    for (int i = 0; i < count; ++i) {
        QString str = QString("\\b%1\\b").arg(keywords[i]);
        rule.pattern = QRegExp(str);
        rule.format = keywordFormat;
        highlightingRules.append(rule);
    }
    
    quotationFormat.setForeground(Qt::darkGreen);
    rule.pattern = QRegExp("\".*\"");
    rule.format = quotationFormat;
    highlightingRules.append(rule);
    rule.pattern = QRegExp("\'.*\'");
    highlightingRules.append(rule);

    functionFormat.setFontItalic(true);
    functionFormat.setForeground(Qt::blue);
    rule.pattern = QRegExp("\\b[A-Za-z0-9_]+(?=\\()");
    rule.format = functionFormat;
    highlightingRules.append(rule);


    commentFormat.setForeground(Qt::red);
    rule.pattern = QRegExp("#[^\n]*");
    rule.format = commentFormat;
    highlightingRules.append(rule);

    MultilineRule multiRule;
    multiRule.startExpression = QRegExp("'''");
    multiRule.endExpression = QRegExp("'''");
    multiRule.format = quotationFormat;
    multilineRules.append(multiRule);

    multiRule.startExpression = QRegExp("\"\"\"");
    multiRule.endExpression = QRegExp("\"\"\"");
    multiRule.format = quotationFormat;
    multilineRules.append(multiRule);

}

void PythonHighlighter::highlightBlock( const QString &text )
{
    Q_FOREACH (const HighlightingRule &rule, highlightingRules) {
        QRegExp expression(rule.pattern);
        int index = expression.indexIn(text);
        while (index >= 0) {
            int length = expression.matchedLength();
            setFormat(index, length, rule.format);
            index = expression.indexIn(text, index + length);
        }
    }

    int multiCount = multilineRules.size();
    for (int i = 0; i < multiCount; ++i) { 
        int blockState = i + 1;
        int prevState = previousBlockState();
        int currentState = currentBlockState();

        if (currentState == blockState && prevState == -1) {
            setCurrentBlockState(-1);
        }

        if (currentState != -1 && prevState != blockState) {
            continue;
        }

        const MultilineRule &multiRule = multilineRules.at(i);
       
        
        int startIndex = 0;
        if (previousBlockState() != blockState) {
            startIndex = multiRule.startExpression.indexIn(text);
        }
        while (startIndex >= 0) {
            int shift = multiRule.startExpression.matchedLength();
            shift = shift > 0 ? shift : 0;
            int endIndex = multiRule.endExpression.indexIn(text, startIndex + shift);
            int commentLength;
            if (endIndex == -1) {
                setCurrentBlockState(blockState);
                commentLength = text.length() - startIndex;
            } else {
                commentLength = endIndex - startIndex
                    + multiRule.endExpression.matchedLength();
                setCurrentBlockState(-1);
            }
            setFormat(startIndex, commentLength, multiRule.format);
            startIndex = multiRule.startExpression.indexIn(text, startIndex + commentLength);
        }
    }
}
