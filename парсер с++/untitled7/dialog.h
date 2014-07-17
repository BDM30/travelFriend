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

public:
    int parseXml();
    void outPut();

    Way findWayById(QString idS) {
        while (listWays->begin) {
            if (listWays->begin->value.id == idS )
                return listWays->begin->value;
            listWays->begin = listWays->begin->next;
        }
        Way slot;
        return slot;
    }

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
