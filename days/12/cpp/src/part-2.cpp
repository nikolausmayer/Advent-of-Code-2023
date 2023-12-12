
#include <cassert>
#include <cstdlib>
#include <algorithm>
#include <array>
#include <fstream>
#include <iomanip>
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

  /// Parse input file
  std::vector<std::tuple<std::vector<char>, std::vector<int>>> records;
  for (;;) {
    std::vector<char> springs;
    std::vector<int> runs;
    while (in.peek() != ' ') {
      springs.push_back(in.get());
    }
    in.get();
    auto i{0};
    while (in.peek() != '\n') {
      if (in.peek() == ',') {
        runs.push_back(i);
        i = 0;
        in.get();
      } else {
        i = i * 10 + (in.get() - '0');
      }
    }
    runs.push_back(i);
    records.push_back({springs, runs});
    springs.clear();
    runs.clear();
    in.get();
    if (in.peek() < 0)
      break;
  }

  /// Part 2: unfold records 5-fold
  for (auto& [springs, runs]: records) {
    {
      decltype(springs) unfolded_springs;
      for (auto fold = 0; fold < 5; ++fold) {
        for (auto c: springs)
          unfolded_springs.push_back(c);
        if (fold < 4)
          unfolded_springs.push_back('?');  /// stupid curveball
      }
      springs = unfolded_springs;
    }
    {
      decltype(runs) unfolded_runs;
      for (auto fold = 0; fold < 5; ++fold)
        for (auto r: runs)
          unfolded_runs.push_back(r);
      runs = unfolded_runs;
    }
  }

  //for (const auto& [s,r]: records) {
  //  for (auto c: s)
  //    std::cout << c;
  //  std::cout << " | ";
  //  for (auto i: r)
  //    std::cout << i << " ";
  //  std::cout << "\n";
  //}

  std::map<std::array<int,4>, std::uint64_t> memory;
  std::function<std::uint64_t(int,int,int,int)> recurse;
  recurse = [&](
    int records_idx,
    int next_spring,
    int next_run,
    int left_in_current_run
  ) -> std::uint64_t 
  {
    /// DP check
    if (not memory.contains({records_idx, next_spring, next_run, left_in_current_run})) {
      //for (auto c: std::get<0>(records[records_idx])) std::cout << c; std::cout << "\n";
      //for (auto i = 0; i < next_spring; ++i) std::cout << " ";
      //std::cout << "^ " << left_in_current_run << " | " << next_run << " | ";
      //for (auto r: std::get<1>(records[records_idx])) std::cout << r << " "; std::cout << "\n";

      const auto& [springs, runs] = records[records_idx];
      auto result{1ull};

      /// Run-over stop condition check
      if (next_spring == static_cast<int>(springs.size())) {
        if (left_in_current_run <= 0 and 
            next_run == static_cast<int>(runs.size())) {
          /// No more springs to cover -> OK, exactly this possibility exists
          //std::cout << "  ok stop\n";
          result = 1ull;
        } else {
          /// Springs left to cover -> impossible configuration
          //std::cout << "  BAD STOP\n";
          result = 0ull;
        }
      } else {
        switch (springs[next_spring]) {
          case '.': {
            if (left_in_current_run > 0) {
              /// OK spring, but we are in a run -> impossible configuration
              //std::cout << "  OK spring, but we are in a run -> BAD\n";
              result = 0ull;
            } else {
              /// OK spring and not in a run -> just recursive continue
              /// This confirms a gap between runs
              //std::cout << "  OK spring and not in a run -> cont\n";
              result = recurse(records_idx, next_spring+1, next_run, -1);
            }
            break;
          }
          case '#': {
            if (left_in_current_run > 0) {
              /// Broken spring and in a run -> just recursive continue
              //std::cout << "  Broken spring and in a run -> cont\n";
              result = recurse(records_idx, next_spring+1, next_run, left_in_current_run-1);
            } else if (left_in_current_run == 0) {
              /// Broken spring but last run just finished -> BAD
              //std::cout << "  Broken spring but last run just finished -> BAD\n";
              result = 0ull;
            } else {
              /// Broken spring but not in a run -> must start next run
              if (next_run < static_cast<int>(runs.size())) {
                /// Start next run
                //std::cout << "  Broken spring but not in a run -> start next run " << next_run << "(" << runs[next_run] << ")\n";
                result = recurse(records_idx, next_spring+1, next_run+1, runs[next_run]-1);
              } else {
                /// No runs left -> impossible configuration
                //std::cout << "  Broken spring but not in a run and no runs left -> BAD\n";
                result = 0ll;
              }
            }
            break;
          }
          default: /*'?'*/ {
            if (left_in_current_run > 0) {
              /// In a run -> must be a broken spring
              //std::cout << "  In a run -> must be a broken spring\n";
              result = recurse(records_idx, next_spring+1, next_run, left_in_current_run-1);
            } else if (left_in_current_run == 0) {
              /// Last run just finished -> cont with gap
              //std::cout << "  Last run just finished -> cont with gap\n";
              result = recurse(records_idx, next_spring+1, next_run, -1);
            } else {
              if (next_run < static_cast<int>(runs.size())) {
                /// Runs left -> could start next... or not 
                /// ! THIS IS THE BIFURCATION POINT !
                //std::cout << "  !! Runs left -> could start next " << next_run << "(" << runs[next_run] << ") or not\n";
                result = recurse(records_idx, next_spring+1, next_run+1, runs[next_run]-1)
                       + recurse(records_idx, next_spring+1, next_run, -1);
              } else {
                /// No runs left and not in a run -> just continue
                //std::cout << "  No runs left and not in a run -> cont\n";
                result = recurse(records_idx, next_spring+1, next_run, 0);
              }
            }
          }
        }
      }

      memory[{records_idx, next_spring, next_run, left_in_current_run}] = result;
      //std::cout << "\n";
    } else {
      //std::cout << "  (mem) " << records_idx << " " << next_spring << " " << next_run << " " << left_in_current_run << "\n";
    }
    return memory[{records_idx, next_spring, next_run, left_in_current_run}];
  };

  auto result{0ull};
  for (auto i = 0uz; i < records.size(); ++i)
    result += recurse(i, 0, 0, -1);

  std::cout << "Sum of possible arrangement counts: " << result << "\n";

  return EXIT_SUCCESS;
}

