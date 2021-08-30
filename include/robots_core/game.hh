#pragma once

#include <robots_core/global.hh>
#include <robots_core/board.hh>
#include <robots_core/null_renderer.hh>
#include <robots_core/asserts.hh>

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

class RobotsGame {
public: //state management
  RobotsGame( int const round = 1, int const tele = 0 );

  void
  load_from_stringified_representation(
    std::string const & str,  int const round,
    int const n_safe_teles,   long int const score
  );

  void reset();

public: //game logic
  template< unsigned int sleepsize = 0 >
  GameOverBool
  cascade(){  
    NullRenderer nr;
    return cascade< sleepsize >( nr );
  }

  template< typename Renderer, unsigned int sleepsize = 0 >
  GameOverBool
  cascade( Renderer && updater );

  //true if game over
  GameOverBool
  move_human( sm_int const dx, sm_int const dy );

  GameOverBool
  teleport();

public: //getters
  Board const & board() const {
    return board_;
  }

  int n_safe_teleports_remaining() const {
    return n_safe_teleports_remaining_;
  }

  int round() const {
    return round_;
  }

  MoveResult latest_result() const {
    return latest_result_;
  }

  long int score() const {
    return score_;
  }

protected: //teleportation utilities
  void danger_tele();

  void safe_tele();

protected: //state management utilities
  template< unsigned int sleepsize = 0 >
  void
  new_round();
  
private:
  Board board_;

  int round_;
  int n_safe_teleports_remaining_;

  long int score_ = 0;
  
  MoveResult latest_result_ = MoveResult::CONTINUE;
};

RobotsGame::RobotsGame( int const round, int const tele ) :
  round_( round ),
  n_safe_teleports_remaining_( tele )
{
  board_.init( round_ );

  long int expected_score = 0;
  for( int r = 1; r < round_; ++r ){
    expected_score += nrobots_per_round( r );
  }
  score_ = expected_score;
}

template< unsigned int sleepsize >
void
RobotsGame::new_round(){

  long int expected_score = 0;
  for( int r = 1; r <= round_; ++r ){
    expected_score += r * 10;
  }

  if( score_ != expected_score ){
    std::cout << "Expected score is " << expected_score << std::endl;
  }

  if( round_ == MAX_N_ROUNDS ){
    latest_result_ = MoveResult::YOU_WIN_GAME;
  } else {
    board_.init( ++round_ );
    if constexpr( sleepsize > 0 ) std::this_thread::sleep_for( std::chrono::milliseconds(sleepsize) );
  }
}

template< typename T, unsigned int sleepsize >
GameOverBool
RobotsGame::cascade( T && updater ){
  int const n_robots_start = board_.n_robots();

  latest_result_ = MoveResult::CONTINUE;
  while( latest_result_ == MoveResult::CONTINUE ){
    latest_result_ = board_.move_robots_1_step();
    updater( *this );
    if constexpr( sleepsize > 0 ) std::this_thread::sleep_for( std::chrono::milliseconds(sleepsize) );
  }

  if( latest_result_ == MoveResult::YOU_WIN_ROUND ){
    score_ += n_robots_start;
    n_safe_teleports_remaining_ += n_robots_start;
    if( n_safe_teleports_remaining_ > 10 ) n_safe_teleports_remaining_ = 10;
    new_round< sleepsize >();
  }

  return latest_result_ == MoveResult::YOU_LOSE || latest_result_ == MoveResult::YOU_WIN_GAME;
}

GameOverBool
RobotsGame::move_human( sm_int const dx, sm_int const dy ){
  int const n_robots_start = board_.n_robots();

  latest_result_ = board_.move_human( dx, dy );

  score_ += ( n_robots_start - board_.n_robots() );

  if( latest_result_ == MoveResult::YOU_WIN_ROUND ){
    new_round();
  }

  return latest_result_ == MoveResult::YOU_LOSE || latest_result_ == MoveResult::YOU_WIN_GAME;
}

void 
RobotsGame::danger_tele(){
  latest_result_ = board_.teleport( false );
  if( latest_result_ == MoveResult::CONTINUE ){
    RC_DEBUG_ASSERT( board_.n_robots() > 0 );
  }
}

void
RobotsGame::safe_tele(){
  latest_result_ = board_.teleport( true );
  --n_safe_teleports_remaining_;
}

GameOverBool
RobotsGame::teleport(){
  int const n_robots_start = board_.n_robots();
    
  if( n_safe_teleports_remaining_ == 0 ) danger_tele();
  else safe_tele();

  score_ += ( n_robots_start - board_.n_robots() );
    
  if( latest_result_ == MoveResult::YOU_WIN_ROUND ){
    new_round();
  }
  else if( latest_result_ == MoveResult::CONTINUE ){
    RC_DEBUG_ASSERT( board_.n_robots() > 0 );
  }

  GameOverBool const game_over =
    (latest_result_ == MoveResult::YOU_LOSE || latest_result_ == MoveResult::YOU_WIN_GAME);
  return game_over;
}

void
RobotsGame::load_from_stringified_representation(
  std::string const & str,
  int const round,
  int const n_safe_teleports_remaining,
  long int const score
) {
  board_.load_from_stringified_representation( str );
  round_ = round;
  n_safe_teleports_remaining_ = n_safe_teleports_remaining;
  score_ = score;
}

void
RobotsGame::reset(){
  board_ = Board();
  round_ = 0;
  n_safe_teleports_remaining_ = 0;
  score_ = 0;
  latest_result_ = MoveResult::CONTINUE;
  new_round();
}

} // namespace robots_core
