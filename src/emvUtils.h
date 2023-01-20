#include <string>
#include <QtXml>

using namespace std;

int ownMap(double, double, double, double, double);

string panMapping(int);

int panMapping(string);

QList<int> getIndexesFromNode(QDomElement _node);

QString getStringByTagName(QDomElement parent, QString _name);

double getSamplingRateFromNode(QDomElement node);

QString convertDecToHexString(int input, int targetLength);

int getControlTypeIndexFromType(QString input);
