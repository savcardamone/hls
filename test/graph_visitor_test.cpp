/**
 * @file graph_visitor_test.cpp
 * @author Salvatore Cardamone
 * @brief Unit tests for the GraphVisitor class.
 */
// clang-format off
#include <gtest/gtest.h>

#include "hls/graph.hpp"
#include "hls/graph_visitor.hpp"
// clang-format on

TEST(GraphVisitorTests, Basic) {

  hls::Graph graph;

  auto vertex_a = std::make_shared<hls::Vertex>();
  auto vertex_b = std::make_shared<hls::Vertex>();
  auto vertex_c = std::make_shared<hls::Vertex>();

  // Circular graph; A -> B -> C
  //                 ^         |
  //                 |_________|
  graph.add_edge(vertex_a, vertex_b, 1);
  graph.add_edge(vertex_b, vertex_c, 2);
  graph.add_edge(vertex_c, vertex_a, 2);

  hls::GraphShortestPath path(vertex_a, vertex_c);

  graph.accept(path);
  ASSERT_EQ(path.path_length(), 3);
  
}
