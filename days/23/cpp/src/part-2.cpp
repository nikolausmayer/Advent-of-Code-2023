
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


int main(int argc, char* argv[]) 
{
  assert(argc == 2);
  std::ifstream in{argv[1]};
  assert(in.is_open() and not in.bad());

  /// Read input
  std::vector<std::string> map;
  for (std::string line;;) {
    std::getline(in, line);
    if (in.eof())
      break;
    map.push_back(line);
  }

  const std::int32_t H = map.size();
  const std::int32_t W = map[0].size();
  
  /// First let's identify path crossings
  std::set<std::array<int,2>> crossings;
  for (auto y = 0; y < H; ++y) {
    for (auto x = 0; x < W; ++x) {
      if (map[y][x] == '#')
        continue;

      auto paths = 0;
      for (auto [dx,dy]: std::vector<std::array<std::int32_t,2>>{{0,-1},{1,0},{0,1},{-1,0}}) {
        const auto nx = x + dx;
        const auto ny = y + dy;
        if (nx >= 0 and nx < W and
            ny >= 0 and ny < H and
            map[ny][nx] != '#')
          ++paths;
      }

      /// 3-way ("T") and 4-way ("+") crossings
      if (paths >= 3)
        crossings.insert({x,y});
    }
  }
  /// start and end point are treated as crossings, for convenience
  crossings.insert({1,0});
  crossings.insert({W-2,H-1});

  /// Now find all direct paths between crossings
  std::map<std::array<std::array<int,2>,2>,int> path_lengths_between_crossings;
  struct Path {
    std::int32_t x, y, xstart, ystart;
    std::int32_t length;
    std::set<std::array<std::int32_t,2>> visited_tiles;
  };

  std::queue<Path> q;
  for (auto [x,y]: crossings)
    q.push({x,y,x,y,-1,{}});

  while (q.size()) {
    auto next = q.front();
    q.pop();

    next.visited_tiles.insert({next.x, next.y});
    ++next.length;

    /// Stop at the next crossing
    if ((next.x != next.xstart or 
         next.y != next.ystart) and 
        crossings.contains({next.x, next.y})) {
      /// Store max path between crossings
      auto& ref = path_lengths_between_crossings[{{{next.xstart, next.ystart}, 
                                                   {next.x, next.y}}}];
      ref = std::max(ref, next.length);
      continue;
    }

    for (auto [dx,dy]: std::vector<std::array<std::int32_t,2>>{{0,-1},{1,0},{0,1},{-1,0}}) {
      const auto nx = next.x + dx;
      const auto ny = next.y + dy;
      if (nx >= 0 and nx < W and
          ny >= 0 and ny < H and
          map[ny][nx] != '#' and
          not next.visited_tiles.contains({nx,ny})) {
        q.push({nx, ny, next.xstart, next.ystart, next.length, next.visited_tiles});
      }
    }
  }

  std::map<std::array<int,2>,std::set<std::array<int,2>>> connections;
  for (const auto& [abcd, _]: path_lengths_between_crossings) {
    connections[{abcd[0][0],abcd[0][1]}].insert({abcd[1][0],abcd[1][1]});
    connections[{abcd[1][0],abcd[1][1]}].insert({abcd[0][0],abcd[0][1]});
  }

  /// Now do path expansion using crossings as nodes (instead of map tiles)
  auto max_path_length = 0;
  q.push({1,0,0,0,0,{}});  /// ! starting length 0, not -1 !
  while (q.size()) {
    auto next = q.front();
    q.pop();

    next.visited_tiles.insert({next.x, next.y});

    if (next.x == W - 2 and next.y == H - 1) {
      if (next.length > max_path_length)
        std::cout << "Path " << next.length << "\n";
      max_path_length = std::max(max_path_length, next.length);
      continue;
    }

    for (const auto& connected: connections[{next.x, next.y}]) {
      if (next.visited_tiles.contains(connected))
        continue;
      q.push({
        connected[0], connected[1], 0, 0,
        next.length + path_lengths_between_crossings[{{{next.x, next.y}, connected}}],
        next.visited_tiles
      });
    }
  }

  std::cout << "The longest possible hike (on dry ground) is " << max_path_length << " steps.\n";

  return EXIT_SUCCESS;
}

