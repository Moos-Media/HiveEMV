#include "emvFileHandler.h"
#include <QtXml>
#include <QDebug>
#include "emvUtils.h"

EmvFileHandler::EmvFileHandler(QString _filepath, QString _language){
	qDebug() << "FileHandler Working";
	/*
	la::avdecc::entity::Entity::CommonInformation common = la::avdecc::entity::Entity::CommonInformation();
	la::avdecc::entity::Entity::InterfacesInformation intinfo = la::avdecc::entity::Entity::InterfacesInformation();

	la::avdecc::entity::Entity *testEntity = new la::avdecc::entity::Entity(common, intinfo);
	*/

	//Set Language 
	setLanguage(_language);
	int currentConfigIndex = root.elementsByTagName("current_configuration").at(0).toElement().text().toInt();

	//Read XML Filea nd get dictonary
	root = xmlGetRootFromFile(_filepath);
	getDictionary(currentConfigIndex);

	//Get Entity ID from File
	QString idReadFromNode = root.elementsByTagName("entity_id").at(0).toElement().text();

	std::stringstream ss;
	std::uint64_t idInt;

	ss << std::hex << idReadFromNode.toStdString();
	ss >> idInt;

	entityID = la::avdecc::UniqueIdentifier(idInt);

	//Either get matching Entity or create virtual one
	setControlledEntity();

	//Set Metadata if Entity is Virtual
	if (!isVirtual)
	{
		auto const staticModel = controlledEntity->getEntityNode().staticModel;
		auto const dynamicModel = controlledEntity->getEntityNode().dynamicModel;

		dynamicModel->currentConfiguration = currentConfigIndex;
		dynamicModel->entityName = xmlReadStringFromNode(root.elementsByTagName("entity_name").at(0).toElement()).toStdString();
		dynamicModel->firmwareVersion = xmlReadStringFromNode(root.elementsByTagName("firmware_version").at(0).toElement()).toStdString();
		dynamicModel->groupName = xmlReadStringFromNode(root.elementsByTagName("group_name").at(0).toElement()).toStdString();
		dynamicModel->serialNumber = xmlReadStringFromNode(root.elementsByTagName("serial_number").at(0).toElement()).toStdString();
	}

	//

	//auto audioUnits = controlledEntity->getCurrentConfigurationNode().audioUnits;
	auto test = la::avdecc::controller::model::ControlNode();
	test.descriptorIndex = 99;
	
	qDebug() << "Index: " << test.descriptorIndex;

	/* Virtual Entity is not yet created

	set metadata
	set config data
	set audio unit data
	set jacks data

	set controls on jacks

	if no hive entity was found earlier add own entity to managed list

	*/


}

//---------------------------------------------------------------------------------------------------------------------------------- Setters

void EmvFileHandler::setLanguage(QString _lang)
{
	if (_lang.length() == 2)
		language = _lang.toUpper();
	else
		qCritical() << "Wrong format for Locale Identifier";
}

void EmvFileHandler::getDictionary(int index) {
	allLocales.clear();
	localeSet.clear();
	localeTemp.clear();

	QDomNode parent = root.elementsByTagName("configurations").at(0).childNodes().at(index);

	//Get "locales" Node
	QDomNode localeRoot = parent.toElement().elementsByTagName("locales").at(0);

	//Get List of all available locales
	QDomNodeList locales = localeRoot.childNodes();

	//Loop through all locales
	for (int i = 0; i < locales.count(); ++i)
	{
		QString identifier;

		//Get current Locale Node
		QDomElement currentLocaleNode = locales.at(i).toElement();

		identifier = currentLocaleNode.elementsByTagName("locale_identifier").at(0).toElement().text().toUpper();

		//Get Locale Strings Node
		QDomElement localeStringsNode = currentLocaleNode.elementsByTagName("locale_strings").at(0).toElement();

		//Loop through all strings
		for (int j = 0; j < localeStringsNode.childNodes().count(); ++j)
		{
			// Get current strings node
			QDomElement currentStringsNode = localeStringsNode.childNodes().at(j).toElement();

			//Loop through strings node
			for (int h = 0; h < currentStringsNode.childNodes().count(); ++h)
			{
				//Save all string texts in strings node
				localeTemp.insert(h, currentStringsNode.childNodes().at(h).toElement().text());
			}
			//Save all strings nodes to Locale Set
			localeSet.insert(j, localeTemp);
		}
		//Save Locale Set to dictionary
		allLocales.insert(identifier, localeSet);
	}
}

void EmvFileHandler::setControlledEntity() {
	auto& manager = hive::modelsLibrary::ControllerManager::getInstance();
	controlledEntity = manager.getControlledEntity(entityID);

	if (controlledEntity)
	{
		isVirtual = false;
		auto actEntity = controlledEntity->getEntity();
	}
	else
	{
		isVirtual = true;
	}
		
}
//---------------------------------------------------------------------------------------------------------------------------------- Read XML

QDomElement EmvFileHandler::xmlGetRootFromFile(QString _filename)
{
	QDomElement entity;

	QFile file(_filename);

	if (!file.open(QIODevice::ReadOnly))
	{
		qCritical() << "EmvFilehandler: Failed to Read File";
		return entity;
	}
	else
	{
		if (!document.setContent(&file))
		{
			qCritical() << "EmvFileHandler: Failed to Open File";
			return entity;
		}
		//qDebug() << file.isReadable();
		file.close();
	}

	//get root element
	entity = document.firstChildElement();
	return entity;
}

QString EmvFileHandler::xmlReadStringFromNode(QDomElement node)
{
	QString value;

	if (node.firstChild().hasChildNodes())
	{
		int index1, index2;
		bool status = false;

		index1 = node.firstChild().firstChild().toElement().text().toInt(&status, 16);
		index2 = node.firstChild().lastChild().toElement().text().toInt();

		value = getLocale(index1, index2);
	}
	else
	{
		value = node.text();
	}
	return value;
}

QString EmvFileHandler::getLocale(int index1, int index2) {
	//Check if language is available
	QString lang = (allLocales.contains(language)) ? language : "EN";

	//If Element exists return element
	if (allLocales[lang].contains(index1) && allLocales[lang][index1].contains(index2))
	{
		return allLocales[lang][index1][index2];
	}
	else
	{
		qCritical() << "Element not found!";
		return "";
	}
}
