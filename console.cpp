#include <QTime>
#include <QDebug>
#include <QPalette>
#include <QDateTime>
#include <QTextDocument>

#include "console.h"

Console::Console(QWidget *parent)
    : QPlainTextEdit(parent)
{
    QPalette p = palette();
    p.setColor(QPalette::Base, Qt::black);
    p.setColor(QPalette::Text, Qt::white);
    this->setPalette(p);

    m_syntaxHighLighter = new SyntaxHighLighter(this->document());

    this->setTabStopWidth(40);
    this->setUndoRedoEnabled(true);
}

void Console::keyPressEvent(QKeyEvent *e)
{
    switch (e->key()) {
    case Qt::Key_Backspace: {
        if (m_cmdBuffer.size() != 0)
            m_cmdBuffer.remove(m_cmdBuffer.size()-1, 1);

        QString tmp = this->toPlainText();
        tmp.remove(tmp.size()-1, 1);
        this->clear();
        this->setPlainText(tmp);
        this->moveCursor(QTextCursor::End);
        }
        break;
    case Qt::Key_Left:
    case Qt::Key_Right:
    case Qt::Key_Up:
    case Qt::Key_Down:
        break;
    default:
        QPlainTextEdit::keyPressEvent(e);

        m_cmdBuffer += e->text();
        if (e->text() == QString::fromLatin1("\r")) {
            emit getData(m_cmdBuffer.toLocal8Bit());
            m_cmdBuffer.clear();
        }
        break;
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
