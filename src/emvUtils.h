#include <string>
#include <QtXml>

using namespace std;

string panMapping(int);
int panMapping(string);
int ownMap(double, double, double, double, double);

QList<int> getIndexesFromNode(QDomElement _node);

double getSamplingRateFromNode(QDomElement node);
