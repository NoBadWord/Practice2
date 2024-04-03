#include "interface.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    TInterface w;
    if(w.connectRabbit())
    {
        return 1;
    }
    w.show();
    return a.exec();
}
