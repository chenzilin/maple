#pragma once

#include <QTime>
#include <QString>
#include <QKeyEvent>
#include <QPlainTextEdit>

#include "syntaxhighlighter.h"

class Console : public QPlainTextEdit
{
    Q_OBJECT

public:
    Console(QWidget *parent = 0);

    void putData(const QByteArray &data);
    void setEchoTimeType(int echoTimeType);

signals:
    void getData(const QByteArray &data);

protected:
    virtual void keyPressEvent(QKeyEvent *e);

private:
    int m_echoTimeType = 0;
    QTime m_startTime;

    QString m_cmdBuffer = QString::fromLatin1("");

    SyntaxHighLighter *m_syntaxHighLighter = 0;
};
