#ifndef EMVSETTINGS_H
#define EMVSETTINGS_H

#include <QDialog>

namespace Ui {
class emvSettings;
}

class emvSettings : public QDialog
{
    Q_OBJECT

public:
	explicit emvSettings(QWidget* parent = nullptr);
	~emvSettings();

private slots:
	void saveSettings();

signals:
	void settingsChanged();

private:
	Ui::emvSettings* ui;
};

#endif // EMVSETTINGS_H
