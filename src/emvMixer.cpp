#include "emvMixer.h"
#include "emvUtils.h"


EmvMixer::EmvMixer(QString type, int amount, la::avdecc::UniqueIdentifier entityID, QWidget *parent)
	: QWidget(parent)
{
	setupUi(this);
	la::avdecc::UniqueIdentifier controlledEntityID = entityID;

	mixerType = type;
	channelAmount = amount;

	addChannels();
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


