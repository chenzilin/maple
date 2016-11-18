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

    void appendText(const QByteArray data);
    void setEchoTimeType(int echoTimeType);

signals:
    void sendData(const QByteArray data);

protected:
    virtual void keyPressEvent(QKeyEvent *e);

private:
    int m_echoTimeType = 0;
    QTime m_startTime;

    QByteArray m_cmdBuffer = QByteArray("");

    SyntaxHighLighter *m_syntaxHighLighter = 0;
};
