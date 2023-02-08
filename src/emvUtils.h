#include <string>
#include <QtXml>

using namespace std;

int ownMap(double n, double start1, double stop1, double start2, double stop2);

int clampValue(int value, int min, int max, int step);

string panMapping(int);

int panMapping(string);

QList<int> getIndexesFromNode(QDomElement _node);

QString getStringByTagName(QDomElement parent, QString _name);

double getSamplingRateFromNode(QDomElement node);

QString convertDecToHexString(int input, int targetLength);

int getControlTypeIndexFromType(QString input);
