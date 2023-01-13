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
    QStringList getTalkerCapabilities();
    int getMaxStreamSinks();
	QStringList getListenerCapabilities();
    bool canBeController();
    QString getAssociationID();
    QString getEntityName();
    QString getFirmwareVersion();
    QString getGroupName();
    QString getSerialNumber();
    int getCurrentConfigurationIndex();
    QString getVendorName();
    QString getModelName();
	QString getConfigurationDescription(int index);
	EmvConfiguration getConfiguration(int index);
	EmvConfiguration getCurrentConfiguration();
	int getConfigurationCount();


    //Entity Locale Getter
    QString getLocale(int index1, int index2);
    QString getLocale(QString locale, int index1, int index2);


	//Setters
	void changeLanguage(QString _lang);

private:
    QString language;
    QDomDocument document;
    QMap<QString, QString> entityData;
    QMap<int, EmvConfiguration> entityConfigurations;

    QDomElement xmlGetRootFromFile(QString _filename);
    void xmlsetEntityMetaData();
    void xmlsetConfigurations();
    void xmlReadStringFromNode(QDomElement node);

	QString getEntityDataString(QString _key);
		int getEntityDataInt(QString _key);
	QStringList parseCapabilites(QString rawData);


};

#endif // EMVENTITY_H
