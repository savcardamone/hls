/**
 * @file graph.hpp
 * @author Salvatore Cardamone
 * @brief Graph representation of scheduling problem.
 */
#ifndef __HLS_GRAPH_HPP
#define __HLS_GRAPH_HPP

#include <algorithm>
#include <vector>

namespace hls {

class Vertex {};

/**
 * @brief A wrapper around two vertices and some "weight" associated with
 * the connection.
 *
 * Note that the edge is directional, in that the connection points from one
 * Vertex to another.
 */
class Edge {
 public:
  /**
   * @brief Class constructor.
   * @param src Source Vertex.
   * @param dest Destination Vertex.
   * @param weight Weight of the connection.
   */
  Edge(vptr src, vptr dest, int weight)
      : src_{src}, dest_{dest}, weight_{weight} {}

  /**
   * @brief Operator overload for equality of Edges. Does no permit multigraphs;
   * just checks that an edge does not involve the same two particpating
   * vertices.
   * @param rhs The Edge to compare with.
   * @return True if the Edges are the same, false otherwise.
   */
  bool operator==(const Edge& rhs) const {
    if (src_ == rhs.src_ && dest_ == rhs.dest_) {
      return true;
    }
  }

 private:
  using vptr = std::shared_ptr<Vertex>;
  vptr src_, dest_;
  int weight;
};
  
/**
 * @brief A Graph in adjacency list form. Wraps vertices and edges, and permits
 * operation on the Graph through the visitor pattern.
 */
class Graph {
 public:
  /**
   * @brief Class constructor.
   */
  Graph() {}

  /**
   * @brief Accept a GraphVisitor instance to manipulate the Graph object.
   * @param visitor The GraphVisitor object.
   */
  //void accept(GraphVisitor& visitor) { visitor.process(*this); }

  /**
   * @brief Add a Vertex to the Graph. Will ensure no equivalent Vertex has
   * already been added, and constructs an entry in the edges map to allow
   * us to add connections at a later date.
   * @param vertex The Vertex to add to the Graph.
   * @throw std::runtime_error If the Vertex has already been added.
   */
  void add_vertex(vptr vertex) {
    if (vertex_exists(vertex)) {
      throw std::runtime_error(
          "Vertex already exists; cannot add it to Graph.");
    }
    vertices_.push_back(vertex);
    edges_[vertex] = {};
  }

  /**
   * @brief Add an Edge to the Graph. If either Vertex hasn't yet been added to
   * the Graph, will add it.
   * @param src The source Vertex in the Edge.
   * @param dest The destination Vertex in the Edge.
   * @param weight The Edge weight.
   * @throw std::runtime_error If an Edge already exists between the
   * participating vertices.
   */
  void add_edge(vptr src, vptr dest) {
    if (!vertex_exists(src)) add_vertex(src);
    if (!vertex_exists(dest)) add_vertex(dest);

    Edge edge(src, dest, weight);
    for (auto& existing_edge : edges_[src]) {
      if (existing_edge == edge) {
        throw std::runtime_error("Edge already exists.");
      }
    }
    edges_[src].push_back(edge);
  }

 private:
  using vptr = std::shared_ptr<Vertex>;
  std::vector<vptr> vertices_;
  std::map<vptr, std::vector<Edge>> edges_;

  /**
   * @brief Checks whether a Vertex has already been added to the Graph.
   * @param vptr Vertex to check.
   * @return False if the Vertex hasn't been added to the Graph, true otherwise.
   */
  bool vertex_exists(vptr vertex) {
    auto result = std::find(vertices_.begin(), vertices_.end(), vertex);
    if (result == vertices_.end()) {
      return false;
    }
    return true;
  }
};

}  // namespace hls

#endif /* #ifndef __HLS_GRAPH_HPP */
