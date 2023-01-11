#ifndef EMVCONFIGURATION_H
#define EMVCONFIGURATION_H

#include <QtXml>

class EmvConfiguration
{
public:
    EmvConfiguration(QDomNode _parent);
    EmvConfiguration();
    QString getLocaleText(QString langiden, int index1, int index2);
	QString getConfigurationDescription(QString langiden);

private:
    QDomNode parent;

    QMap<QString, QMap<int, QMap<int, QString>>> allLocales;
    QMap<int, QMap<int, QString>> localeSet;
    QMap<int, QString> localeTemp;

    void xmlsetDictonary();

};

#endif // EMVCONFIGURATION_H
