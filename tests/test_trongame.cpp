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
// Unit tests of the game rules of Classic Tron and Super Tron
#include <cmath>
#include <set>

#include <QtTest>

#include "trongame.h"

using namespace tron;

namespace {

Player CreateClassic(const int x, const int y, const int direction, const QRgb color)
{
  Player p;
  p.x = x;
  p.y = y;
  p.direction = direction;
  p.color = color;
  p.key_left = Qt::Key_A;
  p.key_right = Qt::Key_D;
  return p;
}

GameSettings CreateSettings(const GameType type, const bool donut, const bool can_walk_through_own_color)
{
  GameSettings s;
  s.type = type;
  s.width = 10;
  s.height = 10;
  s.donut_shaped_world = donut;
  s.can_walk_through_own_color = can_walk_through_own_color;
  s.n_angles = 4;
  return s;
}

const QRgb red = qRgb(255, 0, 0);
const QRgb blue = qRgb(0, 0, 255);

} //~namespace

class TestTronGame : public QObject
{
  Q_OBJECT

private slots:

  void PlayerColorsAreDistinctAndNotBlack()
  {
    QCOMPARE(GetPlayerColor(0), qRgb(255, 0, 0));
    std::set<QRgb> colors;
    for (int i = 0; i != GetNumberOfColors(); ++i)
    {
      QVERIFY(GetPlayerColor(i) != GetBlack());
      colors.insert(GetPlayerColor(i));
    }
    QCOMPARE(static_cast<int>(colors.size()), GetNumberOfColors());
  }

  void PlacePlayersInsideWorldHeadingOutwards()
  {
    for (const GameType type : { GameType::classic, GameType::super })
    {
      GameSettings s;
      s.type = type;
      s.width = 300;
      s.height = 100; //Not square: original code used the width for y
      std::vector<Player> players(4);
      PlacePlayers(s, players);
      for (const Player& p : players)
      {
        QVERIFY(p.x >= 0.0 && p.x < s.width);
        QVERIFY(p.y >= 0.0 && p.y < s.height);
      }
      //First player is right of the center, heading right
      QCOMPARE(players[0].direction, 1);
      QVERIFY(players[0].x > 150.0);
      QCOMPARE(players[1].direction, 2); //Below, heading down
      QCOMPARE(players[2].direction, 3); //Left, heading left
      QCOMPARE(players[3].direction, 0); //Above, heading up
    }
  }

  void ClassicPlayerMovesAndDraws()
  {
    Game g(CreateSettings(GameType::classic, true, false), { CreateClassic(5, 5, 1, red) });
    g.Tick();
    QCOMPARE(g.GetPlayers()[0].x, 6.0);
    QCOMPARE(g.GetPlayers()[0].y, 5.0);
    QCOMPARE(g.GetImage().pixel(6, 5), red);
  }

  void ClassicPlayerTurns()
  {
    Game g(CreateSettings(GameType::classic, true, false), { CreateClassic(5, 5, 0, red) });
    g.PressKey(Qt::Key_A);
    QCOMPARE(g.GetPlayers()[0].direction, 3);
    g.PressKey(Qt::Key_D);
    g.PressKey(Qt::Key_D);
    QCOMPARE(g.GetPlayers()[0].direction, 1);
    g.PressKey(Qt::Key_X); //Not a player key
    QCOMPARE(g.GetPlayers()[0].direction, 1);
  }

  void ClassicDonutWorldWraps()
  {
    Game g(CreateSettings(GameType::classic, true, false), { CreateClassic(9, 0, 1, red) });
    g.Tick();
    QCOMPARE(g.GetPlayers()[0].x, 0.0);
    QCOMPARE(g.GetPlayers()[0].n_losses, 0);
  }

  void ClassicFlatWorldWallCostsAPointAndTurnsBack()
  {
    Game g(CreateSettings(GameType::classic, false, false), { CreateClassic(8, 5, 1, red) });
    g.Tick(); //To x = 9
    QCOMPARE(g.GetImage().pixel(9, 5), red);
    g.Tick(); //Hits wall
    const Player& p = g.GetPlayers()[0];
    QCOMPARE(p.n_losses, 1);
    QCOMPARE(p.direction, 3);
    QCOMPARE(p.x, 8.0);
    QCOMPARE(g.GetImage().pixel(9, 5), GetBlack()); //Trail removed
    QCOMPARE(g.GetImage().pixel(8, 5), red); //New trail
  }

  void ClassicHittingOtherTrail()
  {
    //Blue moves down along x = 6, red moves right into blue's trail
    Game g(CreateSettings(GameType::classic, true, false),
      { CreateClassic(6, 3, 2, blue), CreateClassic(2, 5, 1, red) });
    for (int i = 0; i != 3; ++i) g.Tick(); //Blue at (6,6), red at (5,5)
    QCOMPARE(g.GetImage().pixel(6, 5), blue);
    g.Tick(); //Red bumps into blue's trail at (6,5)
    QCOMPARE(g.GetPlayers()[1].n_losses, 1);
    QCOMPARE(g.GetPlayers()[0].n_wins, 1);
    //Blue's trail is removed, red's is kept
    QCOMPARE(g.GetImage().pixel(6, 4), GetBlack());
    QCOMPARE(g.GetImage().pixel(5, 5), red);
  }

  void ClassicOwnColor()
  {
    //Player runs in a tight circle, back onto its own trail
    for (const bool can_walk : { false, true })
    {
      Game g(CreateSettings(GameType::classic, true, can_walk), { CreateClassic(5, 5, 1, red) });
      g.Tick();                        //Right: (6,5)
      g.PressKey(Qt::Key_D); g.Tick(); //Down: (6,6)
      g.PressKey(Qt::Key_D); g.Tick(); //Left: (5,6)
      g.PressKey(Qt::Key_D); g.Tick(); //Up: (5,5), the start, which has no trail
      g.Tick();                        //Up: (5,4)
      g.PressKey(Qt::Key_D); g.Tick(); //Right: (6,4)
      QCOMPARE(g.GetPlayers()[0].n_losses, 0);
      g.PressKey(Qt::Key_D); g.Tick(); //Down: onto own trail at (6,5)
      QCOMPARE(g.GetPlayers()[0].n_losses, can_walk ? 0 : 1);
      QCOMPARE(g.GetPlayers()[0].n_wins, 0);
    }
  }

  void SuperPlayerMovesAtAnAngle()
  {
    Player p = CreateClassic(5, 5, 0, red);
    p.x = 5.5;
    p.y = 5.5;
    p.angle = 0.0; //Up
    Game g(CreateSettings(GameType::super, true, false), { p });
    g.Tick();
    QCOMPARE(g.GetPlayers()[0].x, 5.5);
    QCOMPARE(g.GetPlayers()[0].y, 4.5);
    g.PressKey(Qt::Key_D); //Quarter turn: right
    g.Tick();
    QVERIFY(std::abs(g.GetPlayers()[0].x - 6.5) < 1e-9);
    QVERIFY(std::abs(g.GetPlayers()[0].y - 4.5) < 1e-9);
    QCOMPARE(g.GetImage().pixel(6, 4), red);
  }

  void SuperFlatWorldDetectsLeftEdge()
  {
    //Moving left from x = 0.5 leaves the world
    Player p = CreateClassic(0, 5, 0, red);
    p.x = 0.5;
    p.y = 5.5;
    p.angle = 1.5 * 3.14159265358979323846; //Left
    Game g(CreateSettings(GameType::super, false, false), { p });
    g.Tick();
    QCOMPARE(g.GetPlayers()[0].n_losses, 1);
    QVERIFY(g.GetPlayers()[0].x >= 0.0 && g.GetPlayers()[0].x < 2.0);
  }
};

QTEST_GUILESS_MAIN(TestTronGame)
#include "test_trongame.moc"
