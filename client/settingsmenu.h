#ifndef SETTINGSMENU_H
#define SETTINGSMENU_H

#include <QWidget>
#include <QSettings>

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

private slots:
    void on_saveBtn_clicked();

private:
    Ui::TSettingsMenu *ui;
};

#endif // SETTINGSMENU_H
