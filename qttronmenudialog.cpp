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
#include "qttronmenudialog.h"

#include <cassert>
#include <functional>

#include <QGuiApplication>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QLabel>
#include <QPushButton>
#include <QScreen>
#include <QSlider>
#include <QVBoxLayout>

#include "qttronaboutdialog.h"
#include "qttrongamedialog.h"
#include "qttrongraphics.h"
#include "qttronpresskeydialog.h"
#include "qttronselectcolordialog.h"

namespace {

constexpr int image_size{64};

void SetFontSize(QWidget * const w, const int point_size, const bool bold = false)
{
  QFont f = w->font();
  f.setPointSize(point_size);
  f.setBold(bold);
  w->setFont(f);
}

QLabel * CreateHeader(const QString& text, QWidget * parent)
{
  QLabel * const label = new QLabel(text, parent);
  label->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
  label->setAlignment(Qt::AlignCenter);
  SetFontSize(label, 14);
  return label;
}

QPixmap GetPixmap(const QString& name)
{
  return QPixmap(":/pics/" + name + ".png").scaled(
    image_size, image_size, Qt::KeepAspectRatio, Qt::FastTransformation);
}

double GetFraction(const QSlider * const slider)
{
  return static_cast<double>(slider->value() - slider->minimum())
    / static_cast<double>(slider->maximum() - slider->minimum());
}

/// A row with a name, a slider and a label showing the slider's value
QSlider * AddSliderRow(
  QVBoxLayout * const layout,
  const QString& name,
  const int min, const int max, const int value,
  const std::function<QString(const QSlider*)>& describe,
  QWidget * parent)
{
  QLabel * const label_name = new QLabel(name, parent);
  label_name->setFixedWidth(80);
  SetFontSize(label_name, 14);
  QSlider * const slider = new QSlider(Qt::Horizontal, parent);
  slider->setRange(min, max);
  slider->setValue(value);
  QLabel * const label_value = new QLabel(parent);
  label_value->setFixedWidth(100);
  label_value->setAlignment(Qt::AlignCenter);
  SetFontSize(label_value, 14);
  const auto update = [=]() { label_value->setText(describe(slider)); };
  QObject::connect(slider, &QSlider::valueChanged, parent, update);
  update();

  QHBoxLayout * const row = new QHBoxLayout;
  row->addWidget(label_name);
  row->addWidget(slider, 1);
  row->addWidget(label_value);
  layout->addLayout(row);
  return slider;
}

} //~namespace

QString tron::GetSpeedText(const double fraction)
{
  if (fraction < 0.10) return "Slowest";
  if (fraction < 0.40) return "Slow";
  if (fraction < 0.60) return "Normal";
  if (fraction < 0.90) return "Fast";
  return "Fastest";
}

QString tron::GetSizeText(const double fraction)
{
  if (fraction < 0.10) return "Smallest";
  if (fraction < 0.40) return "Small";
  if (fraction < 0.60) return "Medium";
  if (fraction < 0.90) return "Large";
  return "Largest";
}

tron::QtTronMenuDialog::QtTronMenuDialog(const GameType type, QWidget * parent)
  : QDialog(parent),
    m_type{type},
    m_players{},
    m_n_players{2},
    m_donut_shaped_world{true},
    m_can_walk_through_own_color{false},
    m_label_n_players{nullptr},
    m_slider_speed{nullptr},
    m_slider_width{nullptr},
    m_slider_height{nullptr},
    m_slider_angles{nullptr},
    m_label_world_chosen{nullptr},
    m_label_own_color_chosen{nullptr},
    m_button_start{nullptr}
{
  setWindowTitle(
    m_type == GameType::classic
    ? QString::fromUtf8("Classic Tron (C) 2008 Richèl Bilderbeek")
    : QString::fromUtf8("Super Tron (C) 2008 Richèl Bilderbeek"));

  QVBoxLayout * const layout = new QVBoxLayout(this);

  //Number of players
  {
    layout->addWidget(CreateHeader("Number of players (+/-)", this));
    QtClickableLabel * const minus = new QtClickableLabel(this);
    minus->setPixmap(QPixmap::fromImage(DrawGlobeMinus(image_size, QColor(255, 196, 196))));
    QtClickableLabel * const plus = new QtClickableLabel(this);
    plus->setPixmap(QPixmap::fromImage(DrawGlobePlus(image_size, QColor(196, 196, 255))));
    m_label_n_players = new QLabel(this);
    m_label_n_players->setAlignment(Qt::AlignCenter);
    SetFontSize(m_label_n_players, 48, true);
    connect(minus, &QtClickableLabel::clicked, this, [this]() { SetNumberOfPlayers(m_n_players - 1); });
    connect(plus , &QtClickableLabel::clicked, this, [this]() { SetNumberOfPlayers(m_n_players + 1); });
    QHBoxLayout * const row = new QHBoxLayout;
    row->addWidget(minus);
    row->addWidget(m_label_n_players, 1);
    row->addWidget(plus);
    layout->addLayout(row);
  }

  //Speed, width, height
  m_slider_speed = AddSliderRow(layout, "Speed", 0, 100, 90,
    [](const QSlider * s) { return GetSpeedText(GetFraction(s)); }, this);
  m_slider_width = AddSliderRow(layout, "Width", 0, 100, 0,
    [](const QSlider * s) { return GetSizeText(GetFraction(s)); }, this);
  m_slider_height = AddSliderRow(layout, "Height", 0, 100, 0,
    [](const QSlider * s) { return GetSizeText(GetFraction(s)); }, this);
  if (m_type == GameType::super)
  {
    m_slider_angles = AddSliderRow(layout, "Angles", 4, 50, 8,
      [](const QSlider * s) { return QString::number(s->value()); }, this);
  }

  //World shape and own color harm: [option A] [chosen] [option B]
  const auto add_choice_row = [this, layout](
    const QString& header,
    const QString& pic_a, const std::function<void()>& on_a,
    QLabel *& chosen,
    const QString& pic_b, const std::function<void()>& on_b)
  {
    layout->addWidget(CreateHeader(header, this));
    QtClickableLabel * const a = new QtClickableLabel(this);
    a->setPixmap(GetPixmap(pic_a));
    connect(a, &QtClickableLabel::clicked, this, on_a);
    QtClickableLabel * const b = new QtClickableLabel(this);
    b->setPixmap(GetPixmap(pic_b));
    connect(b, &QtClickableLabel::clicked, this, on_b);
    chosen = new QLabel(this);
    chosen->setAlignment(Qt::AlignCenter);
    QHBoxLayout * const row = new QHBoxLayout;
    row->addWidget(a);
    row->addWidget(chosen, 1);
    row->addWidget(b);
    layout->addLayout(row);
  };
  add_choice_row("World shape (S)",
    "world_flat", [this]() { SetDonutShapedWorld(false); },
    m_label_world_chosen,
    "world_donut", [this]() { SetDonutShapedWorld(true); });
  add_choice_row("Own color harm (H)",
    "own_color_harmless", [this]() { SetCanWalkThroughOwnColor(true); },
    m_label_own_color_chosen,
    "own_color_harmful", [this]() { SetCanWalkThroughOwnColor(false); });

  //Buttons
  {
    QPushButton * const button_calibrate = new QPushButton("&Calibrate", this);
    SetFontSize(button_calibrate, 14);
    connect(button_calibrate, &QPushButton::clicked, this, &QtTronMenuDialog::OnCalibrate);
    layout->addWidget(button_calibrate);

    m_button_start = new QPushButton("S&tart", this);
    SetFontSize(m_button_start, 14);
    m_button_start->setEnabled(false);
    m_button_start->setToolTip("Calibrate first");
    connect(m_button_start, &QPushButton::clicked, this, &QtTronMenuDialog::OnStart);
    QPushButton * const button_about = new QPushButton("&About", this);
    SetFontSize(button_about, 14);
    connect(button_about, &QPushButton::clicked, this, &QtTronMenuDialog::OnAbout);
    QHBoxLayout * const row = new QHBoxLayout;
    row->addWidget(m_button_start);
    row->addWidget(button_about);
    layout->addLayout(row);
  }

  //Do not let a button take Enter
  for (QPushButton * const b : findChildren<QPushButton*>()) b->setAutoDefault(false);

  SetNumberOfPlayers(2);
  SetDonutShapedWorld(true);
  SetCanWalkThroughOwnColor(false);
  setFixedWidth(320);
}

void tron::QtTronMenuDialog::SetNumberOfPlayers(const int n)
{
  if (n < 2 || n > 20) return;
  if (n != m_n_players)
  {
    m_n_players = n;
    m_button_start->setEnabled(false); //Must calibrate again
  }
  m_label_n_players->setText(QString::number(m_n_players));
}

void tron::QtTronMenuDialog::SetDonutShapedWorld(const bool donut)
{
  m_donut_shaped_world = donut;
  m_label_world_chosen->setPixmap(GetPixmap(donut ? "world_donut" : "world_flat"));
  m_label_world_chosen->setToolTip(donut ? "Donut-shaped world" : "Flat world");
}

void tron::QtTronMenuDialog::SetCanWalkThroughOwnColor(const bool can)
{
  m_can_walk_through_own_color = can;
  m_label_own_color_chosen->setPixmap(GetPixmap(can ? "own_color_harmless" : "own_color_harmful"));
  m_label_own_color_chosen->setToolTip(can ? "Can walk through own color" : "Cannot walk through own color");
}

tron::GameSettings tron::QtTronMenuDialog::GetSettings() const
{
  const QSize screen = QGuiApplication::primaryScreen()
    ? QGuiApplication::primaryScreen()->size()
    : QSize(1024, 768);
  GameSettings s;
  s.type = m_type;
  s.width  = 100 + static_cast<int>(GetFraction(m_slider_width ) * static_cast<double>(screen.width()  - 200));
  s.height = 100 + static_cast<int>(GetFraction(m_slider_height) * static_cast<double>(screen.height() - 200));
  s.donut_shaped_world = m_donut_shaped_world;
  s.can_walk_through_own_color = m_can_walk_through_own_color;
  s.n_angles = m_slider_angles ? m_slider_angles->value() : 4;
  return s;
}

int tron::QtTronMenuDialog::GetInterval() const
{
  const int interval = static_cast<int>(1000.0 - (GetFraction(m_slider_speed) * 999.0));
  assert(interval > 0);
  return interval;
}

void tron::QtTronMenuDialog::keyPressEvent(QKeyEvent * e)
{
  switch (e->key())
  {
    case Qt::Key_Plus: SetNumberOfPlayers(m_n_players + 1); break;
    case Qt::Key_Minus: SetNumberOfPlayers(m_n_players - 1); break;
    case Qt::Key_S: SetDonutShapedWorld(!m_donut_shaped_world); break;
    case Qt::Key_H: SetCanWalkThroughOwnColor(!m_can_walk_through_own_color); break;
    default: QDialog::keyPressEvent(e);
  }
}

void tron::QtTronMenuDialog::OnAbout()
{
  QtTronAboutDialog d(m_type, this);
  d.exec();
}

void tron::QtTronMenuDialog::OnCalibrate()
{
  std::vector<Player> players(m_n_players);
  for (int i = 0; i != m_n_players; ++i)
  {
    {
      QtTronPressKeyDialog d(QString("Player #%1, please press key for 'Turn left'.").arg(i + 1), this);
      d.exec();
      players[i].key_left = d.GetKey();
    }
    {
      QtTronPressKeyDialog d(QString("Player #%1, please press key for 'Turn right'.").arg(i + 1), this);
      d.exec();
      players[i].key_right = d.GetKey();
    }
    {
      QtTronSelectColorDialog d(this);
      d.exec();
      players[i].color = GetPlayerColor(d.GetColorIndex());
    }
  }
  m_players = players;
  m_button_start->setEnabled(true);
  m_button_start->setToolTip(QString());
  m_button_start->setFocus();
}

void tron::QtTronMenuDialog::OnStart()
{
  assert(static_cast<int>(m_players.size()) == m_n_players);
  QtTronGameDialog d(GetSettings(), m_players, GetInterval(), this);
  d.exec();
}
