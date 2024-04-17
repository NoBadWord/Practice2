#ifndef SETTINGSMENU_H
#define SETTINGSMENU_H

#include <QWidget>
#include <QSettings>
#include <QCloseEvent>

#include "LoggingCategories.h"

namespace Ui {
class TSettingsMenu;
}

class TSettingsMenu : public QWidget
{
    Q_OBJECT

public:
    explicit TSettingsMenu(QWidget *parent = nullptr);
    ~TSettingsMenu();
    QString logPath;
    QString logLvl;
    QString strBuf;
    QString hostname;
    QString routingkey;
    QString exchange;
    int port;
    QString userID;

private slots:
    void on_saveBtn_clicked();

private:
    Ui::TSettingsMenu *ui;


signals:
    void closed();

protected:
    void closeEvent(QCloseEvent *event) override;
};

#endif // SETTINGSMENU_H
