#include <QtGui>
#include "syntaxhighlighter.h"

SyntaxHighLighter::SyntaxHighLighter(QTextDocument *parent)
    : QSyntaxHighlighter(parent)
{
}

void SyntaxHighLighter::highlightBlock(const QString &text)
{
    foreach (const HighlightingRule &rule, highlightingRules) {
        QRegExp expression(rule.pattern);
        int index = expression.indexIn(text);
        while (index >= 0) {

            int length = expression.matchedLength();
            setFormat(index, length, rule.format);
            index = expression.indexIn(text, index + length);
        }
    }
    setCurrentBlockState(0);
}
