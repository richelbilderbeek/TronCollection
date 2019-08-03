


#include <QApplication>
#include "qttrongamedialog.h"


int main(int argc, char *argv[])
{
  QApplication a(argc, argv);
  ribi::tron::QtTronGameDialog w;
  w.show();
  return a.exec();
}
