/**
 * @file graph.hpp
 * @author Salvatore Cardamone
 * @brief Graph representation of scheduling problem.
 */
#ifndef __HLS_GRAPH_HPP
#define __HLS_GRAPH_HPP

#include <algorithm>
#include <map>
#include <stdexcept>
#include <vector>

#include "graph_visitor.hpp"

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
  using vptr = std::shared_ptr<Vertex>;

  /**
   * @brief Class constructor.
   * @param src Source Vertex.
   * @param dest Destination Vertex.
   * @param weight Weight of the connection.
   */
  Edge(vptr src, vptr dest, int weight)
      : src_{src}, dest_{dest}, weight_{weight} {}

  /**
   * @brief Getter for the source Vertex in the Edge.
   * @return The source Vertex.
   */
  vptr src() const { return src_; }

  /**
   * @brief Getter for the destination Vertex in the Edge.
   * @return The destination Vertex.
   */
  vptr dest() const { return dest_; }

  /**
   * @brief Getter for the weight of the Edge.
   * @return The Edge weight.
   */
  int weight() const { return weight_; }

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
    return false;
  }

 private:
  vptr src_, dest_;
  int weight_;
};

// Forward declare the visitor class for the Graph
class GraphVisitor;

/**
 * @brief A Graph in adjacency list form. Wraps vertices and edges, and permits
 * operation on the Graph through the visitor pattern.
 */
class Graph {
 public:
  friend class GraphShortestPath;
  using vptr = std::shared_ptr<Vertex>;

  /**
   * @brief Class constructor.
   */
  Graph() {}

  /**
   * @brief Accept a GraphVisitor instance to manipulate the Graph object.
   * @param visitor The GraphVisitor object.
   */
  void accept(GraphVisitor& visitor) { visitor.visit(*this); }

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
  void add_edge(vptr src, vptr dest, int weight) {
    if (!vertex_exists(src)) add_vertex(src);
    if (!vertex_exists(dest)) add_vertex(dest);

    if (edge_exists(src, dest)) {
      throw std::runtime_error("Edge already exists.");
    }

    Edge edge(src, dest, weight);
    // Edge belongs to both the source and destination
    edges_[src].push_back(edge);
    edges_[dest].push_back(edge);
  }

  /**
   * @brief Return the input edges to the argument Vertex.
   * @param vertex The Vertex we're querying the input edges to.
   * @result Input edges to the Vertex.
   */
  std::vector<Edge> inputs(vptr vertex) {
    if (!vertex_exists(vertex)) {
      throw std::runtime_error("Vertex isn't in the Graph.");
    }

    std::vector<Edge> result;
    for (auto& edge : edges_[vertex]) {
      if (edge.src() != vertex) {
	result.push_back(edge);
      }
    }
    
    return result;
  }

  /**
   * @brief Return the output edges from the argument Vertex.
   * @param vertex The Vertex we're querying the output edges from.
   * @result Output edges from the Vertex.
   */
  std::vector<Edge> outputs(vptr vertex) {
    if (!vertex_exists(vertex)) {
      throw std::runtime_error("Vertex isn't in the Graph.");
    }

    std::vector<Edge> result;
    for (auto& edge : edges_[vertex]) {
      if (edge.dest() != vertex) {
	result.push_back(edge);
      }
    }
    
    return result;
  }

  /**
   * @brief Return the vertices connected to the input Vertex and are
   * sources, i.e. feed into the argument vertex.
   * @param vertex The vertex to query the sources of.
   * @result std::vector of source vertices connected to the query Vertex.
   * @throw std::runtime_error If the argument Vertex isn't in the Graph.
   */
  std::vector<vptr> sources(vptr vertex) {
    std::vector<vptr> result;
    for (auto& edge : inputs(vertex)) {
      result.push_back(edge.src());
    }
    return result;
  }

  /**
   * @brief Return the vertices connected to the input Vertex and are
   * destinations, i.e. are fed by the argument vertex.
   * @param vertex The vertex to query the destinations of.
   * @result std::vector of destination vertices connected to the query Vertex.
   * @throw std::runtime_error If the argument Vertex isn't in the Graph.
   */
  std::vector<vptr> destinations(vptr vertex) {
    std::vector<vptr> result;
    for (auto& edge : outputs(vertex)) {
      result.push_back(edge.dest());
    }
    return result;
  }

 private:
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

  /**
   * @brief Check whether an Edge exists between two vertices.
   * @param vertex_a The first Vertex.
   * @param vertex_b The second Vertex.
   * @return True if there's an Edge, false otherwise.
   */
  bool edge_exists(vptr vertex_a, vptr vertex_b) {
    if (!vertex_exists(vertex_a) || !vertex_exists(vertex_b)) {
      return false;
    }
    for (auto& existing_edge : edges_[vertex_a]) {
      if (existing_edge.src() == vertex_b || existing_edge.dest() == vertex_b) {
        return true;
      }
    }
    return false;
  }
};

}  // namespace hls

#endif /* #ifndef __HLS_GRAPH_HPP */
