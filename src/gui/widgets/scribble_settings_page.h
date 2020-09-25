/*
 *    Copyright 2020 Libor Pecháček
 *
 *    This file is part of OpenOrienteering.
 *
 *    OpenOrienteering is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    (at your option) any later version.
 *
 *    OpenOrienteering is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with OpenOrienteering.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef OPENORIENTEERING_SCRIBBLE_SETTINGS_PAGE_H
#define OPENORIENTEERING_SCRIBBLE_SETTINGS_PAGE_H

#include <QObject>
#include <QString>

#include "settings_page.h"

class QListWidget;
class QListWidgetItem;
class QModelIndex;
class QPushButton;
class QWidget;


namespace OpenOrienteering {


class ScribbleSettingsPage : public SettingsPage
{
	Q_OBJECT
	void setButtonColor(QPushButton* button);

public:
	explicit ScribbleSettingsPage(QWidget* parent = nullptr);

	~ScribbleSettingsPage();

	QString title() const override;

	void apply() override;

	void reset() override;

protected:
	QListWidget* list_widget;
	void initializePage();

public slots:
	void handle_editing(QListWidgetItem* item);
	void handle_reordering(QModelIndex parent, int start, int end, QModelIndex dest, int row);
};


}  // namespace OpenOrienteering

#endif // OPENORIENTEERING_SCRIBBLE_SETTINGS_PAGE_H
