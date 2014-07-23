#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QtNetwork/QtNetwork>
#include <QTextEdit>
#include <QPushButton>
#include <QXmlStreamReader>
#include <QList>
#include <iostream>
#include <fstream>

namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = 0);
    ~Dialog();

private slots:
    void slotReadyRead();

private:
    QNetworkAccessManager *manager;
    QNetworkReply *networkReply;
    QXmlStreamReader xmlReader;

    void sortCityList();
    void printCityList();

    int parseXml();
    QList<QString> cityList;
    Ui::Dialog *ui;
};

#endif // DIALOG_H
