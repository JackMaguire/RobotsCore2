#pragma once

#include <robots_core/game.hh>
#include <robots_core/forecasting.hh>
#include <robots_core/asserts.hh>

#include <robots_core/graph/board_as_graph.hh>
#include <robots_core/graph/node.hh>

//#include <array>
#include <cmath>  //log

namespace robots_core{
namespace graph{

template< unsigned int N, typename Real = float >
struct DenseGraph {

  static_assert( N >= static_cast<unsigned int>(SpecialCaseNodes::count) );
  
  using X = std::array< std::array< Real, GraphDecorator::F >, N >;
  using A = std::array< std::array< Real, N >, N >;
  using E = std::array< std::array< std::array< Real, GraphDecorator::S >, N >, N >;

  X x;
  A a;
  E e;

  DenseGraph(){
    clear();
  }

  DenseGraph(
    RobotsGame const & game,
    std::vector< Node > const & nodes
  ){
    construct( game, nodes ); //construct calls clear
  }

  void
  clear(){ //zero initialize
    for( auto & vec : x ) vec.fill( 0.0 );
    for( auto & vec : a ) vec.fill( 0.0 );
    for( auto & vec : e ) for( auto & vec2: vec ) vec.fill( 0.0 );
  }

  void
  construct(
    RobotsGame const & game,
    std::vector< Node > const & nodes
  ){
    RC_ASSERT( nodes.size() <= N );

    //reset to all zeros
    clear();

    for( uint i = 0; i < nodes.size(); ++i ){
      x[ i ] = GraphDecorator::calculate_node( nodes[i], game );

      for( uint j = i+1; j < nodes.size(); ++j ){
	if( GraphDecorator::edge_should_exist( nodes[i], nodes[j], game.board() ) ){
	  a[ i ][ j ] = a[ j ][ i ] = 1.0;
	  e[ i ][ j ] = e[ j ][ i ] = GraphDecorator::calculate_edge( nodes[i], nodes[j] );

	  static_assert( GraphDecorator::edges_are_symmetric );
	  // Not a dealbreaker, just means we need to update the code above
	}
      }
    }
  }
};

}
}
