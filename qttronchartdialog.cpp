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
#include "qttronchartdialog.h"

#include <algorithm>
#include <cmath>

#include <QHBoxLayout>
#include <QPainter>
#include <QPushButton>
#include <QTimer>
#include <QVBoxLayout>

tron::QtTronChartDialog::QtTronChartDialog(
  const std::vector<Player>& players, QWidget * parent)
  : QDialog(parent, Qt::FramelessWindowHint | Qt::Dialog),
    m_can_close{false},
    m_button_close{new QPushButton("Close", this)}
{
  setWindowTitle("Losses and wins per player");
  m_button_close->setEnabled(false);
  connect(m_button_close, &QPushButton::clicked, this, &QDialog::accept);

  QHBoxLayout * const top = new QHBoxLayout;
  top->addWidget(m_button_close);
  top->addStretch();

  QVBoxLayout * const layout = new QVBoxLayout(this);
  layout->addLayout(top);
  layout->addWidget(new QtTronChartWidget(players, this), 1);
  resize(688, 446);

  QTimer::singleShot(1000, this, [this]()
  {
    m_can_close = true;
    m_button_close->setEnabled(true);
    m_button_close->setDefault(true);
    m_button_close->setFocus();
  });
}

void tron::QtTronChartDialog::reject()
{
  if (m_can_close) QDialog::reject();
}

tron::QtTronChartWidget::QtTronChartWidget(
  const std::vector<Player>& players, QWidget * parent)
  : QWidget(parent),
    m_players{players}
{
  setMinimumSize(400, 300);
}

void tron::QtTronChartWidget::paintEvent(QPaintEvent *)
{
  QPainter painter(this);
  painter.setRenderHint(QPainter::Antialiasing);
  painter.fillRect(rect(), Qt::white);

  const QColor color_losses(Qt::red);
  const QColor color_wins(Qt::darkGreen);

  //Title
  QFont title_font = painter.font();
  title_font.setPointSize(16);
  painter.setFont(title_font);
  painter.setPen(Qt::black);
  const QRect title_rect(0, 0, width(), 40);
  painter.drawText(title_rect, Qt::AlignCenter, "Losses and wins per player");

  QFont font = painter.font();
  font.setPointSize(10);
  painter.setFont(font);

  //Legend
  {
    int x = width() - 160;
    const int y = 48;
    for (const auto& item : { std::make_pair(color_losses, QString("Losses")),
                              std::make_pair(color_wins, QString("Wins")) })
    {
      painter.fillRect(QRect(x, y, 12, 12), item.first);
      painter.drawText(QRect(x + 16, y - 2, 60, 16), Qt::AlignLeft | Qt::AlignVCenter, item.second);
      x += 76;
    }
  }

  const int n = static_cast<int>(m_players.size());
  if (n == 0) return;

  int max_value = 1;
  for (const Player& p : m_players)
  {
    max_value = std::max(max_value, std::max(p.n_losses, p.n_wins));
  }

  const QRect plot(50, 70, width() - 70, height() - 130);

  //Y axis with ticks
  const int tick_step = std::max(1, static_cast<int>(std::ceil(max_value / 5.0)));
  const int y_max = ((max_value + tick_step - 1) / tick_step) * tick_step;
  painter.setPen(Qt::black);
  painter.drawLine(plot.bottomLeft(), plot.topLeft());
  painter.drawLine(plot.bottomLeft(), plot.bottomRight());
  for (int v = 0; v <= y_max; v += tick_step)
  {
    const int y = plot.bottom() - (v * plot.height() / y_max);
    painter.setPen(QColor(220, 220, 220));
    if (v != 0) painter.drawLine(plot.left() + 1, y, plot.right(), y);
    painter.setPen(Qt::black);
    painter.drawText(QRect(0, y - 8, plot.left() - 6, 16), Qt::AlignRight | Qt::AlignVCenter, QString::number(v));
  }

  //Bars
  const double group_width = static_cast<double>(plot.width()) / static_cast<double>(n);
  const double bar_width = group_width * 0.35;
  for (int i = 0; i != n; ++i)
  {
    const Player& p = m_players[i];
    const double group_left = plot.left() + (i * group_width);
    const double mid = group_left + (group_width / 2.0);
    const int values[2] = { p.n_losses, p.n_wins };
    const QColor colors[2] = { color_losses, color_wins };
    for (int j = 0; j != 2; ++j)
    {
      const double bar_height = static_cast<double>(values[j] * plot.height()) / static_cast<double>(y_max);
      const QRectF bar(
        mid - bar_width + (j * bar_width),
        plot.bottom() - bar_height,
        bar_width,
        bar_height);
      painter.fillRect(bar, colors[j]);
      painter.setPen(Qt::black);
      painter.drawText(
        QRectF(bar.left(), bar.top() - 18, bar_width, 16),
        Qt::AlignCenter, QString::number(values[j]));
    }
    //Player label, with the player's color
    const QRectF swatch(mid - 6, plot.bottom() + 6, 12, 12);
    painter.fillRect(swatch, QColor(p.color));
    painter.setPen(Qt::black);
    painter.drawRect(swatch);
    painter.drawText(QRectF(group_left, plot.bottom() + 20, group_width, 16),
      Qt::AlignCenter, QString::number(i + 1));
  }

  //X axis title
  painter.drawText(QRect(plot.left(), height() - 22, plot.width(), 20), Qt::AlignCenter, "Player");
}
