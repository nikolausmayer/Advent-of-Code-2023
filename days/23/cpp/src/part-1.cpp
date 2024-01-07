
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
  
  //for (const auto& line: map)
  //  std::cout << line << "\n";

  struct Path {
    std::int32_t x, y;
    std::int32_t length;
    std::set<std::array<std::int32_t,2>> visited_tiles;
  };

  std::queue<Path> q;
  q.push({1,0,-1,{}});

  auto max_path_length = 0;
  while (q.size()) {
    auto next = q.front();
    q.pop();

    next.visited_tiles.insert({next.x, next.y});
    ++next.length;

    if (next.x == W - 2 and next.y == H - 1) {
      std::cout << "Path " << next.length << "\n";
      max_path_length = std::max(max_path_length, next.length);
      continue;
    }

    for (auto [dx,dy]: std::vector<std::array<std::int32_t,2>>{{0,-1},{1,0},{0,1},{-1,0}}) {
      const auto nx = next.x + dx;
      const auto ny = next.y + dy;
      if (nx >= 0 and nx < W and
          ny >= 0 and ny < H and
          map[ny][nx] != '#' and
          not next.visited_tiles.contains({nx,ny})) {
        if ((dx ==  1 and (map[ny][nx] != '.' and map[ny][nx] != '>')) or
            (dx == -1 and (map[ny][nx] != '.' and map[ny][nx] != '<')) or
            (dy ==  1 and (map[ny][nx] != '.' and map[ny][nx] != 'v')) or
            (dy == -1 and (map[ny][nx] != '.' and map[ny][nx] != '^')))
          continue;
        /// Observation: slopes are always singular, there is never e.g. ">>"
        auto is_slope = (map[ny][nx] != '.');
        auto next_visited = next.visited_tiles;
        if (is_slope)
          next_visited.insert({nx + dx, ny + dy});
        q.push({
          next.x + dx * (is_slope + 1),
          next.y + dy * (is_slope + 1),
          next.length + is_slope,
          next_visited
        });
      }
    }
  }

  std::cout << "The longest possible hike is " << max_path_length << " steps.\n";

  return EXIT_SUCCESS;
}

