#pragma once

#include <robots_core/global.hh>

#include <array>
#include <vector>
#include <set>

#include <cstdlib> //rand()
#include <random>
#include <iostream>
#include <algorithm>
#include <sstream>

//#define RC_USE_BOOST_SMALL_VEC //benchmarked - slightly faster with N = MAX_ROBOT / 10

#ifdef RC_USE_BOOST_SMALL_VEC
#include <boost/container/small_vector.hpp>
#endif

namespace robots_core {

constexpr Position STARTING_POSITION({ 23, 15 });

class Board {

public: //constexpr and typing

static constexpr sm_int WIDTH = 45;
static constexpr sm_int HEIGHT = 30;

#ifdef RC_USE_BOOST_SMALL_VEC
  using PositionVec = boost::container::small_vector< Position, MAX_N_ROBOTS >;
#else
  using PositionVec = std::vector< Position >;
#endif

public:
  Board(){
    srand(time(NULL));
    init( 1 );
  }

  Board( std::string const & stringified_rep ){
    load_from_stringified_representation( stringified_rep );
  }

  void clear_board();

  void init( int const round );

public: //inlined getters
  Occupant & cell( Position const & p ){
    return cells_[ p.x ][ p.y ];
  }

  Occupant const & cell( Position const & p ) const {
    return cells_[ p.x ][ p.y ];
  }

  Occupant & cell( int const x, int const y ) {
    return cells_[ x ][ y ];
  }

  Occupant const & cell( int const x, int const y ) const {
    return cells_[ x ][ y ];
  }

  int n_robots() const {
    return robot_positions_.size();
  }

  Position const & human_position() const {
    return human_position_;
  }

  PositionVec const & robots() const {
    return robot_positions_;
  }

public: //game logic
  bool
  cell_is_safe_for_teleport( Position const p ) const;

  MoveResult
  teleport( bool const safe );

  MoveResult
  move_robots_1_step( bool const human_is_safe = false );

  MoveResult
  move_human( sm_int const dx, sm_int const dy );

  bool
  move_is_safe( sm_int const dx, sm_int const dy ) const;

  bool
  move_is_cascade_safe( sm_int const dx, sm_int const dy ) const;

  bool
  move_is_cascade_safe( sm_int const dx, sm_int const dy, int & n_robots_remaining ) const;

  // legacy serialization method
  std::string
  get_safe_moves() const;

public: // serialization
  std::string
  get_stringified_representation() const;

  void
  load_from_stringified_representation( std::string const & str );


public: //static utilities
  static bool position_is_in_bounds( Position p ){
    return p.x >= 0 && p.x < WIDTH && p.y >= 0 && p.y < HEIGHT;
  }

protected:
  //should this be const?
  Position
  find_open_space( bool const allow_robot_movement = true );

  static
  sm_int
  random_x(){
    return rand() % WIDTH;
  }

  static
  sm_int
  random_y(){
    return rand() % HEIGHT;
  }

private:
  std::array< std::array< Occupant, Board::HEIGHT >, Board::WIDTH > cells_;

  PositionVec robot_positions_;
  Position human_position_;
};

void Board::clear_board(){
  for( std::array< Occupant, HEIGHT > & arr : cells_ ){
    for( Occupant & o : arr ){
      o = Occupant::EMPTY;
    }
  }
}

Position
Board::find_open_space( bool const allow_robot_movement ){
  if( robot_positions_.size() < 100 ){
    Position openp;
    do {
      openp.x = random_x();
      openp.y = random_y();
    } while( cell( openp ) != Occupant::EMPTY );
    return openp;
  }
    //This can be very constexpr
    std::vector< Position > empty_positions;
    empty_positions.reserve( (HEIGHT*WIDTH) - 1 - robot_positions_.size() );
    for( sm_int w = 0; w < WIDTH; ++w ){
      for( sm_int h = 0; h < HEIGHT; ++h ){
	Position const p = { w, h };
	if( allow_robot_movement ){
	  if( cell_is_safe_for_teleport( p ) ){
	    empty_positions.emplace_back( p );
	  }
	} else if( cell( p ) == Occupant::EMPTY ){
	  empty_positions.emplace_back( p );
	}
      } //h
    } //w

    if( empty_positions.empty() ){
      if( allow_robot_movement ){
	return find_open_space( false );
      } else {
	std::cout << "No safe positions even in fallback plan!!" << std::endl;
	//This is very unexpected, don't know how to handle it
      }
    }

    std::random_device rd;
    std::mt19937 g( rd() );
    std::shuffle( empty_positions.begin(), empty_positions.end(), g );
    return empty_positions[ 0 ];
    //TODO Just pick random index instead of shuffling
}

bool
Board::cell_is_safe_for_teleport( Position const p ) const {
  for( sm_int x = p.x - 1; x <= p.x + 1; ++x ){
    if( x < 0 || x >= WIDTH) continue;
    for( sm_int y = p.y - 1; y <= p.y + 1; ++y ){
      if( y < 0 || y >= HEIGHT) continue;
      if( cell( Position({ x, y }) ) != Occupant::EMPTY ) return false;
    }
  }

  return cell( p ) == Occupant::EMPTY;
}

MoveResult
Board::teleport( bool const safe ){
  cell( human_position_ ) = Occupant::EMPTY;

  if( safe ){
    human_position_ = find_open_space();
  } else {
    human_position_.x = random_x();
    human_position_.y = random_y();
  }

  cell( human_position_ ) = Occupant::HUMAN;
  return move_robots_1_step( safe );
}

void
Board::init( int const round ){
  //RESET
  clear_board();

  //HUMAN
  human_position_ = STARTING_POSITION;
  cell( human_position_ ) = Occupant::HUMAN;
    
  //ROBOTS
  robot_positions_.resize( nrobots_per_round( round ) );
  if( robot_positions_.size() < 100 ){
    //Use different algorithm if the robot count is << the number of cells
    for( Position & robot : robot_positions_ ){
      do {
	robot.x = random_x();
	robot.y = random_y();
      } while( cell( robot ) != Occupant::EMPTY );

      cell( robot ) = Occupant::ROBOT;	
    }
  } else {
    //This can be very constexpr
    std::vector< Position > empty_positions;
    empty_positions.reserve( (HEIGHT*WIDTH) - 1 );
    for( sm_int w = 0; w < WIDTH; ++w ){
      for( sm_int h = 0; h < HEIGHT; ++h ){
	Position const p = { w, h };
	if( p != STARTING_POSITION ){
	  empty_positions.emplace_back( p );
	}
      }
    }

    std::random_device rd;
    std::mt19937 g( rd() );
    std::shuffle( empty_positions.begin(), empty_positions.end(), g );

    for( unsigned int i = 0; i < robot_positions_.size(); ++i ){
      robot_positions_[ i ] = empty_positions[ i ];
      cell( robot_positions_[ i ] ) = Occupant::ROBOT;
    }
  }
}

MoveResult
Board::move_robots_1_step(
  bool const human_is_safe
){
  //Clear robots from map
  for( sm_int w = 0; w < WIDTH; ++w ){
    for( sm_int h = 0; h < HEIGHT; ++h ){
      if( cells_[ w ][ h ] == Occupant::ROBOT ){
	cells_[ w ][ h ] = Occupant::EMPTY;
      }
    }
  }

  //Some robots will be deleted if they run into each other or into fire
  std::set< int > robots_to_delete;
 
  //keep temporary track of robots just in case they clash
  //elements are indices in robot_positions_
  std::array< std::array< sm_int, HEIGHT >, WIDTH > robot_indices;

  for( unsigned int r = 0; r < robot_positions_.size(); ++r ){
    Position & pos = robot_positions_[ r ];

    if( human_position_.x < pos.x ) pos.x -= 1;
    else if( human_position_.x > pos.x ) pos.x += 1;

    if( human_position_.y < pos.y ) pos.y -= 1;
    else if( human_position_.y > pos.y ) pos.y += 1;

    if( human_is_safe && pos == human_position_ ){
      //This is rare, but just have the robot take one step to the left or right
      if( pos.x == 0 ) ++pos.x;//Don't go out of bounds
      else --pos.x;
    }

    switch( cell( pos ) ){
    case Occupant::EMPTY:
      robot_indices[ pos.x ][ pos.y ] = r;
      cell( pos ) = Occupant::ROBOT;
      break;
    case Occupant::ROBOT:
      {
	int const other_robot_ind = robot_indices[ pos.x ][ pos.y ];
	robots_to_delete.insert( other_robot_ind );
      }
      robots_to_delete.insert( r );
      cell( pos ) = Occupant::FIRE;
      break;
    case Occupant::HUMAN:
      return MoveResult::YOU_LOSE;
      //break;
    case Occupant::FIRE:
      robots_to_delete.insert( r );
      break;
    case Occupant::OOB:
      break;
    }
  }// for int r

  for( auto iter = robots_to_delete.rbegin(), end = robots_to_delete.rend();
       iter != end; ++iter ){
    robot_positions_.erase( robot_positions_.begin() + (*iter) );
  }

  if( robot_positions_.size() == 0 ){
    return MoveResult::YOU_WIN_ROUND;
  }

  return MoveResult::CONTINUE;
} //move_robots_one_step

MoveResult
Board::move_human( sm_int const dx, sm_int const dy ) {
  cell( human_position_ ) = Occupant::EMPTY;

  human_position_.x += dx;
  if( human_position_.x < 0 ) human_position_.x = 0;
  if( human_position_.x >= WIDTH ) human_position_.x -= 1;

  human_position_.y += dy;
  if( human_position_.y < 0 ) human_position_.y = 0;
  if( human_position_.y >= HEIGHT ) human_position_.y -= 1;

  if( cell( human_position_ ) == Occupant::FIRE ) return MoveResult::YOU_LOSE;
  if( cell( human_position_ ) == Occupant::ROBOT ) return MoveResult::YOU_LOSE;

  cell( human_position_ ) = Occupant::HUMAN;

  return move_robots_1_step();
}

bool
Board::move_is_safe( sm_int const dx, sm_int const dy ) const {
  Board copy = (*this);
  MoveResult const result = copy.move_human( dx, dy );
  return result != MoveResult::YOU_LOSE;
}

bool
Board::move_is_cascade_safe( sm_int const dx, sm_int const dy ) const {
  Board copy = (*this);
  MoveResult result = copy.move_human( dx, dy );
  while ( result == MoveResult::CONTINUE ){
    result = copy.move_human( 0, 0 );
  }
  return result != MoveResult::YOU_LOSE;
}

bool
Board::move_is_cascade_safe(
  sm_int const dx,
  sm_int const dy,
  int & nremaining
) const {
  Board copy = (*this);
  MoveResult result = copy.move_human( dx, dy );
  while ( result == MoveResult::CONTINUE ){
    result = copy.move_human( 0, 0 );
  }
  nremaining = copy.n_robots();
  return result != MoveResult::YOU_LOSE;
}


std::string
Board::get_stringified_representation() const {
  std::stringstream ss;
  for( std::array< Occupant, HEIGHT > const & arr : cells_ ){
    for( Occupant const & o : arr ){
      ss << int( o );
    }
  }
  return ss.str();
}

void
Board::load_from_stringified_representation( std::string const & str ) {
  robot_positions_.clear();

  int index = 0;
  Position pos({0,0});
  for( pos.x = 0; pos.x < WIDTH; ++pos.x ){
    for( pos.y = 0; pos.y < HEIGHT; ++pos.y ){
      char const val = str[ index ]; ++index;
      Occupant const state = Occupant(std::stoi(std::string(1,val)));
      cell( pos ) = state;

      switch( state ){
      case( Occupant::ROBOT ):
	robot_positions_.push_back( pos );
	break;
      case( Occupant::HUMAN ):
	human_position_ = pos;
	break;
      case( Occupant::EMPTY ):
      case( Occupant::FIRE ):
      case( Occupant::OOB ):
	break;
      }
    }//y
  }//x
}

std::string
Board::get_safe_moves() const {
  std::stringstream ss;
  for( sm_int dx = -1; dx < 2; ++dx )
    for( sm_int dy = -1; dy < 2; ++dy )
      if( move_is_cascade_safe( dx, dy ) ){
	ss << "1";
      } else {
	ss << "0";
      }
  return ss.str();
}

} // namespace robots_core
