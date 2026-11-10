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
#include "trongame.h"

#include <algorithm>
#include <cassert>
#include <cmath>

namespace {

constexpr double pi = 3.14159265358979323846;

/// Wrap a coordinate into the range [0,max>
double Wrap(double x, const double max)
{
  x = std::fmod(x, max);
  if (x < 0.0) x += max;
  if (x >= max) x = 0.0; //Rounding
  return x;
}

unsigned char GetRed(const double x)
{
  assert(x >= 0.0 && x < 1.0);
  const double f = std::max(0.0,
    (x < 0.5
    ?  std::cos(x * 1.5 * pi)
    : -std::sin(x * 1.5 * pi)
    ) );
  return static_cast<unsigned char>(255.0 * f);
}

unsigned char GetGreen(const double x)
{
  assert(x >= 0.0 && x < 1.0);
  const double f = std::max(0.0, std::sin(x * 1.5 * pi));
  return static_cast<unsigned char>(255.0 * f);
}

unsigned char GetBlue(const double x)
{
  assert(x >= 0.0 && x < 1.0);
  const double f = std::max(0.0, -std::cos(x * 1.5 * pi));
  return static_cast<unsigned char>(255.0 * f);
}

} //~namespace

QRgb tron::GetBlack() noexcept
{
  return qRgb(0, 0, 0);
}

QRgb tron::Rainbow(const double x)
{
  const int r0 = GetRed(x);
  const int g0 = GetGreen(x);
  const int b0 = GetBlue(x);
  const int max = std::max(r0, std::max(g0, b0));
  assert(max != 0);
  const auto scale = [max](const int c)
  {
    return static_cast<int>(255.0 * static_cast<double>(c) / static_cast<double>(max));
  };
  return qRgb(scale(r0), scale(g0), scale(b0));
}

QRgb tron::GetPlayerColor(const int i)
{
  assert(i >= 0 && i < GetNumberOfColors());
  return Rainbow(static_cast<double>(i) / static_cast<double>(GetNumberOfColors()));
}

void tron::PlacePlayers(const GameSettings& settings, std::vector<Player>& players)
{
  const int n_players = static_cast<int>(players.size());
  const double mid_x = static_cast<double>(settings.width) / 2.0;
  const double mid_y = static_cast<double>(settings.height) / 2.0;
  for (int i = 0; i != n_players; ++i)
  {
    Player& p = players[i];
    const double fraction = static_cast<double>(i) / static_cast<double>(n_players);
    const double angle = (0.5 * pi) + (fraction * pi * 2.0);
    p.x = mid_x + ( std::sin(angle) * 10.0);
    p.y = mid_y + (-std::cos(angle) * 10.0);
    p.angle = angle;
    if (angle < 0.75 * pi) p.direction = 1;
    else if (angle < 1.25 * pi) p.direction = 2;
    else if (angle < 1.75 * pi) p.direction = 3;
    else if (angle < 2.25 * pi) p.direction = 0;
    else p.direction = 1;
    if (settings.type == GameType::classic)
    {
      p.x = std::floor(p.x);
      p.y = std::floor(p.y);
    }
    p.x = Wrap(p.x, settings.width);
    p.y = Wrap(p.y, settings.height);
  }
}

tron::Game::Game(const GameSettings& settings, const std::vector<Player>& players)
  : m_settings{settings},
    m_players{players},
    m_image(settings.width, settings.height, QImage::Format_RGB32)
{
  assert(m_settings.width > 0);
  assert(m_settings.height > 0);
  assert(m_settings.n_angles > 0);
  m_image.fill(GetBlack());
}

void tron::Game::PressKey(const int key)
{
  const double delta_angle = (2.0 * pi) / static_cast<double>(m_settings.n_angles);
  for (Player& p : m_players)
  {
    if (p.key_left == key)
    {
      p.direction = (p.direction + 3) % 4;
      p.angle -= delta_angle;
    }
    if (p.key_right == key)
    {
      p.direction = (p.direction + 1) % 4;
      p.angle += delta_angle;
    }
  }
}

void tron::Game::Tick()
{
  if (m_settings.type == GameType::classic) TickClassic();
  else TickSuper();
}

void tron::Game::TickClassic()
{
  const int maxx = m_settings.width;
  const int maxy = m_settings.height;
  const bool donut = m_settings.donut_shaped_world;

  //Check for collisions
  for (Player& p : m_players)
  {
    int x_to = static_cast<int>(p.x);
    int y_to = static_cast<int>(p.y);
    switch (p.direction)
    {
      case 0: --y_to; if (donut && y_to <     0) y_to += maxy; break; //Up
      case 1: ++x_to; if (donut && x_to >= maxx) x_to -= maxx; break; //Right
      case 2: ++y_to; if (donut && y_to >= maxy) y_to -= maxy; break; //Down
      case 3: --x_to; if (donut && x_to <     0) x_to += maxx; break; //Left
    }
    if (!donut && (x_to < 0 || y_to < 0 || x_to >= maxx || y_to >= maxy))
    {
      //Bumped into the wall: lose a point, lose the trail, turn back
      AddPointToLosers(p.color);
      RemoveColor(p.color);
      p.direction = (p.direction + 2) % 4;
      continue;
    }
    RespondToCollision(p, x_to, y_to);
  }

  //Move the players
  for (Player& p : m_players)
  {
    switch (p.direction)
    {
      case 0: p.y -= 1.0; break;
      case 1: p.x += 1.0; break;
      case 2: p.y += 1.0; break;
      case 3: p.x -= 1.0; break;
    }
    p.x = Wrap(p.x, maxx);
    p.y = Wrap(p.y, maxy);
  }

  //Draw new player locations
  for (const Player& p : m_players)
  {
    SetPixel(static_cast<int>(p.x), static_cast<int>(p.y), p.color);
  }
}

void tron::Game::TickSuper()
{
  const int maxx = m_settings.width;
  const int maxy = m_settings.height;
  const double speed = sm_super_speed;

  //Check for collisions
  for (Player& p : m_players)
  {
    const int x_from = static_cast<int>(std::floor(p.x));
    const int y_from = static_cast<int>(std::floor(p.y));
    int x_to = static_cast<int>(std::floor(p.x + (std::sin(p.angle) * speed)));
    int y_to = static_cast<int>(std::floor(p.y - (std::cos(p.angle) * speed)));
    if (m_settings.donut_shaped_world)
    {
      if (x_to <     0) x_to += maxx;
      if (x_to >= maxx) x_to -= maxx;
      if (y_to <     0) y_to += maxy;
      if (y_to >= maxy) y_to -= maxy;
    }
    else if (x_to < 0 || y_to < 0 || x_to >= maxx || y_to >= maxy)
    {
      //Bumped into the wall: lose a point, lose the trail, turn back
      AddPointToLosers(p.color);
      RemoveColor(p.color);
      p.angle += pi;
      continue;
    }
    if (x_from == x_to && y_from == y_to) continue; //No movement visible
    RespondToCollision(p, x_to, y_to);
  }

  //Move the players
  for (Player& p : m_players)
  {
    p.x = Wrap(p.x + (std::sin(p.angle) * speed), maxx);
    p.y = Wrap(p.y - (std::cos(p.angle) * speed), maxy);
  }

  //Draw new player locations
  for (const Player& p : m_players)
  {
    SetPixel(static_cast<int>(p.x), static_cast<int>(p.y), p.color);
  }
}

void tron::Game::RespondToCollision(Player& p, const int x, const int y)
{
  assert(m_image.valid(x, y));
  const QRgb color = m_image.pixel(x, y);
  if (color == GetBlack()) return; //Nothing there
  if (color != p.color)
  {
    //Bumped into someone else's trail:
    //the owner of that trail wins a point, the player loses one,
    //and the trail bumped into is removed
    AddPointToWinners(color);
    AddPointToLosers(p.color);
    RemoveColor(color);
  }
  else if (!m_settings.can_walk_through_own_color)
  {
    //Bumped into own trail
    AddPointToLosers(p.color);
    RemoveColor(color);
  }
}

void tron::Game::AddPointToWinners(const QRgb color)
{
  for (Player& p : m_players)
  {
    if (p.color == color) ++p.n_wins;
  }
}

void tron::Game::AddPointToLosers(const QRgb color)
{
  for (Player& p : m_players)
  {
    if (p.color == color) ++p.n_losses;
  }
}

void tron::Game::RemoveColor(const QRgb color)
{
  const int maxx = m_image.width();
  const int maxy = m_image.height();
  for (int y = 0; y != maxy; ++y)
  {
    QRgb * const line = reinterpret_cast<QRgb*>(m_image.scanLine(y));
    for (int x = 0; x != maxx; ++x)
    {
      if (line[x] == color) line[x] = GetBlack();
    }
  }
}

void tron::Game::SetPixel(const int x, const int y, const QRgb color)
{
  if (m_image.valid(x, y)) m_image.setPixel(x, y, color);
}
