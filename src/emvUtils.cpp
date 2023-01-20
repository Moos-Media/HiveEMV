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

QString convertDecToHexString(int input, int targetLength) {
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
	int missing = targetLength - temp.length();
	QString output = "";

	//Make sure, output String is four characters long
	for (int i = 0; i < missing; ++i)
	{
		output.append("0");
	}
	return output.append(temp);
}

int getControlTypeIndexFromType(QString input)
{
	if (input == "90e0f00000000000")
		return 0;
	else if (input == "90e0f00000000001")
		return 1;
	else if (input == "90e0f00000000002")
		return 2;
	else if (input == "90e0f00000000003")
		return 3;
	else if (input == "90e0f00000000004")
		return 4;
	else if (input == "90e0f00000000005")
		return 5;
	else if (input == "90e0f00000000006")
		return 6;
	else if (input == "90e0f00000000007")
		return 7;
	else if (input == "90e0f00000000008")
		return 8;
	else if (input == "90e0f00000000009")
		return 9;
	else if (input == "90e0f0000000000a")
		return 10;
	else if (input == "90e0f0000000000b")
		return 11;
	else if (input == "90e0f0000000000c")
		return 12;
	else if (input == "90e0f0000000000d")
		return 13;
	else if (input == "90e0f0000000000e")
		return 14;
	else if (input == "90e0f0000000000f")
		return 15;
	else if (input == "90e0f00000000010")
		return 16;
	else if (input == "90e0f00000000011")
		return 17;
	else if (input == "90e0f00000000012")
		return 18;
	else if (input == "90e0f00000000013")
		return 19;
	else if (input == "90e0f00000000014")
		return 20;
	else if (input == "90e0f00000000015")
		return 21;
	else if (input == "90e0f00000000016")
		return 22;
	else if (input == "90e0f00000000017")
		return 23;
	else if (input == "90e0f00000000018")
		return 24;
	else if (input == "90e0f00000000019")
		return 25;
	else if (input == "90e0f0000000001a")
		return 26;
	else if (input == "90e0f0000000001b")
		return 27;
	else if (input == "90e0f0000000001c")
		return 28; //-----------------------------------------------
	else if (input == "90e0f00000010000")
		return 30;
	else if (input == "90e0f00000010001")
		return 31;
	else if (input == "90e0f00000010002")
		return 32;
	else if (input == "90e0f00000010003")
		return 33;
	else if (input == "90e0f00000010004")
		return 34;

	else
		return -1;

}
