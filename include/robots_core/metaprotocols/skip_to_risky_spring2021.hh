#pragma once

#include <robots_core/board.hh>
#include <robots_core/null_renderer.hh>
#include <robots_core/strategy/recursion.hh>
#include <robots_core/strategy/stall.hh>

#include <array>

namespace robots_core{

template< typename Renderer = NullRenderer >
GameOverBool
skip_to_risky_spring2021(
  RobotsGame & game,
  Renderer & renderer
){
  while( true ) {

    ///////////////////
    // Deeper Recursion
    bool const recursive_solution_found =
      run_recursive_search< 4 >( game, 7, renderer );

    if( recursive_solution_found ){
      bool const game_over_rec = game.cascade();
      renderer( game );
      //if( game_over_rec ) return true; //impossible
      continue;
    }

    // TODO
    foo const outcome = run_ml( game );
    //pick up here
    

    AutoPilotResult const apr =
      run_autopilot( game_, current_prediction_ );

    if( apr.apre == AutoPilotResultEnum::MOVE ){
      Board copy = game_.board();
      int const n_robots_before = copy.n_robots();
      auto const result = copy.move_human( apr.dx, apr.dy );
      if( result == MoveResult::YOU_LOSE ) break;
      if( result == MoveResult::YOU_WIN_GAME ) break;

      int const n_robots_desired = ( 15-game_.n_safe_teleports_remaining() );

      if( copy.n_robots() < n_robots_before ){
        if( copy.n_robots() < n_robots_desired ){
          break;
        }
      }


      if( game_.n_safe_teleports_remaining() < 2 ){
        break;
      }
    }

    bool const game_over =
      handle_move(
        apr.dx,
        apr.dy,
        apr.apre == AutoPilotResultEnum::TELEPORT,
        apr.apre == AutoPilotResultEnum::CASCADE
      );

    this->update();
    app_->processEvents();

    if( game_over ) return true;

    std::this_thread::sleep_for (std::chrono::milliseconds( ms ));
  }

  return false;
}

}
