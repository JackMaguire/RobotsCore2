#pragma once

#include <robots_core/board.hh>
#include <robots_core/null_renderer.hh>

#include <array>

namespace robots_core{

//TODO compress this
struct MoveLegacy {
  signed char dx = -2; //nullop
  signed char dy = -2; //nullop
  bool nullop = true;
};

//TODO maybe this is too small??
class Move1 {
private:
  char data = 0;

public:
  bool nullop() const {
    return data == 0;
  }

  void
  set_dx( int const move ){
    switch( move ){
    case( -1 ):
      break;
    case( 0 ):
      break;
    case( 1 ):
      break;
    }
  }

  signed char dx() const {
    switch( data & 0b11 ){
    case( 1 ):       return 1;
    case( 2 ):       return -1;
    case( 3 ):       return 0;
    }
    return 0;
  }

  signed char dy() const {
    switch( (data & 0b1100) >> 2 ){
    case( 1 ):       return 1;
    case( 2 ):       return -1;
    case( 3 ):       return 0;
    }
    return 0;
  }
};

struct Move2 { //2 bytes
  signed char dx = -2;
  signed char dy = -2;

  bool nullop() const {
    return dx == -2 and dy == -2;
  }

  void set_nullop( bool const setting ) {
    if( setting ) {
      dx = -2;
      dy = -2;
    }
  }

};

using Move = Move2;

//static_assert( sizeof(Move) );

template< int DEPTH >
struct SearchResult {
  //path
  std::array< Move, DEPTH > moves; //default construct to nullop

  //outcome
  bool cascade = false;
  int nrobots_killed_cascading = -1; //-1 UNLESS CASCADE

  bool
  is_better_than( SearchResult< DEPTH > const & other ) const {

    if( cascade ){

      if( other.cascade ) return nrobots_killed_cascading > other.nrobots_killed_cascading;
      else                return true;

    } else {

      if( other.cascade ) return false;
      else                return false; //both are losers

    }
  }
};

template< int TOTAL_DEPTH >
SearchResult< TOTAL_DEPTH >
_recursive_search(
  Board const & board,
  int const min_sufficient_robots_killed,
  std::array< Move, TOTAL_DEPTH > const & moves,
  int const min_n_robots,
  int const recursion_round, //first call is 0
  bool const check_for_cascade
){

  using ResultType = SearchResult< TOTAL_DEPTH >;

  // Check for termination

  // Case 1: Valid Solution
  if( check_for_cascade ){
    if( board.move_is_cascade_safe( 0, 0 ) ){
      ResultType result;
      result.moves = moves;
      result.cascade = true;
      result.nrobots_killed_cascading = board.n_robots();
      return result;
    }
  }


  if(
    (recursion_round == TOTAL_DEPTH) // Case 2: Max Depth
    or
    (board.n_robots() < min_n_robots) // Case 3: Not Enough Robots
  ){
    ResultType result;
    result.moves = moves;
    result.cascade = false;
    result.nrobots_killed_cascading = -1;
    return result;
  }

  // Propogate
  ResultType best_result;
  Position hpos = board.human_position();

  bool min_sufficient_robots_killed_met = false;

  for( int dx = -1; dx <= 1; ++dx ){
    for( int dy = -1; dy <= 1; ++dy ){
      switch( board.cell( hpos.x + dx, hpos.y + dy ) ){
      case( Occupant::ROBOT ):
      case( Occupant::FIRE ):
      case( Occupant::OOB ):
	continue;
      case( Occupant::EMPTY ):
      case( Occupant::HUMAN ):
	break;
      }

      Board copy( board );
      MoveResult const move_result = copy.move_human( dx, dy );
      ResultType result;
      result.moves = moves;

      switch( move_result ){
      case( MoveResult::YOU_LOSE ):
	continue;

      case( MoveResult::YOU_WIN_ROUND ):
      case( MoveResult::YOU_WIN_GAME ):
	result.moves[ recursion_round ].dx = dx;
	result.moves[ recursion_round ].dy = dy;
	result.moves[ recursion_round ].set_nullop( false );
	result.cascade = true; //technically...
	result.nrobots_killed_cascading = 0;
	break;

      case( MoveResult::CONTINUE ):
	result.moves[ recursion_round ].dx = dx;
	result.moves[ recursion_round ].dy = dy;
	result.moves[ recursion_round ].set_nullop( false );

	int const inner_suff_cutoff = std::min( min_sufficient_robots_killed, copy.n_robots() );

	ResultType const best_subresult =
	  _recursive_search< TOTAL_DEPTH >(
	    copy,
	    inner_suff_cutoff, //min_sufficient_robots_killed,
	    result.moves,
	    min_n_robots,
	    recursion_round + 1,
	    not( dx == 0 and dy == 0 )
	  );

	result = best_subresult;
	break;
      }

      if( result.is_better_than( best_result ) ){
	best_result = result;
      }

      min_sufficient_robots_killed_met = best_result.nrobots_killed_cascading >= min_sufficient_robots_killed;

      if( min_sufficient_robots_killed_met ) break;
    }
    if( min_sufficient_robots_killed_met ) break;
  }

  return best_result;
}

template< int MAX_DEPTH >
SearchResult< MAX_DEPTH >
recursive_search_for_cascade(
  Board const & board,
  int const min_sufficient_robots_killed = 10,
  int const min_n_robots = 0
){
  std::array< Move, MAX_DEPTH > moves;
  return _recursive_search< MAX_DEPTH >(
    board,
    min_sufficient_robots_killed,
    moves,
    min_n_robots,
    0,
    true
  );
}

template< unsigned int DEPTH, typename Renderer = NullRenderer >
bool
run_recursive_seach(
  RobotsGame & game,
  int const n_tele_desired,
  Renderer & renderer
){

  int const min_n_robots = std::min(
    n_tele_desired - game.n_safe_teleports_remaining(),
    game.board().n_robots()
  );

  SearchResult< DEPTH > const search_result =
    recursive_search_for_cascade< DEPTH >( game.board() );

  if( not search_result.cascade ){
    return false;
  }

  if( search_result.nrobots_killed_cascading < min_n_robots ){
    return false;
  }

  for( Move const m : search_result.moves ){
    if( m.nullop() ) break;

    game.move_human( m.dx, m.dy );
    renderer.render( game );
  }

  return true;
}

}
