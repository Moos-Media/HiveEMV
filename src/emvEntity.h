#ifndef EMVENTITY_H
#define EMVENTITY_H

#include <QObject>
#include "emvConfiguration.h"
#include "C://Qt/5.15.2/msvc2019_64/include/QtXml/qdom.h"
#include "C://Qt/5.15.2/msvc2019_64/include/QtXml/qtxml-config.h"
#include "C://Qt/5.15.2/msvc2019_64/include/QtXml/qtxmlglobal.h"
#include "C://Qt/5.15.2/msvc2019_64/include/QtXml/qtxmlversion.h"
#include "C://Qt/5.15.2/msvc2019_64/include/QtXml/qxml.h"




class EmvEntity
{
public:
    EmvEntity(QString _filename, QString _language = "en");
	EmvEntity();
    QDomElement root;

    //Entity MetaData Getters
    QString getEntityID();
    QString getEntityModelID();
    QString getEntityCapabilities();
    int getMaxStreamSources();
    QString getTalkerCapabilities();
    int getMaxStreamSinks();
    QString getListenerCapabilities();
    bool canBeController();
    QString getAssociationID();
    QString getEntityName();
    QString getFirmwareVersion();
    QString getGroupName();
    QString getSerialNumber();
    int getCurrentConfiguration();
    QString getVendorName();
    QString getModelName();

    //Entity Locale Getter
    QString getLocale(int index1, int index2);
    QString getLocale(QString locale, int index1, int index2);





private:
    QString language;
    QDomDocument document;
    QMap<QString, QString> entityData;
    EmvConfiguration entityConfigurations[100];

    QDomElement xmlReadFile(QString _filename);
    void xmlsetEntityMetaData();
    void xmlsetConfigurations();
    void xmlReadStringFromNode(QDomElement node);


};

#endif // EMVENTITY_H
