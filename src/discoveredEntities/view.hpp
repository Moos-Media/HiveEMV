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

#include "avdecc/controllerModel.hpp"
#include "settingsManager/settings.hpp"
#include "settingsManager/settingsSignaler.hpp"
#include "visibilitySettings.hpp"

#include <QtMate/widgets/tableView.hpp>
#include <QtMate/widgets/dynamicHeaderView.hpp>
#include <QtMate/widgets/headerViewSortSectionFilter.hpp>
#include <hive/widgetModelsLibrary/imageItemDelegate.hpp>
#include <hive/widgetModelsLibrary/errorItemDelegate.hpp>

#include <QSortFilterProxyModel>

namespace discoveredEntities
{
class View final : public qtMate::widgets::TableView
{
	Q_OBJECT
public:
	View(QWidget* parent = nullptr);
	virtual ~View();

	void setupView(hive::VisibilityDefaults const& defaults) noexcept;
	void restoreState() noexcept;
	la::avdecc::UniqueIdentifier selectedControlledEntity() const noexcept;

	// Public signals
	Q_SIGNAL void selectedControlledEntityChanged(la::avdecc::UniqueIdentifier const entityID);
	Q_SIGNAL void doubleClicked(la::avdecc::UniqueIdentifier const entityID);
	Q_SIGNAL void contextMenuRequested(la::avdecc::UniqueIdentifier const entityID, QPoint const& pos);

private:
	void saveDynamicHeaderState() const noexcept;
	void clearSelection() noexcept;
	la::avdecc::UniqueIdentifier controlledEntityIDAtIndex(QModelIndex const& index) const noexcept;

	// qtMate::widgets::TableView overrides
	virtual void showEvent(QShowEvent* event) override;

	// Slots
	//void handleCreateChannelConnectionsFinished(avdecc::CreateConnectionsInfo const& info);

private:
	avdecc::ControllerModel _model{ this };
	QSortFilterProxyModel _proxyModel{};
	qtMate::widgets::DynamicHeaderView _dynamicHeaderView{ Qt::Horizontal, this };
	qtMate::widgets::HeaderViewSortSectionFilter _headerSectionSortFilter{ &_dynamicHeaderView };
	hive::widgetModelsLibrary::ImageItemDelegate _imageItemDelegate{ true, qtMate::material::color::Palette::name(qApp->property(settings::SettingsManager::PropertyName).value<settings::SettingsManager*>()->getValue(settings::General_ThemeColorIndex.name).toInt()), this };
	hive::widgetModelsLibrary::ErrorItemDelegate _errorItemDelegate{ true, qtMate::material::color::Palette::name(qApp->property(settings::SettingsManager::PropertyName).value<settings::SettingsManager*>()->getValue(settings::General_ThemeColorIndex.name).toInt()), this };
	SettingsSignaler _settingsSignaler{};
	la::avdecc::UniqueIdentifier _selectedControlledEntity{};
};

} // namespace discoveredEntities
