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
