
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
  std::vector<std::string> starting_nodes;
  for (;;) {
    std::string node, left, right, _;
    in >> node >> _ >> left >> right;
    if (in.eof())
      break;
    left  = left.substr(1, 3);
    right = right.substr(0, 3);
    nodes[node] = {node, left, right};
    if (node[2] == 'A')
      starting_nodes.push_back(node);
  }

  /// Off to the desert!
  /// First find all starting nodes' periods and goal-state offsets
  std::map<std::string, std::size_t> loop_periods;
  std::size_t earliest_goal_step{99999999};
  std::string earliest_goal_start;
  for (const auto& start: starting_nodes) {
    std::cout << "starting node " << start << ":\n";
    std::map<std::tuple<std::string, std::size_t>, std::size_t> map_node_to_step;
    auto current_node = nodes[start];
    for (auto i = 0uz, steps = 1uz;; i = (i + 1) % instructions.size(), ++steps) {
      /// Check for loop
      if (map_node_to_step.contains({current_node[0], i})) {
        loop_periods[start] = (steps - map_node_to_step[{current_node[0], i}]);
        std::cout << "  loop found: entry " << current_node[0] << " at " << map_node_to_step[{current_node[0], i}] << " -> " << steps << ", period " << loop_periods[start] << "\n";
        break;
      }
      map_node_to_step[{current_node[0], i}] = steps;

      /// Check for goal(s)
      const auto next = current_node[1 + (instructions[i] == 'L' ? 0 : 1)];
      if (next[2] == 'Z') {
        if (steps < earliest_goal_step) {
          earliest_goal_step  = steps;
          earliest_goal_start = start;
        }
        std::cout << "  goal state " << next << " at " << steps << "\n";
      }
      current_node = nodes[next];
    }
  }

  /// Looks like the puzzle input is remarkable well-behaved, in that a simple
  /// LCM is directly enough to solve the puzzle?!
  /// Well let's find the LCM then
  std::map<int,int> map_primefactor_to_max_exponent;
  for (auto [_, period]: loop_periods) {
    //std::cout << "period " << period << ":\n";
    std::map<int,int> map_primefactor_to_exponent;
    for (auto div = 2u; period > 1; ++div) {
      while (period % div == 0) {
        //std::cout << "  pf " << div << "\n";
        ++map_primefactor_to_exponent[div];
        period /= div;
      }
    }
    for (auto [prime_factor, exponent]: map_primefactor_to_exponent)
      map_primefactor_to_max_exponent[prime_factor] = std::max(exponent,
            map_primefactor_to_max_exponent[prime_factor]);
  }

  auto result{1ull};
  for (auto [prime_factor, exponent]: map_primefactor_to_max_exponent) {
    auto tmp{1ull};
    for (auto i = 0; i < exponent; ++i)
      tmp *= prime_factor;
    result *= tmp;
  }
  std::cout << "All ghosts at goal states after " << result << " steps\n";

  return EXIT_SUCCESS;
}

