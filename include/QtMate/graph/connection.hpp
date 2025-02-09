/*
* Copyright (C) 2017-2023, Emilien Vallot, Christophe Calmejane and other contributors

* This file is part of Hive.

* Hive is free software: you can redistribute it and/or modify
* it under the terms of the GNU Lesser General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.

* Hive is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU Lesser General Public License for more details.

* You should have received a copy of the GNU Lesser General Public License
* along with Hive.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#include <QGraphicsPathItem>

#include <unordered_set>

namespace qtMate
{
namespace graph
{
class InputSocketItem;
class OutputSocketItem;

class ConnectionItem : public QGraphicsPathItem
{
public:
	ConnectionItem();
	~ConnectionItem();

	void setStart(QPointF const& p);
	void setStop(QPointF const& p);

	virtual int type() const override;

	void connectInput(InputSocketItem* input);
	InputSocketItem* input() const;
	void disconnectInput();

	void connectOutput(OutputSocketItem* output);
	OutputSocketItem* output() const;
	void disconnectOutput();

	void disconnect();

private:
	virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;
	void updatePath();

private:
	QPointF _start{};
	QPointF _stop{};

	InputSocketItem* _input{ nullptr };
	OutputSocketItem* _output{ nullptr };
};

using ConnectionItems = std::unordered_set<ConnectionItem*>;

} // namespace graph
} // namespace qtMate
