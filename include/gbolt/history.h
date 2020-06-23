#ifndef INCLUDE_HISTORY_H_
#define INCLUDE_HISTORY_H_

#include <gbolt/common.h>
#include <gbolt/graph.h>

namespace gbolt {

class History {
 public:
  History(int max_edges, int max_vertice) : edge_size_(0) {
    edges_ = new ConstEdgePointer[max_edges + 1];
    has_edges_ = new bool[max_edges + 1]();
    has_vertice_ = new bool[max_vertice + 1]();
  }

  void build(const prev_dfs_t &start, const Graph &graph);

  void build_edges(const prev_dfs_t &start, const Graph &graph);

  void build_edges_min(const MinProjection &projection, const Graph &graph, int start);

  void build_vertice(const prev_dfs_t &start, const Graph &graph);

  void build_vertice_min(const MinProjection &projection, const Graph &graph, int start);

  bool has_edges(int index) const {
    return has_edges_[index];
  }

  bool has_vertice(int index) const {
    return has_vertice_[index];
  }

  const edge_t *get_p_edge(int index) const {
    return edges_[edge_size_ - index - 1];
  }

  ~History() {
    delete[] edges_;
    delete[] has_edges_;
    delete[] has_vertice_;
  }

 private:
  typedef const edge_t * ConstEdgePointer;
  ConstEdgePointer *edges_;
  bool *has_edges_;
  bool *has_vertice_;
  int edge_size_;
};

}  // namespace gbolt

#endif  // INCLUDE_HISTORY_H_
