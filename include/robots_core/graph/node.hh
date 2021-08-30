#pragma once

#include <robots_core/game.hh>
#include <robots_core/forecasting.hh>
#include <robots_core/asserts.hh>

//#include <array>
#include <cmath>  //log
#include <vector>

namespace robots_core{
namespace graph{

enum class SpecialCaseNodes : unsigned char {
  Q = 0,
  W,
  E,
  A,
  S,
  D,
  Z,
  X,
  C,
  LEFT_OOB,
  TOP_OOB,
  RIGHT_OOB,
  BOTTOM_OOB,
  
  count, //these should stay at the end
  none = count //these should stay at the end
};


struct Node {
  Position position;
  SpecialCaseNodes special_case = SpecialCaseNodes::none;

  int dx() const {
    switch( special_case ){
    case SpecialCaseNodes::Q:
    case SpecialCaseNodes::A:
    case SpecialCaseNodes::Z:
      return -1;
    case SpecialCaseNodes::W:
    case SpecialCaseNodes::S:
    case SpecialCaseNodes::X:
      return 0;
    case SpecialCaseNodes::E:
    case SpecialCaseNodes::D:
    case SpecialCaseNodes::C:
      return 1;
    default:
      RC_DEBUG_ASSERT( false );
    }
  };

  int dy() const {
    switch( special_case ){
    case SpecialCaseNodes::Z:
    case SpecialCaseNodes::X:
    case SpecialCaseNodes::C:
      return -1;
    case SpecialCaseNodes::A:
    case SpecialCaseNodes::S:
    case SpecialCaseNodes::D:
      return 0;
    case SpecialCaseNodes::Q:
    case SpecialCaseNodes::W:
    case SpecialCaseNodes::E:
      return 1;
    default:
      RC_DEBUG_ASSERT( false );
    }
  };
};

/*std::vector< Node >
determine_nodes_for_graph(
  RobotsGame const & game,
  unsigned int const max_nodes
){
  //TODO
}*/

}
}
