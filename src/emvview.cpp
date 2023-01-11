#include "emvview.h"
#include "emvMixer.h"
#include "emvEntity.h"

EmvView::EmvView(QWidget *parent)
	: QWidget(parent)
{
	setupUi(this);
	QObject::connect(addTabButton, SIGNAL(clicked()), this, SLOT(addMixer()));
	QObject::connect(openFilePushButton, SIGNAL(clicked()), this, SLOT(openFile()));
	QObject::connect(configurationPicker, SIGNAL(currentIndexChanged(int)), this, SLOT(resetView(false)));
	QObject::connect(configurationChangeButton, SIGNAL(clicked()), this, SLOT(changeConfigurationClicked()));
	tabWidget->removeTab(0);

	//EmvEntity myEntity = EmvEntity();
	
}

EmvView::~EmvView()
{}

void EmvView::addMixer()
{

	QString mixerName = getEntityName();
	
	QString mixerAmount = Ui_EmvViewClass::amount->text();
	std::string temp = mixerAmount.toStdString();
	int amount = stoi(temp);
	EmvMixer* newMixer = new EmvMixer(mixerName, amount, controlledEntityID);
	tabWidget->addTab(newMixer, mixerName);
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
	QString fileName = QFileDialog::getOpenFileName(this, "Open Entity XML", "C://");
	qCritical() << fileName << "FILENAME";

	//entity = EmvEntity(fileName, "EN");

	//entityLabel->text(entity.getEntityName);
}
