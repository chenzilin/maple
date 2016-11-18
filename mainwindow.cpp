#include <QIcon>
#include <QLabel>
#include <QDebug>
#include <QMessageBox>
#include <QSerialPort>

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "console.h"
#include "settingsdialog.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    m_console = new Console;
    m_console->setEnabled(true);
    this->setCentralWidget(m_console);

    m_serial = new QSerialPort(this);
    m_settings = new SettingsDialog;

    ui->actionConnect->setEnabled(true);
    ui->actionDisconnect->setEnabled(false);
    ui->actionQuit->setEnabled(true);
    ui->actionConfigure->setEnabled(true);

    m_status = new QLabel;
    ui->statusBar->addWidget(m_status);

    initActionsConnections();

    connect(m_serial, SIGNAL(error(QSerialPort::SerialPortError)), this,
            SLOT(handleError(QSerialPort::SerialPortError)));

    connect(m_serial, SIGNAL(readyRead()), this, SLOT(readData()));
    connect(m_console, SIGNAL(sendData(const QByteArray)), this, SLOT(writeData(const QByteArray)));
}

MainWindow::~MainWindow()
{
    delete m_settings;
    delete ui;
}

void MainWindow::setEchoTimeType()
{
    if ((QAction*)sender() == ui->actionLocalTime) {
        if (ui->actionRealTime->isChecked()) {
            ui->actionRealTime->setChecked(false);
        }

        if (ui->actionLocalTime->isChecked())
            m_console->setEchoTimeType(1);
        else m_console->setEchoTimeType(0);
    }
    else if ((QAction*)sender() == ui->actionRealTime) {
        if (ui->actionLocalTime->isChecked()) {
            ui->actionLocalTime->setChecked(false);
        }

        if (ui->actionRealTime->isChecked())
            m_console->setEchoTimeType(2);
        else m_console->setEchoTimeType(0);
    }
}

void MainWindow::openSerialPort()
{
    SettingsDialog::Settings p = m_settings->settings();
    m_serial->setPortName(p.name);
    m_serial->setBaudRate(p.baudRate);
    m_serial->setDataBits(p.dataBits);
    m_serial->setParity(p.parity);
    m_serial->setStopBits(p.stopBits);
    m_serial->setFlowControl(p.flowControl);
    if (m_serial->open(QIODevice::ReadWrite)) {
        m_console->setEnabled(true);
        ui->actionConnect->setEnabled(false);
        ui->actionDisconnect->setEnabled(true);
        ui->actionConfigure->setEnabled(false);
        showStatusMessage(tr("Connected to %1 : %2, %3, %4, %5, %6")
                          .arg(p.name).arg(p.stringBaudRate).arg(p.stringDataBits)
                          .arg(p.stringParity).arg(p.stringStopBits).arg(p.stringFlowControl));
    } else {
        QMessageBox::critical(this, tr("Error"), m_serial->errorString());

        showStatusMessage(tr("Open error"));
    }
}

void MainWindow::closeSerialPort()
{
    if (m_serial->isOpen())
        m_serial->close();
    m_console->setEnabled(false);
    ui->actionConnect->setEnabled(true);
    ui->actionDisconnect->setEnabled(false);
    ui->actionConfigure->setEnabled(true);
    showStatusMessage(tr("Disconnected"));
}

void MainWindow::about()
{
    QMessageBox::about(this, tr("About Maple"), tr("The <b>Maple</b>, a tool to connect your serial port."));
}

void MainWindow::writeData(const QByteArray data)
{
    m_serial->write(data);
}

void MainWindow::readData()
{
    QByteArray data = m_serial->readAll();
    m_console->appendText(data);
}

void MainWindow::handleError(QSerialPort::SerialPortError error)
{
    if (error == QSerialPort::ResourceError) {
        QMessageBox::critical(this, tr("Critical Error"), m_serial->errorString());
        closeSerialPort();
    }
}

void MainWindow::initActionsConnections()
{
    connect(ui->actionConnect, SIGNAL(triggered()), this, SLOT(openSerialPort()));
    connect(ui->actionDisconnect, SIGNAL(triggered()), this, SLOT(closeSerialPort()));
    connect(ui->actionQuit, SIGNAL(triggered()), this, SLOT(close()));
    connect(ui->actionConfigure, SIGNAL(triggered()), m_settings, SLOT(show()));
    connect(ui->actionClear, SIGNAL(triggered()), m_console, SLOT(clear()));
    connect(ui->actionAbout, SIGNAL(triggered()), this, SLOT(about()));
    connect(ui->actionRealTime, SIGNAL(triggered()), this, SLOT(setEchoTimeType()));
    connect(ui->actionLocalTime, SIGNAL(triggered()), this, SLOT(setEchoTimeType()));
}

void MainWindow::showStatusMessage(const QString &message)
{
    m_status->setText(message);
}
