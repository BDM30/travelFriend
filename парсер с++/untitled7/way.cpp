#pragma once
#include <QString>
#include "list.h"

class Way {
public:
    void setNode(QString *input) {
        nodes->push(input);
        amountNodes++;
    }
    void setId(QString input) {
        id = input;
    }
    Way()
    {
        amountNodes = 0;
        nodes = new List<QString>();
    }
    QString id;
    List<QString> *nodes;
    int amountNodes;
};
