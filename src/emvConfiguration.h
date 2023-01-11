#ifndef EMVCONFIGURATION_H
#define EMVCONFIGURATION_H

#include "C://Qt/5.15.2/msvc2019_64/include/QtXml/qdom.h"
#include "C://Qt/5.15.2/msvc2019_64/include/QtXml/qtxml-config.h"
#include "C://Qt/5.15.2/msvc2019_64/include/QtXml/qtxmlglobal.h"
#include "C://Qt/5.15.2/msvc2019_64/include/QtXml/qtxmlversion.h"
#include "C://Qt/5.15.2/msvc2019_64/include/QtXml/qxml.h"

class EmvConfiguration
{
public:
    EmvConfiguration(QDomNode _parent);
    EmvConfiguration();
    QString getLocaleText(QString langiden, int index1, int index2);

private:
    QDomNode parent;

    QMap<QString, QMap<int, QMap<int, QString>>> allLocales;
    QMap<int, QMap<int, QString>> localeSet;
    QMap<int, QString> localeTemp;

    void xmlsetDictonary();
};

#endif // EMVCONFIGURATION_H
