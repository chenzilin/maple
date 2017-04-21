#include <QTime>
#include <QDebug>
#include <QRegExp>
#include <QPalette>
#include <QDateTime>
#include <QClipboard>
#include <QApplication>
#include <QTextDocument>
#include <QRegularExpression>

#include "console.h"

Console::Console(QWidget *parent)
    : QPlainTextEdit(parent)
{
    QPalette p = palette();
    p.setColor(QPalette::Base, Qt::black);
    p.setColor(QPalette::Text, Qt::lightGray);
    this->setPalette(p);

    QTextDocument *doc = this->document();
    QFont font = doc->defaultFont();
    font.setPointSize(12);
    font.setFamily("Monospace");
    doc->setDefaultFont(font);

    this->setTabStopWidth(40);
    this->setMaximumBlockCount(10000);
    this->setContextMenuPolicy(Qt::ActionsContextMenu);
    this->setCursorWidth(8);

    m_copyAct = new QAction(tr("&Copy"), this);
    m_copyAct->setEnabled(true);
    m_copyAct->setShortcut(tr("Ctrl+Shift+C"));
    m_copyAct->setStatusTip(tr("Copy the current selection's contents to the clipboard."));
    connect(m_copyAct, SIGNAL(triggered()), this, SLOT(copy()));

    m_pasteAct = new QAction(tr("&Paste"), this);
    m_pasteAct->setEnabled(true);
    m_pasteAct->setShortcut(tr("Ctrl+Shift+V"));
    m_pasteAct->setStatusTip(tr("Paste the clipboard's contents."));
    connect(m_pasteAct, SIGNAL(triggered()), this, SLOT(doPaste()));

    this->addAction(m_copyAct);
    this->addAction(m_pasteAct);

    connect(this, SIGNAL(textChanged()), this, SLOT(centerCursor()));
}

void Console::doPaste()
{
    this->moveCursor(QTextCursor::End);
    this->paste();
    QClipboard *clipboard = QApplication::clipboard();
    m_cmdBuffer += clipboard->text();
}

void Console::processContextColor()
{
    m_originContext.replace(QRegExp("\u001B\\[1;30m"), "<font color = #000000>");
    m_originContext.replace(QRegExp("\u001B\\[1;31m"), "<font color = #FF0000>");
    m_originContext.replace(QRegExp("\u001B\\[1;32m"), "<font color = #00BB00>");
    m_originContext.replace(QRegExp("\u001B\\[1;33m"), "<font color = #FFFF37>");
    m_originContext.replace(QRegExp("\u001B\\[1;34m"), "<font color = #2828FF>");
    m_originContext.replace(QRegExp("\u001B\\[1;35m"), "<font color = #8600FF>");
    m_originContext.replace(QRegExp("\u001B\\[1;36m"), "<font color = #8600FF>");
    m_originContext.replace(QRegExp("\u001B\\[1;37m"), "<font color = #FFFFFF>");

    m_originContext.replace(QRegExp("\u001B\\[0m"), "</font>");

    m_originContext.replace(QRegExp("[\\r|\\n]+"), "<br/>");

    this->clear();
    this->appendHtml(m_originContext);
    this->moveCursor(QTextCursor::End);
}

void Console::keyPressEvent(QKeyEvent *e)
{
    switch (e->key()) {
    case Qt::Key_Backspace: {
        if (m_cmdBuffer.size() != 0) {
            m_cmdBuffer.remove(m_cmdBuffer.size()-1, 1);
            m_originContext.remove(m_originContext.size()-1, 1);    // 把存在origincontext内部的命令行也删掉
            this->clear();
            this->appendHtml(m_originContext);
            this->moveCursor(QTextCursor::End);

        }

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
        m_originContext += e->text();
        processContextColor();
        if (e->text() == QString::fromLatin1("\r")) {
            this->sendData(m_cmdBuffer);
            if (m_cmdBuffer == "clear\r") m_originContext.clear();
            m_cmdBuffer.clear();
        }
        break;
    }
}

void Console::appendText(const QByteArray data)
{
    QTime time(0,0,0);
    QString echoTime;
    int index = 0;
    QString inputDate = data;

    switch (m_echoTimeType) {
    case 1:
        echoTime = QDateTime::currentDateTime().toString("[HH:mm:ss.zzz]: ");
        break;
    case 2:
        time = time.addMSecs(m_startTime.msecsTo(QTime::currentTime()));
        echoTime = time.toString("[HH:mm:ss.zzz]: ");
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

    m_originContext += inputDate;
    processContextColor();
    QApplication::processEvents();
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
