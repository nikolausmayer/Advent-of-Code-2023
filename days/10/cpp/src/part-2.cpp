
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

  /// Find starting position and a starting direction
  int xs, ys;
  char dir;
  for (auto y = 0uz; y < map.size(); ++y) {
    for (auto x = 0uz; x < map[0].size(); ++x) {
      if (map[y][x] == 'S') {
        xs = x;
        ys = y;
        if (x > 0 and (map[y][x-1] == '-' or
                       map[y][x-1] == 'L' or
                       map[y][x-1] == 'F'))
          dir = 'L';
        if (x < map[0].size()-1 and (map[y][x+1] == '-' or
                                     map[y][x+1] == '7' or
                                     map[y][x+1] == 'J'))
          dir = 'R';
        if (y > 0 and (map[y-1][x] == '|' or
                       map[y-1][x] == '7' or
                       map[y-1][x] == 'F'))
          dir = 'U';
        if (y < map.size()-1 and (map[y+1][x] == '|' or
                                  map[y+1][x] == 'J' or
                                  map[y+1][x] == 'L'))
          dir = 'D';
        //std::cout << "start " << xs << "," << ys << " " << dir << "\n";
      }
    }
  }

  /// Walk whole pipe loop
  /// Mark pipe tiles as already processed (they act as a boundary later)
  /// Collect exploration tiles along the way; but we only know after closing
  /// the loop whether it's a clockwise or counterclockwise loop, so we collect
  /// both candidate sets and pick the right one later (this decision is based
  /// on how many left vs right turns a pipe traverser takes).
  auto x{xs}, y{ys}, steps{0}, l_turns{0}, r_turns{0};
  std::vector<std::array<int,2>> tile_queue_cw, tile_queue_ccw;
  std::set<std::array<int,2>> processed_tiles;
  for (;; ++steps) {
    processed_tiles.insert({x,y});
    if (steps != 0 and x == xs and y == ys)
      break;
    x += (dir == 'L' ? -1 : dir == 'R' ? 1 : 0);
    y += (dir == 'U' ? -1 : dir == 'D' ? 1 : 0);
    auto nx_cw = x + (dir == 'U' ? 1 : dir == 'D' ? -1 : 0);
    auto ny_cw = y + (dir == 'R' ? 1 : dir == 'L' ? -1 : 0);
    auto nx_ccw = x + (dir == 'U' ? -1 : dir == 'D' ? 1 : 0);
    auto ny_ccw = y + (dir == 'R' ? -1 : dir == 'L' ? 1 : 0);
    tile_queue_cw.push_back({nx_cw,ny_cw});
    tile_queue_ccw.push_back({nx_ccw,ny_ccw});
    if (dir == 'L' and map[y][x] == 'L') { dir = 'U'; ++r_turns; }
    if (dir == 'L' and map[y][x] == 'F') { dir = 'D'; ++l_turns; }
    if (dir == 'R' and map[y][x] == 'J') { dir = 'U'; ++l_turns; }
    if (dir == 'R' and map[y][x] == '7') { dir = 'D'; ++r_turns; }
    if (dir == 'U' and map[y][x] == '7') { dir = 'L'; ++l_turns; }
    if (dir == 'U' and map[y][x] == 'F') { dir = 'R'; ++r_turns; }
    if (dir == 'D' and map[y][x] == 'J') { dir = 'L'; ++r_turns; }
    if (dir == 'D' and map[y][x] == 'L') { dir = 'R'; ++l_turns; }
    nx_cw = x + (dir == 'U' ? 1 : dir == 'D' ? -1 : 0);
    ny_cw = y + (dir == 'R' ? 1 : dir == 'L' ? -1 : 0);
    nx_ccw = x + (dir == 'U' ? -1 : dir == 'D' ? 1 : 0);
    ny_ccw = y + (dir == 'R' ? -1 : dir == 'L' ? 1 : 0);
    tile_queue_cw.push_back({nx_cw,ny_cw});
    tile_queue_ccw.push_back({nx_ccw,ny_ccw});
  }

  auto& tile_queue = (l_turns > r_turns ? tile_queue_ccw : tile_queue_cw);

  /// I think this is DFS, haven't thought it through too much
  auto result{0};
  while (tile_queue.size()) {
    auto current = tile_queue.back();
    tile_queue.pop_back();
    if (processed_tiles.contains(current))
      continue;

    const auto [x,y] = current;
    if (x < 0 or x >= static_cast<int>(map[0].size()) or
        y < 0 or y >= static_cast<int>(map.size()))
      continue;

    ++result;
    processed_tiles.insert(current);

    /// Debug printout
    map[y][x] = 'I';
    for (const auto& row: map) {
      std::cout << row << "\n";
    }
    std::cout << "\n";

    if (not processed_tiles.contains({x-1,y})) tile_queue.push_back({x-1,y});
    if (not processed_tiles.contains({x+1,y})) tile_queue.push_back({x+1,y});
    if (not processed_tiles.contains({x,y-1})) tile_queue.push_back({x,y-1});
    if (not processed_tiles.contains({x,y+1})) tile_queue.push_back({x,y+1});
  };

  /// (Round up in case of odd loop length)
  std::cout << "Tiles enclosed by loop: " << result << "\n";

  return EXIT_SUCCESS;
}

