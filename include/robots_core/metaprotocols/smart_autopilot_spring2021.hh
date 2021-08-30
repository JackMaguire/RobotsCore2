#pragma once

#include <robots_core/board.hh>
#include <robots_core/null_renderer.hh>
#include <robots_core/strategy/recursion.hh>
#include <robots_core/strategy/stall.hh>
#include <robots_core/metaprotocols/skip_to_risky_spring2021.hh>

#include <array>

namespace robots_core{

template< typename Renderer = NullRenderer >
void
loop_smart_autopilot_spring2021(
  RobotsGame & game,
  Renderer & renderer
){
  while( true ){
    //Each call returns a GameOverBool

    if( skip_to_risky_w_recursion( ms ) ) break; //TODO

    ///////////////////
    // Deeper Recursion
    bool const recursive_solution_found =
      run_recursive_search< 6 >( game, 7, renderer );
    if( recursive_solution_found ){
      game.cascade();
      continue;
    }

    //////////////////
    // Stall For Time
    bool const any_stall_steps_taken =
      stall_for_time( game, renderer );
    if( any_stall_steps_taken ) {
      //run recursion again
      bool const recursive_solution_found2 =
	run_recursive_search< 6 >( game, 7, renderer );
      if( recursive_solution_found2 ) continue;
    }

    
    /////////////////
    // Maybe Teleport
    if( game_.n_safe_teleports_remaining() > 0 ) {
      MoveResult const tele_result = game.teleport();
      if( tele_result == MoveResult::YOU_WIN_GAME or 
	tele_result == MoveResult::YOU_LOSE ){
	break;
      }
    } else {
      // Risky Moves?
      bool const game_over = loop_autopilot( -1, ms, 1 );
      if( game_over ) break;
    }

  }
}

}
