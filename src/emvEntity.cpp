#include "emvEntity.h"

EmvEntity::EmvEntity(QString _filename, QString _language)
{
	//Set Language
	changeLanguage(_language);

	//Read XML File
	root = xmlGetRootFromFile(_filename);

	//Read Configs first to get Locale Data Set
    xmlsetConfigurations();

    //Get Meta Data
    xmlsetEntityMetaData();

}

EmvEntity::EmvEntity(){}

//---------------------------------------------------------------------------------------------------------------------------------- External Data Getters

QString EmvEntity::getEntityID()
{
	return getEntityDataString("entity_id");
}

QString EmvEntity::getEntityModelID()
{
	return getEntityDataString("entity_model_id");
}

QString EmvEntity::getEntityCapabilities()
{
	return getEntityDataString("entity_capabilities");
}

int EmvEntity::getMaxStreamSources()
{
	return getEntityDataInt("talker_stream_sources");
}

QStringList EmvEntity::getTalkerCapabilities()
{
	
	return parseCapabilites(getEntityDataString("talker_capabilities"));
}

int EmvEntity::getMaxStreamSinks()
{
	return getEntityDataInt("listener_stream_sinks");
}

QStringList EmvEntity::getListenerCapabilities()
{
	return parseCapabilites(getEntityDataString("listener_capabilities"));
}

bool EmvEntity::canBeController()
{
	if (entityData.contains("controller_capabilities"))
		return (entityData["controller_capabilities"] == "00000000") ? false : true;
	else
		return false;
}

QString EmvEntity::getAssociationID()
{
	return getEntityDataString("association_id");
}

QString EmvEntity::getEntityName()
{
	return getEntityDataString("entity_name");
}

QString EmvEntity::getFirmwareVersion()
{
	return getEntityDataString("firmware_version");
}

QString EmvEntity::getGroupName()
{
	return getEntityDataString("group_name");
}

QString EmvEntity::getSerialNumber()
{
	return getEntityDataString("serial_number");
}

int EmvEntity::getCurrentConfigurationIndex()
{
	return getEntityDataInt("current_configuration");
}

QString EmvEntity::getVendorName()
{
	return getEntityDataString("vendor_name");
}

QString EmvEntity::getModelName()
{
	return getEntityDataString("model_name");
}

QString EmvEntity::getConfigurationDescription(int index)
{
	return entityConfigurations[index].getConfigurationDescription(language);
}

EmvConfiguration EmvEntity::getConfiguration(int index)
{
	return entityConfigurations[index];
}

EmvConfiguration EmvEntity::getCurrentConfiguration()
{
	return entityConfigurations[getCurrentConfigurationIndex()];
}

QString EmvEntity::getLocale(int index1, int index2)
{
	return getLocale(language, index1, index2);
	
}

QString EmvEntity::getLocale(QString locale, int index1, int index2)
{
	return entityConfigurations[getCurrentConfigurationIndex()].getLocaleText(language, index1, index2);
}

int EmvEntity::getConfigurationCount()
{
	return entityConfigurations.size();
}
	//---------------------------------------------------------------------------------------------------------------------------------- Internal Data Getters

QString EmvEntity::getEntityDataString(QString _key)
{
	if (entityData.contains(_key))
		return entityData[_key];
	else
		return "Not Found";
}

int EmvEntity::getEntityDataInt(QString _key)
{
	int maxSources = -1;
	try
	{
		maxSources = entityData[_key].toInt();
	}
	catch (...)
	{
		qCritical() << "Could not convert Sources.";
	}
	return maxSources;
}

QStringList EmvEntity::parseCapabilites(QString rawData) {
	QStringList output;

	//Check if ATDECC is implemented
	if (rawData.at(3) != '1')
		return output;

	//Parse first index
	QChar firstIndex = rawData.at(0);
	QChar secondIndex = rawData.at(1);

	if (firstIndex == '8')
		output.append("Video");
	if (firstIndex == '4')
		output.append("Audio");
	if (firstIndex == '2')
		output.append("MIDI");
	if (firstIndex == '1')
		output.append("SMPTE");
	if (secondIndex == '8')
		output.append("Media Clock");
	if (secondIndex == '4')
		output.append("Control");
	if (secondIndex == '2')
		output.append("Other");

	return output;
}

//---------------------------------------------------------------------------------------------------------------------------------- Setters

void EmvEntity::changeLanguage(QString _lang)
{
	if (_lang.length() == 2)
		language = _lang.toUpper();
	else
		qCritical() << "Wrong format for Locale Identifier";
}

//---------------------------------------------------------------------------------------------------------------------------------- Read XML

QDomElement EmvEntity::xmlGetRootFromFile(QString _filename)
{
    QDomElement entity;

    QFile file(_filename);

    if(!file.open(QIODevice::ReadOnly))
    {
        qCritical() << "Failed to Read File";
        return entity;
    }
    else
    {
        if(!document.setContent(&file))
            {
               qCritical() << "Error: Open file ";
               return entity;
            }
            //qDebug() << file.isReadable();
            file.close();
    }

   //get root element
   entity = document.firstChildElement();
   return entity;
}

void EmvEntity::xmlsetEntityMetaData()
{
    xmlReadStringFromNode(root.elementsByTagName("entity_id").at(0).toElement());
    xmlReadStringFromNode(root.elementsByTagName("entity_model_id").at(0).toElement());
    xmlReadStringFromNode(root.elementsByTagName("entity_capabilities").at(0).toElement());
    xmlReadStringFromNode(root.elementsByTagName("talker_stream_sources").at(0).toElement());
    xmlReadStringFromNode(root.elementsByTagName("talker_capabilities").at(0).toElement());
    xmlReadStringFromNode(root.elementsByTagName("listener_stream_sinks").at(0).toElement());
    xmlReadStringFromNode(root.elementsByTagName("listener_capabilities").at(0).toElement());
    xmlReadStringFromNode(root.elementsByTagName("controller_capabilities").at(0).toElement());
    xmlReadStringFromNode(root.elementsByTagName("association_id").at(0).toElement());
    xmlReadStringFromNode(root.elementsByTagName("entity_name").at(0).toElement());
    xmlReadStringFromNode(root.elementsByTagName("firmware_version").at(0).toElement());
    xmlReadStringFromNode(root.elementsByTagName("group_name").at(0).toElement());
    xmlReadStringFromNode(root.elementsByTagName("serial_number").at(0).toElement());
    xmlReadStringFromNode(root.elementsByTagName("current_configuration").at(0).toElement());
    xmlReadStringFromNode(root.elementsByTagName("vendor_name").at(0).toElement());
    xmlReadStringFromNode(root.elementsByTagName("model_name").at(0).toElement());
}

void EmvEntity::xmlsetConfigurations()
{
    //Get Root Config Node
    QDomNode configurationsRoot = root.elementsByTagName("configurations").at(0);

    //Create Config Instances
	for (int i = 0; i < configurationsRoot.toElement().elementsByTagName("configuration").count(); ++i) {
		entityConfigurations.insert(i, EmvConfiguration(configurationsRoot.childNodes().at(i)));
    }
}

void EmvEntity::xmlReadStringFromNode(QDomElement node)
{
    QString key, value;
    key = node.tagName();

    if (node.firstChild().hasChildNodes())
    {
        int index1, index2;
        bool status = false;

        index1 = node.firstChild().firstChild().toElement().text().toInt(&status, 16);
        index2 = node.firstChild().lastChild().toElement().text().toInt();

        value =  getLocale(index1, index2);
    }
    else
    {
        value = node.text();
    }
    entityData.insert(key, value);
}

