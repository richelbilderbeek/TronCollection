#pragma GCC diagnostic push


#include <QApplication>
#include "qttrongamedialog.h"
#pragma GCC diagnostic pop

int main(int argc, char *argv[])
{
  QApplication a(argc, argv);
  ribi::tron::QtTronGameDialog w;
  w.show();
  return a.exec();
}
