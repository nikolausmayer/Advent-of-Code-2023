
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
  std::map<std::array<int,2>,bool> map_position_to_tile;
  int sx, sy, W, H;
  {
    auto y{0};
    for (std::string line;;) {
      std::getline(in, line);
      if (in.eof())
        break;
      for (auto x{0}; x < static_cast<std::int32_t>(line.size()); ++x) {
        const auto c{line[x]};
        if (c == 'S') {
          sx = x;
          sy = y;
        }
        map_position_to_tile[{x,y}] = (c != '#');
        W = x+1;
      }
      ++y;
    }
    H = y;
  }

  //for (auto y = 0; y < H; ++y) {
  //  for (auto x = 0; x < W; ++x) {
  //    std::cout << (map_position_to_tile[{x,y}] ? '.' : '#');
  //  }
  //  std::cout << "\n";
  //}

  std::queue<std::array<int,3>> q;
  q.push({sx, sy, 64});
  std::set<std::array<int,3>> visited;
  std::set<std::array<int,2>> goal_tiles;
  while (q.size()) {
    const auto [x, y, steps_left] = q.front();
    q.pop();
    if (visited.contains({x, y, steps_left}))
      continue;
    visited.insert({x, y, steps_left});

    if (steps_left == 0) {
      goal_tiles.insert({x, y});
      continue;
    }

    for (const auto [dx,dy]: std::vector<std::array<int,2>>{{0,-1},{1,0},{0,1},{-1,0}}) {
      const auto nx{x + dx};
      const auto ny{y + dy};
      if (nx >= 0 and nx < W and
          ny >= 0 and ny < H and
          map_position_to_tile[{nx,ny}])
        q.push({nx, ny, steps_left - 1});
    }
  }
  
  std::cout << "Can reach " << goal_tiles.size() << " tiles with exactly 64 steps\n";

  return EXIT_SUCCESS;
}

