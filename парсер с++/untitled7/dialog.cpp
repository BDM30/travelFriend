#include "dialog.h"
#include "ui_dialog.h"
#include <iostream>

using namespace std;

int const amount = 15000;

class Node {
public:
    void set(QString id1, double lat1, double lon1) {
       id = id1;
       lat = lat1;
       lon = lon1;
    }
    Node()
    {

    }
    QString id;
    double lat;
    double lon;
};

class Way {
public:
    void setNode(QString input) {
        nodes[amountNodes] = input;
        amountNodes++;
    }
    void setId(QString input) {
        id = input;
    }
    Way()
    {
        amountNodes = 0;
        nodes = new QString[amount]();
    }
    QString id;
    //QString nodes[amount];
    QString *nodes;
    int amountNodes;
};

class Relation {
public:
    void setId(QString input) {
         id = input;
    }
    void setWay(QString input) {
        ways[amountWays] = input;
        amountWays++;
    }

    Relation() {
        amountWays = 0;
        ways = new QString[amount]();
    }
    QString id;
    QString *ways;
    int amountWays;
};



Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog),
    manager(new QNetworkAccessManager(this))
{
    ui->setupUi(this);
    ui->loadButton->setEnabled(false);

    //networkReply = manager->get(QNetworkRequest(QUrl("http://overpass-api.de/api/interpreter?data=(rel[name='Новозыбков'];>;);out;")));
    networkReply = manager->get(QNetworkRequest(QUrl("C:\js\file.xml")));
    //networkReply = manager->get(QNetworkRequest(QUrl("http://overpass-api.de/api/interpreter?data=area[%22boundary%22=%22administrative%22][%22name%22=%22%D0%A1%D0%B0%D0%BD%D0%BA%D1%82-%D0%9F%D0%B5%D1%82%D0%B5%D1%80%D0%B1%D1%83%D1%80%D0%B3%22]-%3E.a;%28relation%28area.a%29[%22admin_level%22=%225%22];%29;out%20body;%3E;out%20skel%20qt;")));
    connect(networkReply, SIGNAL(readyRead()), this, SLOT(slotReadyRead()));

}

Dialog::~Dialog()
{
    delete ui;
}

void Dialog::slotReadyRead()
{
    int statusCode = networkReply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        if (statusCode >= 200 && statusCode < 300)
        {
            QByteArray data = networkReply->readAll();
            QTextCodec *codec = QTextCodec::codecForName("Windows-1251");
            xmlReader.addData(codec->toUnicode(data));
            parseXml();
        }
}

void Dialog::parseXml()
{
    Node *arrayNodes = new Node[amount]();
    int amountNodes = 0;

    long counterWays = 0; // следит за открытием\закрытием тега way
    long amountWays = 0;
    Way *arrayWays = new Way[amount]();

    long counterRelations = 0; //  следит за открытием\закрытием тега relation
    long amountRelations = 0;
    Relation *arrayRelations = new Relation[amount/10]();


    while (!xmlReader.atEnd())
        {
         xmlReader.readNext();
         QStringRef token = xmlReader.name();
         QXmlStreamAttributes attrib = xmlReader.attributes();
         if (token == "node") {
             QString one = attrib.value("id").toString();
             double two = attrib.value("lat").toDouble();
             double three = attrib.value("lon").toDouble();
             if (one != "")
             {
                arrayNodes[amountNodes].set(one,two,three);
                amountNodes++;
             }
         }

         if (token == "way") {
                      if (counterWays % 2 == 0) { // тег way открылся
                          QString one = attrib.value("id").toString();
                          arrayWays[amountWays].setId(one);
                      }
                      else { // тег way закрылся
                          amountWays++;
                      }
                      ui->textEdit->append("\nway\n");
                      counterWays++;
         }
         if (counterWays % 2 == 1 && token == "nd") {
             QString one = attrib.value("ref").toString();\
             if (one != "") {
                arrayWays[amountWays].setNode(one);
             }
         }

         if (token == "relation") {

             if (counterRelations % 2 == 0) { // тег relation открылся
                 QString one = attrib.value("id").toString();
                 arrayRelations[amountRelations].setId(one);
             }
             else { // тег relation закрылся
                 amountRelations++;
             }
             counterRelations++;
         }
         if (counterRelations % 2 == 1 && token == "member") {
             QString one = attrib.value("ref").toString();
             QString two = attrib.value("type").toString();
             if (one != "" && two== "way") {
                arrayRelations[amountRelations].setWay(one);
             }
         }

        }
    // тестовый вывод

    for (int i = 0; i < amountNodes; i++) {
        ui->textEdit->append(arrayNodes[i].id+ " " + QString::number(arrayNodes[i].lat) + " " + QString::number(arrayNodes[i].lon));
    }
    for (int i = 0; i < amountWays; i++ ) {
            ui->textEdit->append("\n" + arrayWays[i].id + "\n");
        }
    for (int i = 0; i < arrayWays[0].amountNodes; i++) {
        ui->textEdit->append(" " + arrayWays[0].nodes[i] + " ");
    }

    for (int i = 0; i < arrayRelations[0].amountWays; i++) {
        ui->textEdit->append(" " + arrayRelations[0].ways[i] + " ");
    }

}
