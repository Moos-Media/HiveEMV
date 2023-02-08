#include "emvClasses.h"
#include <QtXml>
#include <QDebug>
#include "emvUtils.h"

EmvControl::EmvControl(QDomNode _parent, int index){
	isDebug = true;
	controlIndex = index;
	//Set Parent Element
	parent = _parent.toElement();

	//Get Indexes of Localized Description
	QList<int> indexes = getIndexesFromNode(parent.elementsByTagName("localized_description").at(0).toElement());

	descriptionIndex[0] = indexes[0];
	descriptionIndex[1] = indexes[1];

	//Get QString values
	blockLatency = getStringByTagName(parent, "block_latency");
	controlLatency = getStringByTagName(parent, "control_latency");
	controlDomain = getStringByTagName(parent, "control_domain");
	controlType = getStringByTagName(parent, "control_type");
	controlValueType = getStringByTagName(parent, "control_value_type");
	resetTime = getStringByTagName(parent, "reset_time");
	signalType = getStringByTagName(parent, "signal_type");
	signalIndex = getStringByTagName(parent, "signal_index");
	signalOutput = getStringByTagName(parent, "signal_output");

	//Get Values
	valuesCount = 0;
	auto valuesParentNode = parent.elementsByTagName("values").at(0);
	for (int i = 0; i < valuesParentNode.childNodes().count(); ++i)
	{
		values.append(EmvControlValues(valuesParentNode.childNodes().at(i)));
		valuesCount += 1;
	}

	//Control Widget

	controlTypeIndex = getControlTypeIndexFromType(controlType);

}
EmvControl::EmvControl() {}

EmvControl::EmvControl(la::avdecc::UniqueIdentifier _entityID, la::avdecc::controller::model::ControlNode _node) {
	isDebug = false;

	controlName = hive::modelsLibrary::helper::localizedString(*hive::modelsLibrary::ControllerManager::getInstance().getControlledEntity(_entityID), 0, _node.staticModel->localizedDescription);

	blockLatency = QString::number(_node.staticModel->blockLatency);
	controlLatency = QString::number(_node.staticModel->controlLatency);
	controlDomain = QString::number(_node.staticModel->controlDomain);
	controlType = QString::number(_node.staticModel->controlType, 16);
	controlValueType = QString::number(_node.staticModel->controlValueType);
	controlTypeIndex = getControlTypeIndexFromType(controlType);
	resetTime = QString::number(_node.staticModel->resetTime);
	node = &_node;
	entityID = _entityID;
}

EmvControlValues::EmvControlValues(QDomNode _parent) {
	//Set Parent Element
	parent = _parent.toElement().firstChild().toElement();

	//Set Type
	type = parent.attribute("xsi:type");

	//Set Values
	bool temp;
	units = getStringByTagName(parent, "units");
	minValue = getStringByTagName(parent, "minimum").toInt(&temp, 16);
	maxValue = getStringByTagName(parent, "maximum").toInt(&temp, 16);
	stepValue = getStringByTagName(parent, "step").toInt(&temp, 16);
	currentValue = getStringByTagName(parent, "current").toInt(&temp, 16);
	defaultValue = getStringByTagName(parent, "default").toInt(&temp, 16);
}

EmvControlValues::EmvControlValues() {}

EmvJack::EmvJack() {}

EmvJack::EmvJack(QDomNode _parent, int* controlReference) {
	controlIndex = controlReference;

	//Set Parent Node
	parent = _parent.toElement();

	//Set Data
	direction = (parent.parentNode().toElement().tagName() == "input_jacks") ? "IN" : "OUT";

	QList<int> indexes = getIndexesFromNode(parent.elementsByTagName("localized_description").at(0).toElement());
	descriptionIndex[0] = indexes[0];
	descriptionIndex[1] = indexes[1];

	flags = getStringByTagName(parent, "jack_flags");
	bool conversion;
	type = getStringByTagName(parent, "jack_type").toInt(&conversion, 16);

	//Get Root Controls Node via last occurence
	QDomNode controlRoot = parent.elementsByTagName("controls").at(0);

	//Get List of all available Controls
	QDomNodeList controlsList = controlRoot.childNodes();

	//Create Config Instances
	controlsCount = 0;
	for (int i = 0; i < controlsList.count(); ++i)
	{
		controls.insert(i, EmvControl(controlsList.at(i), *controlIndex));
		++controlIndex;
		++controlsCount;

	}
}

EmvAudioUnit::EmvAudioUnit() {}

EmvAudioUnit::EmvAudioUnit(QDomNode _parent, int* controlReference) {
	controlIndex = controlReference;

	//Set Parent Element
	parent = _parent.toElement();

	//Get Description Index
	QList<int> indexes = getIndexesFromNode(parent.elementsByTagName("localized_description").at(0).toElement());
	descriptionIndex[0] = indexes[0];
	descriptionIndex[1] = indexes[1];

	//Get clock Domain
	clockDomainIndex = getStringByTagName(parent, "clock_domain_index").toInt();

	//Get Current Sampling Rate
	currentSamplingRate = getSamplingRateFromNode(parent.elementsByTagName("current_sampling_rate").at(0).toElement());


	//Get controls
	int index = parent.elementsByTagName("controls").count();
	if (index != 0)
	{
		QDomElement controlsNode = parent.elementsByTagName("controls").at(index - 1).toElement();

		controlsCount = 0;
		for (int i = 0; i < controlsNode.toElement().elementsByTagName("control").count(); ++i)
		{
			int controlValue = *controlIndex;
			controls.insert(i, EmvControl(controlsNode.toElement().elementsByTagName("control").at(i), controlValue));
			++controlIndex;
			++controlsCount;
		}
	}
	

	//Get Input External Ports
	index = parent.elementsByTagName("input_external_ports").count();
	if (index != 0)
	{
		QDomElement inputExternalNode = parent.elementsByTagName("input_external_ports").at(0).toElement();

		inputExternalPortsCount = 0;
		for (int i = 0; i < inputExternalNode.toElement().elementsByTagName("external_port").count(); ++i)
		{
			inputExternalPorts.insert(i, EmvPort(inputExternalNode.toElement().elementsByTagName("external_port").at(i), "IN", controlIndex));
			++inputExternalPortsCount;
		}
	}

	//Get Output External Ports
	index = parent.elementsByTagName("output_external_ports").count();
	if (index != 0)
	{
		QDomElement outputExternalNode = parent.elementsByTagName("output_external_ports").at(0).toElement();

		inputExternalPortsCount = 0;
		for (int i = 0; i < outputExternalNode.toElement().elementsByTagName("external_port").count(); ++i)
		{
			outputExternalPorts.insert(i, EmvPort(outputExternalNode.toElement().elementsByTagName("external_port").at(i), "OUT", controlIndex));
			++outputExternalPortsCount;
		}
	}

	
}

QList<EmvPort> EmvAudioUnit::getExternalPorts(QString _dir) {
	if (_dir.toUpper() == "IN")
		return inputExternalPorts;
	else
		return outputExternalPorts;
}

EmvPort::EmvPort() {}

EmvPort::EmvPort(QDomNode _parent, QString _dir, int* controlReference) {
	controlIndex = controlReference;

	//Set Parent + Direction
	parent = _parent.toElement();
	dir = _dir.toUpper();

	//Get Data
	clockDomainIndex = getStringByTagName(parent, "clock_domain_index");
	portFlags = getStringByTagName(parent, "port_flags");
	signalType = getStringByTagName(parent, "signal_type");
	signalIndex = getStringByTagName(parent, "signal_index");
	signalOutput = getStringByTagName(parent, "signal_output");
	blockLatency = getStringByTagName(parent, "block_latency");
	bool temp = false;
	jackIndex = getStringByTagName(parent, "jack_index").toInt(&temp, 16);
	controlsCount = 0;

	//Get Controls
	int index = parent.elementsByTagName("controls").count();
	if (index == 0)
		return;

	QDomElement controlsNode = parent.elementsByTagName("controls").at(0).toElement();

	controlsCount = 0;
	for (int i = 0; i < controlsNode.toElement().elementsByTagName("control").count(); ++i)
	{
		int controlIndexValue = *controlIndex;
		controls.insert(i, EmvControl(controlsNode.toElement().elementsByTagName("control").at(i), controlIndexValue));
		++controlIndex;
		++controlsCount;
	}
}
