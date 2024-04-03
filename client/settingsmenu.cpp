#include "settingsmenu.h"
#include "ui_settingsmenu.h"

TSettingsMenu::TSettingsMenu(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TSettingsMenu)
{
    ui->setupUi(this);
    this->setWindowTitle(QString("Настройки"));


    QSettings settings(QString("settings.ini"),QSettings::IniFormat);

    QString logPath = settings.value("Logging/logPath").toString();
    QString logLvl = settings.value("Logging/logLevel").toString();

    QString strBuf = settings.value("Network/hostname").toString();
    QByteArray byteArray = strBuf.toUtf8();
    const char* hostname = byteArray.constData();
    int port = settings.value("Network/port").toInt();

    QString userID = settings.value("User/id").toString();

    ui->hostLineEdit->setText(hostname);
    ui->idLineEdit->setText(userID);
    ui->loglvlLineEdit->setText(logLvl);
    ui->logpathLineEdit->setText(logPath);
    ui->portLineEdit->setText(QString::number(port));
}

TSettingsMenu::~TSettingsMenu()
{
    delete ui;
}


void TSettingsMenu::on_saveBtn_clicked()
{
     QSettings settings(QString("settings.ini"),QSettings::IniFormat);
     settings.setValue("Logging/logPath",ui->logpathLineEdit->text());
     settings.setValue("Logging/logLevel",ui->loglvlLineEdit->text());
     settings.setValue("Network/hostname",ui->hostLineEdit->text());
     settings.setValue("Network/port",ui->portLineEdit->text());
     settings.setValue("User/id",ui->idLineEdit->text());
     this->close();
}

