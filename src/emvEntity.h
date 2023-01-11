#ifndef EMVENTITY_H
#define EMVENTITY_H

#include <QObject>
#include "emvConfiguration.h"




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
	QString getConfigurationDescription(int index);


    //Entity Locale Getter
    QString getLocale(int index1, int index2);
    QString getLocale(QString locale, int index1, int index2);


	//Setters
	void changeLanguage(QString _lang);

	



private:
    QString language;
    QDomDocument document;
    QMap<QString, QString> entityData;
    EmvConfiguration entityConfigurations[100];

    QDomElement xmlGetRootFromFile(QString _filename);
    void xmlsetEntityMetaData();
    void xmlsetConfigurations();
    void xmlReadStringFromNode(QDomElement node);

	QString getEntityDataString(QString _key);
		int getEntityDataInt(QString _key);


};

#endif // EMVENTITY_H
