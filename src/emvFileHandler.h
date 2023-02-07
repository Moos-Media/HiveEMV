#include <QtXml>

class EmvFileHandler {

public:
	EmvFileHandler(QString _filepath, QString _language = "en");


	QDomElement root;
	QString language;

	//Setters
	

private:
	QString filepath;
	QDomDocument document;
	bool isVirtual;

	QMap<QString, QMap<int, QMap<int, QString>>> allLocales;
	QMap<int, QMap<int, QString>> localeSet;
	QMap<int, QString> localeTemp;


	la::avdecc::UniqueIdentifier entityID;
	la::avdecc::controller::ControlledEntityGuard controlledEntity;
	//la::avdecc::entity::Entity actEntity;

	void setControlledEntity();

	void setLanguage(QString _lang);
	void getDictionary(int index);
	QDomElement xmlGetRootFromFile(QString _filename);
	QString xmlReadStringFromNode(QDomElement node);

	QString getLocale(int index1, int index2);
};

