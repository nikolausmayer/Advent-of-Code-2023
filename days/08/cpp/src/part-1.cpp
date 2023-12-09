
#include <cassert>
#include <cstdlib>
#include <algorithm>
#include <array>
#include <fstream>
#include <iostream>
#include <limits>
#include <map>
#include <memory>
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

  /// Parse input
  /// RL
  /// 
  /// AAA = (BBB, CCC)
  /// BBB = (DDD, EEE)
  const std::string instructions = [&in]{
    std::string s;
    in >> s;
    return s;
  }();
  std::map<std::string, std::array<std::string, 3>> nodes;
  for (;;) {
    std::string node, left, right, _;
    in >> node >> _ >> left >> right;
    if (in.eof())
      break;
    left  = left.substr(1, 3);
    right = right.substr(0, 3);
    nodes[node] = {node, left, right};
    //std::cout << "<" << node << "> -> <" << left << "> <" << right << ">\n";
  }

  /// Off to the desert!
  auto current_node = nodes["AAA"];
  for (auto i = 0uz, steps = 1uz;; i = (i + 1) % instructions.size(), ++steps) {
    const auto next = current_node[1 + (instructions[i] == 'L' ? 0 : 1)];
    //std::cout << i << " " << current_node[0] << "-" << current_node[1] << "-" << current_node[2] << " | " << instructions[i] << " -> " << next << "\n";
    if (next == "ZZZ") {
      std::cout << "Reached ZZZ after " << steps << " steps\n";
      break;
    }
    current_node = nodes[next];
  }

  return EXIT_SUCCESS;
}

