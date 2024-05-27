#include "settingsmenu.h"
#include "ui_settingsmenu.h"

extern QScopedPointer<QFile> g_logFile;
extern QString g_logLvl;

TSettingsMenu::TSettingsMenu(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TSettingsMenu)
{
    ui->setupUi(this);
    this->setWindowTitle(QString("Настройки"));

    qInstallMessageHandler(messageHandler);
}

TSettingsMenu::~TSettingsMenu()
{
    delete ui;
}

void TSettingsMenu::updateSettings()
{
    QSettings settings(m_settingsFile,QSettings::IniFormat);

    m_logPath = settings.value("Logging/logPath").toString();
    m_logLvl = settings.value("Logging/logLevel").toString();

    m_hostname = settings.value("Network/hostname").toString();
    m_port = settings.value("Network/port").toInt();

    m_userID = settings.value("User/id").toString();
    m_routingkey = settings.value("Network/routingkey").toString();
    m_exchange = settings.value("Network/exchange").toString();

    ui->hostLineEdit->setText(m_hostname);
    ui->idLineEdit->setText(m_userID);
    ui->loglvlLineEdit->setText(m_logLvl);
    ui->logpathLineEdit->setText(m_logPath);
    ui->portLineEdit->setText(QString::number(m_port));
    ui->routingKeyLineEdit->setText(m_routingkey);
    ui->exchangeLineEdit->setText(m_exchange);

    g_logFile.reset(new QFile(m_logPath));
    g_logFile.data()->open(QFile::Append | QFile::Text);
    g_logLvl = m_logLvl;
}

void TSettingsMenu::setSettingsFile(QString settingsFile)
{
    m_settingsFile = settingsFile;
    updateSettings();
}

void TSettingsMenu::on_saveBtn_clicked()
{
     QSettings settings(m_settingsFile,QSettings::IniFormat);

     m_logPath = ui->logpathLineEdit->text();
     m_logLvl = ui->loglvlLineEdit->text();
     m_hostname = ui->hostLineEdit->text();
     m_port = ui->portLineEdit->text().toInt();
     m_userID = ui->idLineEdit->text();
     m_routingkey = ui->routingKeyLineEdit->text();
     m_exchange = ui->exchangeLineEdit->text();

     settings.setValue("Logging/logPath",m_logPath);
     settings.setValue("Logging/logLevel",m_logLvl);
     settings.setValue("Network/hostname",m_hostname);
     settings.setValue("Network/port",m_port);
     settings.setValue("User/id",m_userID);
     settings.setValue("Network/routingkey",m_routingkey);
     settings.setValue("Network/exchange",m_exchange);
     updateSettings();
     emit saveSettingsSignal();
     this->close();
}

QString TSettingsMenu::logPath(){return m_logPath;}
QString TSettingsMenu::logLvl(){return m_logLvl;}
QString TSettingsMenu::hostname(){return m_hostname;}
QString TSettingsMenu::routingkey(){return m_routingkey;}
QString TSettingsMenu::exchange(){return m_exchange;}
int TSettingsMenu::port(){return m_port;}
QString TSettingsMenu::userID(){return m_userID;}
