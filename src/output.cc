#include <output.h>
#include <fstream>
#include <sstream>
#include <iostream>

namespace gbolt {

std::string graph_to_str(const Graph& graph, const DfsCodesLocal& dfs_codes, const Projection& projection) {
  std::stringstream ss;
  for (auto i = 0; i < graph.size(); ++i) {
	const vertex_t *vertex = graph.get_p_vertex(i);
	ss << "v " << vertex->id << " " << vertex->label << std::endl;
  }
  for (auto i = 0; i < dfs_codes.size(); ++i) {
	  if (dfs_codes[i].to > 20) {
		  std::cout << "WAIT " << dfs_codes[i].to << std::endl;
	  }
	ss << "e " << dfs_codes[i].from << " " << dfs_codes[i].to
	  << " " << dfs_codes[i].edge_label << std::endl;
  }
  ss << "x: ";
  int prev = 0;
  for (auto i = 0; i < projection.size(); ++i) {
	if (i == 0 || projection[i].id != prev) {
	  prev = projection[i].id;
	  ss << prev << " ";
	}
  }
  ss << std::endl;
  return ss.str();
}

void Output::push_back(const DfsCodes& dfs_code, const Graph& graph, const Projection& projection,
					   int nsupport, int graph_id) {

  support_.push_back(nsupport);
  graph_id_.push_back(graph_id);
  dfs_codes_.emplace_back();
  for (const dfs_code_t* dc : dfs_code) {
	  dfs_codes_.back().emplace_back(*dc);
  }
  graphs_.push_back(graph);
  projections_.push_back(projection);
}

void Output::push_back(const DfsCodes& dfs_code, const Graph& graph, const Projection& projection,
					   int nsupport, int graph_id, int thread_id, int parent_id) {
  push_back(dfs_code, graph, projection, nsupport, graph_id);
  thread_id_.push_back(thread_id);
  parent_id_.push_back(parent_id);
}

void Output::push_back(const string &str, int nsupport, int graph_id) {
  buffer_.push_back(str);
  support_.push_back(nsupport);
  graph_id_.push_back(graph_id);
}

void Output::push_back(const string &str, int nsupport, int graph_id, int thread_id, int parent_id) {
  buffer_.push_back(str);
  support_.push_back(nsupport);
  graph_id_.push_back(graph_id);
  thread_id_.push_back(thread_id);
  parent_id_.push_back(parent_id);
}

void Output::push_back(const Output& out) {
	for (size_t i = 0; i < out.size(); ++i) {
	  support_.push_back(out.support_[i]);
	  graph_id_.push_back(this->size());
	  dfs_codes_.push_back(out.dfs_codes_[i]);
	  graphs_.push_back(out.graphs_[i]);
	  projections_.push_back(out.projections_[i]);
	  thread_id_.push_back(out.thread_id_[i]);
	}
}

void Output::save(std::string output_file, bool output_parent, bool output_pattern) {
  std::ofstream out(output_file.c_str());

  for (auto i = 0; i < support_.size(); ++i) {
    out << "t # " << graph_id_[i] << " * " << support_[i] << std::endl;
    if (output_parent == true) {
      if (parent_id_[i] == -1)
        out << "parent : -1" << std::endl;
      else
        out << "parent : " << parent_id_[i] << " thread : " << thread_id_[i] << std::endl;
    }
    if (output_pattern == true) {
	  out << graph_to_str(graphs_[i], dfs_codes_[i], projections_[i]) << std::endl;
    }
  }
  out.close();
}

}  // namespace gbolt
