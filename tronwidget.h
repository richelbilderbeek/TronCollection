#ifndef TRONWIDGET_H
#define TRONWIDGET_H

#include <cassert>

#pragma GCC diagnostic push



#include "tronplayer.h"
#pragma GCC diagnostic pop

namespace ribi {
namespace tron {

struct Widget
{
  Widget(const std::vector<Player>& players,
    const int width,
    const int height
  );
};

} // ~namespace tron
} // ~namespace ribi

#endif // TRONWIDGET_H
