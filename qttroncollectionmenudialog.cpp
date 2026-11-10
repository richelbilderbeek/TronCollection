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
#include "qttroncollectionmenudialog.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QVBoxLayout>

#include "qttrongraphics.h"
#include "qttronmenudialog.h"

tron::QtTronCollectionMenuDialog::QtTronCollectionMenuDialog(QWidget * parent)
  : QDialog(parent)
{
  setWindowTitle(QString::fromUtf8("The Tron Collection (C) 2008 Richèl Bilderbeek"));

  QLabel * const title = new QLabel("The Tron Collection", this);
  title->setAlignment(Qt::AlignCenter);
  QFont f = title->font();
  f.setPointSize(40);
  title->setFont(f);

  QtClickableLabel * const classic = new QtClickableLabel(this);
  classic->setPixmap(QPixmap(":/pics/classic_tron.png"));
  classic->setToolTip("Classic Tron");
  connect(classic, &QtClickableLabel::clicked, this, [this]()
  {
    QtTronMenuDialog d(GameType::classic, this);
    d.exec();
  });

  QtClickableLabel * const super = new QtClickableLabel(this);
  super->setPixmap(QPixmap(":/pics/super_tron.png"));
  super->setToolTip("Super Tron");
  connect(super, &QtClickableLabel::clicked, this, [this]()
  {
    QtTronMenuDialog d(GameType::super, this);
    d.exec();
  });

  //Ultimate Tron was never made
  QLabel * const ultimate = new QLabel(this);
  ultimate->setPixmap(QPixmap(":/pics/ultimate_tron.png"));
  ultimate->setToolTip("Ultimate Tron (not available)");

  QHBoxLayout * const games = new QHBoxLayout;
  for (QLabel * const l : { static_cast<QLabel*>(classic), static_cast<QLabel*>(super), ultimate })
  {
    l->setAlignment(Qt::AlignCenter);
    games->addWidget(l);
  }

  QVBoxLayout * const layout = new QVBoxLayout(this);
  layout->addWidget(title);
  layout->addLayout(games);
}
