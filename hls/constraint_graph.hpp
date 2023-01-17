/**
 * @file constraint_graph.hpp
 * @author Salvatore Cardamone
 * @brief Constraint graph specialisation of the Graph class.
 */
#ifndef __HLS_CONSTRAINT_GRAPH_HPP
#define __HLS_CONSTRAINT_GRAPH_HPP

#include "graph.hpp"

namespace hls {

/**
 * @brief Convenience class that's basically a really bad AST node for
 * expressions of the form x_a - x_b <= b.
 */
class ConstraintExpr {
 public:
  /**
   * @brief Class constructor.
   * @param var_a Name of variable in the constraint expression.
   * @param var_b Name of variable in the constraint expression.
   * @param leq_than Value that the variable difference is less-than
   * or equal to.
   */
  ConstraintExpr(const std::string& xa, const std::string& xb, const int& b)
      : xa_{xa}, xb_{xb}, b_{b} {}

  /**
   * @brief Getter for the first variable in the constraint expression.
   * @return Name of the variable.
   */
  std::string xa() const { return xa_; }

  /**
   * @brief Getter for the second variable in the constraint expression.
   * @return Name of the variable.
   */
  std::string xb() const { return xb_; }

  /**
   * @brief Getter for the value the expression is less-than or equal-to.
   * @return Value of the constraint.
   */
  int b() const { return b_; }

 private:
  std::string xa_, xb_;
  int b_;
};

/**
 * @brief Specialisation of the Vertex class; vertices in the constraint
 * graph are simply the variable names.
 */
class ConstraintVertex : public Vertex {
 public:
  /**
   * @brief Class constructor.
   * @param name Name of the variable.
   */
  ConstraintVertex(const std::string& name) : name_{name} {}

 private:
  std::string name_;
};

  /**
   * @brief Specialisation of the Graph class. Wraps an adjacency list, but
   * provides some convenience methods to construct the Graph from constraint
   * expressions (via the ConstraintExpr class).
   */
class ConstraintGraph : public Graph {
 public:
  /** 
   * @brief Class constructor.
   */
  ConstraintGraph() {}

  /**
   * @brief Add a constraint to the graph. Inequalities of the form
   * x_a - x_b <= b results in a pair of vertices, x_a and x_b, with
   * an edge from x_b to x_a of weight b.
   * @param expr The constraint expression we're adding to the Graph.
   */
  void add_constraint(const ConstraintExpr& expr) {
    add_edge(std::make_shared<ConstraintVertex>(expr.var_b()),
             std::make_shared<ConstraintVertex>(expr.var_a()), expr.leq_than());
  }
};

}  // namespace hls

#endif /* #ifndef __HLS_CONSTRAINT_GRAPH_HPP */
