#pragma once

#include <QHash>
#include <QTextDocument>
#include <QTextCharFormat>
#include <QSyntaxHighlighter>

class SyntaxHighLighter : public QSyntaxHighlighter
{
    Q_OBJECT

public:
    SyntaxHighLighter(QTextDocument *parent = 0);

protected:
    void highlightBlock(const QString &text);

private:
    struct HighlightingRule
    {
        QRegExp pattern;
        QTextCharFormat format;
    };
    QVector<HighlightingRule> highlightingRules;
};
