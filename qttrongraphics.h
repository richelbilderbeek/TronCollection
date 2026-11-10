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
#ifndef QTTRONGRAPHICS_H
#define QTTRONGRAPHICS_H

#include <QImage>
#include <QLabel>

namespace tron {

/// A shaded globe.
/// From http://www.richelbilderbeek.nl/CppDrawGlobe.htm
QImage DrawGlobe(const int size, const QColor& max_color);

/// A shaded globe with a minus sign on it.
/// From http://www.richelbilderbeek.nl/CppDrawGlobeMinus.htm
QImage DrawGlobeMinus(const int size, const QColor& max_color);

/// A shaded globe with a plus sign on it.
/// From http://www.richelbilderbeek.nl/CppDrawGlobePlus.htm
QImage DrawGlobePlus(const int size, const QColor& max_color);

/// A QLabel that emits clicked() when clicked,
/// like a VCL TImage with an OnClick handler
class QtClickableLabel : public QLabel
{
  Q_OBJECT

public:
  explicit QtClickableLabel(QWidget * parent = nullptr) : QLabel(parent)
  {
    setCursor(Qt::PointingHandCursor);
  }

signals:
  void clicked();

protected:
  void mousePressEvent(QMouseEvent *) override { emit clicked(); }
};

} //~namespace tron

#endif // QTTRONGRAPHICS_H
