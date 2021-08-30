#pragma once

#include <robots_core/game.hh>
#include <robots_core/forecasting.hh>
#include <robots_core/asserts.hh>

#include <robots_core/graph/board_as_graph.hh>
#include <robots_core/graph/node.hh>

#include <array>
#include <vector>
#include <cmath>  //log

namespace robots_core{
namespace graph{

template< typename Real = float >
struct SparseGraph {

  static_assert( N >= static_cast<unsigned int>(SpecialCaseNodes::count) );
  
  using X = std::vector< std::array< Real, GraphDecorator::F > >;
  using A = std::vector< std::vector< Real > >;
  using E = std::vector< std::array< Real, GraphDecorator::S >, N >; //How does this work in this context?

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
