#pragma once

#include <QWidget>
#include "ui_emvMixer.h"

class EmvMixer : public QWidget, public Ui::EmvMixerClass
{
	Q_OBJECT

public:
	EmvMixer(QString type, int amount, la::avdecc::UniqueIdentifier entityID, QWidget *parent = nullptr);
	~EmvMixer();

	QString mixerType;
	int channelAmount;
	la::avdecc::UniqueIdentifier controlledEntityID;

	

	void addChannels();

private slots:
	void updateGainDial();
	void updateGainPad();
	void updatePanDial();
	void updatePanReset();
	void updateLevelsSlider();
	void updateLevelsMute();

private:
	//Gain
	QCheckBox* phantomPowerBoxes[10][10];
	QLabel* gainLabels[10][10];
	QDial* gainDials[10][10];
	QCheckBox* padCheckBoxes[10][10];

	//Pan
	QLabel* panLabels[10][10];
	QDial* panDials[10][10];
	QPushButton* panResetButtons[10][10];

	//Levels
	QLabel* levelsLabels[10][10];
	QSlider* levelsSliders[10][10];
	QCheckBox* muteCheckBoxes[10][10];
	QLabel* testLabelsPan[10];
};
