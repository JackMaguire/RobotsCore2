#pragma once

#include <robots_core/game.hh>
#include <robots_core/null_renderer.hh>

namespace robots_core{

template< typename Renderer = NullRenderer >
bool
stall_for_time(
  RobotsGame & game,
  Renderer & renderer
){
  bool any_solution_found = false;
  bool solution_found = true;

  while( solution_found ){
    solution_found = false;

    for( int dx = -1; dx <= 1; ++dx ){
      for( int dy = -1; dy <= 1; ++dy ){
        Board copy( game.board() );
        MoveResult const move_result = copy.move_human( dx, dy );

        if( move_result != MoveResult::CONTINUE ) continue;

        solution_found = (copy.n_robots() == game.board().n_robots());

	//GREEDY!
        if( solution_found ) {
          any_solution_found = true;
	  game.move_human( dx, dy );
	  renderer.render( game );
          break;
        }
      }
      if( solution_found ) break;
    }

  }

  return any_solution_found;
}

bool
stall_for_time(
  RobotsGame & game
){
  NullRenderer r;
  return stall_for_time( game, r );
}


}
