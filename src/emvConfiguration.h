#ifndef EMVCONFIGURATION_H
#define EMVCONFIGURATION_H

#include <QtXml>
#include "emvClasses.h"

class EmvConfiguration
{
public:
    EmvConfiguration(QDomNode _parent);
    EmvConfiguration();
    QString getLocaleText(QString langiden, int index1, int index2);
	QString getConfigurationDescription(QString langiden);
	EmvControl getControl(int index);
	EmvJack getJack(QString dir, int index);
	int getJackAmount(QString dir);
	int getControlsAmount();
	EmvAudioUnit getAudioUnit(int index);

private:
    QDomNode parent;

    QMap<QString, QMap<int, QMap<int, QString>>> allLocales;
    QMap<int, QMap<int, QString>> localeSet;
    QMap<int, QString> localeTemp;

    void xmlsetDictonary();
	void xmlsetControls();
	void xmlsetJacks();
	void xmlsetAudioUnits();
	QMap<int, EmvControl> controls;
	int controlsAmount;
	QMap<int, EmvJack> input;
	QMap<int, EmvJack> output;
	QList<EmvAudioUnit> audioUnits;
};

#endif // EMVCONFIGURATION_H
