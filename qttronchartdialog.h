/*
  The Tron Collection, three simple tron games
  Copyright (C) 2008  Richel Bilderbeek

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
// From http://www.richelbilderbeek.nl
#ifndef QTTRONCHARTDIALOG_H
#define QTTRONCHARTDIALOG_H

#include <vector>

#include <QDialog>

#include "trongame.h"

class QPushButton;

namespace tron {

/// Bar chart of the losses and wins per player, shown after a game.
/// It can only be closed after a second, so that players still
/// pressing their keys do not close it by accident.
class QtTronChartDialog : public QDialog
{
  Q_OBJECT

public:
  QtTronChartDialog(const std::vector<Player>& players, QWidget * parent = nullptr);

protected:
  void reject() override;

private:
  bool m_can_close;
  QPushButton * const m_button_close;
};

/// Draws the bar chart
class QtTronChartWidget : public QWidget
{
  Q_OBJECT

public:
  QtTronChartWidget(const std::vector<Player>& players, QWidget * parent = nullptr);

protected:
  void paintEvent(QPaintEvent * e) override;

private:
  const std::vector<Player> m_players;
};

} //~namespace tron

#endif // QTTRONCHARTDIALOG_H
