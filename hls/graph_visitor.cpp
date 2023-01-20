/**
 * @file graph_visitor.cpp
 * @author Salvatore Cardamone
 * @brief Implementation of GraphVisitor methods.
 */

#include <limits>
#include <set>

// clang-format off
#include "graph_visitor.hpp"
#include "graph.hpp"
// clang-format on

namespace hls {

void GraphShortestPath::dijkstra(Graph& graph) {
  // All vertices begin as unvisited
  std::set<vptr> unvisited(graph.vertices_.begin(), graph.vertices_.end());

  // All distances are initially infinite, with the exception of the start
  // Vertex, which we initialise with a distance of zero
  std::map<vptr, int> distances;
  for (auto& vertex : graph.vertices_) {
    distances[vertex] = std::numeric_limits<int>::max();
  }
  distances[start_] = 0;

  auto current = start_;
  // Iterate until we arrive at our final destination Vertex
  while (current != end_) {
    // Loop over all neighbours to the current Vertex
    for (auto& edge : graph.outputs(current)) {
      // If the neighbour has already been visited, ignore it
      if (unvisited.find(edge.dest()) == unvisited.end()) {
        continue;
      }

      // Update Vertex distances if going via the current improves
      // upon its previous best distance
      int temp_dist = distances[current] + edge.weight();
      if (temp_dist <= distances[edge.dest()]) {
        distances[edge.dest()] = temp_dist;
      }
    }

    // We've now visited the Vertex that was current
    unvisited.erase(current);

    // Find our next current from the unvisited Vertex set
    int shortest = std::numeric_limits<int>::max();
    for (auto& vertex : unvisited) {
      if (distances[vertex] < shortest) {
        shortest = distances[vertex];
        current = vertex;
      }
    }
  }
  path_.distance = distances[end_];
}

}  // namespace hls
