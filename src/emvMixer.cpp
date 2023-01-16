#include "emvMixer.h"
#include "emvUtils.h"



EmvMixer::EmvMixer(EmvEntity *entity, QString type, QWidget *parent)
	: QWidget(parent)
{
	setupUi(this);

	// Setup Header Styling
	headerStyle = "font-weight: bold; font-size: 8pt; text-align: center;";

	myEntity = entity;

	if (type.toUpper() == "METADATA")
		addMetaData();
	else if (type.toUpper() == "JACKSIN")
		addJacks("IN");
	else if (type.toUpper() == "JACKSOUT")
		addJacks("OUT");
	else if (type.toUpper() == "CONFIGCONTROLS")
		addConfigurationControls();
	
}

EmvMixer::~EmvMixer()
{}


//SLOTS------------------------------------------------------------------------------------------------------

void EmvMixer::updateGainDial()
{
	int amount = 13;
	QDial* changedDial = qobject_cast<QDial*>(sender());
	int value = changedDial->value();
	int index = mixerArea->indexOf(changedDial);
	int channel = (index - (index % amount)) / amount;

	int gain = 0;
	if (padCheckBoxes[0][channel]->isChecked())
		gain = gain - 26;

	gain = gain + ownMap(value, 0, 99, 20, 64);

	string output = to_string(gain);
	gainLabels[0][channel]->setText(QString::fromStdString(output));
}
void EmvMixer::updateGainPad()
{
	int amount = 13;
	QCheckBox* changedCheckBox = qobject_cast<QCheckBox*>(sender());
	int index = mixerArea->indexOf(changedCheckBox);
	int channel = (index - (index % amount)) / amount;

	//Get old dial position
	int value = gainDials[0][channel]->value();
	//Trigger Update on dial with old value. Will take pad into account.
	gainDials[0][channel]->valueChanged(value);
}
void EmvMixer::updatePanDial()
{
	int amount = 13;
	int snappingRange = 5;
	QDial* changedDial = qobject_cast<QDial*>(sender());
	int value = changedDial->value();
	int index = mixerArea->indexOf(changedDial);
	int channel = (index - (index % amount)) / amount;

	//Snap to center
	if (50 - snappingRange < value && value < 50 + snappingRange)
	{
		value = 50;
		//panDials[0][channel]->setV(value);
		//TODO Snapping is not working correctly.
	}

	//Update Label
	QString labelText;
	labelText = QString::fromStdString(panMapping(value));
	panLabels[0][channel]->setText(labelText);
}
void EmvMixer::updatePanReset()
{
	int amount = 13;
	QPushButton* pushedButton = qobject_cast<QPushButton*>(sender());
	int index = mixerArea->indexOf(pushedButton);
	int channel = (index - (index % amount)) / amount;

	panDials[0][channel]->setValue(50);
}
void EmvMixer::updateLevelsSlider()
{
	int amount = 13;
	QSlider* changedSlider = qobject_cast<QSlider*>(sender());
	int index = mixerArea->indexOf(changedSlider);
	int value = changedSlider->value();
	int channel = (index - (index % amount)) / amount;

	value = ownMap(value, 0, 99, -60, 10);
	string output = to_string(value);
	levelsLabels[0][channel]->setText(QString::fromStdString(output));
}
void EmvMixer::updateLevelsMute()
{
	QString muted = "color: red; font-weight: bold;";
	QString unmuted = "color: black;";

	int amount = 13;
	QCheckBox* checkedBox = qobject_cast<QCheckBox*>(sender());
	int index = mixerArea->indexOf(checkedBox);
	int channel = (index - (index % amount)) / amount;

	if (checkedBox->isChecked())
	{
		levelsLabels[0][channel]->setText("Mute");
		levelsLabels[0][channel]->setStyleSheet(muted);
	}
	if (!checkedBox->isChecked())
	{
		int value = levelsSliders[0][channel]->value();
		levelsSliders[0][channel]->valueChanged(value);
		levelsLabels[0][channel]->setStyleSheet(unmuted);
	}
}

//HELPERS--------------------------------------------------------------------------------------------------------

void EmvMixer::addChannels()
{
	auto& manager = hive::modelsLibrary::ControllerManager::getInstance();
	auto controlledEntity = manager.getControlledEntity(controlledEntityID);
	//auto index = la::avdecc::entity::model::control

	//manager.setControlValues(controlledEntityID, 0, 0);


	for (int i = 0; i < channelAmount; i++)
	{
		QString headerStyle = "font-weight: bold; font-size: 12pt; text-align: center;";
		int newColumn = mixerArea->columnCount();
		int channelIndex = newColumn - 1;
		int mixerIndex = 0;


		//Gain
		QLabel* gainHeader = new QLabel("Gain");
		gainHeader->setTextFormat(Qt::RichText);
		gainHeader->setStyleSheet(headerStyle);
		gainHeader->setAlignment(Qt::AlignHCenter);
		mixerArea->addWidget(gainHeader, 0, newColumn);

		QCheckBox* phantomPowerCheckBox = new QCheckBox("+48V");
		mixerArea->addWidget(phantomPowerCheckBox, 1, newColumn);
		phantomPowerBoxes[mixerIndex][channelIndex] = phantomPowerCheckBox;


		QLabel* gainLabel = new QLabel("20");
		gainLabel->setAlignment(Qt::AlignHCenter);
		mixerArea->addWidget(gainLabel, 2, newColumn);
		gainLabels[mixerIndex][channelIndex] = gainLabel;

		QDial* gainDial = new QDial();
		QObject::connect(gainDial, SIGNAL(valueChanged(int)), this, SLOT(updateGainDial()));
		gainDial->setMaximumHeight(50);
		mixerArea->addWidget(gainDial, 3, newColumn);
		gainDials[mixerIndex][channelIndex] = gainDial;

		QCheckBox* padCheckBox = new QCheckBox("Pad");
		QObject::connect(padCheckBox, SIGNAL(stateChanged(int)), this, SLOT(updateGainPad()));
		mixerArea->addWidget(padCheckBox, 4, newColumn);
		padCheckBoxes[mixerIndex][channelIndex] = padCheckBox;


		//Pan
		QLabel* panHeader = new QLabel("Pan");
		panHeader->setTextFormat(Qt::RichText);
		panHeader->setStyleSheet(headerStyle);
		panHeader->setAlignment(Qt::AlignHCenter);
		mixerArea->addWidget(panHeader, 6, newColumn);

		QLabel* panLabel = new QLabel("C");
		panLabel->setAlignment(Qt::AlignHCenter);
		mixerArea->addWidget(panLabel, 7, newColumn);
		panLabels[mixerIndex][channelIndex] = panLabel;

		QDial* panDial = new QDial();
		QObject::connect(panDial, SIGNAL(valueChanged(int)), this, SLOT(updatePanDial()));
		panDial->setMaximumHeight(50);
		panDial->setMaximum(100);
		panDial->setValue(50);
		mixerArea->addWidget(panDial, 8, newColumn);
		panDials[mixerIndex][channelIndex] = panDial;

		QPushButton* resetButton = new QPushButton("Reset");
		QObject::connect(resetButton, SIGNAL(clicked()), this, SLOT(updatePanReset()));
		mixerArea->addWidget(resetButton, 9, newColumn);
		panResetButtons[mixerIndex][channelIndex] = resetButton;


		//Levels
		QLabel* levelsHeader = new QLabel("Levels");
		levelsHeader->setTextFormat(Qt::RichText);
		levelsHeader->setStyleSheet(headerStyle);
		levelsHeader->setAlignment(Qt::AlignHCenter);
		mixerArea->addWidget(levelsHeader, 11, newColumn);

		QLabel* levelsLabel = new QLabel("0");
		levelsLabel->setAlignment(Qt::AlignHCenter);
		mixerArea->addWidget(levelsLabel, 12, newColumn);
		levelsLabels[mixerIndex][channelIndex] = levelsLabel;

		QSlider* levelsSlider = new QSlider();
		QObject::connect(levelsSlider, SIGNAL(valueChanged(int)), this, SLOT(updateLevelsSlider()));
		mixerArea->addWidget(levelsSlider, 13, newColumn);
		levelsSliders[mixerIndex][channelIndex] = levelsSlider;

		QCheckBox* muteCheckBox = new QCheckBox("Mute");
		QObject::connect(muteCheckBox, SIGNAL(stateChanged(int)), this, SLOT(updateLevelsMute()));
		mixerArea->addWidget(muteCheckBox, 14, newColumn);
		muteCheckBoxes[mixerIndex][channelIndex] = muteCheckBox;
	}
}

void EmvMixer::addJacks(QString _dir) {	
	for (int i = 0; i < myEntity->getCurrentConfiguration().getJackAmount(_dir.toUpper()); i++)
	{
		int newRow = 0;
		int newColumn = mixerArea->columnCount();
		EmvJack currentJack = myEntity->getCurrentConfiguration().getJack(_dir.toUpper(), i);

		//----------------------------------------------------------------- Jack Title
		QString jackLabel = myEntity->getLocale(currentJack.descriptionIndex[0], currentJack.descriptionIndex[1]);
		QLabel* jackHeader = new QLabel(jackLabel);
		jackHeader->setTextFormat(Qt::RichText);
		jackHeader->setStyleSheet(headerStyle);
		jackHeader->setAlignment(Qt::AlignHCenter);
		mixerArea->addWidget(jackHeader, newRow, newColumn, Qt::AlignTop);
		++newRow;

		//----------------------------------------------------------------- Jack Image
		QLabel* picture = new QLabel();
		QPixmap myMap;

		//Get Jack Image
		switch (currentJack.type)
		{
			case (0):
				myMap = QPixmap(":/jack_speaker");
				break;
			case (1):
				myMap = QPixmap(":/jack_headphone");
				break;
			case (2):
				myMap = QPixmap(":/jack_microphone");
				break;
			case (3):
				myMap = QPixmap(":/jack_spdif");
				break;
			case (4):
				myMap = QPixmap(":/jack_adat");
				break;
			case (5):
				myMap = QPixmap(":/jack_tdif");
				break;
			case (6):
				myMap = QPixmap(":/jack_madi");
				break;
			case (7):
				myMap = QPixmap(":/jack_unbalanced");
				break;
			case (8):
				myMap = QPixmap(":/jack_balanced");
				break;
			case (9):
				myMap = QPixmap(":/jack_digital");
				break;
			case (10):
				myMap = QPixmap(":/jack_midi");
				break;
			case (11):
				myMap = QPixmap(":/jack_aesebu");
				break;
			case (12):
				myMap = QPixmap(":/jack_composite");
				break;
			case (13):
				myMap = QPixmap(":/jack_svhs");
				break;
			case (14):
				myMap = QPixmap(":/jack_component");
				break;
			case (15):
				myMap = QPixmap(":/jack_dvi");
				break;
			case (16):
				myMap = QPixmap(":/jack_hdmi");
				break;
			case (17):
				myMap = QPixmap(":/jack_udi");
				break;
			case (18):
				myMap = QPixmap(":/jack_displayport");
				break;
			case (19):
				myMap = QPixmap(":/jack_antenna");
				break;
			case (20):
				myMap = QPixmap(":/jack_tuner");
				break;
			case (21):
				myMap = QPixmap(":/jack_ethernet");
				break;
			case (22):
				myMap = QPixmap(":/jack_wifi");
				break;
			case (23):
				myMap = QPixmap(":/jack_usb");
				break;
			default:
				myMap = QPixmap(":/default");
				break;
		}

		//Add Image to Area
		picture->setPixmap(myMap.scaledToWidth(50));
		picture->setAlignment(Qt::AlignHCenter);
		mixerArea->addWidget(picture, newRow, newColumn, Qt::AlignTop);
		++newRow;

		//----------------------------------------------------------------- Jack Flags

		bool temp;
		QString flagtext;
		switch (currentJack.flags.toInt(&temp, 16))
		{
			case (1):
				flagtext = "Is Clock Source";
				break;
			case (2):
				flagtext = "Is Captive";
				break;
			default:
				flagtext = "No flags";
				break;
		}

		QLabel* jackFlags = new QLabel(flagtext);
		jackFlags->setAlignment(Qt::AlignHCenter);
		mixerArea->addWidget(jackFlags, newRow, newColumn, Qt::AlignTop);
		++newRow;

		//----------------------------------------------------------------- Jack Controls

		for (int j = 0; j < currentJack.controlsCount; ++j)
		{
			auto currentControl = currentJack.controls[j];

			int indexx = currentControl.descriptionIndex[0];
			int indexy = currentControl.descriptionIndex[1];
			mixerArea->addWidget(new QLabel(myEntity->getLocale(indexx, indexy)), newRow, newColumn);
			++newRow;
			addControlsToPage(currentControl.controlTypeIndex, newRow, newColumn);
			++newRow;
		}

		//----------------------------------------------------------------- Port Controls

		auto portList = myEntity->getCurrentConfiguration().getAudioUnit(0).getExternalPorts(_dir);

		for (int j = 0; j < portList.size(); ++j)
		{
			auto currentPort = portList[j];

			if (currentPort.jackIndex == i)
			{
				for (int z = 0; z < currentPort.controlsCount; ++z)
				{
					int indexx = currentPort.controls[z].descriptionIndex[0];
					int indexy = currentPort.controls[z].descriptionIndex[1];
					mixerArea->addWidget(new QLabel(myEntity->getLocale(indexx, indexy)), newRow, newColumn);
					++newRow;
					addControlsToPage(currentPort.controls[z].controlTypeIndex, newRow, newColumn);
					++newRow;
				}
			}
		}
	}
}

void EmvMixer::addMetaData() {
	//Create all Labels
	QLabel *entityID = new QLabel();
	QLabel *entityIDHeader = new QLabel();
	QLabel* entityModelID = new QLabel();
	QLabel* entityModelIDHeader = new QLabel();
	QLabel* entityCapabilities = new QLabel();
	QLabel* entityCapabilitiesHeader = new QLabel();
	QLabel* streamMaxSources = new QLabel();
	QLabel* streamMaxSourcesHeader = new QLabel();
	QLabel* talkerCapabilities = new QLabel();
	QLabel* talkerCapabilitiesHeader = new QLabel();
	QLabel* streamMaxSinks = new QLabel();
	QLabel* streamMaxSinksHeader = new QLabel();
	QLabel* listenerCapabilities = new QLabel();
	QLabel* listenerCapabilitiesHeader = new QLabel();
	QLabel* controllerCapabilites = new QLabel();
	QLabel* controllerCapabilitesHeader = new QLabel();
	QLabel* associationID = new QLabel();
	QLabel* associationIDHeader = new QLabel();
	QLabel* entityName = new QLabel();
	QLabel* entityNameHeader = new QLabel();
	QLabel* vendorName = new QLabel();
	QLabel* vendorNameHeader = new QLabel();
	QLabel* modelName = new QLabel();
	QLabel* modelNameHeader = new QLabel();
	QLabel* firmwareVersion = new QLabel();
	QLabel* firmwareVersionHeader = new QLabel();
	QLabel* groupName = new QLabel();
	QLabel* groupNameHeader = new QLabel();
	QLabel* serialNumber = new QLabel();
	QLabel* serialNumberHeader = new QLabel();
	QLabel* currentConfig = new QLabel();
	QLabel* currentConfigHeader = new QLabel();
	QStringList capabilites;
	QString outputText;


	//Set Data Labels
	entityID->setText(myEntity->getEntityID());
	entityModelID->setText(myEntity->getEntityModelID());
	entityCapabilities->setText(myEntity->getEntityCapabilities());
	streamMaxSources->setText(QString::number(myEntity->getMaxStreamSources()));

	capabilites = myEntity->getTalkerCapabilities();
	outputText.append("Can be Source of: ");

	for (int i = 0; i < capabilites.size(); ++i)
	{
		outputText.append(capabilites.at(i)).append(", ");
	}
	outputText.chop(2);
	outputText.append(" Streams");
	talkerCapabilities->setText(outputText);

	streamMaxSinks->setText(QString::number(myEntity->getMaxStreamSinks()));

	capabilites = myEntity->getListenerCapabilities();
	outputText = "";
	outputText.append("Can be sink of: ");

	for (int i = 0; i < capabilites.size(); ++i)
	{
		outputText.append(capabilites.at(i)).append(", ");
	}
	outputText.chop(2);
	outputText.append(" Streams");

	listenerCapabilities->setText(outputText);

	if (myEntity->canBeController())
		controllerCapabilites->setText("Can be controller");
	else
		controllerCapabilites->setText("Can't be controller");
	associationID->setText(myEntity->getAssociationID());
	entityName->setText(myEntity->getEntityName());
	vendorName->setText(myEntity->getVendorName());
	modelName->setText(myEntity->getModelName());
	firmwareVersion->setText(myEntity->getFirmwareVersion());
	groupName->setText(myEntity->getGroupName());
	serialNumber->setText(myEntity->getSerialNumber());
	currentConfig->setText(myEntity->getConfigurationDescription(myEntity->getCurrentConfigurationIndex()));

	//Set Header Labels
	entityIDHeader->setText("Entity ID:");
	entityModelIDHeader->setText("Entity Model ID:");
	entityCapabilitiesHeader->setText("Entity Capabilities:");
	streamMaxSourcesHeader->setText("Maximum number of Stream Sources:");
	talkerCapabilitiesHeader->setText("Talker Capabilities:");
	streamMaxSinksHeader->setText("Maximum number of Stream Sinks:");
	listenerCapabilitiesHeader->setText("Listener Capabilites:");
	controllerCapabilitesHeader->setText("Controller Capabilities:");
	associationIDHeader->setText("Association ID:");
	entityNameHeader->setText("Entity Name:");
	vendorNameHeader->setText("Vendor Name:");
	modelNameHeader->setText("Model Name:");
	firmwareVersionHeader->setText("Firmware Version:");
	groupNameHeader->setText("Group Name:");
	serialNumberHeader->setText("Serial Number:");
	currentConfigHeader->setText("Currently Selected Configuration");

	//Set Styling
	headerStyle = "font-weight: bold; font-size: 8pt;";
	entityIDHeader->setStyleSheet(headerStyle);
	entityModelIDHeader->setStyleSheet(headerStyle);
	entityCapabilitiesHeader->setStyleSheet(headerStyle);
	streamMaxSourcesHeader->setStyleSheet(headerStyle);
	talkerCapabilitiesHeader->setStyleSheet(headerStyle);
	streamMaxSinksHeader->setStyleSheet(headerStyle);;
	listenerCapabilitiesHeader->setStyleSheet(headerStyle);
	controllerCapabilitesHeader->setStyleSheet(headerStyle);
	associationIDHeader->setStyleSheet(headerStyle);
	entityNameHeader->setStyleSheet(headerStyle);
	vendorNameHeader->setStyleSheet(headerStyle);
	modelNameHeader->setStyleSheet(headerStyle);
	firmwareVersionHeader->setStyleSheet(headerStyle);
	groupNameHeader->setStyleSheet(headerStyle);
	serialNumberHeader->setStyleSheet(headerStyle);
	currentConfigHeader->setStyleSheet(headerStyle);

	//Add Labels to Area
	int row = 0;

	mixerArea->addWidget(entityIDHeader, row, 0);
	mixerArea->addWidget(entityID, row, 1);
	++row;

	mixerArea->addWidget(entityModelIDHeader, row, 0);
	mixerArea->addWidget(entityModelID, row, 1);
	++row;

	mixerArea->addWidget(entityCapabilitiesHeader, row, 0);
	mixerArea->addWidget(entityCapabilities, row, 1);
	++row;

	mixerArea->addWidget(streamMaxSourcesHeader, row, 0);
	mixerArea->addWidget(streamMaxSources, row, 1);
	++row;

	mixerArea->addWidget(talkerCapabilitiesHeader, row, 0);
	mixerArea->addWidget(talkerCapabilities, row, 1);
	++row;

	mixerArea->addWidget(streamMaxSinksHeader, row, 0);
	mixerArea->addWidget(streamMaxSinks, row, 1);
	++row;

	mixerArea->addWidget(listenerCapabilitiesHeader, row, 0);
	mixerArea->addWidget(listenerCapabilities, row, 1);
	++row;

	mixerArea->addWidget(controllerCapabilitesHeader, row, 0);
	mixerArea->addWidget(controllerCapabilites, row, 1);
	++row;

	mixerArea->addWidget(associationIDHeader, row, 0);
	mixerArea->addWidget(associationID, row, 1);
	++row;

	mixerArea->addWidget(entityNameHeader, row, 0);
	mixerArea->addWidget(entityName, row, 1);
	++row;

	mixerArea->addWidget(vendorNameHeader, row, 0);
	mixerArea->addWidget(vendorName, row, 1);
	++row;

	mixerArea->addWidget(modelNameHeader, row, 0);
	mixerArea->addWidget(modelName, row, 1);
	++row;

	mixerArea->addWidget(firmwareVersionHeader, row, 0);
	mixerArea->addWidget(firmwareVersion, row, 1);
	++row;

	mixerArea->addWidget(serialNumberHeader, row, 0);
	mixerArea->addWidget(serialNumber, row, 1);
	++row;

	mixerArea->addWidget(currentConfigHeader, row, 0);
	mixerArea->addWidget(currentConfig, row, 1);
}

void EmvMixer::addConfigurationControls() {

	//Get Amount of Controls to display
	EmvConfiguration current = myEntity->getCurrentConfiguration();
	int amount = current.getControlsAmount();

	for (int i = 0; i < amount; ++i)
	{
		int newColumn = mixerArea->columnCount();

		int index1 = current.getControl(i).descriptionIndex[0];
		int index2 = current.getControl(i).descriptionIndex[1];
		QString labelText = myEntity->getLocale(index1, index2);

		QLabel* descLabel = new QLabel(labelText);
		descLabel->setStyleSheet(headerStyle);
		mixerArea->addWidget(descLabel, 0, newColumn);

		addControlsToPage(current.getControl(i).controlTypeIndex, 1, newColumn);
	}
}

void EmvMixer::addControlsToPage(int index, int row, int column) {
	QCheckBox *enable = new QCheckBox();
	QPushButton* identify = new QPushButton("Identify");
	QCheckBox *mute = new QCheckBox();
	QCheckBox *invert = new QCheckBox();
	QDial *gain = new QDial();
	QDial *attenuate = new QDial();
	QDial *delay = new QDial();
	QDial *srcMode = new QDial();
	QPushButton* snapshot = new QPushButton("Snapshot");
	QPushButton* pwrFrequency = new QPushButton("Power Line Frequency");
	QPushButton* pwrStatus = new QPushButton("Power Status");
	QPushButton* fanStatus = new QPushButton("Fan Status");
	QPushButton* temp = new QPushButton("Temperatures");
	QPushButton* alt = new QPushButton("Altitude");
	QPushButton* relHum = new QPushButton("Relative Humidity");
	QPushButton* absHum = new QPushButton("Absolute Humidity");
	QPushButton* orientation = new QPushButton("Orientation");
	QPushButton* velocity = new QPushButton("Velocity");
	QPushButton* acceleration = new QPushButton("Acceleration");
	QPushButton* filter = new QPushButton("Filter Response");
	QPushButton* pressure = new QPushButton("Barometric Pressure");
	QPushButton* manURL = new QPushButton("Manufacturer URL");
	QPushButton* entityURL = new QPushButton("Entity URL");
	QPushButton* configURL = new QPushButton("Configuration URL");
	QPushButton* genURL = new QPushButton("Generic URL");
	QPushButton* fault = new QPushButton("Fault State");
	QPushButton* targetEntity = new QPushButton("Target Entity");
	QPushButton* targetObject = new QPushButton("Target Object");
	QPushButton* latency = new QPushButton("Latency Compensation");
	QDial *panDial = new QDial();
	QCheckBox *phantomPower = new QCheckBox();

	if (index == 0)
		mixerArea->addWidget(enable, row, column);
	else if(index == 1)
		mixerArea->addWidget(identify, row, column);
	else if(index == 2)
		mixerArea->addWidget(mute, row, column);
	else if(index == 3)
		mixerArea->addWidget(invert, row, column);
	else if(index == 4)
		mixerArea->addWidget(gain, row, column);
	else if(index == 5)
		mixerArea->addWidget(attenuate, row, column);
	else if(index == 6)
		mixerArea->addWidget(delay, row, column);
	else if(index == 7)
		mixerArea->addWidget(srcMode, row, column);
	else if(index == 8)
		mixerArea->addWidget(snapshot, row, column);
	else if(index == 9)
		mixerArea->addWidget(pwrFrequency, row, column);
	else if(index == 10)
		mixerArea->addWidget(pwrStatus, row, column);
	else if(index == 11)
		mixerArea->addWidget(fanStatus, row, column);
	else if(index == 12)
		mixerArea->addWidget(temp, row, column);
	else if(index == 13)
		mixerArea->addWidget(alt, row, column);
	else if(index == 14)
		mixerArea->addWidget(absHum, row, column);
	else if(index == 15)
		mixerArea->addWidget(relHum, row, column);
	else if(index == 16)
		mixerArea->addWidget(orientation, row, column);
	else if(index == 17)
		mixerArea->addWidget(velocity, row, column);
	else if(index == 18)
		mixerArea->addWidget(acceleration, row, column);
	else if(index == 19)
		mixerArea->addWidget(filter, row, column);
	else if(index == 20)
		mixerArea->addWidget(pressure, row, column);
	else if(index == 21)
		mixerArea->addWidget(manURL, row, column);
	else if(index == 22)
		mixerArea->addWidget(entityURL, row, column);
	else if(index == 23)
		mixerArea->addWidget(configURL, row, column);
	else if(index == 24)
		mixerArea->addWidget(genURL, row, column);
	else if(index == 25)
		mixerArea->addWidget(fault, row, column);
	else if(index == 26)
		mixerArea->addWidget(targetEntity, row, column);
	else if(index == 27)
		mixerArea->addWidget(targetObject, row, column);
	else if(index == 28)//---------------------------------------------
		mixerArea->addWidget(panDial, row, column);
	else if(index == 30)
		mixerArea->addWidget(phantomPower, row, column);
	else
		mixerArea->addWidget(new QLabel("Control not found"), row, column);

}
