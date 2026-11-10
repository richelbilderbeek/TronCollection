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
#include "qttronpresskeydialog.h"

#include <QKeyEvent>
#include <QLabel>
#include <QVBoxLayout>

tron::QtTronPressKeyDialog::QtTronPressKeyDialog(
  const QString& text, QWidget * parent)
  : QDialog(parent, Qt::FramelessWindowHint | Qt::Dialog),
    m_key{0}
{
  setWindowTitle(text);
  QLabel * const label = new QLabel(text, this);
  label->setAlignment(Qt::AlignCenter);
  label->setWordWrap(true);
  QFont f = label->font();
  f.setPointSize(16);
  label->setFont(f);
  label->setFrameStyle(QFrame::Panel | QFrame::Raised);
  QVBoxLayout * const layout = new QVBoxLayout(this);
  layout->setContentsMargins(0, 0, 0, 0);
  layout->addWidget(label);
  setFixedSize(480, 189);
}

bool tron::QtTronPressKeyDialog::event(QEvent * e)
{
  //Also catch Tab and Backtab, that are otherwise used to change focus
  if (e->type() == QEvent::KeyPress)
  {
    keyPressEvent(static_cast<QKeyEvent*>(e));
    return true;
  }
  return QDialog::event(e);
}

void tron::QtTronPressKeyDialog::keyPressEvent(QKeyEvent * e)
{
  //Ignore keys held down from a previous dialog
  if (e->isAutoRepeat() || e->key() == 0 || e->key() == Qt::Key_unknown) return;
  m_key = e->key();
  accept();
}
