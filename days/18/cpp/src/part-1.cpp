
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
#include <queue>
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

  /// Read input file
  std::vector<std::string> plan;
  for (std::string s;;) {
    std::getline(in, s);
    if (in.eof())
      break;
    plan.push_back(s);
  }

  /// Inside/outside code copied from day 10 part 2
  auto x{0}, y{0}, l_turns{0}, r_turns{0};
  std::vector<std::array<int,2>> tile_queue_cw, tile_queue_ccw;
  std::set<std::array<int,2>> processed_tiles;
  /// Walk through plan and dig trench
  for (const auto& instruction: plan) {
    const auto dir{instruction[0]};
    auto steps{std::stoi(instruction.substr(2, instruction.size() - 12))};
    for (; steps > 0; --steps) {
      processed_tiles.insert({x,y});
      x += (dir == 'L' ? -1 : dir == 'R' ? 1 : 0);
      y += (dir == 'U' ? -1 : dir == 'D' ? 1 : 0);
      auto nx_cw = x + (dir == 'U' ? 1 : dir == 'D' ? -1 : 0);
      auto ny_cw = y + (dir == 'R' ? 1 : dir == 'L' ? -1 : 0);
      auto nx_ccw = x + (dir == 'U' ? -1 : dir == 'D' ? 1 : 0);
      auto ny_ccw = y + (dir == 'R' ? -1 : dir == 'L' ? 1 : 0);
      tile_queue_cw.push_back({nx_cw,ny_cw});
      tile_queue_ccw.push_back({nx_ccw,ny_ccw});
    }
  }

  {
    auto minx{1000}, maxx{-1000}, miny{1000}, maxy{-1000};
    std::set<std::array<int,2>> map;
    for (const auto& [x,y]: processed_tiles) {
      minx = std::min(minx, x); maxx = std::max(maxx, x);
      miny = std::min(miny, y); maxy = std::max(maxy, y);
      map.insert({x,y});
    }
    for (auto y{miny}; y <= maxy; ++y) {
      for (auto x{minx}; x <= maxx; ++x) {
        std::cout << (map.contains({x,y}) ? '#' : '.');
      }
      std::cout << "\n";
    }
    std::cout << "\n";
  }

  /// Dig out "inside" area
  auto& tile_queue = (l_turns > r_turns ? tile_queue_ccw : tile_queue_cw);
  auto result{processed_tiles.size()};
  while (tile_queue.size()) {
    auto current = tile_queue.back();
    tile_queue.pop_back();
    if (processed_tiles.contains(current))
      continue;

    const auto [x,y] = current;

    ++result;
    processed_tiles.insert(current);

    if (not processed_tiles.contains({x-1,y})) tile_queue.push_back({x-1,y});
    if (not processed_tiles.contains({x+1,y})) tile_queue.push_back({x+1,y});
    if (not processed_tiles.contains({x,y-1})) tile_queue.push_back({x,y-1});
    if (not processed_tiles.contains({x,y+1})) tile_queue.push_back({x,y+1});
  };

  {
    auto minx{1000}, maxx{-1000}, miny{1000}, maxy{-1000};
    std::set<std::array<int,2>> map;
    for (const auto& [x,y]: processed_tiles) {
      minx = std::min(minx, x); maxx = std::max(maxx, x);
      miny = std::min(miny, y); maxy = std::max(maxy, y);
      map.insert({x,y});
    }
    for (auto y{miny}; y <= maxy; ++y) {
      for (auto x{minx}; x <= maxx; ++x) {
        std::cout << (map.contains({x,y}) ? '#' : '.');
      }
      std::cout << "\n";
    }
    std::cout << "\n";
  }

  /// (Round up in case of odd loop length)
  std::cout << "Tiles enclosed by loop: " << result << "\n";


  return EXIT_SUCCESS;
}

