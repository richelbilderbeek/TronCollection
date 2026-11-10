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
#include "qttrongamedialog.h"

#include <algorithm>

#include <QCloseEvent>
#include <QKeyEvent>
#include <QPainter>
#include <QTimer>

#include "qttronchartdialog.h"

namespace {

std::vector<tron::Player> Place(
  const tron::GameSettings& settings, std::vector<tron::Player> players)
{
  tron::PlacePlayers(settings, players);
  return players;
}

} //~namespace

tron::QtTronGameDialog::QtTronGameDialog(
  const GameSettings& settings,
  const std::vector<Player>& players,
  const int interval_msec,
  QWidget * parent)
  : QDialog(parent),
    m_game(settings, Place(settings, players)),
    m_timer{new QTimer(this)},
    m_is_closing{false}
{
  setWindowTitle(
    settings.type == GameType::classic
    ? QString::fromUtf8("Classic Tron (C) 2008 Richèl Bilderbeek")
    : QString::fromUtf8("Super Tron (C) 2008 Richèl Bilderbeek"));
  setAttribute(Qt::WA_OpaquePaintEvent);
  setFocusPolicy(Qt::StrongFocus);
  setMinimumSize(settings.width, settings.height);
  resize(std::max(settings.width, 680), std::max(settings.height, 440));

  connect(m_timer, &QTimer::timeout, this, &QtTronGameDialog::OnTimer);
  m_timer->setTimerType(Qt::PreciseTimer);
  m_timer->start(interval_msec);
  OnTimer();
}

void tron::QtTronGameDialog::closeEvent(QCloseEvent * e)
{
  if (!m_is_closing)
  {
    m_is_closing = true;
    m_timer->stop();
    QtTronChartDialog d(m_game.GetPlayers(), this);
    d.exec();
  }
  QDialog::closeEvent(e);
}

void tron::QtTronGameDialog::keyPressEvent(QKeyEvent * e)
{
  const int key = e->key();
  const auto& players = m_game.GetPlayers();
  const bool is_player_key = std::any_of(players.begin(), players.end(),
    [key](const Player& p) { return p.key_left == key || p.key_right == key; });
  if (is_player_key)
  {
    m_game.PressKey(key);
    return;
  }
  if (key == Qt::Key_Escape)
  {
    close();
    return;
  }
  QDialog::keyPressEvent(e);
}

void tron::QtTronGameDialog::OnTimer()
{
  m_game.Tick();
  update();
}

void tron::QtTronGameDialog::paintEvent(QPaintEvent *)
{
  QPainter painter(this);
  painter.setRenderHint(QPainter::SmoothPixmapTransform, false);
  painter.drawImage(rect(), m_game.GetImage());
}
