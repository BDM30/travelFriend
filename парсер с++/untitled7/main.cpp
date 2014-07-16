#include "dialog.h"
#include <QApplication>

// создать отдельный проход где будут создавать голые элементы и списки Ways и Relations

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Dialog w;
    w.show();

    return a.exec();
}
