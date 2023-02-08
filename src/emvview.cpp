#include "emvview.h"
#include "emvMixer.h"
#include <QXmlStreamReader>
#include <QFile>
#include <QDebug>
#include <QCoreApplication>
#include "emvEntity.h"
#include "emvFileHandler.h"
#include "emvSettings.h"


EmvView::EmvView(QWidget * parent)
	: QWidget(parent)
{
	isDebug = true;
	languageIdentifier = "EN";
	entityPickerIndex = -1;
	setupUi(this);

	if (isDebug)
	{
		openFilePushButton->setEnabled(true);
		languagePicker->setEnabled(true);
	}

	QObject::connect(openFilePushButton, SIGNAL(clicked()), this, SLOT(openFile()));
	QObject::connect(openSettingsPushButton, SIGNAL(clicked()), this, SLOT(openSettings()));
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

	myEntity = EmvEntity(controlledEntityID);
	EmvMixer* metaData = new EmvMixer(&myEntity, "METADATA", this);
	tabWidget->addTab(metaData, "Entity Information");

	//Update Titles
	setWindowTitle(getEntityName());
	entityLabel->setText(getEntityName());

	//Update Window
	setView();
	
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
	//Add Configs to Picker
	configurationPicker->clear();
	for (int i = 0; i < myEntity.getConfigurationCount(); ++i)
	{
		if (isDebug)
			configurationPicker->addItem(myEntity.getConfigurationDescription(i));
		else
			configurationPicker->addItem(myEntity.getConfiguration(i).getConfigName());
	}
	configurationPicker->setCurrentIndex(myEntity.getCurrentConfigurationIndex());
}

void EmvView::setView()
{
	//Update Entity Picker
	QObject::disconnect(entityPicker, SIGNAL(currentIndexChanged(int)), this, SLOT(changeEntity()));
	
	entityPicker->clear();

	for (int i = 0; i < entityList.size(); ++i)
	{
		entityPicker->addItem(entityList[i].getEntityName());
	}
	entityPicker->setCurrentIndex(entityPickerIndex);
	QObject::connect(entityPicker, SIGNAL(currentIndexChanged(int)), this, SLOT(changeEntity()));

	//Save current State of Tab Widget
	int oldIndex = tabWidget->currentIndex();

	//Block Config Change button
	if (myEntity.getConfigurationCount() > 1)
		configurationChangeButton->setEnabled(true);

	//Clear old Tabs
	tabWidget->clear();	

	//Change Label
	QString labelText = "";
	labelText = labelText.append(myEntity.getVendorName()).append(" ").append(myEntity.getEntityName());
	entityLabel->setText(labelText);

	updateConfigurationPicker();

	//Add Metadata Tab
	EmvMixer* metaData = new EmvMixer(&myEntity, "METADATA", this);
	tabWidget->addTab(metaData, "Entity Information");

	//Add Config Controls Tab
	EmvMixer* configControls = new EmvMixer(&myEntity, "CONFIGCONTROLS", this);
	tabWidget->addTab(configControls, "Configuration Controls");

	//Add Jacks Tabs
	addJacksViews();

	//Add Channels Tab
	EmvMixer* mixerControls = new EmvMixer(&myEntity, "MIXER", this);
	tabWidget->addTab(mixerControls, "Channels view");

	//Get back to old Tab
	tabWidget->setCurrentIndex(oldIndex);
}

void EmvView::changeConfigurationClicked() {

	int target = configurationPicker->currentIndex();
	hive::modelsLibrary::ControllerManager::getInstance().setConfiguration(controlledEntityID, target);
	QThread::sleep(2); // Temp fix
	setControlledEntityID(controlledEntityID);
}

void EmvView::openFile() {
	fileLocation = QFileDialog::getOpenFileName(this, "Open Entity XML", "G://Meine Ablage/__Studium/9. Semester/Bachelorarbeit");
	//fileLocation = "G:/Meine Ablage/__Studium/9. Semester/Bachelorarbeit/Models/12mic.aemxml";

	if (!fileLocations.contains(fileLocation))
	{
		fileLocations.append(fileLocation);
		int newIndex = entityList.size();
		entityList.insert(newIndex, EmvEntity(fileLocation, languageIdentifier));
		myEntity = entityList[newIndex];
		++entityPickerIndex;
	}

	//EmvFileHandler myHandler = EmvFileHandler(fileLocation, languageIdentifier);

	setView();
}

void EmvView::openSettings() {
	if (!settingsAreOpen)
	{
		emvSettings* emvSettingsWindow = new emvSettings();
		emvSettingsWindow->setWindowTitle("Settings");
		emvSettingsWindow->show();
		settingsAreOpen = true;
		QObject::connect(emvSettingsWindow, SIGNAL(finished(int)), this, SLOT(settingsFinished(int)));
		QObject::connect(emvSettingsWindow, SIGNAL(settingsChanged()), this, SLOT(setView()));
	}
}

void EmvView::settingsFinished(int resultCode) {
	settingsAreOpen = false;
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

void EmvView::changeEntity() {
	entityPickerIndex = entityPicker->currentIndex();
	myEntity = entityList[entityPickerIndex];

	setView();
}
