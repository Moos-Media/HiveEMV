#include "emvConfiguration.h"

EmvConfiguration::EmvConfiguration(QDomNode _parent)
{
    parent = _parent;
    xmlsetDictonary(); //Has to be first to set up locales
}

EmvConfiguration::EmvConfiguration(){}

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
