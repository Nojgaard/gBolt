#ifndef INCLUDE_GBOLT_H_
#define INCLUDE_GBOLT_H_

#include <gbolt/common.h>
#include <gbolt/graph.h>
#include <gbolt/history.h>
#include <gbolt/path.h>
#include <gbolt/output.h>
#include <map>
#include <vector>
#include <string>
#include <queue>

namespace gbolt {

struct gbolt_instance_t {
  Graph *min_graph = NULL;
  DfsCodes *min_dfs_codes = NULL;
  History *history = NULL;
  Output *output = NULL;
  Path<int> *right_most_path = NULL;
  MinProjection *min_projection = NULL;

  ~gbolt_instance_t() {
    delete this->min_graph;
    delete this->min_dfs_codes;
    delete this->history;
    delete this->output;
    delete this->right_most_path;
    delete this->min_projection;
  }
};

class GBolt {
 public:
  GBolt(int nsupport) : nsupport_(nsupport),
	min_num_edges_(0), max_num_edges_(10000),
    output_frequent_nodes_(0), gbolt_instances_(0) {}

  GBolt(int nsupport, int min_num_edges, int max_num_edges) :
	nsupport_(nsupport),
	min_num_edges_(min_num_edges), max_num_edges_(max_num_edges),
	output_frequent_nodes_(0), gbolt_instances_(0) {}

  GBolt(int nsupport, int min_num_edges, int max_num_edges, size_t min_outlier_support,
		  size_t max_non_outlier_nsupport, std::vector<bool> outliers,
		size_t max_ngraphs = 0) :
	nsupport_(nsupport),
	min_num_edges_(min_num_edges), max_num_edges_(max_num_edges),
	min_outlier_support_(min_outlier_support),
	max_non_outlier_nsupport_(max_non_outlier_nsupport), outliers_(outliers),
	max_ngraphs_(max_ngraphs),
	output_frequent_nodes_(0), gbolt_instances_(0)
  {}

  void execute(const std::vector<Graph>& graphs);

  void save(std::string output_file, bool output_parent = false,
			bool output_pattern = false, bool output_frequent_nodes = false);

  Output get_output();

  ~GBolt() {
    if (gbolt_instances_ != 0) {
      delete[] gbolt_instances_;
    }
    if (output_frequent_nodes_ != 0) {
      delete output_frequent_nodes_;
    }
  }

 private:
  typedef map<dfs_code_t, Projection, dfs_code_project_compare_t> ProjectionMap;
  typedef map<dfs_code_t, Projection, dfs_code_backward_compare_t> ProjectionMapBackward;
  typedef map<dfs_code_t, Projection, dfs_code_forward_compare_t> ProjectionMapForward;

 private:
  // Mine
  void init_instances(const vector<Graph> &graphs);

  void project(const vector<Graph> &graphs);

  void find_frequent_nodes_and_edges(const vector<Graph> &graphs);

  bool check_outliers(int nsupport, const Projection& projection);
  bool check_queue(int nsupport);
  void mine_subgraph(
    const vector<Graph> &graphs,
    const Projection &projection,
    DfsCodes &dfs_codes,
    int prev_nsupport,
    int prev_thread_id,
    int prev_graph_id);

  // Extend
  void build_right_most_path(const DfsCodes &dfs_codes, Path<int> &right_most_path) {
    int prev_id = -1;

    for (auto i = dfs_codes.size(); i > 0; --i) {
      if (dfs_codes[i - 1]->from < dfs_codes[i - 1]->to &&
        (right_most_path.empty() || prev_id == dfs_codes[i - 1]->to)) {
        prev_id = dfs_codes[i - 1]->from;
        right_most_path.push_back(i - 1);
      }
    }
  }

  void update_right_most_path(const DfsCodes &dfs_codes, Path<int> &right_most_path) {
    auto *last_dfs_code = dfs_codes.back();
    // filter out a simple case
    if (last_dfs_code->from > last_dfs_code->to) {
      return;
    }
    right_most_path.reset();
    build_right_most_path(dfs_codes, right_most_path);
  }

  void enumerate(
    const vector<Graph> &graphs,
    const DfsCodes &dfs_codes,
    const Projection &projection,
    const Path<int> &right_most_path,
    ProjectionMapBackward &projection_map_backward,
    ProjectionMapForward &projection_map_forward);

  bool get_forward_init(
    const vertex_t &vertex,
    const Graph &graph,
    Edges &edges);

  void get_first_forward(
    const prev_dfs_t &prev_dfs,
    const History &history,
    const Graph &graph,
    const DfsCodes &dfs_codes,
    const Path<int> &right_most_path,
    ProjectionMapForward &projection_map_forward);

  void get_other_forward(
    const prev_dfs_t &prev_dfs,
    const History &history,
    const Graph &graph,
    const DfsCodes &dfs_codes,
    const Path<int> &right_most_path,
    ProjectionMapForward &projection_map_forward);

  void get_backward(
    const prev_dfs_t &prev_dfs,
    const History &history,
    const Graph &graph,
    const DfsCodes &dfs_codes,
    const Path<int> &right_most_path,
    ProjectionMapBackward &projection_map_backward);

  // Count
  int count_support(const Projection &projection);

  void build_graph(const DfsCodes &dfs_codes, Graph &graph);

  bool is_min(const DfsCodes &dfs_codes);

  bool is_projection_min(
    const DfsCodes &dfs_codes,
    const Graph &min_graph,
    History &history,
    DfsCodes &min_dfs_codes,
    Path<int> &right_most_path,
    MinProjection &projection,
    size_t projection_start_index);

  bool judge_backward(
    const Path<int> &right_most_path,
    const Graph &min_graph,
    History &history,
    dfs_code_t &min_dfs_code,
    DfsCodes &min_dfs_codes,
    MinProjection &projection,
    size_t projection_start_index,
    size_t projection_end_index);

  bool judge_forward(
    const Path<int> &right_most_path,
    const Graph &min_graph,
    History &history,
    dfs_code_t &min_dfs_code,
    DfsCodes &min_dfs_codes,
    MinProjection &projection,
    size_t projection_start_index,
    size_t projection_end_index);

  // Report
  void report(const DfsCodes &dfs_codes, const Projection &projection,
    int nsupport, int prev_thread_id, int prev_graph_id);

 private:
  // Graphs after reconstructing
  vector<Graph> graphs_;
  // Single instance of minigraph
//  #ifdef GBOLT_PERFORMANCE
  unordered_map<int, vector<int> > frequent_vertex_labels_;
  unordered_map<int, int> frequent_edge_labels_;
//  #else
//  map<int, vector<int> > frequent_vertex_labels_;
//  map<int, int> frequent_edge_labels_;
//  #endif
  int nsupport_;

  int min_num_edges_, max_num_edges_;
  size_t min_outlier_support_, max_non_outlier_nsupport_;
  std::vector<bool> outliers_;
  size_t max_ngraphs_ = 0;
  std::priority_queue <size_t, vector<size_t>, std::greater<size_t>> max_graphs_;

  Output *output_frequent_nodes_;
  gbolt_instance_t *gbolt_instances_;
  dfs_code_project_compare_t dfs_code_project_compare_;
  dfs_code_forward_compare_t dfs_code_forward_compare_;
  dfs_code_backward_compare_t dfs_code_backward_compare_;

};

}  // namespace gbolt

#endif  // INCLUDE_GBOLT_H_
