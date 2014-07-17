#pragma once
#include <QString>
#include "list.h"

class Relation {
public:
    void setId(QString input) {
         id = input;
    }
    void setWay(QString *input) {
        ways->push(input);
        amountWays++;
    }

    Relation() {
        amountWays = 0;
        ways = new List<QString>();
    }
    QString id;
    List<QString> *ways;
    int amountWays;
};
