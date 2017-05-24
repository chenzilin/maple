#pragma once

#include <QTime>
#include <QAction>
#include <QString>
#include <QKeyEvent>
#include <QPlainTextEdit>

class Console : public QPlainTextEdit
{
    Q_OBJECT

public:
    Console(QWidget *parent = 0);

    void appendText(const QByteArray data);
    void setEchoTimeType(int echoTimeType);

    void setLocalEchoEnabled(bool set);

signals:
    void sendData(const QByteArray data);

protected:
    virtual void keyPressEvent(QKeyEvent *e);

private slots:
    void doPaste();

private:
    int m_echoTimeType = 0;
    QTime m_startTime;

    QByteArray m_cmdBuffer = QByteArray("");

    QAction* m_copyAct;
    QAction* m_pasteAct;


    void processContextColor();
    QString m_originContext = "";

    bool m_localEchoEnabled;
};
