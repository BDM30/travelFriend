#include "dialog.h"
#include "ui_dialog.h"

using namespace std;

int const amount = 15000;

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog),
    manager(new QNetworkAccessManager(this)),
    amountNodes(0),
    amountWays(0),
    amountRelations(0)
{

    ui->setupUi(this);
    ui->loadButton->setEnabled(false);

    networkReply = manager->get(QNetworkRequest(QUrl("http://overpass-api.de/api/interpreter?data=(rel[name='Прокопьевск'];>;);out;")));
    connect(networkReply, SIGNAL(finished()), this, SLOT(slotReadyRead()));

    listNodes = new List<Node>();
    listWays = new List<Way>();
    listRelations = new List<Relation>();

    Node *headNode = new Node();
    listNodes->push(headNode);

    Way *headWay = new Way();
    listWays->push(headWay);


    Relation *headRel = new Relation();
    listRelations->push(headRel);
}

Dialog::~Dialog()
{
    delete ui;
}

void Dialog::slotReadyRead()
{
     ui->textEdit->append("slot used\n");
    int statusCode = networkReply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        if (statusCode >= 200 && statusCode < 300)
        {
            QByteArray data = networkReply->readAll();
            QTextCodec *codec = QTextCodec::codecForName("Windows-1251");
            xmlReader.addData(codec->toUnicode(data));
            parseXml();
            outPut();
        }
}

int Dialog::parseXml()
{


    long counterWays = 0; // следит за открытием\закрытием тега way
    long counterRelations = 0; //  следит за открытием\закрытием тега relation




    while (!xmlReader.atEnd())
        {
         xmlReader.readNext();
         QStringRef token = xmlReader.name();
         QXmlStreamAttributes attrib = xmlReader.attributes();
         if (token == "node") {
             if (listNodes->begin) {
                Node *slot = new Node();
                slot->set(attrib.value("id").toString(),attrib.value("lat").toDouble(), attrib.value("lon").toDouble());
                if (attrib.value("id").toString() != "")
                {
                    listNodes->push(slot);
                    amountNodes++;
                }
             }
             else
             {
                 listNodes->begin->value.set(attrib.value("id").toString(),attrib.value("lat").toDouble(), attrib.value("lon").toDouble());
                 amountNodes++;
             }
         }

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


    ui->textEdit->append("check sum\n");
    ui->textEdit->append("amount nodes "+QString::number(amountNodes));
    ui->textEdit->append("amount ways " + QString::number(amountWays));
    ui->textEdit->append("amount relation "+QString::number(amountRelations ));
    ui->textEdit->append("=====================");
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



    /*
    while (listRelations->begin) {
        Relation slot = listRelations->begin->value;
        while(slot.ways->begin) {
            QString ke = slot.ways->begin->value;
            cout << ke.toStdString() << endl;
            slot.ways->begin = slot.ways->begin->next;
        }
        listRelations->begin = listRelations->begin->next;
    }
    */


return 0;
}

void Dialog::outPut() {
    QString currentWay = ""; // текущий путь из Relation
    QString shapeBegin = ""; // начало контура
    QString shapeEnd = ""; // конец контура
    while (listRelations->begin) { // идем по всем Relations
        //ui->textEdit->append("here");
        if (currentWay == "") { // если начальный путь не выбран выбираем его (берем первый)
            Relation slotR = listRelations->begin->value;
            currentWay = slotR.ways->begin->value;
            // затем его надо удалить из списка путей этого Relation
            slotR.ways->remove(currentWay);
            // теперь найдем его в общем списке путей
            Way slotW = findWayById(currentWay);
            // получим конец и начало нашего строящегося полигона
            shapeBegin = slotW.nodes->begin->value;
            shapeEnd = slotW.nodes->end->value;
            // если путь и есть контур, идем раньше
            if (shapeBegin == shapeEnd) {
                // полигон = все точки Way с id = currentWay
            }
        }
        else
            // теперь нужно среди оставшихся путей Relation выбрать те, кто имеет общее (начало || конец) с построенным
        {

        }
        listRelations->begin = listRelations->begin->next;
        break;
    }

    ofstream outFile;
    //QString test="V la Dick!";
    outFile.open("hard.html", ios::out);
    //outFile << test.toStdString();
    outFile.close();
}
