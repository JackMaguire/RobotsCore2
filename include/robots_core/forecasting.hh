#pragma once

#include <robots_core/global.hh>
#include <robots_core/board.hh>

#include <array>
#include <vector>
#include <set>

#include <cstdlib> //rand()
#include <random>
#include <iostream>
#include <algorithm>
#include <sstream>
#include <thread>         // std::this_thread::sleep_for
#include <chrono>         // std::chrono::seconds
#include <math.h> //sqrt

namespace robots_core {

struct ForecastResults {
  bool legal = false;
  bool cascade_safe = false;
  unsigned int robots_killed = 0;
};

ForecastResults
forecast_move( Board const & board, sm_int const dx, sm_int const dy ){
  ForecastResults results;
  Board copy = board;

  //std::cout << board.n_robots() << " " << copy.n_robots() << std::endl;

  MoveResult move_result = copy.move_human( dx, dy );
  //std::cout << "    " << int( move_result ) << std::endl;

  if( move_result == MoveResult::YOU_LOSE ) return results;  

  results.legal = true;
  results.robots_killed = board.n_robots() - copy.n_robots();

  //Check for cascade safety
  while ( move_result == MoveResult::CONTINUE ){
    move_result = copy.move_human( 0, 0 );
  }
  results.cascade_safe = (move_result != MoveResult::YOU_LOSE);

  return results;
}


std::array< std::array< ForecastResults, 3 >, 3 >
forecast_all_moves( Board const & board ) {
  std::array< std::array< ForecastResults, 3 >, 3 > forecasts;

  Position const human_position = board.human_position();

  for( sm_int dx = -1; dx <= 1; ++dx ){
    if( human_position.x+dx < 0 || human_position.x+dx >= Board::WIDTH ) continue;

    for( sm_int dy = -1; dy <= 1; ++dy ){
      if( human_position.y+dy < 0 || human_position.y+dy >= Board::HEIGHT ) continue;

      forecasts[ dx+1 ][ dy+1 ] = forecast_move( board, dx, dy );
    }
  }

  return forecasts;
}

} // namespace robots_core
