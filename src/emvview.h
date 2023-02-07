#pragma once

#include <QWidget>
#include "ui_emvview.h"
#include "emvEntity.h"

class EmvView : public QWidget, public Ui::EmvViewClass
{
	Q_OBJECT

public:
	EmvView(QWidget* parent = nullptr);
	~EmvView();

	EmvEntity myEntity;

	QPushButton* button;

	QString nameTag;

	la::avdecc::UniqueIdentifier controlledEntityID;

	void setControlledEntityID(la::avdecc::UniqueIdentifier const entityID);

	void updateConfigurationPicker();
	QString getEntityName();

	void addJacksViews();
	void setDebug(bool _isDebug);
	QString languageIdentifier;
	


private slots:
	void addMixer();
	void setView();
	void changeConfigurationClicked();
	void openFile();
	void openSettings();
	void changeLanguage();
	void settingsFinished(int resultCode);

private:
	bool isDebug;
	bool settingsAreOpen = false;;
	QString fileLocation;
	
};
