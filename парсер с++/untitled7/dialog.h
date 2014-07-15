#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QtNetwork/QtNetwork>
#include <QTextEdit>
#include <QPushButton>
#include <QXmlStreamReader>

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

private:
    void parseXml();

    Ui::Dialog *ui;
    QNetworkAccessManager *manager;
    QNetworkReply *networkReply;
    QXmlStreamReader xmlReader;
};

#endif // DIALOG_H
