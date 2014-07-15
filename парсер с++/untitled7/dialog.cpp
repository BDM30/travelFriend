#include "dialog.h"
#include "ui_dialog.h"
#include <iostream>

using namespace std;

int const amount = 1000;

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
    Node arrayNodes[amount];
    int curId = 0;
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
                arrayNodes[curId].set(one,two,three);
                curId++;
             }
         }
        }
    for (int i = 0; i < curId; i++) {
        ui->textEdit->append(arrayNodes[i].id+ " " + QString::number(arrayNodes[i].lat) + " " + QString::number(arrayNodes[i].lon));
    }
}
