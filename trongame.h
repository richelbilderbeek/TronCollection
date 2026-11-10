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
#ifndef TRONGAME_H
#define TRONGAME_H

#include <vector>

#include <QImage>
#include <QRgb>

namespace tron {

/// Classic Tron moves in four directions, Super Tron in any angle
enum class GameType { classic, super };

/// A player. Classic Tron uses 'direction', Super Tron uses 'angle'.
struct Player
{
  double x{0.0};
  double y{0.0};
  int direction{0};   ///< Classic Tron: 0: up, 1: right, 2: down, 3: left
  double angle{0.0};  ///< Super Tron: radians, 0: up, clockwise
  QRgb color{qRgb(0, 0, 0)};
  int key_left{0};    ///< A Qt::Key
  int key_right{0};   ///< A Qt::Key
  int n_wins{0};
  int n_losses{0};
};

struct GameSettings
{
  GameType type{GameType::classic};
  int width{100};
  int height{100};
  bool donut_shaped_world{true};
  bool can_walk_through_own_color{false};
  int n_angles{8}; ///< Super Tron: number of steps in a full turn
};

/// The game rules, without any GUI
class Game
{
public:
  Game(const GameSettings& settings, const std::vector<Player>& players);

  /// Turn the players that use this key
  void PressKey(const int key);

  /// Do one step of the game: check collisions, move and draw players
  void Tick();

  const QImage& GetImage() const noexcept { return m_image; }
  const std::vector<Player>& GetPlayers() const noexcept { return m_players; }
  const GameSettings& GetSettings() const noexcept { return m_settings; }

  /// The step a Super Tron player moves per tick
  static constexpr double sm_super_speed{1.0};

private:
  GameSettings m_settings;
  std::vector<Player> m_players;
  QImage m_image;

  void TickClassic();
  void TickSuper();

  void AddPointToWinners(const QRgb color);
  void AddPointToLosers(const QRgb color);
  void RemoveColor(const QRgb color);
  void SetPixel(const int x, const int y, const QRgb color);

  /// Handle the player bumping into the pixel at (x,y)
  void RespondToCollision(Player& player, const int x, const int y);
};

/// Put the players in a circle around the center of the world,
/// heading outwards
void PlacePlayers(const GameSettings& settings, std::vector<Player>& players);

/// The color of black, which is the color of an empty spot
QRgb GetBlack() noexcept;

/// The rainbow color at x, where x is in the range [0,1>.
/// From http://www.richelbilderbeek.nl/CppRainbow.htm
QRgb Rainbow(const double x);

/// The number of player colors one can choose from
constexpr int GetNumberOfColors() noexcept { return 20; }

/// The player color with index i, where i is in the range [0,20>
QRgb GetPlayerColor(const int i);

} //~namespace tron

#endif // TRONGAME_H
