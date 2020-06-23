#include <gbolt/database.h>
#include <gbolt/graph.h>
#include <gbolt/common.h>
#include <fstream>
#include <cstdlib>
#include <cstring>

namespace gbolt {

void Database::read_input(const string &input_file, const string &separator) {
  std::ifstream fin(input_file.c_str());
  char line[FILE_MAX_LINE];

  if (!fin.is_open()) {
    LOG_ERROR("Open file error! %s", input_file.c_str());
  }

  int num_line = 0;
  while (fin.getline(line, FILE_MAX_LINE)) {
    char *pch = NULL;
    pch = strtok(line, separator.c_str());
    input_.resize(num_line + 1);
    if (*pch == 't') {
      ++num_graph_;
    }
    while (pch != NULL) {
      input_[num_line].emplace_back(pch);
      pch = strtok(NULL, separator.c_str());
    }
    ++num_line;
  }
  fin.close();
}

// Construct graph
void Database::construct_graphs(vector<Graph> &graphs) {
  int graph_index = 0;
  int edge_id = 0;
  graphs.resize(num_graph_);
  Vertice *vertice = graphs[graph_index].get_p_vertice();

  for (auto i = 0; i < input_.size(); ++i) {
    if (input_[i][0] == "t") {
      if (i != 0) {
        graphs[graph_index].set_nedges(edge_id);
        vertice = graphs[++graph_index].get_p_vertice();
        edge_id = 0;
      }
      graphs[graph_index].set_id(atoi(input_[i][2].c_str()));
    } else if (input_[i][0] == "v") {
      int id = atoi(input_[i][1].c_str());
      int label = atoi(input_[i][2].c_str());
      vertice->emplace_back(id, label);
    } else if (input_[i][0] == "e") {
      int from = atoi(input_[i][1].c_str());
      int to = atoi(input_[i][2].c_str());
      int label = atoi(input_[i][3].c_str());
      // Add an edge
      // Forward direction edge
      (*vertice)[from].edges.emplace_back(from, label, to, edge_id);
      // Backward direction edge
      (*vertice)[to].edges.emplace_back(to, label, from, edge_id);
      ++edge_id;
    } else {
      LOG_ERROR("Reading input error!");
    }
  }
  graphs[graph_index].set_nedges(edge_id);
}

// Construct graph by labels
void Database::construct_graphs(
  #ifdef GBOLT_PERFORMANCE
  const unordered_map<int, std::vector<int> > &frequent_vertex_labels,
  const unordered_map<int, int> &frequent_edge_labels,
  #else
  const map<int, std::vector<int> > &frequent_vertex_labels,
  const map<int, int> &frequent_edge_labels,
  #endif
  vector<Graph> &graphs) {
  vector<int> labels;
  #ifdef GBOLT_PERFORMANCE
  unordered_map<int, int> id_map;
  #else
  map<int, int> id_map;
  #endif
  int graph_index = 0;
  int edge_id = 0;
  int vertex_id = 0;
  graphs.resize(num_graph_);
  Vertice *vertice = graphs[graph_index].get_p_vertice();

  for (int i = 0; i < input_.size(); ++i) {
    if (input_[i][0] == "t") {
      if (i != 0) {
        graphs[graph_index].set_nedges(edge_id);
        vertice = graphs[++graph_index].get_p_vertice();
        edge_id = 0;
        vertex_id = 0;
        labels.clear();
        id_map.clear();
      }
      graphs[graph_index].set_id(atoi(input_[i][2].c_str()));
    } else if (input_[i][0] == "v") {
      int id = atoi(input_[i][1].c_str());
      int label = atoi(input_[i][2].c_str());
      labels.emplace_back(label);
      // Find a node with frequent label
      if (frequent_vertex_labels.find(label) != frequent_vertex_labels.end()) {
        vertice->emplace_back(vertex_id, label);
        id_map[id] = vertex_id;
        ++vertex_id;
      }
    } else if (input_[i][0] == "e") {
      int from = atoi(input_[i][1].c_str());
      int to = atoi(input_[i][2].c_str());
      int label = atoi(input_[i][3].c_str());
      int label_from = labels[from];
      int label_to = labels[to];
      // Find an edge with frequent label
      if (frequent_vertex_labels.find(label_from) != frequent_vertex_labels.end() &&
        frequent_vertex_labels.find(label_to) != frequent_vertex_labels.end() &&
        frequent_edge_labels.find(label) != frequent_edge_labels.end()) {
        // First edge
        (*vertice)[id_map[from]].edges.
          emplace_back(id_map[from], label, id_map[to], edge_id);
        // Second edge
        (*vertice)[id_map[to]].edges.
          emplace_back(id_map[to], label, id_map[from], edge_id);
        ++edge_id;
      }
    } else {
      LOG_ERROR("Reading input error!");
    }
  }
  graphs[graph_index].set_nedges(edge_id);
}

}  // namespace gbolt
