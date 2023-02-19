#include "emvSettings.h"
#include "ui_emvSettings.h"

emvSettings::emvSettings(QWidget* parent)
	: QDialog(parent)
	, ui(new Ui::emvSettings)
{
	ui->setupUi(this);
	QObject::connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(saveSettings()));

	auto* const settings = qApp->property(settings::SettingsManager::PropertyName).value<settings::SettingsManager*>();
	ui->comboBoxEnable->setCurrentIndex(settings->getValue("emvSettingsEnable").toInt());
	ui->comboBoxGain->setCurrentIndex(settings->getValue("emvSettingsGain").toInt());
	ui->comboBoxInvert->setCurrentIndex(settings->getValue("emvSettingsInvert").toInt());
	ui->comboBoxPhantom->setCurrentIndex(settings->getValue("emvSettingsPhantom").toInt());
	ui->autoChangeCheckBox->setChecked(settings->getValue("emvAutoChange").toBool());
}

emvSettings::~emvSettings()
{
    delete ui;
}

void emvSettings::saveSettings() {
	auto* const settings = qApp->property(settings::SettingsManager::PropertyName).value<settings::SettingsManager*>();

	settings->setValue("emvSettingsEnable", ui->comboBoxEnable->currentIndex());
	settings->setValue("emvSettingsGain", ui->comboBoxGain->currentIndex());
	settings->setValue("emvSettingsInvert", ui->comboBoxInvert->currentIndex());
	settings->setValue("emvSettingsPhantom", ui->comboBoxPhantom->currentIndex());
	settings->setValue("emvAutoChange", ui->autoChangeCheckBox->isChecked());

	emit settingsChanged();
}
