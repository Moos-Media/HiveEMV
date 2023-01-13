#include "emvview.h"
#include "emvMixer.h"
#include <QXmlStreamReader>
#include <QFile>
#include <QDebug>
#include <QCoreApplication>
#include "emvEntity.h"


EmvView::EmvView(bool _isDebug, QWidget * parent)
	: QWidget(parent)
{
	isDebug = _isDebug;
	setupUi(this);

	if (isDebug)
	{
		openFilePushButton->setEnabled(true);	
	}

	QObject::connect(addTabButton, SIGNAL(clicked()), this, SLOT(addMixer()));
	QObject::connect(openFilePushButton, SIGNAL(clicked()), this, SLOT(openFile()));
	QObject::connect(configurationPicker, SIGNAL(currentIndexChanged(int)), this, SLOT(resetView(false)));
	QObject::connect(configurationChangeButton, SIGNAL(clicked()), this, SLOT(changeConfigurationClicked()));
	tabWidget->removeTab(0);
	
}

EmvView::~EmvView()
{}

void EmvView::addMixer()
{

	QString mixerName = getEntityName();
	
	QString mixerAmount = Ui_EmvViewClass::amount->text();
	std::string temp = mixerAmount.toStdString();
	int amount = stoi(temp);
	//EmvMixer* newMixer = new EmvMixer(mixerName, amount, controlledEntityID);
	//tabWidget->addTab(newMixer, mixerName);
}

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

	resetView(true);
}

void EmvView::resetView(bool shownew)
{
	int tabAmount = tabWidget->count();
	for (int i = 0; i < tabAmount; i++)
	{
		tabWidget->removeTab(0);
	}
	if (shownew)
	{
		int currentSelection = configurationPicker->currentIndex();

		for (int i = 0; i < currentSelection; i++)
		{
			addMixer();
		}
	}
	
}

void EmvView::changeConfigurationClicked() {
	//TODO:: Change Config

	resetView(true);
}

void EmvView::openFile() {
	QString fileName = QFileDialog::getOpenFileName(this, "Open Entity XML", "G://Meine Ablage/__Studium/9. Semester/Bachelorarbeit/Models");
	//QString fileName = "G:/Meine Ablage/__Studium/9. Semester/Bachelorarbeit/Models/12mic.aemxml";

	// Get Entity
	myEntity = EmvEntity(fileName, "DE");

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

	

	EmvMixer* metaData = new EmvMixer(&myEntity, "METADATA", this);
	tabWidget->addTab(metaData, "Entity Information");

	addJacksViews();

	for (int i = 0; i < 30; ++i)
	{
		int index1 = myEntity.getCurrentConfiguration().getAudioUnit(0).controls[i].descriptionIndex[0];
		int index2 = myEntity.getCurrentConfiguration().getAudioUnit(0).controls[i].descriptionIndex[1];
		qDebug() << myEntity.getLocale(index1, index2) << " control " << i;
	}
	qDebug() << myEntity.getCurrentConfiguration().getAudioUnit(0).controlsCount << "controls count";

	//qDebug() << myEntity.getConfiguration(0).getControl(0).values[0].type;
	//qDebug() << myEntity.getConfiguration(myEntity.getCurrentConfiguration()).getControl(0).descriptionIndex[0;
}
