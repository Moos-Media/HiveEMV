#pragma once

#include <QWidget>
#include "ui_emvview.h"
#include "emvEntity.h"

class EmvView : public QWidget, public Ui::EmvViewClass
{
	Q_OBJECT

public:
	EmvView(bool _isDebug = "false", QWidget* parent = nullptr);
	~EmvView();

	EmvEntity myEntity;

	QPushButton* button;

	QString nameTag;

	la::avdecc::UniqueIdentifier controlledEntityID;

	void setControlledEntityID(la::avdecc::UniqueIdentifier const entityID);

	void updateConfigurationPicker();
	QString getEntityName();

	void addJacksViews();
	


private slots:
	void addMixer();
	void resetView(bool);
	void changeConfigurationClicked();
	void openFile();

private:
	bool isDebug;
};
