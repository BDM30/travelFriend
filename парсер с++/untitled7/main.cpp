#include "dialog.h"
#include <QApplication>

// parseXml() = берет кусками, это плохо. Исправить!!!
// Пример Прокопьевска = 167/2/1

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Dialog w;
    w.show();

    return a.exec();
}
