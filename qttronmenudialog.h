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
#ifndef QTTRONMENUDIALOG_H
#define QTTRONMENUDIALOG_H

#include <vector>

#include <QDialog>

#include "trongame.h"

class QLabel;
class QPushButton;
class QSlider;

namespace tron {

/// The menu of Classic Tron or Super Tron:
/// set up the number of players, speed, world size and rules,
/// calibrate the players' keys and colors, then start the game.
class QtTronMenuDialog : public QDialog
{
  Q_OBJECT

public:
  explicit QtTronMenuDialog(const GameType type, QWidget * parent = nullptr);

  int GetNumberOfPlayers() const noexcept { return m_n_players; }
  bool IsDonutShapedWorld() const noexcept { return m_donut_shaped_world; }
  bool CanWalkThroughOwnColor() const noexcept { return m_can_walk_through_own_color; }

  void SetNumberOfPlayers(const int n);
  void SetDonutShapedWorld(const bool donut);
  void SetCanWalkThroughOwnColor(const bool can);

  /// The game settings as currently chosen
  GameSettings GetSettings() const;

  /// The timer interval in milliseconds, as currently chosen
  int GetInterval() const;

protected:
  void keyPressEvent(QKeyEvent * e) override;

private:
  const GameType m_type;
  std::vector<Player> m_players;
  int m_n_players;
  bool m_donut_shaped_world;
  bool m_can_walk_through_own_color;

  QLabel * m_label_n_players;
  QSlider * m_slider_speed;
  QSlider * m_slider_width;
  QSlider * m_slider_height;
  QSlider * m_slider_angles;
  QLabel * m_label_world_chosen;
  QLabel * m_label_own_color_chosen;
  QPushButton * m_button_start;

  void OnAbout();
  void OnCalibrate();
  void OnStart();
};

/// Describe a slider position as 'Slowest' to 'Fastest', or 'Smallest' to 'Largest'
QString GetSpeedText(const double fraction);
QString GetSizeText(const double fraction);

} //~namespace tron

#endif // QTTRONMENUDIALOG_H
