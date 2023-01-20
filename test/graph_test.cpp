/**
 * @file graph_test.cpp
 * @author Salvatore Cardamone
 * @brief Unit tests for the Graph class.
 */
// clang-format off
#include <gtest/gtest.h>

#include "hls/graph.hpp"
// clang-format on

/**
 * @brief Verify that we can add vertices to the Graph.
 */
TEST(GraphTests, VertexAddition) {
  hls::Graph graph;
  auto vertex = std::make_shared<hls::Vertex>();
  graph.add_vertex(vertex);
  ASSERT_THROW(graph.add_vertex(vertex), std::runtime_error);
}

/**
 * @brief Verify that we can add edges to the Graph.
 */
TEST(GraphTests, EdgeAddition) {
  hls::Graph graph;

  auto vertex_a = std::make_shared<hls::Vertex>();
  auto vertex_b = std::make_shared<hls::Vertex>();
  int edge_weight = 20;

  graph.add_edge(vertex_a, vertex_b, edge_weight);
  ASSERT_THROW(graph.add_edge(vertex_a, vertex_b, edge_weight),
               std::runtime_error);
  // Can't add a backedge to a pair of vertices already connected
  // by an edge
  ASSERT_THROW(graph.add_edge(vertex_b, vertex_a, edge_weight),
               std::runtime_error);
}

/**
 * @brief Verify that we can query the connections to vertices in
 * the Graph.
 */
TEST(GraphTests, VertexConnections) {
  hls::Graph graph;

  auto vertex_a = std::make_shared<hls::Vertex>();
  auto vertex_b = std::make_shared<hls::Vertex>();
  auto vertex_c = std::make_shared<hls::Vertex>();

  // Circular graph; A -> B -> C
  //                 ^         |
  //                 |_________|
  graph.add_edge(vertex_a, vertex_b, 1);
  graph.add_edge(vertex_b, vertex_c, 1);
  graph.add_edge(vertex_c, vertex_a, 1);

  auto a_srcs = graph.sources(vertex_a);
  auto a_dests = graph.destinations(vertex_a);
  ASSERT_EQ(a_srcs.size(), 1);
  ASSERT_EQ(a_srcs[0], vertex_c);
  ASSERT_EQ(a_dests.size(), 1);
  ASSERT_EQ(a_dests[0], vertex_b);

  auto b_srcs = graph.sources(vertex_b);
  auto b_dests = graph.destinations(vertex_b);
  ASSERT_EQ(b_srcs.size(), 1);
  ASSERT_EQ(b_srcs[0], vertex_a);
  ASSERT_EQ(b_dests.size(), 1);
  ASSERT_EQ(b_dests[0], vertex_c);

  auto c_srcs = graph.sources(vertex_c);
  auto c_dests = graph.destinations(vertex_c);
  ASSERT_EQ(c_srcs.size(), 1);
  ASSERT_EQ(c_srcs[0], vertex_b);
  ASSERT_EQ(c_dests.size(), 1);
  ASSERT_EQ(c_dests[0], vertex_a);
}
