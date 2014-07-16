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
        //QString *nodes = new QString[amount]();
    }
    QString id;
    QString nodes[amount];
    //QString *nodes;
    int amountNodes;
};



Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog),
    manager(new QNetworkAccessManager(this))
{
    ui->setupUi(this);
    ui->loadButton->setEnabled(false);
    networkReply = manager->get(QNetworkRequest(QUrl("http://overpass-api.de/api/interpreter?data=(rel[name='Новозыбков'];>;);out;")));
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
    //Node arrayNodes[amount];
    Node *arrayNodes = new Node[amount]();
    int amountNodes = 0;

    long counter = 0; // следит за открытием\закрытием тега way
    long amountWays = 0;
    //Way arrayWays[amount];
    Way *arrayWays = new Way[amount]();

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
                      if (counter % 2 == 0) { // тег way открылся
                          QString one = attrib.value("id").toString();
                          arrayWays[amountWays].setId(one);
                      }
                      else { // тег way закрылся
                          amountWays++;
                      }
                      ui->textEdit->append("\nway\n");
                      counter++;
          }
         if (counter % 2 == 1 && token == "nd") {
             QString one = attrib.value("ref").toString();\
             if (one != "") {
                arrayWays[amountWays].setNode(one);
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
}
