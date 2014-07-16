#include "dialog.h"
#include "ui_dialog.h"
#include "list.h"
#include "node.cpp"
#include "way.cpp"
#include <relation.cpp>
#include <iostream>

using namespace std;

int const amount = 15000;

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

    List<Node> *listNodes = new List<Node>();
    int amountNodes = 0;

    long counterWays = 0; // следит за открытием\закрытием тега way
    long amountWays = 0;
    List<Way> *listWays = new List<Way>();

    long counterRelations = 0; //  следит за открытием\закрытием тега relation
    long amountRelations = 0;
    Relation *arrayRelations = new Relation[amount/10]();

    Way headWay;
    listWays->push(headWay);

    while (!xmlReader.atEnd())
        {
         xmlReader.readNext();
         QStringRef token = xmlReader.name();
         QXmlStreamAttributes attrib = xmlReader.attributes();
         if (token == "node") {
             Node slot;
             slot.set(attrib.value("id").toString(),attrib.value("lat").toDouble(), attrib.value("lon").toDouble());
             if (attrib.value("id").toString() != "")
             {
                listNodes->push(slot);
                amountNodes++;
             }
         }
         //Way *headWay = new Way();
         //Way headWay;
         //listWays->push(headWay);

         if (token == "way") {
             //cout << " 1\n";
                      if (counterWays % 2 == 0) { // тег way открылся
                          QString one = attrib.value("id").toString();
                          listWays->end->value.setId(attrib.value("id").toString());
                          //cout << listWays->end->value.id.toStdString() << endl;
                      }
                      else { // тег way закрылся
                          //cout << "closed\n";
                          Way slot;
                          listWays->push(slot);
                          amountWays++;
                      }
                      counterWays++;
         }
         if (counterWays % 2 == 1 && token == "nd") {
             QString one = attrib.value("ref").toString();
             if (one != "") {
                 listWays->end->value.setNode(one);
                 //cout << listWays->end->value.nodes->end->value.toStdString() << endl;
             }
         }

         /*
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
         */

    }
    // тестовый вывод

    /*
    cout << "check sum\n";
    cout << "amount nodes" << amountNodes << endl;
    cout << "amount ways " << amountWays << endl;
    cout << "amount relation" << amountRelations << endl;
    cout <<"=====================" << endl;
    */



    /*
    while (listNodes->begin) {
        Node slot = listNodes->begin->value;
        cout << slot.id.toStdString() << " " << slot.lat << " " << slot.lon << endl;
        listNodes->begin = listNodes->begin->next;
    }
    */

    while (listWays->begin) {
        Way slot = listWays->begin->value;
        while(slot.nodes->begin) {
            QString ke = slot.nodes->begin->value;
            cout << ke.toStdString() << endl;
            slot.nodes->begin = slot.nodes->begin->next;
        }
    }


}
