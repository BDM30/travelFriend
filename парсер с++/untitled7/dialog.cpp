#include "dialog.h"
#include "ui_dialog.h"

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

    listNodes = new List<Node>();
    listWays = new List<Way>();
    listRelations = new List<Relation>();
    arrayRelations = new Relation[amount/10]();
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

int Dialog::parseXml()
{


    int amountNodes = 0;

    long counterWays = 0; // следит за открытием\закрытием тега way
    long amountWays = 0;


    long counterRelations = 0; //  следит за открытием\закрытием тега relation
    long amountRelations = 0;


    Way *headWay = new Way();
    listWays->push(headWay);
    while (!xmlReader.atEnd())
        {
         xmlReader.readNext();
         QStringRef token = xmlReader.name();
         QXmlStreamAttributes attrib = xmlReader.attributes();
         if (token == "node") {
             Node *slot = new Node();
             slot->set(attrib.value("id").toString(),attrib.value("lat").toDouble(), attrib.value("lon").toDouble());
             if (attrib.value("id").toString() != "")
             {
                listNodes->push(slot);
                amountNodes++;
             }
         }
         Way *headWay = new Way();
         listWays->push(headWay);

         if (token == "way") {
                      if (counterWays % 2 == 0) { // тег way открылся
                          listWays->end->value.setId(attrib.value("id").toString());
                      }
                      else { // тег way закрылся
                          Way *slot = new Way();
                          listWays->push(slot);
                          amountWays++;
                      }
                      counterWays++;
         }
         if (counterWays % 2 == 1 && token == "nd") {
             QString *one = new QString(attrib.value("ref").toString());
             if (*one != "") {
                 listWays->end->value.setNode(one);
             }
         }

         Relation *headRel = new Relation();
         listRelations->push(headRel);

         if (token == "relation") {

             if (counterRelations % 2 == 0) { // тег relation открылся
                 listRelations->end->value.setId(attrib.value("id").toString());
             }
             else { // тег relation закрылся
                          Relation *slot = new Relation();
                          listRelations->push(slot);
                          amountRelations++;
             }
             counterRelations++;
         }
         if (counterRelations % 2 == 1 && token == "member" && attrib.value("type").toString() == "way") {

             QString *one = new QString(attrib.value("ref").toString());
             if (*one != "") {
                 listRelations->end->value.setWay(one);
             }
         }


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


    /*
    while (listWays->begin) {
        Way slot = listWays->begin->value;
        while(slot.nodes->begin) {
            QString ke = slot.nodes->begin->value;
            cout << ke.toStdString() << endl;
            slot.nodes->begin = slot.nodes->begin->next;
        }
        listWays->begin = listWays->begin->next;
    }
    */


    while (listRelations->begin) {
        Relation slot = listRelations->begin->value;
        while(slot.ways->begin) {
            QString ke = slot.ways->begin->value;
            cout << ke.toStdString() << endl;
            slot.ways->begin = slot.ways->begin->next;
        }
        listRelations->begin = listRelations->begin->next;
    }



return 0;
}
