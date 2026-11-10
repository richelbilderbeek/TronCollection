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
#ifndef QTTRONPRESSKEYDIALOG_H
#define QTTRONPRESSKEYDIALOG_H

#include <QDialog>

namespace tron {

/// Asks the user to press a key. Any key is accepted, including Escape.
class QtTronPressKeyDialog : public QDialog
{
  Q_OBJECT

public:
  QtTronPressKeyDialog(const QString& text, QWidget * parent = nullptr);

  /// The Qt::Key pressed
  int GetKey() const noexcept { return m_key; }

protected:
  bool event(QEvent * e) override;
  void keyPressEvent(QKeyEvent * e) override;
  void reject() override {} //Cannot be cancelled: Escape is a valid key

private:
  int m_key;
};

} //~namespace tron

#endif // QTTRONPRESSKEYDIALOG_H
