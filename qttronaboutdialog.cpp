#include "qttronaboutdialog.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QPlainTextEdit>
#include <QVBoxLayout>

namespace {

QLabel * CreateLabel(const QString& text, const int point_size, QWidget * parent)
{
  QLabel * const label = new QLabel(text, parent);
  label->setAlignment(Qt::AlignCenter);
  QFont f = label->font();
  f.setPointSize(point_size);
  label->setFont(f);
  return label;
}

} //~namespace

tron::QtTronAboutDialog::QtTronAboutDialog(const GameType type, QWidget * parent)
  : QDialog(parent)
{
  const bool is_classic = type == GameType::classic;
  const QString name = is_classic ? "Classic Tron" : "Super Tron";
  const QString author = QString::fromUtf8("Richèl Bilderbeek");
  setWindowTitle("About");

  //Picture of the author
  QLabel * const picture = new QLabel(this);
  const QPixmap pixmap(is_classic ? ":/pics/author_classic.png" : ":/pics/author_super.png");
  picture->setPixmap(pixmap.scaledToHeight(360, Qt::SmoothTransformation));
  picture->setAlignment(Qt::AlignCenter);

  //Text
  QVBoxLayout * const text = new QVBoxLayout;
  text->addWidget(CreateLabel(name, 32, this));
  text->addSpacing(24);
  text->addWidget(CreateLabel(QString::fromUtf8("(C) 2008 ") + author, 18, this));
  text->addSpacing(12);
  text->addWidget(CreateLabel("programmed by", 20, this));
  text->addWidget(CreateLabel(author, 20, this));
  text->addWidget(CreateLabel(is_classic ? "on the 1st and 2nd" : "on the 26th October", 20, this));
  text->addWidget(CreateLabel(is_classic ? "January of 2008" : "of 2008", 20, this));
  text->addSpacing(24);
  text->addWidget(CreateLabel("Licenced under GPL 3.0", 14, this));
  QLabel * const url = CreateLabel(
    "<a href=\"http://www.richelbilderbeek.nl\">http://www.richelbilderbeek.nl</a>", 14, this);
  url->setOpenExternalLinks(true);
  text->addWidget(url);
  text->addStretch();

  QHBoxLayout * const top = new QHBoxLayout;
  top->addWidget(picture);
  top->addLayout(text, 1);

  //Licence
  QPlainTextEdit * const licence = new QPlainTextEdit(this);
  licence->setReadOnly(true);
  licence->setPlainText(
    "  " + name + ", a simple game\n"
    "  Copyright (C) 2008  " + author + "\n"
    "\n"
    "  This program is free software: you can redistribute it and/or modify\n"
    "  it under the terms of the GNU General Public License as published by\n"
    "  the Free Software Foundation, either version 3 of the License, or\n"
    "  (at your option) any later version.\n"
    "\n"
    "  This program is distributed in the hope that it will be useful,\n"
    "  but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
    "  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n"
    "  GNU General Public License for more details.\n"
    "\n"
    "  You should have received a copy of the GNU General Public License\n"
    "  along with this program.  If not, see <http://www.gnu.org/licenses/>.\n");
  licence->setFixedHeight(110);

  QVBoxLayout * const layout = new QVBoxLayout(this);
  layout->addLayout(top, 1);
  layout->addWidget(licence);
  resize(690, 540);
}
