#include <iostream>
#include <QDebug>
#include <QtXml>
#include "emvUtils.h"

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

double getSamplingRateFromNode(QDomElement node) {
	int pull, base;
	double multiplier = 1.0;
	bool temp;

	pull = node.firstChild().toElement().text().toInt();
	base = node.lastChild().toElement().text().toInt(&temp, 16);

	switch (pull)
	{
		case (0):
			multiplier = 1.0;
			break;
		case (1):
			multiplier = 1/1.001;
			break;
		case (2):
			multiplier = 1.001;
			break;
		case (3):
			multiplier = 24/25;
			break;
		case (4):
			multiplier = 25/24;
			break;
		case (5):
			multiplier = 1/8;
			break;
		default:
			multiplier = 1.0;
			break;
	}

	return base * multiplier;
}

QString convertCapabilitiesDecToHexString(int input) {
	// Basic structure found on:
	// www.educative.io/answers/hot-to-convert-a-number-from-decimal-to-hexadecimal-in-cpp
	int remainder, product = 1;
	std::string hex_dec = "";
	while (input != 0)
	{
		remainder = input % 16;
		char ch;
		if (remainder >= 10)
			ch = remainder + 55;
		else
			ch = remainder + 48;
		hex_dec += ch;

		input = input / 16;
		product *= 10;
	}
	std::reverse(hex_dec.begin(), hex_dec.end());
	QString temp = QString::fromStdString(hex_dec);
	int missing = 4 - temp.length();
	QString output = "";

	//Make sure, output String is four characters long
	for (int i = 0; i < missing; ++i)
	{
		output.append("0");
	}
	return output.append(temp);
}
