#include "dialog.h"
#include "ui_dialog.h"
#include "QVector"

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
    QString idsNode[508];
    int curId = 0;
    int c = 0;
    while (!xmlReader.atEnd())
        {
         xmlReader.readNext();
         QStringRef token = xmlReader.name();
         ui->textEdit->append("c = " + QString::number(c));
         ui->textEdit->append(xmlReader.name().toString());
         QXmlStreamAttributes attrib = xmlReader.attributes();
         if (token == "node") {
             QString one = attrib.value("id").toString();
             if (one != "")
             {
                idsNode[curId] = one;
                curId++;
             }
         }
        }
    for (int i = 0; i < curId; i++) {
        ui->textEdit->append(idsNode[i]);
    }
}
