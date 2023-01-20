#pragma once

#include <QWidget>
#include "ui_emvMixer.h"
#include "emvEntity.h"

class EmvMixer : public QWidget, public Ui::EmvMixerClass
{
	Q_OBJECT

public:
	EmvMixer(EmvEntity *entity, QString type, QWidget *parent);
	~EmvMixer();

	EmvEntity* myEntity;
	QString mixerType;
	QString headerStyle;
	int channelAmount;
	la::avdecc::UniqueIdentifier controlledEntityID;

	bool isBlinking;

	void addChannels();

private slots:
	void updateGainDial();
	void updateGainPad();
	void updatePanDial();
	void updatePanReset();
	void updateLevelsSlider();
	void updateLevelsMute();

	void controlCheckBoxChanged();
	void controlPushButtonChanged();
	void controlDialChanged();

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

	void addJacks(QString _dir);
	void addMetaData();
	void addConfigurationControls();
	void addControlsToPage(int index, int row, int column);
	QMap<int, EmvControl> controls;

};
