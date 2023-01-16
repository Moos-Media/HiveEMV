#include "emvview.h"
#include "emvMixer.h"
#include <QXmlStreamReader>
#include <QFile>
#include <QDebug>
#include <QCoreApplication>
#include "emvEntity.h"


EmvView::EmvView(QWidget * parent)
	: QWidget(parent)
{
	isDebug = true;
	languageIdentifier = "EN";
	setupUi(this);

	if (isDebug)
	{
		openFilePushButton->setEnabled(true);
		languagePicker->setEnabled(true);
	}

	QObject::connect(openFilePushButton, SIGNAL(clicked()), this, SLOT(openFile()));
	//QObject::connect(configurationPicker, SIGNAL(currentIndexChanged(int)), this, SLOT(setView()));
	QObject::connect(configurationChangeButton, SIGNAL(clicked()), this, SLOT(changeConfigurationClicked()));
	QObject::connect(languagePicker, SIGNAL(currentIndexChanged(int)), this, SLOT(changeLanguage()));
	tabWidget->removeTab(0);
	
}

EmvView::~EmvView()
{}

void EmvView::addJacksViews() {
	//IN
	QString mixerName = "Input Jacks";
	QString type = "JACKSIN";

	if (myEntity.getCurrentConfiguration().getJackAmount("IN") != 0)
	{
		EmvMixer* newMixer = new EmvMixer(&myEntity, type, this);
		tabWidget->addTab(newMixer, mixerName);
	}

	//OUT
	mixerName = "Output Jacks";
	type = "JACKSOUT";

	if (myEntity.getCurrentConfiguration().getJackAmount("OUT") != 0)
	{
		EmvMixer* newMixer2 = new EmvMixer(&myEntity, type, this);
		tabWidget->addTab(newMixer2, mixerName);
	}
}

void EmvView::setControlledEntityID(la::avdecc::UniqueIdentifier const entityID)
{
	//Save Entity ID
	controlledEntityID = entityID;

	

	//Update Titles
	setWindowTitle(getEntityName());
	entityLabel->setText(getEntityName());

	//Update Window
	updateConfigurationPicker();

	myEntity = EmvEntity(controlledEntityID);
	EmvMixer* metaData = new EmvMixer(&myEntity, "METADATA", this);
	tabWidget->addTab(metaData, "Entity Information");
}

QString EmvView::getEntityName()
{
	auto& manager = hive::modelsLibrary::ControllerManager::getInstance();
	auto controlledEntity = manager.getControlledEntity(controlledEntityID);

	
	if (controlledEntity)
	{
		return hive::modelsLibrary::helper::smartEntityName(*controlledEntity);
	}
	else
	{
		return "";
	}
}

void EmvView::updateConfigurationPicker()
{
	// Removing Items from Selection
	configurationPicker->removeItem(0);
	configurationPicker->removeItem(0);

	//Adding test configs
	configurationPicker->addItem("Null");
	configurationPicker->addItem("Eins");
	configurationPicker->addItem("Zwei");

	setView();
}

void EmvView::setView()
{
	int tabAmount = tabWidget->count();
	for (int i = 0; i < tabAmount; i++)
	{
		tabWidget->removeTab(0);
	}

	//Change Label
	QString labelText = "";
	labelText = labelText.append(myEntity.getVendorName()).append(" ").append(myEntity.getEntityName());
	entityLabel->setText(labelText);

	//Add Configs to Picker
	configurationPicker->clear();
	for (int i = 0; i < myEntity.getConfigurationCount(); ++i)
	{
		configurationPicker->addItem(myEntity.getConfigurationDescription(i));
	}

	//Add Metadata Tab
	EmvMixer* metaData = new EmvMixer(&myEntity, "METADATA", this);
	tabWidget->addTab(metaData, "Entity Information");

	//Add Config Controls Tab
	EmvMixer* configControls = new EmvMixer(&myEntity, "CONFIGCONTROLS", this);
	tabWidget->addTab(configControls, "Configuration Controls");

	//Add Jacks Tabs
	addJacksViews();
}

void EmvView::changeConfigurationClicked() {
	//TODO:: Change Config

	setView();
}

void EmvView::openFile() {
	fileLocation = QFileDialog::getOpenFileName(this, "Open Entity XML", "G://Meine Ablage/__Studium/9. Semester/Bachelorarbeit/Models");
	//fileLocation = "G:/Meine Ablage/__Studium/9. Semester/Bachelorarbeit/Models/12mic.aemxml";

	// Get Entity
	myEntity = EmvEntity(fileLocation, languageIdentifier);

	qDebug() << myEntity.getLocale(0, 2);
	setView();
}

void EmvView::addMixer() {}

void EmvView::changeLanguage() {
	//Get current Index of selected Language
	int langIndex = languagePicker->currentIndex();

	if (langIndex == 0)
		languageIdentifier = "EN";
	else if (langIndex == 1)
		languageIdentifier = "DE";
	else if (langIndex == 2)
		languageIdentifier = "FR";
	else
		languageIdentifier = "EN";

	
	myEntity.changeLanguage(languageIdentifier);
	qDebug() << languageIdentifier << "Lang identifier";

	if (fileLocation != "")
		setView();
	qDebug() << myEntity.language << "Lang im Entity";
}

void EmvView::setDebug(bool _isDebug) {
	isDebug = _isDebug;

	if (!isDebug)
	{
		openFilePushButton->setEnabled(false);
		languagePicker->setEnabled(false);
	}
		
}
