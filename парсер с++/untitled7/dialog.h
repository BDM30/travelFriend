#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QtNetwork/QtNetwork>
#include <QTextEdit>
#include <QPushButton>
#include <QXmlStreamReader>
#include "list.h"
#include "node.cpp"
#include "way.cpp"
#include <relation.cpp>
#include <iostream>


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

public:
    int parseXml();

    Ui::Dialog *ui;
    QNetworkAccessManager *manager;

    QNetworkReply *networkReply;
    QXmlStreamReader xmlReader;

    List<Node> *listNodes;
    List<Way> *listWays;
    List<Relation> *listRelations;

    long amountNodes;
    long amountWays;
    long amountRelations;

};

#endif // DIALOG_H
