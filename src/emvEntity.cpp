#include "emvEntity.h"

EmvEntity::EmvEntity(QString _filename, QString _language)
{
    for(int i = 0; i<100; ++i)
    {
        entityConfigurations[i] = EmvConfiguration();
    }
    language = _language.toUpper();
    root = xmlReadFile(_filename);
    xmlsetConfigurations(); // Has to happen first to set up locales

    xmlsetEntityMetaData();

}

EmvEntity::EmvEntity(){}

//---------------------------------------------------------------------------Getters

QString EmvEntity::getEntityID()
{
    return entityData["entity_id"];
}

QString EmvEntity::getEntityModelID()
{
    return entityData["entity_model_id"];
}

QString EmvEntity::getEntityCapabilities()
{
    return entityData["entity_capabilities"];
}

int EmvEntity::getMaxStreamSources()
{
    int maxSources = 0;
    try {
        maxSources = entityData["talker_stream_sources"].toInt();
    } catch (...){
        qCritical() << "Could not convert Sources.";
    }
    return maxSources;
}

QString EmvEntity::getTalkerCapabilities()
{
    return entityData["talker_capabilities"];
}

int EmvEntity::getMaxStreamSinks()
{
    int maxSources = 0;
    try {
        maxSources = entityData["listener_stream_sinks"].toInt();
    } catch (...){
        qCritical() << "Could not convert Sources.";
    }
    return maxSources;
}

QString EmvEntity::getListenerCapabilities()
{
    return entityData["listener_capabilities"];
}

bool EmvEntity::canBeController()
{
    return (entityData["controller_capabilities"] == "00000000") ? false : true;
}

QString EmvEntity::getAssociationID()
{
    return entityData["association_id"];
}

QString EmvEntity::getEntityName()
{
    return entityData["entity_name"];
}

QString EmvEntity::getFirmwareVersion()
{
    return entityData["firmware_version"];
}

QString EmvEntity::getGroupName()
{
    return entityData["group_name"];
}

QString EmvEntity::getSerialNumber()
{
    return entityData["serial_number"];
}

int EmvEntity::getCurrentConfiguration()
{
    int currentIndex = 0;
    try {
        currentIndex = entityData["current_configuration"].toInt();
    } catch (...){
        qCritical() << "Could not convert Index.";
    }
    return currentIndex;
}

QString EmvEntity::getVendorName()
{
    return entityData["vendor_name"];
}

QString EmvEntity::getModelName()
{
    return entityData["model_name"];
}

QString EmvEntity::getLocale(int index1, int index2)
{
    return getLocale(language, index1, index2);
}

QString EmvEntity::getLocale(QString locale, int index1, int index2)
{
    return entityConfigurations[getCurrentConfiguration()].getLocaleText(language, index1, index2);
}

//---------------------------------------------------------------------------Read XML

QDomElement EmvEntity::xmlReadFile(QString _filename)
{
    QDomElement entity;

    QFile file(_filename);

    if(!file.open(QIODevice::ReadOnly))
    {
        qCritical() << "Failes to open file";
        return entity;
    }
    else
    {
        if(!document.setContent(&file))
            {
               qCritical() << "Error: Open file ";
               return entity;
            }
            qDebug() << file.isReadable();
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
    for (int i = 0; i < configurationsRoot.childNodes().count(); ++i) {
        entityConfigurations[i] = EmvConfiguration(configurationsRoot.childNodes().at(i));
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

        value =  getLocale("EN", index1, index2);
    }
    else
    {
        value = node.text();
    }
    entityData.insert(key, value);
}

