#include "settingsmenu.h"
#include "ui_settingsmenu.h"

TSettingsMenu::TSettingsMenu(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TSettingsMenu)
{
    ui->setupUi(this);
    this->setWindowTitle(QString("Настройки"));

    QSettings settings(QString("settings.ini"),QSettings::IniFormat);

    logPath = settings.value("Logging/logPath").toString();
    logLvl = settings.value("Logging/logLevel").toString();

    hostname = settings.value("Network/hostname").toString();
    port = settings.value("Network/port").toInt();

    userID = settings.value("User/id").toString();
    routingkey = settings.value("Network/routingkey").toString();
    exchange = settings.value("Network/exchange").toString();

    ui->hostLineEdit->setText(hostname);
    ui->idLineEdit->setText(userID);
    ui->loglvlLineEdit->setText(logLvl);
    ui->logpathLineEdit->setText(logPath);
    ui->portLineEdit->setText(QString::number(port));
    ui->routingKeyLineEdit->setText(routingkey);
    ui->exchangeLineEdit->setText(exchange);
}

TSettingsMenu::~TSettingsMenu()
{
    delete ui;
}


void TSettingsMenu::on_saveBtn_clicked()
{
     QSettings settings(QString("settings.ini"),QSettings::IniFormat);

     logPath = ui->logpathLineEdit->text();
     logLvl = ui->loglvlLineEdit->text();
     hostname = ui->hostLineEdit->text();
     port = ui->portLineEdit->text().toInt();
     userID = ui->idLineEdit->text();
     routingkey = ui->routingKeyLineEdit->text();
     exchange = ui->exchangeLineEdit->text();

     settings.setValue("Logging/logPath",logPath);
     settings.setValue("Logging/logLevel",logLvl);
     settings.setValue("Network/hostname",hostname);
     settings.setValue("Network/port",port);
     settings.setValue("User/id",userID);
     settings.setValue("Network/routingkey",routingkey);
     settings.setValue("Network/exchange",exchange);

     this->close();
}

void TSettingsMenu::closeEvent(QCloseEvent *event)
{
    emit closed();
    event->accept();
}

