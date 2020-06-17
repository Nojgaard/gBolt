#ifndef INCLUDE_OUTPUT_H_
#define INCLUDE_OUTPUT_H_

#include <gbolt/common.h>
#include <vector>
#include <string>
#include <gbolt/graph.h>

namespace gbolt {

class Output {
 public:
  explicit Output() {}

  int size() const {
    return support_.size();
  }

  int support(size_t idx) { return support_[idx]; }
  int thread_id(size_t idx) { return thread_id_[idx]; }
  int parent_id(size_t idx) { return parent_id_[idx]; }
  int graph_id(size_t idx) { return graph_id_[idx]; }

  vector<size_t> support_vector(size_t idx) {
	  vector<size_t> out;
	  const Projection& projection = projections_[idx];
	  int prev = 0;
	  for (auto i = 0; i < projection.size(); ++i) {
		if (i == 0 || projection[i].id != prev) {
		  prev = projection[i].id;
		  out.push_back(prev);
		}
	  }
	  return out;
  }

  const Graph& graph(size_t idx) {
	  return graphs_[idx];
  }

  void push_back(const Output& out);

  void push_back(const DfsCodes& dfs_code, const Graph& graph, const Projection& projection,
				 int nsupport, int graph_id);
  void push_back(const DfsCodes& dfs_code, const Graph& graph, const Projection& projection,
				 int nsupport, int graph_id, int thread_id, int parent_id);

  void push_back(const string &str, int nsupport, int graph_id);

  void push_back(const string &str, int nsupport, int graph_id, int thread_id, int parent_id);

  void save(std::string output_file, bool output_parent = false, bool output_pattern = false);

 private:
  vector<string> buffer_;
  vector<int> support_;
  vector<int> thread_id_;
  vector<int> parent_id_;
  vector<int> graph_id_;
//  const string output_file_;

  vector<Projection> projections_;
  vector<Graph> graphs_;
  vector<DfsCodesLocal> dfs_codes_;
};

}  // namespace gbolt

#endif  // INCLUDE_OUTPUT_H_
