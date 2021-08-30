#pragma once

#include <robots_core/game.hh>

namespace robots_core{

class RobotsGame;

struct NullRenderer {
  void render( RobotsGame const & ){};
};

}
