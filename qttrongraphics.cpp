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
#include "qttrongraphics.h"

#include <algorithm>
#include <cmath>

#include <QPainter>
#include <QPen>

QImage tron::DrawGlobe(const int size, const QColor& max_color)
{
  QImage image(size, size, QImage::Format_RGB32);
  const double mid = static_cast<double>(size) / 2.0;
  const double max_dist = mid;
  for (int y = 0; y != size; ++y)
  {
    const double dy = static_cast<double>(y) - mid;
    for (int x = 0; x != size; ++x)
    {
      const double dx = static_cast<double>(x) - mid;
      const double dist = std::sqrt((dx * dx) + (dy * dy));
      if (dist <= max_dist)
      {
        const double rel_dist = dist / max_dist;
        const int r = std::max(1, static_cast<int>(rel_dist * max_color.red()));
        const int g = std::max(1, static_cast<int>(rel_dist * max_color.green()));
        const int b = std::max(1, static_cast<int>(rel_dist * max_color.blue()));
        image.setPixel(x, y, qRgb(r, g, b));
      }
      else
      {
        image.setPixel(x, y, qRgb(0, 0, 0));
      }
    }
  }
  return image;
}

QImage tron::DrawGlobeMinus(const int size, const QColor& max_color)
{
  QImage image = DrawGlobe(size, max_color);
  QPainter painter(&image);
  painter.setPen(QPen(max_color, size / 10));
  painter.drawLine(1 * size / 4, size / 2, 3 * size / 4, size / 2);
  return image;
}

QImage tron::DrawGlobePlus(const int size, const QColor& max_color)
{
  QImage image = DrawGlobe(size, max_color);
  QPainter painter(&image);
  painter.setPen(QPen(max_color, size / 10));
  painter.drawLine(1 * size / 4, size / 2, 3 * size / 4, size / 2);
  painter.drawLine(size / 2, 1 * size / 4, size / 2, 3 * size / 4);
  return image;
}
