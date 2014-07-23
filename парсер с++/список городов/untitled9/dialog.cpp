#include "dialog.h"
#include "ui_dialog.h"

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    manager(new QNetworkAccessManager(this)),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);
    ui->getResult->setEnabled(false);

    networkReply = manager->get(QNetworkRequest(QUrl("http://overpass-api.de/api/interpreter?data=[timeout:150];area[name='Российская Федерация'];(node[place='city'](area););out;")));
    connect(networkReply, SIGNAL(finished()), this, SLOT(slotReadyRead()));
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
               QTextCodec *codec = QTextCodec::codecForName("UTF-8");
               xmlReader.addData(codec->toUnicode(data));
               parseXml();
           }
}

void Dialog::sortCityList()
{
        for (int j = 0; j < cityList.length() - 1; j++)  {
             for (int i = 0; i < cityList.length() - 1; i++) {
                    QString a = cityList.at(i);
                    QString b = cityList.at(i+1);
                    int counter = 0;
                    int res = 0;
                    while( counter < a.length() && counter < b.length()) {
                        if (a.at(counter) != b.at(counter)) {
                            if (a.at(counter) > b.at(counter)) {
                                res = 1;
                            }
                            else
                                res = 2;
                            break;
                        }
                        counter++;
                    }
                    if (res == 1) {
                        cityList.swap(i, i + 1);
                    }
             }
        }
}

void Dialog::printCityList()
{
     for (int j = 0; j < cityList.length() - 1; j++) {
         ui->textEdit->append(cityList.at(j));
         ui->comboBoxCity->addItem(cityList.at(j));
     }

}

int Dialog::parseXml()
{
    while (!xmlReader.atEnd()) {
             xmlReader.readNext();
             QStringRef token = xmlReader.name();
             QXmlStreamAttributes attrib = xmlReader.attributes();
             if (token == "tag" && attrib.value("k").toString() == "name:ru") {
                 cityList.append(attrib.value("v").toString());
             }
     }
    sortCityList();
    printCityList();
    ui->getResult->setEnabled(true);
    return 0;
}
