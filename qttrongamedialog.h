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
#ifndef QTTRONGAMEDIALOG_H
#define QTTRONGAMEDIALOG_H

#include <vector>

#include <QDialog>

#include "trongame.h"

class QTimer;

namespace tron {

/// The window in which a game of Classic Tron or Super Tron is played.
/// The world is stretched over the whole window.
/// When closed, the losses and wins per player are shown.
class QtTronGameDialog : public QDialog
{
  Q_OBJECT

public:
  QtTronGameDialog(
    const GameSettings& settings,
    const std::vector<Player>& players,
    const int interval_msec,
    QWidget * parent = nullptr);

  const Game& GetGame() const noexcept { return m_game; }

protected:
  void closeEvent(QCloseEvent * e) override;
  bool focusNextPrevChild(bool) override { return false; } //Tab is a game key
  void keyPressEvent(QKeyEvent * e) override;
  void paintEvent(QPaintEvent * e) override;

private:
  Game m_game;
  QTimer * const m_timer;
  bool m_is_closing;

  void OnTimer();
};

} //~namespace tron

#endif // QTTRONGAMEDIALOG_H
