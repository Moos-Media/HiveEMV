#include "emvEntity.h"
#include "emvUtils.h"

EmvEntity::EmvEntity(){};

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

EmvEntity::EmvEntity(la::avdecc::UniqueIdentifier _entityID)
{
	auto& manager = hive::modelsLibrary::ControllerManager::getInstance();
	auto controlledEntity = manager.getControlledEntity(_entityID);

	if (controlledEntity)
	{
		la::avdecc::controller::model::ConfigurationNode configurationNode;
		try
		{
			configurationNode = controlledEntity->getCurrentConfigurationNode();
		}
		catch (la::avdecc::controller::ControlledEntity::Exception const&)
		{
			return;
		}
		auto actEntity = controlledEntity->getEntity();

		auto const* const staticModel = controlledEntity->getEntityNode().staticModel;
		auto const* const dynamicModel = controlledEntity->getEntityNode().dynamicModel;

		entityData.insert("entity_name", hive::modelsLibrary::helper::entityName(*controlledEntity));		entityData.insert("group_name", hive::modelsLibrary::helper::groupName(*controlledEntity));
		entityData.insert("entity_id", hive::modelsLibrary::helper::uniqueIdentifierToString(_entityID));
		entityData.insert("entity_model_id", hive::modelsLibrary::helper::uniqueIdentifierToString(actEntity.getEntityModelID()));
		entityData.insert("talker_stream_sources", QString::number(actEntity.getTalkerStreamSources()));
		entityData.insert("listener_stream_sinks", QString::number(actEntity.getListenerStreamSinks()));
		entityData.insert("talker_capabilities", convertCapabilitiesDecToHexString(actEntity.getTalkerCapabilities().value()));
		entityData.insert("listener_capabilities", convertCapabilitiesDecToHexString(actEntity.getListenerCapabilities().value()));
		entityData.insert("controller_capabilities", convertCapabilitiesDecToHexString(actEntity.getControllerCapabilities().value()));
		entityData.insert("entity_capabilities", convertCapabilitiesDecToHexString(actEntity.getEntityCapabilities().value()));
		if (actEntity.getAssociationID().has_value())
			entityData.insert("association_id", QString::number(actEntity.getAssociationID().value(), 16));
		entityData.insert("model_name", hive::modelsLibrary::helper::localizedString(*controlledEntity, staticModel->modelNameString));
		entityData.insert("vendor_name", hive::modelsLibrary::helper::localizedString(*controlledEntity, staticModel->vendorNameString));

		entityData.insert("serial_number", dynamicModel->serialNumber.data());
		entityData.insert("firmware_version", dynamicModel->firmwareVersion.data());
	}
}

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
		return (entityData["controller_capabilities"].toInt() == 0) ? false : true;
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
	return entityConfigurations[getCurrentConfigurationIndex()].getLocaleText(locale, index1, index2);
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

	xmlsetEntityMetaData();
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

