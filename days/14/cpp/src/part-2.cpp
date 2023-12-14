
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
  std::vector<std::string> map;
  for (;;) {
    std::string s;
    std::getline(in, s);
    if (in.eof())
      break;
    map.push_back(s);
  }
  const int H = map.size();
  const int W = map[0].size();

  //for (const auto& row: map)
  //  std::cout << row << "\n";
  //std::cout << "\n";

  /// oh god RIP RAM
  std::map<decltype(map), std::uint64_t> state_memory;

  auto skipped{false};
  for (auto i = 1ull; i <= 1'000'000'000ull; ++i) {
    /// SPIN CYYYYYYCLEEEEE
    /// Roll north
    for (auto y = 0; y < H; ++y) {
      for (auto x = 0; x < W; ++x) {
        if (map[y][x] == 'O') {
          auto ty{y};
          while (ty > 0 and map[ty-1][x] == '.') {
            map[ty][x] = '.';
            map[ty-1][x] = 'O';
            --ty;
          }
        }
      }
    }
    /// Roll west
    for (auto x = 0; x < W; ++x) {
      for (auto y = 0; y < H; ++y) {
        if (map[y][x] == 'O') {
          auto tx{x};
          while (tx > 0 and map[y][tx-1] == '.') {
            map[y][tx] = '.';
            map[y][tx-1] = 'O';
            --tx;
          }
        }
      }
    }
    /// Roll south
    for (auto y = H-1; y >= 0; --y) {
      for (auto x = 0; x < W; ++x) {
        if (map[y][x] == 'O') {
          auto ty{y};
          while (ty < H-1 and map[ty+1][x] == '.') {
            map[ty][x] = '.';
            map[ty+1][x] = 'O';
            ++ty;
          }
        }
      }
    }
    /// Roll east
    for (auto x = W-1; x >= 0; --x) {
      for (auto y = 0; y < H; ++y) {
        if (map[y][x] == 'O') {
          auto tx{x};
          while (tx < W-1 and map[y][tx+1] == '.') {
            map[y][tx] = '.';
            map[y][tx+1] = 'O';
            ++tx;
          }
        }
      }
    }

    /// Loop detection
    if (not skipped) {
      if (state_memory.contains(map)) {
        std::cout << "Loop detected: " << state_memory[map] << " -> " << i << "\n";
        const auto loop_period{i - state_memory[map]};

        /// Skip periodic part
        i += ((1'000'000'000ull - i) / loop_period) * loop_period;
        std::cout << "Hyperdrive skip to iteration " << i << "\n";

        skipped = true;
      }
      state_memory[map] = i;
    }

    //std::cout << i << "\n";
    
    //for (const auto& row: map)
    //  std::cout << row << "\n";
    //std::cout << "\n";
  }

  /// Sum up result
  auto result{0ull};
  for (auto y = 0; y < H; ++y) {
    for (auto x = 0; x < W; ++x) {
      if (map[y][x] == 'O') {
        result += H - y;
      }
    }
  }
  
  std::cout << "Total round-rocks load on north support beams: " << result << "\n";

  return EXIT_SUCCESS;
}

