#include "emvUtils.h"
#include <iostream>
#include <QDebug>
#include <QtXml>

int ownMap(double n, double start1, double stop1, double start2, double stop2)
{
    return (n - start1) / (stop1 - start1) * (stop2 - start2) + start2;
}

string panMapping(int input) {
    string output = "";
    int value = 2*(input - 50);

    //Center
    if (input == 50)
        output = "C";

    //Left
    if(input < 50)
        output = to_string(-value)+"L";

    //Right
    if(input > 50)
        output = to_string(value)+"R";

    return output;
}

int panMapping(string input) {
    int output = 0;
    string value = "";
    string dir = "";

    //Getting Direction and Value for Off-Center settings
    if (input.length() == 3)
    {
        dir = toupper(input[2]);
        value = input.substr(0, 2);
    }

    if (input.length() == 4)
    {
        dir = toupper(input[3]);
        value = input.substr(0, 3);
    }

    // Center
    if ((input.length() == 1) && (input == "C" || "c"))
        output = 50;

    //Left
    if (dir == "l" || dir == "L") {
        output = ownMap(stoi(value), 0, 100, 50, 0);
    }

    //Right
    if (dir == "r" || dir == "R") {
        output = ownMap(stoi(value), 0, 100, 50, 100);
}
    return output;
}

QList<int> getIndexesFromNode(QDomElement node)
{
	QList<int> indexes;
	QList<int> wrong;
	wrong.append(0);
	wrong.append(0);
	bool temp = false;
	indexes.append(node.firstChild().firstChild().toElement().text().toInt(&temp, 16));
	indexes.append(node.firstChild().lastChild().toElement().text().toInt());

	if (temp)
		return indexes;
	else
		return wrong;
}

QString getStringByTagName(QDomElement parent, QString _name)
{
	return parent.elementsByTagName(_name).at(0).toElement().text();
}
