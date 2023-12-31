
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
#include <set>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>

struct Part
{
  std::string next_workflow;
  std::array<std::array<int,2>,4> ranges;
};
struct Rule
{
  bool lt{false};
  int gate{0};
  int gate_idx{0};
  bool is_default{false};
  std::string destination;
};
struct Workflow
{
  std::vector<Rule> rules;
};

int main(int argc, char* argv[]) 
{
  assert(argc == 2);
  std::ifstream in{argv[1]};
  assert(in.is_open() and not in.bad());

  /// Read input file
  std::vector<std::string> workflow_strings, part_ratings;
  {
    auto* target = &workflow_strings;
    for (std::string s;;) {
      std::getline(in, s);
      if (in.eof()) {
        break;
      }
      if (s == "") {
        target = &part_ratings;
        continue;
      }
      target->push_back(s);
    }
  }

  /// Parse workflows
  std::map<std::string, Workflow> map_name_to_workflow;
  for (auto s: workflow_strings) {
    /// Extract workflow name and collection-of-rules
    const auto name = s.substr(0, s.find('{'));
    //std::cout << s << " --> " << name;
    s = s.substr(name.size() + 1, s.size() - name.size() - 2);
    
    Workflow w;

    /// Parse workflow rules
    Rule r;
    std::string buffer, tested, rule;
    for (auto c: s) {
      if (c == ',') {
        /// Rule complete
        r.destination = buffer;
        buffer = "";
        w.rules.push_back(r);
        r = Rule{};
      } else if (c == '<' or c == '>') {
        tested = buffer;
        rule = c;
        buffer = "";
      } else if (c == ':') {
        /// Condition complete
        const auto value = std::stoi(buffer);
        r.lt = (rule == "<");
        r.gate = value;
        r.gate_idx = (tested == "x" ? 0 :
                      tested == "m" ? 1 :
                      tested == "a" ? 2 :
                                      3);
        buffer = "";
      } else {
        buffer += c;
      }
    }
    /// Store default rule
    //std::cout << "  DEF<" << buffer << "> ";
    r.is_default = true;
    r.destination = buffer;
    w.rules.push_back(r);

    map_name_to_workflow[name] = w;
    //std::cout << "\n";
  }

  /// Part set
  std::queue<Part> q;
  q.push({"in", {{{1,4000},{1,4000},{1,4000},{1,4000}}}});
  std::vector<std::array<std::array<int,2>,4>> accepted_hypercubes;

  while (q.size()) {
    auto next = q.front();
    q.pop();

    if (next.next_workflow == "A") {
      accepted_hypercubes.push_back(next.ranges);
    } else if (next.next_workflow == "R") {
      continue;
    }

    /// Load workflow
    const auto& workflow = map_name_to_workflow[next.next_workflow];
    /// Step through rules
    for (const auto& rule: workflow.rules) {
      if (rule.is_default) {
        q.push({rule.destination, next.ranges});
        std::cout << "  (default to <" << rule.destination << "> ";
        for (auto [from, to]: next.ranges) {
          std::cout << "[" << from << "," << to << "]";
        }
        std::cout << ")\n";
        break;
      }

      auto gated_range = next.ranges[rule.gate_idx];
      std::cout << next.next_workflow 
                << "\tgate " << rule.gate_idx << " ";
      for (auto [from, to]: next.ranges) {
        std::cout << "[" << from << "," << to << "]";
      }
      std::cout << "\t" << (rule.lt ? "<" : ">") << rule.gate;
      /// Apply rule for passthrough
      if (rule.lt) {
        gated_range[1] = std::min(gated_range[1], rule.gate - 1);
      } else {
        gated_range[0] = std::max(gated_range[0], rule.gate + 1);
      }
      /// Check for empty range
      if (gated_range[0] <= gated_range[1]) {
        /// Push passthrough for next workflow
        auto new_ranges = next.ranges;
        new_ranges[rule.gate_idx] = gated_range;
        q.push({rule.destination, new_ranges});

        std::cout << "  -->";
        for (auto [from, to]: new_ranges) {
          std::cout << "[" << from << "," << to << "]";
        }
      }
      std::cout << "\n";

      /// Invert rule to check next rule
      if (rule.lt) {
        next.ranges[rule.gate_idx][0] = std::max(next.ranges[rule.gate_idx][0],
                                                 gated_range[1] + 1);
      } else {
        next.ranges[rule.gate_idx][1] = std::min(next.ranges[rule.gate_idx][1],
                                                 gated_range[0] - 1);
      }

      /// Check for empty range
      if (next.ranges[rule.gate_idx][0] > next.ranges[rule.gate_idx][1]) {
        break;
      }
    }
  }

  auto result{0ull};
  /// First add up all accepted range hypervolumes (but this multi-counts!)
  for (const auto hc: accepted_hypercubes) {
    auto vol{1ull};
    for (auto [from, to]: hc) {
      std::cout << "[" << from << "," << to << "] ";
      vol *= (to - from + 1);
    }
    result += vol;
    std::cout << "\n";
  }

  // /// Collect all points on all dimensions where any hypercubes ends
  // std::array<std::set<int>,4> dimension_splits;
  // for (const auto& hc: accepted_hypercubes) {
  //   for (auto dim = 0; dim < 4; ++dim) {
  //     dimension_splits[dim].insert(hc[dim][0]-1);
  //     dimension_splits[dim].insert(hc[dim][1]);
  //   }
  // }
  // /// Split all hypercubes at all split points
  // std::set<std::array<std::array<int,2>,4>> all_splits;
  // for (const auto hc: accepted_hypercubes) {
  //   decltype(accepted_hypercubes) last_dim_splits;
  //   last_dim_splits.push_back(hc);
  //   for (auto dim = 0; dim < 4; ++dim) {
  //     decltype(last_dim_splits) new_dim_splits;
  //     for (const auto& sc: last_dim_splits) {
  //       auto previous = sc[dim][0];
  //       for (auto split_point: dimension_splits[dim]) {
  //         //if (split_point >= sc[dim][0] and
  //         //    split_point <  sc[dim][1]) {
  //         //  auto split_a = sc;
  //         //  split_a[dim][1] = split_point;
  //         //  auto split_b = sc;
  //         //  split_a[dim][0] = split_point+1;
  //         //  new_dim_splits.push_back(split_a);
  //         //  new_dim_splits.push_back(split_b);
  //         //} else {
  //         //  new_dim_splits.push_back(sc);
  //         //}
  //         if (split_point >= sc[dim][0] and
  //             split_point <  sc[dim][1]) {
  //           auto split = sc;
  //           split[dim][0] = previous;
  //           split[dim][1] = split_point;
  //           new_dim_splits.push_back(split);
  //           previous = split_point + 1;
  //         }
  //       }
  //       {
  //         auto split = sc;
  //         split[dim][0] = previous;
  //         new_dim_splits.push_back(split);
  //       }
  //     }
  //     last_dim_splits = new_dim_splits;
  //   }
  //   for (auto s: last_dim_splits) {
  //     if (all_splits.contains(s))
  //       std::cout << "dupe!\n";
  //     all_splits.insert(s);
  //   }
  // }

  // /// (Remove duplicates)

  // /// Add up all (now unique) hypercube volumes
  // for (const auto hc: all_splits) {
  //   auto vol{1ull};
  //   for (auto [from, to]: hc) {
  //     std::cout << "[" << from << "," << to << "] ";
  //     vol *= (to - from + 1);
  //   }
  //   result += vol;
  //   std::cout << "\n";
  // }
  

  std::cout << "Number of distinct accepted rating combinations: " << result << "\n";

  return EXIT_SUCCESS;
}

