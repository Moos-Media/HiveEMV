#include "emvview.h"
#include "emvMixer.h"
#include <QXmlStreamReader>
#include <QFile>
#include <QDebug>
#include <QCoreApplication>
#include "emvEntity.h"
#include "emvFileHandler.h"
#include "entityInspector.hpp"
#include "emvSettings.h"


EmvView::EmvView(QWidget * parent)
	: QWidget(parent)
{
	setupUi(this);
	auto* const settings = qApp->property(settings::SettingsManager::PropertyName).value<settings::SettingsManager*>();

	int langSetting = settings->getValue("languagePreference").toInt();

	if (langSetting == 0)
		languageIdentifier = QLocale::system().name().mid(0, 2).toUpper();
	else if (langSetting == 1)
		languageIdentifier = "EN";
	else if (langSetting == 2)
		languageIdentifier == "DE";
	else if (langSetting == 3)
		languageIdentifier = "FR";
	else
		languageIdentifier = "EN";

	entityPickerIndex = -1;
	

	openFilePushButton->setEnabled(true);
	languagePicker->setCurrentIndex(langSetting);


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
	if (entityList.size() == 0)
		return;

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
	hive::modelsLibrary::ControllerManager::getInstance().setConfiguration(myEntity.getLaEntityID(), target);
	QThread::sleep(2); // Temp fix
}

void EmvView::openFile() {
	fileLocation = QFileDialog::getOpenFileName(this, "Open Entity XML", "C://", tr("AEMXML (*.aemxml)"));

	if (fileLocation.isEmpty())
		return;

	if (!fileLocations.contains(fileLocation))
	{
		fileLocations.append(fileLocation);
		int newIndex = entityList.size();
		entityList.insert(newIndex, EmvEntity(fileLocation, languageIdentifier));
		myEntity = entityList[newIndex];
		++entityPickerIndex;
	}

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

	auto* const settings = qApp->property(settings::SettingsManager::PropertyName).value<settings::SettingsManager*>();

	settings->setValue("languagePreference", langIndex);

	if (langIndex == 0)
		languageIdentifier = QLocale::system().name().mid(0, 2).toUpper();
	else if (langIndex == 1)
		languageIdentifier = "EN";
	else if (langIndex == 2)
		languageIdentifier == "DE";
	else if (langIndex == 3)
		languageIdentifier = "FR";
	else
		languageIdentifier = "EN";

	qDebug() << languageIdentifier;	

	for (int i = 0; i < entityList.size(); ++i)
	{
		auto currentEntity = entityList.value(i);
		currentEntity.changeLanguage(languageIdentifier);
	}

	if (fileLocation != "")
		setView();
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

void EmvView::controlledEntityChanged(la::avdecc::UniqueIdentifier _entityID)
{
	auto* const settings = qApp->property(settings::SettingsManager::PropertyName).value<settings::SettingsManager*>();

	bool doUpdate = settings->getValue("emvAutoChange").toBool();

	if (!doUpdate)
		return;

	for (int i = 0; i < entityList.size(); ++i)
	{
		auto currentEntity = entityList.value(i);

		if (currentEntity.getLaEntityID() == _entityID)
		{
			entityPicker->setCurrentIndex(i);
			break;
		}
	}
	
}
