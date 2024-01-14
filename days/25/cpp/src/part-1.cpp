
#include <cassert>
#include <cstdlib>
#include <algorithm>
#include <array>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <list>
#include <map>
#include <memory>
#include <optional>
#include <queue>
#include <random>
#include <set>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>


int main(int argc, char* argv[]) 
{
  assert(argc == 2);
  std::ifstream in{argv[1]};
  assert(in.is_open() and not in.bad());

  /// Read and parse input
  std::map<std::string, std::set<std::string>> edges;
  std::set<std::string> nodes;
  for (std::string line;;) {
    std::getline(in, line);
    if (in.eof())
      break;
    const auto from = line.substr(0, 3);
    for (auto i = 5uz; i + 3 <= line.size(); i += 4) {
      const auto to = line.substr(i, 3);
      edges[from].insert(to);
      edges[to].insert(from);
      nodes.insert(from);
      nodes.insert(to);
    }
  }

  /// Disjoint Set Union structure to keep track of which nodes are fused below
  struct DSU 
  {
    std::map<std::string, std::string> parent;
    void insert(std::string s) {
      parent[s] = s;
    }
    std::string find(std::string s) {
      if (parent[s] == s)
        return s;
      else
        return parent[s] = find(parent[s]);
    }
    void join(std::string a, std::string b) {
      const auto pa = find(a);
      const auto pb = find(b);
      parent[std::min(pa, pb)] = std::max(pa, pb);
    }
  } dsu;
  for (auto n: nodes)
    dsu.insert(n);

  /// Stoer-Wagner algorithm to find min-cut to partition graph
  std::vector<std::vector<int>> mat;
  for (auto n: nodes) {
    mat.push_back({});

    for (auto m: nodes) {
      if (edges[n].contains(m)) {
        mat.back().push_back(1);
      } else {
        mat.back().push_back(0);
      }
    }
  }
  /// Edited from: https://github.com/kth-competitive-programming/kactl/blob/main/content/graph/GlobalMinCut.h (CC0 license)
  {
    std::pair<int, std::vector<int>> best = {INT_MAX, {}};
    int n = mat.size();
    std::vector<std::vector<int>> co(n);

    for (int i = 0; i < n; i++)
      co[i] = {i};

    auto besttrack = -1;

    for (int ph = 1; ph < n; ph++) {
      std::vector<int> w = mat[0];
      size_t s = 0, t = 0;
      for (int it = 0; it < n - ph; it++) {
        w[t] = INT_MIN;
        s = t, t = std::max_element(w.begin(), w.end()) - w.begin();
        for (int i = 0; i < n; i++) w[i] += mat[t][i];
      }
      best = std::min(best, {w[t] - mat[t][t], co[t]});
      if (besttrack == -1) {
        besttrack = best.first;
      }
      if (best.first < besttrack) {
        besttrack = best.first;
      } else {
        dsu.join(*std::next(nodes.begin(),s), *std::next(nodes.begin(),t));
      }
      co[s].insert(co[s].end(), co[t].begin(), co[t].end());
      for (int i = 0; i < n; i++) mat[s][i] += mat[t][i];
      for (int i = 0; i < n; i++) mat[i][s] = mat[s][i];
      mat[0][t] = INT_MIN;
    }
  }

  /// Find sizes of 2 remaining graph partitions
  std::map<std::string,int> cardinality;
  for (auto n: nodes)
    ++cardinality[dsu.find(n)];
  auto result = 1;
  for (auto [n,c]: cardinality)
    result *= c;

  std::cout << "Product of group sizes: " << result << "\n";

  return EXIT_SUCCESS;
}

