/**
 * @file graph_visitor.hpp
 * @author Salvatore Cardamone
 * @brief Classes which can act as visitors to the Graph class.
 */
#ifndef __HLS_GRAPH_VISITOR_HPP
#define __HLS_GRAPH_VISITOR_HPP

#include <memory>
#include <vector>

namespace hls {

// Forward-declaration of quantities the visitor needs to operate on
class Graph;
class Vertex;

/**
 * @brief Abstract base Visistor class for Graph objects.
 */
class GraphVisitor {
 public:
  /**
   * @brief Process the Graph in some way.
   * @param graph The Graph object to operate on.
   */
  virtual void visit(Graph& graph) = 0;
};

/**
 * @brief Visitor for Graph class that will find the shortest path
 * between two vertices in the Graph.
 */
class GraphShortestPath : public GraphVisitor {
 public:

  using vptr = std::shared_ptr<Vertex>;

  struct Result {
    int distance;
    std::vector<vptr> route;
  };
  
  /**
   * @brief Class constructor.
   */
  GraphShortestPath(const vptr start, const vptr end)
      : start_{start}, end_{end} {}

  /**
   * @brief Defer to the requested shortest-path algorithm to operate on the
   * Graph.
   * @param graph The Graph to operate on.
   */
  void visit(Graph& graph) override {
    return dijkstra(graph);
  };

  int path_length() {
    return path_.distance;
  }
  
 private:
  vptr start_, end_;
  Result path_;
  
  /**
   * @brief Implement the Dijkstra shortest-path algorithm on an input Graph.
   *
   */
  void dijkstra(Graph& graph);
};

}  // namespace hls

#endif /* #ifndef __HLS_GRAPH_VISITOR_HPP */
