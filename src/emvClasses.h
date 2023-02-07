#include <QtXml>

class EmvControlValues
{
public:
	EmvControlValues(QDomNode _parent);
	EmvControlValues();

	QDomElement parent;
	QString type;
	int descriptionIndex[2];
	QString units;
	int minValue;
	int maxValue;
	int stepValue;
	int currentValue;
	int defaultValue;

private:
};

class EmvControl
{
public:
	EmvControl(QDomNode _parent);
	EmvControl(la::avdecc::UniqueIdentifier _entityID,
		la::avdecc::controller::model::ControlNode _node);
	EmvControl();

	
	QDomElement parent;
	int descriptionIndex[2];
	QString blockLatency;
	QString controlLatency;
	QString controlDomain;
	QString controlType;
	QString controlValueType;
	QString resetTime;
	QString signalType;
	QString signalIndex;
	QString signalOutput;
	QList<EmvControlValues> values;

	int valuesCount;
	int controlTypeIndex;

	bool isDebug;
	QString controlName;
	la::avdecc::controller::model::ControlNode * node;
	la::avdecc::UniqueIdentifier entityID;
	int index;

private:
};

class EmvJack
{
public:
	EmvJack();
	EmvJack(QDomNode _parent);

	QDomElement parent;
	QString direction;
	int descriptionIndex[2];
	QString flags;
	int type;
	QList<EmvControl> controls;
	int controlsCount;
};

class EmvPort
{
public:
	EmvPort();
	EmvPort(QDomNode _parent, QString _dir);

	QDomElement parent;
	QString dir;
	QString clockDomainIndex;
	QString portFlags;
	QList<EmvControl> controls;
	int controlsCount;
	QString signalType;
	QString signalIndex;
	QString signalOutput;
	QString blockLatency;
	int jackIndex;
};

class EmvAudioUnit
{
public:
	EmvAudioUnit();
	EmvAudioUnit(QDomNode _parent);

	QDomElement parent;
	int descriptionIndex[2];
	int clockDomainIndex;
	QList<EmvControl> controls;
	int controlsCount;
	double currentSamplingRate;
	QList<EmvPort> inputExternalPorts;
	QList<EmvPort> outputExternalPorts;

	int inputExternalPortsCount;
	int outputExternalPortsCount;

	QList<EmvPort> getExternalPorts(QString _dir);

};



