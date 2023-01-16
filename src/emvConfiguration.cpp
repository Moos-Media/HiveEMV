#include "emvConfiguration.h"
#include <QDebug>

EmvConfiguration::EmvConfiguration(QDomNode _parent)
{
    parent = _parent;
	controlsAmount = 0;
	xmlsetControls();
	xmlsetJacks();
	xmlsetAudioUnits();
    xmlsetDictonary(); //Has to be first to set up locales
	
}

EmvConfiguration::EmvConfiguration(){}


//--------------------------------------------------------------------------------- Public Getters
QString EmvConfiguration::getLocaleText(QString langiden, int index1, int index2)
{
    //Check if language is available
    QString language = (allLocales.contains(langiden)) ? langiden : "EN";

    //If Element exists return element
    if(allLocales[language].contains(index1) && allLocales[language][index1].contains(index2))
    {
        return allLocales[language][index1][index2];
    }
    else
    {
        qCritical() << "Element not found!";
        return "";
    }
}

QString EmvConfiguration::getConfigurationDescription(QString langiden) {
	QString key, value;

	QDomNode node = parent.toElement().elementsByTagName("localized_description").at(0);

	if (node.firstChild().hasChildNodes())
	{
		int index1, index2;
		bool status = false;

		index1 = node.firstChild().firstChild().toElement().text().toInt(&status, 16);
		index2 = node.firstChild().lastChild().toElement().text().toInt();

		value = getLocaleText(langiden, index1, index2);
	}
	else
	{
		value = node.toElement().text();
	}

	return value;
}

EmvControl EmvConfiguration::getControl(int index) {
	return controls[index];
}

EmvJack EmvConfiguration::getJack(QString dir, int index) {
	return (dir.toUpper() == "IN") ? input[index] : output[index];
}

int EmvConfiguration::getJackAmount(QString dir) {
	return (dir.toUpper() == "IN") ? input.size() : output.size();
}

int EmvConfiguration::getControlsAmount() {
	if (controlsAmount != 0)
		return controlsAmount;
	else
		return -1;
}

EmvAudioUnit EmvConfiguration::getAudioUnit(int index) {
	return audioUnits[index];
}
	//--------------------------------------------------------------------------------- Private Functions
void EmvConfiguration::xmlsetDictonary()
{
    allLocales.clear();
    localeSet.clear();
    localeTemp.clear();

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
        for (int j = 0; j<localeStringsNode.childNodes().count(); ++j)
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

void EmvConfiguration::xmlsetControls() {

	//Get Total amount of controls to find last occurence
	int lastNode = parent.toElement().elementsByTagName("controls").count()-1;

	//Get Root Controls Node via last occurence
	if (lastNode < 0)
		return;

	QDomNode controlRoot = parent.toElement().elementsByTagName("controls").at(lastNode);

	//Get List of all available Controls
	QDomNodeList controlsList = controlRoot.childNodes();

	//Create Config Instances
	for (int i = 0; i < controlsList.count(); ++i)
	{
		controls.insert(i, EmvControl(controlsList.at(i)));
		controlsAmount += 1;
	}
}

void EmvConfiguration::xmlsetJacks() {

	QDomNode jackNode;
	QDomNodeList jackList;
	//INPUT JACKS
	//Get Root Input Jack Node
	jackNode = parent.toElement().elementsByTagName("input_jacks").at(0);

	//Get List of all available Controls
	jackList = jackNode.childNodes();

	//Create Config Instances
	for (int i = 0; i < jackList.count(); ++i)
	{
		input.insert(i, EmvJack(jackList.at(i)));
	}

	//OUTPUT JACKS
	//Get Root Input Jack Node
	jackNode = parent.toElement().elementsByTagName("output_jacks").at(0);

	//Get List of all available Controls
	jackList = jackNode.childNodes();

	//Create Config Instances
	for (int i = 0; i < jackList.count(); ++i)
	{
		output.insert(i, EmvJack(jackList.at(i)));
	}


}

void EmvConfiguration::xmlsetAudioUnits() {
	QDomNode audioUnitsNode;
	QDomNodeList audioUnitsList;


	//Get Audio Units Node
	audioUnitsNode = parent.toElement().elementsByTagName("audio_units").at(0);

	//Get List of all available Controls
	audioUnitsList = audioUnitsNode.toElement().elementsByTagName("audio_unit");

	if (audioUnitsList.count() < 1)
		return;

	//Create Config Instances
	for (int i = 0; i < audioUnitsList.count(); ++i)
	{
		audioUnits.append(EmvAudioUnit(audioUnitsList.at(i)));
	}
}
