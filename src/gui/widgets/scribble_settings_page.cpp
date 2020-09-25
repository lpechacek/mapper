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

#include <algorithm>
#include <vector>

#include <QAbstractItemModel>
#include <QAbstractItemView>
#include <QBrush>
#include <QColor>
#include <QColorDialog>
#include <QFlags>
#include <QIcon>
#include <QLabel>
#include <QLatin1String>
#include <QListWidget>
#include <QListWidgetItem>
#include <QModelIndex>
#include <QString>
#include <QVBoxLayout>
#include <Qt>

#include "scribble_settings_page.h"
#include "gui/widgets/settings_page.h"
#include "settings.h"

class QWidget;


namespace OpenOrienteering {


ScribbleSettingsPage::ScribbleSettingsPage(QWidget* parent)
    : SettingsPage(parent)
{
	auto* layout = new QVBoxLayout();

	auto* user_instructions = new QLabel(tr("Change or add scibble colors with a double click on the entry."
	                                        " Change color order with drag-and-drop."
	                                        " Remove color by moving it to the bottom of the list."));
	user_instructions->setWordWrap(true);
	layout->addWidget(user_instructions);

	list_widget = new QListWidget();
	list_widget->setDragDropMode(QListWidget::InternalMove);
	list_widget->setSelectionMode(QListWidget::ContiguousSelection);
	layout->addWidget(list_widget);

	setLayout(layout);

	connect(list_widget, SIGNAL(itemDoubleClicked(QListWidgetItem*)),
	        this, SLOT(handle_editing(QListWidgetItem*)));
	connect(list_widget->model(), SIGNAL(rowsMoved(QModelIndex,int,int,QModelIndex,int)),
	        this, SLOT(handle_reordering(QModelIndex,int,int,QModelIndex,int)));

	initializePage();
}


ScribbleSettingsPage::~ScribbleSettingsPage() = default;


QString ScribbleSettingsPage::title() const
{
	return QLatin1String("Scribble");
}


void ScribbleSettingsPage::apply()
{
	std::vector<QColor> new_colors;

	new_colors.reserve(list_widget->count()-1);

	for (int row = 0; row < list_widget->count()-1; ++row)
		new_colors.emplace_back(list_widget->item(row)->background().color());

	Settings::getInstance().setScribbleColors(new_colors);
}


void ScribbleSettingsPage::reset()
{
	initializePage();
}


void ScribbleSettingsPage::initializePage()
{
	list_widget->clear();

	const auto scribble_colors = Settings::getInstance().scribbleColors();

	for (const auto& color : scribble_colors)
	{
		auto* color_cell = new QListWidgetItem(color.name().right(6).toUpper());
		color_cell->setBackground(QBrush(color));
		list_widget->addItem(color_cell);
	}

	auto* new_color_cell = new QListWidgetItem(QIcon(QStringLiteral(":/images/plus.png")),
	                                           tr("Add new color"), list_widget,
	                                           QListWidgetItem::UserType);
	new_color_cell->setFlags(new_color_cell->flags() & ~Qt::ItemIsDragEnabled);
	list_widget->addItem(new_color_cell);
}


void ScribbleSettingsPage::handle_editing(QListWidgetItem* item)
{
	auto const new_color = QColorDialog::getColor(item->background().color());
	if (new_color.isValid())
	{
		if (item->type() == QListWidgetItem::UserType)
		{
			item = new QListWidgetItem();
			list_widget->insertItem(list_widget->count() - 1, item);
		}
		item->setBackground(QBrush(new_color));
		item->setText(new_color.name().right(6).toUpper());
	}
}


void ScribbleSettingsPage::handle_reordering(QModelIndex /*parent*/, int /*start*/, int /*end*/, QModelIndex /*dest*/, int row)
{
	if (row == list_widget->count())
		delete list_widget->takeItem(row-1);
}


}  // namespace OpenOrienteering
