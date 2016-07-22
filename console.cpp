#include <QRegExp>
#include <QDateTime>
#include <QScrollBar>
#include <QtCore/QDebug>

#include "console.h"

Console::Console(QWidget *parent)
    : QPlainTextEdit(parent)
    , localEchoEnabled(false), m_echoTimeType(0),m_startTime()
{
    document()->setMaximumBlockCount(100);
    QPalette p = palette();
    p.setColor(QPalette::Base, Qt::black);
    p.setColor(QPalette::Text, Qt::white);
    setPalette(p);
}

void Console::setEchoTimeType(int echoTimeType)
{
    if (echoTimeType != m_echoTimeType) {
        m_echoTimeType = echoTimeType;

        if (2 == echoTimeType) {
            m_startTime = QTime::currentTime();
        }
    }
}

void Console::putData(const QByteArray &data)
{
    QTime time(0,0,0);
    QString echoTime;
    int index = 0;
    QString inputDate = data;

    switch (m_echoTimeType) {
    case 1:
        echoTime = QDateTime::currentDateTime().toString("HH:mm:ss.zzz : ");
        break;
    case 2:
        time = time.addMSecs(m_startTime.msecsTo(QTime::currentTime()));
        echoTime = time.toString("HH:mm:ss.zzz : ");
        break;
    default:
        goto lable_end;
        break;
    }

    inputDate.replace('\r', '\n');
    inputDate.replace(QRegExp("\\n{2,}"), "\n");

    while (-1 != (index = inputDate.indexOf('\n', index))) {
        inputDate.insert(index+1, echoTime);
        index += echoTime.length();
    }

lable_end:
    insertPlainText(inputDate);

    QScrollBar *bar = verticalScrollBar();
    bar->setValue(bar->maximum());
}

void Console::setLocalEchoEnabled(bool set)
{
    localEchoEnabled = set;
}

void Console::keyPressEvent(QKeyEvent *e)
{
    switch (e->key()) {
    case Qt::Key_Backspace:
    case Qt::Key_Left:
    case Qt::Key_Right:
    case Qt::Key_Up:
    case Qt::Key_Down:
        break;
    default:
        if (localEchoEnabled)
            QPlainTextEdit::keyPressEvent(e);
        emit getData(e->text().toLocal8Bit());
    }
}

void Console::mousePressEvent(QMouseEvent *e)
{
    Q_UNUSED(e)
    setFocus();
}

void Console::mouseDoubleClickEvent(QMouseEvent *e)
{
    Q_UNUSED(e)
}

void Console::contextMenuEvent(QContextMenuEvent *e)
{
    Q_UNUSED(e)
}
