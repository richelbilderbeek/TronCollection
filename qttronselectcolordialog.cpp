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
#include "qttronselectcolordialog.h"

#include <QKeyEvent>
#include <QMouseEvent>
#include <QPainter>

#include "trongame.h"

namespace {

QString GetKeyName(const int index)
{
  if (index < 10) return QString("F%1").arg(index + 1);
  return QString::number((index - 9) % 10);
}

} //~namespace

tron::QtTronSelectColorDialog::QtTronSelectColorDialog(QWidget * parent)
  : QDialog(parent, Qt::FramelessWindowHint | Qt::Dialog),
    m_color_index{-1}
{
  setWindowTitle("Please select your player's color");
  setFixedSize(GetNumberOfColors() * sm_cell_size + 2, (2 * sm_cell_size) + 1);
  setCursor(Qt::PointingHandCursor);
}

int tron::QtTronSelectColorDialog::KeyToColorIndex(const int key) noexcept
{
  if (key >= Qt::Key_F1 && key <= Qt::Key_F10) return key - Qt::Key_F1;
  if (key >= Qt::Key_1 && key <= Qt::Key_9) return 10 + (key - Qt::Key_1);
  if (key == Qt::Key_0) return 19;
  return -1;
}

void tron::QtTronSelectColorDialog::keyPressEvent(QKeyEvent * e)
{
  if (e->isAutoRepeat()) return;
  const int index = KeyToColorIndex(e->key());
  if (index == -1) return;
  m_color_index = index;
  accept();
}

void tron::QtTronSelectColorDialog::mousePressEvent(QMouseEvent * e)
{
  #if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
  const int x = static_cast<int>(e->position().x());
  #else
  const int x = e->pos().x();
  #endif
  const int index = (x - 1) / sm_cell_size;
  if (index < 0 || index >= GetNumberOfColors()) return;
  m_color_index = index;
  accept();
}

void tron::QtTronSelectColorDialog::paintEvent(QPaintEvent *)
{
  QPainter painter(this);
  painter.fillRect(rect(), palette().window());
  QFont f = painter.font();
  f.setPointSize(11);
  f.setBold(true);
  painter.setFont(f);
  for (int i = 0; i != GetNumberOfColors(); ++i)
  {
    const QRect color_rect(1 + (i * sm_cell_size), 1, sm_cell_size, sm_cell_size);
    painter.fillRect(color_rect, QColor(GetPlayerColor(i)));
    const QRect key_rect(1 + (i * sm_cell_size), 1 + sm_cell_size, sm_cell_size - 1, sm_cell_size - 1);
    painter.setPen(palette().mid().color());
    painter.drawRect(key_rect);
    painter.setPen(palette().windowText().color());
    painter.drawText(key_rect, Qt::AlignCenter, GetKeyName(i));
  }
}
